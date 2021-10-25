cmake_minimum_required(VERSION 3.21)

project(${project_name})
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

FILE(GLOB MyCSources src/*.cpp)
FILE(GLOB MyHSources src/*.hpp)
ADD_EXECUTABLE(${project_name} ${MyCSources} ${MyHSources})

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} -lpthread -lm)

file(GLOB MY_SHADERS "src/*.tesc" "src/*.tese" "src/*.vert" "src/*.frag" "src/*.geom")
file(COPY ${MY_SHADERS} DESTINATION .)
