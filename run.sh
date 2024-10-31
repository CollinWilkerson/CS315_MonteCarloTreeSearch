#!/bin/bash

#check for cmake error, if error you run /opt/homebrew/bin/cmake .

res = cmake .
if [ $? -ne 0 ]; then
    /opt/homebrew/bin/cmake .
fi

make
./MonteCarlo
