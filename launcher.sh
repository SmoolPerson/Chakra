#!/bin/bash
if [ ! -f build/libmandelbrot.so ]; then
    ./compile.sh
fi
./run.sh