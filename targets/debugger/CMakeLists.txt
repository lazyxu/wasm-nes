cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes)
project(${PROJECT_NAME})
set(NES_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include_directories(/usr/local/include)
include_directories(${LWS_OPENSSL_INCLUDE_DIRS})
include_directories(cJSON)
include_directories(${NES_ROOT_DIR}/core/include)

add_subdirectory(cJSON)
add_subdirectory(${NES_ROOT_DIR} nes-lib)

file(GLOB NATIVE_SOURCE_FILES "*.c")

get_property(NES_DEFINITIONS GLOBAL PROPERTY NES_DEFINITIONS)
message(STATUS "NES_DEFINITIONS           " ${NES_DEFINITIONS})

add_definitions(${NES_DEFINITIONS})

add_executable(${PROJECT_NAME} ${NATIVE_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${LWS_OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libwebsockets.dylib)
target_link_libraries(${PROJECT_NAME} nes-core)
target_link_libraries(${PROJECT_NAME} cjson)