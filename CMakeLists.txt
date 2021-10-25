cmake_minimum_required(VERSION 3.21)
project(my_solution)
set(CMAKE_CXX_STANDARD 14)


MACRO(PROJECT_INIT)

    string(REPLACE _lecture "" project_name ${lecture}_${project})
    project(${project_name})

    FILE(GLOB MyCSources *.cpp)
    FILE(GLOB MyHSources *.hpp)
    ADD_EXECUTABLE(${project_name} ${MyCSources} ${MyHSources})

    include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
    target_link_libraries(${project_name} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} -lpthread -lm)

    file(GLOB MY_SHADERS "*.tesc" "*.tese" "*.vert" "*.frag" "*.geom")

    file(COPY ${MY_SHADERS} DESTINATION .)
#    message(${CMAKE_CURRENT_BINARY_DIR})
#    foreach(shader ${MY_SHADERS})
#        configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
#    endforeach()

ENDMACRO()




MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR})


find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)

#project(util_default)
#add_library(util_default SHARED util/util.cpp util/util.hpp)
#project(util_geom)
#add_library(util_geom SHARED util/util_g.cpp util/util_g.hpp)
#project(util_tess)
#add_library(util_tess SHARED util/util_t.cpp util/util_t.hpp)

#set_target_properties(util_default PROPERTIES PUBLIC_HEADER util/util.hpp)
#set_target_properties(util_geom PROPERTIES PUBLIC_HEADER util/util_g.hpp)
#set_target_properties(util_tess PROPERTIES PUBLIC_HEADER util/util_t.hpp)

FOREACH(lecture ${SUBDIRS})
    IF(NOT ${lecture} MATCHES "[0-9][0-9]_lecture" )
        continue()
    ENDIF()
    SUBDIRLIST(projects ${PROJECT_SOURCE_DIR}/${lecture})
    FOREACH(project ${projects})
        IF(${project} MATCHES "build" )
            continue()
        ENDIF()

        add_subdirectory(
                "${PROJECT_SOURCE_DIR}/${lecture}/${project}"
                "${PROJECT_SOURCE_DIR}/build/${lecture}/${project}")

    ENDFOREACH()
ENDFOREACH()

