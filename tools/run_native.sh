#!/usr/bin/env bash


LOG_LEVEL=TRACE
BUILD_DIR=build/native
OUTPUT_DIR=${BUILD_DIR}/bin

mkdir -p ${BUILD_DIR} && mkdir -p ${OUTPUT_DIR} && cd ${BUILD_DIR}
cmake -DLOG_LEVEL=${LOG_LEVEL} -DOUTPUT_DIR=${OUTPUT_DIR} ../..
if [ $? -ne 0 ]; then
    exit -1
fi

make
if [ $? -ne 0 ]; then
    exit -1
fi

./bin/nes
