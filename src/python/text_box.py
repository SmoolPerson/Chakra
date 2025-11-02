import pygame
from dataclasses import dataclass

@dataclass
class Box:
    firstx: int
    secondx: int
    firsty: int
    secondy: int

class TextBox:
    def __init__(self, x1, x2, y1, y2, max_chars=5):
        self.box = Box(x1, x2, y1, y2)
        self.active = False
        self.text = ""
        self.max_chars = max_chars

    def handle_event(self, event: pygame.event.Event):
        if event.type == pygame.MOUSEBUTTONDOWN:
            # If the user clicked on the input box rect
            if self.is_within(event.pos[0], event.pos[1]):
                self.active = True
            else:
                self.active = False
        if event.type == pygame.KEYDOWN:
            if not self.active:
                return
            if event.key == pygame.K_BACKSPACE:
                self.text = self.text[:-1]
            elif len(self.text) < self.max_chars:
                self.text += event.unicode
    def draw(self, screen: pygame.Surface, font: pygame.font.Font):

        # Create a rect surface
        color = None
        if self.active:
            color = (0, 255, 128)
        else:
            color = (255, 255, 255)
        pygame.draw.rect(screen, color, (self.box.firstx, self.box.firsty, self.box.secondx - self.box.firstx, self.box.secondy - self.box.firsty))
        # Create a text surface
        text_surface = font.render(self.text, True, (255, 0, 0))

        # Blit the text onto the screen
        screen.blit(text_surface, (self.box.firstx+5, self.box.firsty+5))

        # create a line
        # get the edge of the text box
        x_val = self.box.firstx+5 + text_surface.get_width()
        y_val1 = self.box.firsty+5
        y_val2 = self.box.firsty+5+ text_surface.get_height()
        # draw based on time and modulo so it blinks an even amount of time
        time_elapsed = pygame.time.get_ticks() / 1000
        if self.active and time_elapsed % 1 > 0.5:
            pygame.draw.line(screen, (0,0,0), (x_val, y_val1), (x_val, y_val2), width=1)
    
    def get_text(self):
        return self.text
    def is_within(self, x, y):
        return self.box.firstx <= x <= self.box.secondx and self.box.firsty <= y <= self.box.secondy