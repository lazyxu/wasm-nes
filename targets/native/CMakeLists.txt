cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes)
project(${PROJECT_NAME})

set(NES_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories(${NES_ROOT_DIR}/core/include)

add_subdirectory(${NES_ROOT_DIR} nes-lib)

file(GLOB NATIVE_SOURCE_FILES "*.c")

add_executable(${PROJECT_NAME} ${NATIVE_SOURCE_FILES})
