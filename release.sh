#!/usr/bin/env bash

target_js=example/static/nes.js
target_wasm=example/static/nes.wasm
# emcc ./src/*.c -O1 -s WASM=1 -s SIDE_MODULE=1 -s DYNAMIC_EXECUTION=1 -o example/static/nes_module.wasm
emcc ./src/*.c -Os -s WASM=1 -o ${target_js} -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]'
echo "js: "`ls -l ${target_js} | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l ${target_wasm} | awk '{print $5}'`" bytes"
cd example && node server.js