cmake_minimum_required(VERSION 3.20)
project(miaosha C)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -g -ggdb -std=c99 -O2 -Wno-strict-aliasing -Wno-typedef-redefinition -Wno-sign-compare -Wno-unused-parameter")

SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_subdirectory(src)
