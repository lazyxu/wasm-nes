#!/usr/bin/env bash

sh targets/wasm/build.sh

BUILD_DIR=build/wasm
EXAMPLE_DIR=targets/wasm/example

cp ${BUILD_DIR}/nes.js ${EXAMPLE_DIR}/static
cp ${BUILD_DIR}/nes.wasm ${EXAMPLE_DIR}/static

cd ${EXAMPLE_DIR}
npm i
node server.js