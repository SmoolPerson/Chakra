#!/bin/bash
if [ ! -f build/libmandelbrot.so ]; then
    ./compile-linux.sh
fi
./run.sh