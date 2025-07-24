#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)

just_updated=false
# check if gcc is installed
if ! command -v gcc &> /dev/null; then
    if [ "$just_updated" = false ]; then
        sudo apt update
        just_updated=true
    fi
    sudo apt install gcc -y
fi

# check if python3 is installed
if ! command -v python3 &> /dev/null; then
    if [ "$just_updated" = false ]; then
        sudo apt update
        just_updated=true
    fi
    sudo apt install python3 -y
fi

# check if python3-venv is installed
if ! dpkg -s python3-venv &> /dev/null; then
    if [ "$just_updated" = false ]; then
        sudo apt update
    fi
    sudo apt install python3-venv -y
fi

gcc -shared -fPIC -fopenmp -Wall -o $build_path/libmandelbrot.so $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c
gcc  -fopenmp -Wall -o $build_path/mandelbrot $C_path/main.c $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c