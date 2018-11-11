#!/usr/bin/env bash

target_js=example/static/nes.js
target_wasm=example/static/nes.wasm
emcc ./src/*.c -Os -s WASM=1 -o ${target_js} -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=2 -Werror -DDEBUG
echo "js: "`ls -l ${target_js} | awk '{print $5}'`" bytes"
echo "wasm: "`ls -l ${target_wasm} | awk '{print $5}'`" bytes"
cd example && node server.js