#!/bin/bash
#shared: 
gcc-15 -shared -fPIC -fopenmp -Wall -o ./build/libmandelbrot.so lodepng.c color.c meta.c fractalm.c
# normal:
gcc-15  -fopenmp -Wall -o ./build/mandelbrot main.c lodepng.c color.c meta.c fractalm.c