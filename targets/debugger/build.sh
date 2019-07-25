#!/usr/bin/env bash

# mkdir -p build/debugger/libwebsockets
# cd build/debugger/libwebsockets

# cmake \
#   -DLWS_OPENSSL_INCLUDE_DIRS=/usr/local/Cellar/openssl/1.0.2s/include \
#   -DLWS_OPENSSL_LIBRARIES="/usr/local/Cellar/openssl/1.0.2s/lib/libssl.a;/usr/local/Cellar/openssl/1.0.2s/lib/libcrypto.a" \
#   ../../../targets/debugger/libwebsockets
# make

# cd ../../../
mkdir -p build/debugger
cd build/debugger
cmake \
  -DDEBUG=ON \
  -DLWS_OPENSSL_INCLUDE_DIRS=/usr/local/Cellar/openssl/1.0.2s/include \
  -DLWS_OPENSSL_LIBRARIES="/usr/local/Cellar/openssl/1.0.2s/lib/libssl.a;/usr/local/Cellar/openssl/1.0.2s/lib/libcrypto.a" \
  ../../targets/debugger
make
cd ../../
cp build/debugger/nes nes