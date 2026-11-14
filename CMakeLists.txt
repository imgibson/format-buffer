cmake_minimum_required(VERSION 3.10)

project(format_buffer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSONS OFF)

add_library(format_buffer INTERFACE
  include/format_buffer/format_buffer.h
)
target_include_directories(format_buffer INTERFACE
  include
)
