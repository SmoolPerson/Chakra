import pygame
import threading as t
from connection import get_mandelbrot, get_default_config, Config, init_gpu
from dataclasses import dataclass
from text_box import TextBox
import numpy as np
# disable high dpi support to render a higher quality image
import os
os.environ['SDL_VIDEO_HIGHDPI_DISABLED'] = '1'
color_step = 0.0005
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
    total_scroll: float
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
        self.total_scroll = 1
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

def update_screen_dim(amount_scrolled: float, context: Context, mx, my) -> Bounds:
    my = -my + context.SCREEN_DIM # flip y axis since pygame has 0,0 at top left
    screen_x_factor = mx / context.SCREEN_DIM
    screen_y_factor = my / context.SCREEN_DIM # find the amount the mouse is off from the top left corner of the screen
    mandelbrot_x = context.bounds.width_min * (1 - screen_x_factor) + context.bounds.width_max * screen_x_factor # linear interpolate to get in mandelbrot coords
    mandelbrot_y = context.bounds.height_min * (1 - screen_y_factor) + context.bounds.height_max * screen_y_factor
    # we perform a dilation around the mandelbrot point the mouse is at
    translated_bounds = Bounds(
        context.bounds.width_min - mandelbrot_x,
        context.bounds.width_max - mandelbrot_x,
        context.bounds.height_min - mandelbrot_y,
        context.bounds.height_max - mandelbrot_y)
    zoom_factor = 1 + (amount_scrolled*0.025)
    scaled_bounds = Bounds(translated_bounds.width_min * zoom_factor,
                           translated_bounds.width_max * zoom_factor,
                           translated_bounds.height_min * zoom_factor,
                           translated_bounds.height_max * zoom_factor)
    translated_back_bounds = Bounds(
        scaled_bounds.width_min + mandelbrot_x,
        scaled_bounds.width_max + mandelbrot_x,
        scaled_bounds.height_min + mandelbrot_y,
        scaled_bounds.height_max + mandelbrot_y)
    

    return translated_back_bounds

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
    config.MAX_ITER = 300
    config.ANTI_ALIASING = 1
    config.ANTI_ALIASING_NUM_PTS = 2
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
            context.textbox.handle_event(event)
            if event.type == pygame.QUIT:
                context.running = False
            if event.type == pygame.MOUSEWHEEL and not context.mandelbrot_thread:
                context.bounds = update_screen_dim(event.y, context, *pygame.mouse.get_pos())
                # Attempt to get the text from the text box and convert it to an int, if it fails just ignore it
                try:
                    new_dim = float(context.textbox.get_text())
                    if new_dim > 0:
                        global color_step
                        color_step = new_dim
                except:
                    pass
                context.mandelbrot_thread = generate_mandelbrot(context, do_in_thread=True)
        context.clock.tick(60)
        pygame.display.flip()
if __name__ == "__main__":
    try:
        main_loop()
    finally:
        pygame.quit()
    