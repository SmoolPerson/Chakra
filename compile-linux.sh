#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)

sudo apt update

# check if gcc is installed
if ! command -v gcc &> /dev/null; then
    sudo apt install gcc -y
fi

# check if python3 is installed
if ! command -v python3 &> /dev/null; then
    sudo apt install python3 -y
fi

# check if python3-venv is installed
if ! dpkg -s python3-venv &> /dev/null; then
    sudo apt install python3-venv -y
fi

gcc -shared -fPIC -fopenmp -Wall -o $build_path/libmandelbrot.so $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c -lm
gcc  -fopenmp -Wall -o $build_path/mandelbrot $C_path/main.c $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c -lm