cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes)
project(${PROJECT_NAME} C)

set(NES_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories(${NES_ROOT_DIR}/core/include ${NES_ROOT_DIR}/core/mapper/include)

add_subdirectory(${NES_ROOT_DIR} nes-lib)

file(GLOB SOURCE_CORE ${CMAKE_CURRENT_SOURCE_DIR}/*.c)

add_executable(${PROJECT_NAME} ${SOURCE_CORE})
target_link_libraries(${PROJECT_NAME} nes-core)