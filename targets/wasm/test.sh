#!/usr/bin/env bash

bash ./tools/build_release.sh

EXAMPLE_DIR=targets/wasm/example
cd ${EXAMPLE_DIR} && node server.js