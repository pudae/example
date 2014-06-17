cmake_minimum_required(VERSION 2.8)

set(AUTH_DEFAULT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")

project(railway)

add_definitions(
  -Wall
  -Wextra
  -Werror
  -g
  -Wno-deprecated
  -std=c++0x
)

add_executable(example1
  example1.cpp
)

