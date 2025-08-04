#include "cfstruct.hpp"

#ifndef COLOR_H
#define COLOR_H

float lerp(double a, double b, double t);
rgb_type calculate_hue(double mandelbrot_val, Config *config);
void get_and_set_pixel_color(int imagex, int imagey, unsigned char *rgb_data, Config *config);
extern "C" {unsigned char *generate_mandelbrot(Config *config, bool logging);}

#endif