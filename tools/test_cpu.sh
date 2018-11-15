#!/usr/bin/env bash


LOG_LEVEL=OFF
BUILD_DIR=build/cpu_test
OUTPUT_DIR=${BUILD_DIR}/bin

mkdir -p ${BUILD_DIR} && mkdir -p ${OUTPUT_DIR} && cd ${BUILD_DIR}
cmake -DLOG_LEVEL=${LOG_LEVEL} -DOUTPUT_DIR=${OUTPUT_DIR} -DTEST=CPU ../.. && make

./bin/nes
