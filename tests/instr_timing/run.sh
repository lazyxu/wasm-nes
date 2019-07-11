#!/usr/bin/env bash

LOG_LEVEL=TRACE
PROJECT_NAME=instr_timing
BUILD_DIR=build/${PROJECT_NAME}

mkdir -p ${BUILD_DIR} && cd ${BUILD_DIR}

cmake -DLOG_LEVEL=${LOG_LEVEL} -DDEBUG=ON ../../tests/${PROJECT_NAME}
if [ $? -ne 0 ]; then
    exit -1
fi

make
if [ $? -ne 0 ]; then
    exit -1
fi

cd ../..

# echo "--- Run nestest ---"
./${BUILD_DIR}/nes

# expect_file=tests/nestest/nestest.no_opdata.log
# actual_file=build/nestest/nestest.log
# echo diff $expect_file $actual_file
# diff $expect_file $actual_file > /dev/null
# if [ $? == 0 ]; then
#     echo "Pass!"
# else
#     echo "Error!"
# fi