cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes)
project(${PROJECT_NAME})

include_directories(/usr/local/include)
include_directories(${LWS_OPENSSL_INCLUDE_DIRS})

file(GLOB NATIVE_SOURCE_FILES "*.c")

add_executable(${PROJECT_NAME} ${NATIVE_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${LWS_OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libwebsockets.dylib)