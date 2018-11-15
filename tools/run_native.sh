#!/usr/bin/env bash


LOG_LEVEL=OFF
BUILD_DIR=build/native
OUTPUT_DIR=${BUILD_DIR}/bin

mkdir -p ${BUILD_DIR} && mkdir -p ${OUTPUT_DIR} && cd ${BUILD_DIR}
cmake -DLOG_LEVEL=${LOG_LEVEL} -DOUTPUT_DIR=${OUTPUT_DIR} ../.. && make

./bin/nes
