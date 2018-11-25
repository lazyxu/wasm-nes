#!/usr/bin/env bash

LOG_LEVEL=TRACE
OUTPUT_DIR=targets/wasm/example/static

source ~/emsdk/emsdk_env.sh
mkdir -p build/wasm && cd build/wasm
cmake -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake\
    -DLOG_LEVEL=${LOG_LEVEL} \
    -DOUTPUT_DIR=${OUTPUT_DIR} ../..
if [ $? -ne 0 ]; then
    exit -1
fi

make
if [ $? -ne 0 ]; then
    exit -1
fi

cd ../..

target_js=${OUTPUT_DIR}/nes.js
target_wasm=${OUTPUT_DIR}/nes.wasm

echo "js: "`ls -l ${target_js} | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l ${target_wasm} | awk '{print $5}'`" bytes"