cmake_minimum_required(VERSION 3.21)
project(proj)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

include_directories(src)


FILE(GLOB MyCSources src/*.cpp)
FILE(GLOB MyHSources src/*.hpp)
ADD_EXECUTABLE(proj ${MyCSources} ${MyHSources})

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(proj ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} -lpthread -lm)


file(GLOB MY_SHADERS "src/*.*shader" "src/*.vert" "src/*.frag" "src/*.tese" "src/*.tesc" )
file(COPY  ${MY_SHADERS} DESTINATION .)


#add_executable(proj
#        src/torus.cpp
#        src/torus.hpp
#        src/util_t.cpp
#        src/util_t.hpp)
#configure_file(src/SimpleFragmentShader.frag SimpleFragmentShader.frag COPYONLY)
#configure_file(src/SimpleVertexShader.vert SimpleVertexShader.vert COPYONLY)
#configure_file(src/TessCont.tesc TessCont.tesc COPYONLY)
#configure_file(src/TessEval.tese TessEval.tese COPYONLY)