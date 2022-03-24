cmake_minimum_required(VERSION 3.21)
project(sol_konika)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(../../opengl_libs ${CMAKE_CURRENT_BINARY_DIR}/opengl_libs)
set(SHADER_LIST 
    "SimpleFragmentShader.frag" 
    "SimpleVertexShader.vert" 
    "SimpleVertexShader_t.vert" 
    "TessCont.tesc" 
    "TessEval.tese" 
)

foreach(shader ${SHADER_LIST}) 
   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(files_for_compile main.cpp)
ADD_EXECUTABLE(sol_konika ${files_for_compile})
target_link_libraries(sol_konika SHADER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)