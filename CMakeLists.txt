cmake_minimum_required(VERSION 3.7)

set(PROJECT_NAME nes)
project(${PROJECT_NAME} C)

if(NOT DEFINED LOG_LEVEL)
  if(DEFINED DEBUG AND "${DEBUG}" STREQUAL "ON")
    set(LOG_LEVEL 3)
  else()
    set(LOG_LEVEL 0)
  endif()
else()
  if("${LOG_LEVEL}" STREQUAL "TRACE")
    set(LOG_LEVEL 4)
  elseif("${LOG_LEVEL}" STREQUAL "DEBUG")
    set(LOG_LEVEL 3)
  elseif("${LOG_LEVEL}" STREQUAL "OFF")
    set(LOG_LEVEL 0)
  else()
    message(FATAL_ERROR "Invalid LOG_LEVEL ${LOG_LEVEL}, excepted TRACE, DEBUG or OFF!")
  endif()
endif()
set(NES_DEFINITIONS "${NES_DEFINITIONS} -DLOG_LEVEL=${LOG_LEVEL}")

if(DEFINED DEBUG AND "${DEBUG}" STREQUAL "ON")
  set(NES_DEFINITIONS "${NES_DEFINITIONS} -DDEBUG")
else()
  set(DEBUG "OFF")
endif()

message(STATUS "SYSTEM                    " ${CMAKE_SYSTEM_NAME})
message(STATUS "COMPILER                  " ${CMAKE_C_COMPILER})

message(STATUS "LOG_LEVEL                 " ${LOG_LEVEL})
message(STATUS "DEBUG                     " ${DEBUG})
message(STATUS "NES_DEFINITIONS           " ${NES_DEFINITIONS})

add_definitions(${NES_DEFINITIONS})
set_property(GLOBAL PROPERTY NES_DEFINITIONS ${NES_DEFINITIONS})

add_subdirectory(core)
