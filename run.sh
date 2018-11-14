#!/usr/bin/env bash

version=DEBUG
target_directory=example/static
target_js=${target_directory}/nes.js
target_wasm=${target_directory}/nes.wasm

mkdir -p build && cd build
source ~/emsdk/emsdk_env.sh
cmake -DCMAKE_TOOLCHAIN_FILE=~/emsdk/emscripten/1.38.16/cmake/Modules/Platform/Emscripten.cmake -DVERSION=${version} .. && make
cd ..

echo "js: "`ls -l ${target_js} | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l ${target_wasm} | awk '{print $5}'`" bytes"
cd example && node server.js