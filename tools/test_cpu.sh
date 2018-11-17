#!/usr/bin/env bash

LOG_LEVEL=OFF
BUILD_DIR=build/cpu_test
OUTPUT_DIR=${BUILD_DIR}/bin

mkdir -p ${BUILD_DIR} && mkdir -p ${OUTPUT_DIR} && cd ${BUILD_DIR}

cmake -DLOG_LEVEL=${LOG_LEVEL} -DOUTPUT_DIR=${OUTPUT_DIR} -DTEST=CPU ../..
if [ $? -ne 0 ]; then
    exit -1
fi

make
if [ $? -ne 0 ]; then
    exit -1
fi

cd ../..

./${BUILD_DIR}/bin/nes
