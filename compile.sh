#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)

#check if brew is installed
if ! command -v brew >/dev/null 2>&1; then
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  eval "$(/opt/homebrew/bin/brew shellenv 2>/dev/null || /usr/local/bin/brew shellenv 2>/dev/null)"
fi


# check if gcc is installed
if ! command -v gcc-15 &> /dev/null; then
    brew install gcc
fi

# check if homebrew python3 is installed
if [ ! -f /opt/homebrew/bin/python3 ]; then
    brew install python3
fi

gcc-15 -shared -fPIC -fopenmp -lm -Wall -o $build_path/libmandelbrot.so $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c
gcc-15  -fopenmp -lm -Wall -o $build_path/mandelbrot $C_path/main.c $C_path/lodepng.c $C_path/color.c $C_path/meta.c $C_path/fractalm.c
