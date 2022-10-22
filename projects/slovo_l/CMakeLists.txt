cmake_minimum_required(VERSION 3.21)
project(sol_slovo_l)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(../../utils/shader_lib ${CMAKE_CURRENT_BINARY_DIR}/shader_lib)
add_subdirectory(../../utils/render_lib ${CMAKE_CURRENT_BINARY_DIR}/render_lib)

set(SHADER_LIST
    "SimpleFragmentShader.frag" 
    "SimpleVertexShader.vert" 
)

foreach(shader ${SHADER_LIST}) 
   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(files_for_compile main.cpp)
ADD_EXECUTABLE(sol_slovo_l ${files_for_compile})
target_link_libraries(sol_slovo_l SHADER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)