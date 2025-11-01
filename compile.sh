#!/bin/bash
build_path=$(readlink -f ./build)
C_path=$(readlink -f ./src/C)
METAL_path=$(readlink -f ./src/metal)
#check if brew is installed
if ! command -v brew >/dev/null 2>&1; then
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

eval "$(/opt/homebrew/bin/brew shellenv 2>/dev/null || /usr/local/bin/brew shellenv 2>/dev/null)"

# check if gcc is installed
if ! command -v gcc-15 >/dev/null 2>&1; then
    brew install gcc@15
fi

# Check if homebrew python3 is installed
if ! brew list python >/dev/null 2>&1; then
    brew install python
fi

clang++ -x objective-c++ -shared -fPIC -Wall -o "$build_path/libmandelbrot.so" "$C_path/lodepng.c" "$C_path/color.cpp" "$C_path/meta.cpp" "$C_path/fractalmath.cpp" "$METAL_path/gpu_bridge.mm" -lm -fobjc-arc -framework Metal -framework Foundation -framework AppKit -framework MetalKit
clang++ -x objective-c++ -Wall -o "$build_path/mandelbrot" "$C_path/main.cpp" "$C_path/lodepng.c" "$C_path/color.cpp" "$C_path/meta.cpp" "$C_path/fractalmath.cpp" "$METAL_path/gpu_bridge.mm" -lm -fobjc-arc -framework Metal -framework Foundation -framework AppKit -framework MetalKit
