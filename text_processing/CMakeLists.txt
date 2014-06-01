cmake_minimum_required(VERSION 2.8)

project(text_processing)

set(PRJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
  ${PRJ_ROOT}
)

add_definitions(
  -Wall
  -Wextra
  -Werror
  -g
  -Wno-deprecated
  -D_DEBUG
  -std=c++0x
  -DBOOST_TEST_DYN_LINK
)

add_executable(example1
  ${PRJ_ROOT}/example1.cpp
)

