cmake_minimum_required(VERSION 3.16)

add_library(SHADER
        Shader.h
        Shader.cpp)
set_target_properties(SHADER    PROPERTIES PUBLIC_HEADER Shader.h)
target_include_directories (SHADER PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
