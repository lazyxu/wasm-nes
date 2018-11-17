#!/usr/bin/env bash

LOG_LEVEL=TRACE
EXAMPLE_DIR=targets/wasm/example
WASM_STATIC_DIR=${EXAMPLE_DIR}/static

source ~/emsdk/emsdk_env.sh
mkdir -p build/wasm && cd build/wasm
cmake -DCMAKE_TOOLCHAIN_FILE=~/emsdk/emscripten/1.38.16/cmake/Modules/Platform/Emscripten.cmake\
    -DLOG_LEVEL=${LOG_LEVEL} \
    -DWASM_STATIC_DIR=${WASM_STATIC_DIR} ../..
if [ $? -ne 0 ]; then
    exit -1
fi

make
if [ $? -ne 0 ]; then
    exit -1
fi

cd ../..

target_js=${WASM_STATIC_DIR}/nes.js
target_wasm=${WASM_STATIC_DIR}/nes.wasm

echo "js: "`ls -l ${target_js} | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l ${target_wasm} | awk '{print $5}'`" bytes"
cd ${EXAMPLE_DIR} && node server.js