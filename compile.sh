#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)
gcc-15 -shared -fPIC -fopenmp -Wall -o $build_path/libmandelbrot.so $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c
gcc-15  -fopenmp -Wall -o $build_path/mandelbrot $C_path/main.c $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c