cmake_minimum_required(VERSION 3.21)
project(my_solution)
set(CMAKE_CXX_STANDARD 17)


MACRO(PROJECT_INIT)

    file(GLOB MY_SHADERS "*.tesc" "*.tese" "*.vert" "*.frag" "*.geom")
    foreach(shader ${MY_SHADERS})
        configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
    endforeach()


    set(files_for_compile main.cpp)
    if(EXISTS main.hpp)
        list(APPEND files_for_compile main.hpp)
    endif()

    ADD_EXECUTABLE(${project_name} ${files_for_compile})

    include_directories(
            ${OPENGL_INCLUDE_DIRS}
            ${GLUT_INCLUDE_DIRS}
            ${GLEW_INCLUDE_DIRS})


    target_link_libraries(
            ${project_name}
            ${OPENGL_LIBRARIES}
            SHADER
            ${GLUT_LIBRARY}
            ${GLEW_LIBRARIES} -lpthread -lm -lGL -lGLU -lglut)





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



find_package(OpenGL)
find_package(GLUT)
find_package(GLEW)


#add_library(UTIL_DEF    util/util.cpp   util/util.hpp)
#add_library(UTIL_GEOM   util/util_g.cpp util/util_g.hpp)
#add_library(UTIL_TESS   util/util_t.cpp util/util_t.hpp)
add_library(SHADER      util/Shader.cpp util/Shader.h)


#set_target_properties(UTIL_DEF  PROPERTIES PUBLIC_HEADER util/util.hpp)
#set_target_properties(UTIL_GEOM PROPERTIES PUBLIC_HEADER util/util_g.hpp)
#set_target_properties(UTIL_TESS PROPERTIES PUBLIC_HEADER util/util_t.hpp)
set_target_properties(SHADER    PROPERTIES PUBLIC_HEADER util/Shader.h)


#target_include_directories (UTIL_DEF    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories (UTIL_GEOM   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
#target_include_directories (UTIL_TESS   PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (SHADER      PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})



SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR})
FOREACH(lecture ${SUBDIRS})
    IF(NOT ${lecture} MATCHES "[0-9][0-9]_lecture" )
        continue()
    ENDIF()

    SUBDIRLIST(projects ${PROJECT_SOURCE_DIR}/${lecture})
    FOREACH(project ${projects})
        IF(
                ${project} MATCHES "build" OR
                ${project} MATCHES "util")
            continue()
        ENDIF()


        string(REPLACE _lecture "" project_name ${lecture}_${project})
        project(${project_name})

        add_subdirectory(
                "${PROJECT_SOURCE_DIR}/${lecture}/${project}"
                "${PROJECT_BINARY_DIR}/${lecture}/${project}"
        )




    ENDFOREACH()
ENDFOREACH()



