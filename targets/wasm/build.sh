#!/usr/bin/env bash

# export CC=/Users/xuliang/emsdk/fastcomp/emscripten/emcc
# export CXX=/Users/xuliang/emsdk/fastcomp/emscripten/emcc
mkdir -p build/wasm
cd build/wasm
cmake \
  -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake \
  ../../targets/wasm
make