#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)

#check if brew is installed
if ! command -v brew >/dev/null 2>&1; then
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

eval "$(/opt/homebrew/bin/brew shellenv 2>/dev/null || /usr/local/bin/brew shellenv 2>/dev/null)"

# check if gcc is installed
if [ ! -f /opt/homebrew/bin/gcc-15 ] || [ ! -f /usr/local/bin/gcc-15 ]; then
    brew install gcc
fi

# Check if homebrew python3 is installed
if ! brew list python@3 >/dev/null 2>&1; then
    brew install python@3
fi

gcc-15 -shared -fPIC -fopenmp -Wall -o "$build_path/libmandelbrot.so" "$C_path/lodepng.c" "$C_path/color.c" "$C_path/meta.c" "$C_path/fractalm.c" -lm
gcc-15  -fopenmp -Wall -o "$build_path/mandelbrot" "$C_path/main.c" "$C_path/lodepng.c" "$C_path/color.c" "$C_path/meta.c" "$C_path/fractalm.c" -lm
