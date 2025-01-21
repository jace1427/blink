#!/bin/bash

# Get the base name of the current directory
current_dir=$(basename "$PWD")

# Check if the directory name is "build"
if [ "$current_dir" != "build" ]; then
  echo "Not in ./build, Exiting"
else
  echo "In ./build, Building"
  rm -rf ./*
  cmake .. -DPICO_BOARD=pico2_w -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
fi
