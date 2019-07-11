#!/usr/bin/env bash

# export CC=/Users/xuliang/emsdk/fastcomp/emscripten/emcc
# export CXX=/Users/xuliang/emsdk/fastcomp/emscripten/emcc

BUILD_DIR=build/wasm
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake \
  -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake \
  -DDEBUG=ON \
  ../../targets/wasm
make

echo "js: "`ls -l nes.js | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l nes.wasm | awk '{print $5}'`" bytes"