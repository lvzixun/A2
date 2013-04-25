#!/bin/sh

lua ./src/dynasm/dynasm.lua  ./src/a2_jitx64.desc  > ./src/a2_jit.h
make clean
make