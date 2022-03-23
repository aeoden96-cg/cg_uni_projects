cmake_minimum_required(VERSION 3.21)
project(my_solution)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(../../opengl_libs ${CMAKE_CURRENT_BINARY_DIR}/opengl_libs)
set(SHADER_LIST 
    "FragmentShader.frag" 
    "FragmentShader2.frag" 
    "FragmentShader3.frag" 
    "GeometryShader.geom" 
    "GeometryShader2.geom" 
    "TessCont1.tesc" 
    "TessEval1.tese" 
    "TessEval2.tese" 
    "TessEval3.tese" 
    "VertexShader.vert" 
)

foreach(shader ${SHADER_LIST}) 
   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(files_for_compile main.cpp main.hpp)
ADD_EXECUTABLE(my_solution ${files_for_compile})
target_link_libraries(my_solution SHADER RENDERER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)