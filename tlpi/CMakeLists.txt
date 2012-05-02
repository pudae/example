cmake_minimum_required(VERSION 2.8)

project(tlpi_example)

add_definitions(-g -Wall --std=c++0x)

include_directories(
	/usr/local/include
)

add_subdirectory(ch5)
add_subdirectory(ch6)
add_subdirectory(ch7)
add_subdirectory(ch10)
