#!/bin/bash
if [ ! -f build/libmandelbrot.so ]; then
    ./compile.sh
    ./compile-metal.sh
fi
./run.sh