cmake_minimum_required(VERSION 2.8)

project(thrift_tool)

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

add_executable(type_id_assigner
  ${PRJ_ROOT}/type_id_assigner.cpp
)

target_link_libraries(type_id_assigner
  boost_system
  boost_filesystem
)

