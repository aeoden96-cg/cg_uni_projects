cmake_minimum_required(VERSION 3.21)
project(02_SimpleAnimIdle)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

include_directories(src)
add_executable(02_SimpleAnimIdle
        src/main.cpp
        src/main.hpp
        src/util.cpp
        src/util.hpp)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(02_SimpleAnimIdle ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} -lpthread -lm)

configure_file(src/SimpleFragmentShader.fragmentshader SimpleFragmentShader.fragmentshader COPYONLY)
configure_file(src/SimpleVertexShader.vertexshader SimpleVertexShader.vertexshader COPYONLY)
