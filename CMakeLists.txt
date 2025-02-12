cmake_minimum_required(VERSION 3.26)
project(calc)

set(CMAKE_CXX_STANDART 20)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src)
#add_subdirectory(test)
