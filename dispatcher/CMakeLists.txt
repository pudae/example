cmake_minimum_required(VERSION 2.8)

project(dispatcher)

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

add_executable(test_dispatcher
  ${PRJ_ROOT}/test/type_table.t.cpp
  ${PRJ_ROOT}/test/dispatcher.t.cpp
  ${PRJ_ROOT}/test/serialize.t.cpp
)

target_link_libraries(test_dispatcher
  boost_unit_test_framework
)
