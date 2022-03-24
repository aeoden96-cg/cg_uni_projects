cmake_minimum_required(VERSION 3.21)
project(sol_cube_zadatak)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(../../opengl_libs ${CMAKE_CURRENT_BINARY_DIR}/opengl_libs)
set(SHADER_LIST 
    "SimpleFragmentShader.frag" 
    "SimpleVertexShader.vert" 
    "TessCont.tesc" 
    "TessEval.tese" 
)

foreach(shader ${SHADER_LIST}) 
   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(files_for_compile main.cpp main.hpp)
ADD_EXECUTABLE(sol_cube_zadatak ${files_for_compile})
target_link_libraries(sol_cube_zadatak SHADER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)