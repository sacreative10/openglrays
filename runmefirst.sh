#!/usr/bin/env sh

# initialize all submodules
git submodule update --init --recursive

mkdir build
cd build
cmake ..
cd ..
ln build/compile_commands.json .
