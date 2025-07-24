#include "cfstruct.h"

#ifndef FRACTALM_H
#define FRACTALM_H

double mandelbrot(double cx, double cy, Config *config);
rgb_type get_gradient(double value, rgb_type *gradient, int len);

#endif