cmake_minimum_required(VERSION 3.21)
project(proj)
set(CMAKE_CXX_STANDARD 14)

include_directories(src)

FILE(GLOB Vectors src/*Vector.cpp src/*Vector.h)
FILE(GLOB Matrices  src/*Matrix.cpp src/*Matrix.h)


ADD_LIBRARY(math_library ${Vectors} ${Matrices})

add_executable(proj src/main.cpp ${math_library})
