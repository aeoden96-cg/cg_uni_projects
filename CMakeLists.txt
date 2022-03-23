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


    target_link_libraries( ${project_name} SHADER RENDERER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)

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



add_subdirectory(opengl_libs)



SUBDIRLIST(SUBDIRS ${PROJECT_SOURCE_DIR})
FOREACH(lecture ${SUBDIRS})
    IF(NOT ${lecture} MATCHES "[0-9][0-9]_lecture" )
        continue()
    ENDIF()

    SUBDIRLIST(projects ${PROJECT_SOURCE_DIR}/${lecture})
    FOREACH(project ${projects})
        IF(
                ${project} MATCHES "build" OR
                ${project} MATCHES "opengl_libs")
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



