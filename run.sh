#!/bin/bash

if command -v cmake >/dev/null 2>&1; then
    cmake .
else
    /opt/homebrew/bin/cmake .
fi

make
./MonteCarlo
