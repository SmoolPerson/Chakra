import pygame
import threading as t
from connection import get_mandelbrot, get_default_config, Config, init_gpu
from dataclasses import dataclass
from text_box import TextBox
import numpy as np
color_step = 0.03
@dataclass
class Box:
    firstx: int
    secondx: int
    firsty: int
    secondy: int

@dataclass
class Bounds:
    width_min: float
    width_max: float
    height_min: float
    height_max: float

@dataclass
class Context:
    SCREEN_DIM: int
    bounds: Bounds
    box: Box
    running: bool
    generating: bool
    clock: pygame.time.Clock
    mandelbrot_thread: t.Thread
    res: list
    surf: pygame.Surface
    screen: pygame.Surface
    screen_height: int
    def __init__(self):
        self.box = Box(0,0,0,0)
        pygame.init()
        # Scale down a little so it doesn't take up the entire screen including the doc bar
        self.screen_height = 750
        self.SCREEN_DIM = int(self.screen_height)
        print(self.SCREEN_DIM)
        self.bounds = Bounds(-2,2,-2,2)
        self.screen = pygame.display.set_mode((self.screen_height, self.screen_height))
        self.running = True
        self.generating = False
        self.clock = pygame.time.Clock()

        self.mandelbrot_thread = None
        self.res = []
        self.surf = generate_mandelbrot(self, do_in_thread=False)
        self.font = pygame.font.SysFont("arial", 14)
        self.textbox = TextBox(10, 110, 10, 40, max_chars=13)

def draw_selection_rectangle(context: Context):
    box = context.box
    screen = context.screen
    rect_x = min(box.firstx, box.secondx)
    rect_y = min(box.firsty, box.secondy)
    rect_w = abs(box.secondx - box.firstx)
    rect_h = abs(box.secondy - box.firsty)
    pygame.draw.rect(screen, (255,0,0), ((rect_x, rect_y, rect_w, rect_h)), 1)

def update_screen_dim(context: Context):
    box = context.box
    bounds = context.bounds
    lowest_x, highest_x = sorted((box.firstx, box.secondx))
    # subtract by SCREEN_DIM since y is opposite between pygame and math
    invert_firsty = context.screen_height - box.firsty
    invert_secondy = context.screen_height - box.secondy
    # Since subtracting changes which one is the lowest
    lowest_y, highest_y = sorted((invert_firsty, invert_secondy))
    mandelbrot_width = bounds.width_max - bounds.width_min
    mandelbrot_height = bounds.height_max - bounds.height_min

    # get the leftmost x value, and divide by SCREEN_DIM to normalize it between 0 and 1
    # then multiply it by the real width to bring it into mandelbrot space, and add width_min to offset it to the screen
    # similar logic for ones that follow
    new_bounds = Bounds(0,0,0,0)
    new_bounds.width_min = bounds.width_min + (lowest_x/context.screen_height * mandelbrot_width)
    new_bounds.width_max = bounds.width_min + (highest_x/context.screen_height * mandelbrot_width)
    new_bounds.height_min = bounds.height_min + (lowest_y/context.screen_height * mandelbrot_height)
    new_bounds.height_max = bounds.height_min + (highest_y/context.screen_height * mandelbrot_height)
    
    new_bounds.width_min, new_bounds.width_max = sorted((new_bounds.width_min, new_bounds.width_max))
    new_bounds.height_min, new_bounds.height_max = sorted((new_bounds.height_min, new_bounds.height_max))

    return new_bounds

def generate_mandelbrot(context, do_in_thread=True):
    res = context.res
    bounds = context.bounds
    config = get_default_config()
    attributes = ["width_min", "width_max", "height_min", "height_max"]
    # so that we don't have to manually set each attribute
    for attr in attributes:
        setattr(config, attr, getattr(bounds, attr))
    config.WIDTH = context.SCREEN_DIM
    config.HEIGHT = context.SCREEN_DIM

    # TODO: Make a config.json file to store these values instead of hardcoding them
    config.MAX_ITER = 600
    config.ANTI_ALIASING = 1
    config.ANTI_ALIASING_NUM_PTS = 100
    config.COLOR_STEP_MULTIPLIER = color_step
    config.COLOR_OFFSET = 0
    config.ITERATION_CHECK = 1
    config.GPU = True
    if do_in_thread:
        thread = t.Thread(target=get_mandelbrot, kwargs={"config": config, "res": res})
        thread.start()
        return thread
    else:
        get_mandelbrot(config, res)
        return res[0]

def main_loop():
    init_gpu()
    print("Press ctrl + c to quit")
    context = Context()
    while context.running:
        context.screen.fill((0,0,0))
        context.screen.blit(context.surf, (0,0))
        # If the thread has ended, then get the surface
        if context.res:
            context.surf = context.res[0]
            context.res = []
            context.mandelbrot_thread = None
        context.textbox.draw(context.surf, context.font)
        for event in pygame.event.get():
            # check if mouse is inside text box, some things dont apply if it is
            is_within = context.textbox.is_within(pygame.mouse.get_pos()[0], pygame.mouse.get_pos()[1])
            context.textbox.handle_event(event)
            if event.type == pygame.QUIT:
                context.running = False

            # If the user does an action with the left click and there is nothing on the thread generating
            if getattr(event, 'button', None):
                if event.button == 1 and not context.mandelbrot_thread and not is_within:
                    # If the user clicks once
                    if event.type == pygame.MOUSEBUTTONDOWN:
                        # This is the first corner the user chose as part of the new view
                        context.box.firstx, context.box.firsty = pygame.mouse.get_pos()
                
                    # If the user releases the mouse
                    elif event.type == pygame.MOUSEBUTTONUP:
                        # first, let's update some important globals for generating our new mandelbrot
                        context.bounds = update_screen_dim(context)
                        # Attempt to get the text from the text box and convert it to an int, if it fails just ignore it
                        try:
                            new_dim = float(context.textbox.get_text())
                            if new_dim > 0:
                                global color_step
                                color_step = new_dim
                        except:
                            print("failed to convert userinput to float, ignoring")
                        context.mandelbrot_thread = generate_mandelbrot(context, do_in_thread=True)
    
        # While the user keeps pressing the mouse
        if pygame.mouse.get_pressed()[0] and not context.mandelbrot_thread and not is_within:
            # This is the other corner the user chooses when they hold the mouse, make it so that only the x can control how big it is so that the user doesn't select a nonsquare
            context.box.secondx, context.box.secondy = pygame.mouse.get_pos()
            diff = abs(context.box.secondx - context.box.firstx)
            context.box.secondy = context.box.firsty + diff

            # Draw a rect to show exactly where their new view will end up
            draw_selection_rectangle(context)
        context.clock.tick(60)
        pygame.display.flip()
if __name__ == "__main__":
    try:
        main_loop()
    finally:
        pygame.quit()
    