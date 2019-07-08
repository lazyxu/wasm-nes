#!/usr/bin/env bash

mkdir -p build/native
cd build/native
cmake ../../targets/native
make