cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes.wasm)
project(${PROJECT_NAME})

set(NES_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories(ENV{EMSCRIPTEN}/system/include)

include_directories(${NES_ROOT_DIR}/core/include)

add_subdirectory(${NES_ROOT_DIR} nes-lib)

file(GLOB TARGET_FILES "*.c")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s WASM=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ERROR_ON_UNDEFINED_SYMBOLS=0")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"cwrap\"]'")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s ASSERTIONS=2")
add_definitions(-DWASM)

add_executable(${PROJECT_NAME} ${TARGET_FILES})