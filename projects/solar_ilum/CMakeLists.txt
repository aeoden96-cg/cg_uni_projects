cmake_minimum_required(VERSION 3.21)
project(sol_solar_ilum)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(../../utils/shader_lib ${CMAKE_CURRENT_BINARY_DIR}/shader_lib)
add_subdirectory(../../utils/render_lib ${CMAKE_CURRENT_BINARY_DIR}/render_lib)

set(SHADER_LIST
    "FragmentShader.frag" 
    "SimpleVertexShader.vert" 
    "TessCont.tesc" 
    "TessEval.tese" 
)

foreach(shader ${SHADER_LIST}) 
   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

set(files_for_compile main.cpp main.hpp)
ADD_EXECUTABLE(sol_solar_ilum ${files_for_compile})
target_link_libraries(sol_solar_ilum SHADER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)