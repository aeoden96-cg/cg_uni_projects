cmake_minimum_required(VERSION 3.21)
project(my_solution)
set(CMAKE_CXX_STANDARD 14)

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

FOREACH(lecture ${SUBDIRS})
    IF(NOT ${lecture} MATCHES "[0-9][0-9]_lecture" )
        continue()
    ENDIF()
    SUBDIRLIST(projects ${PROJECT_SOURCE_DIR}/${lecture})
    FOREACH(project ${projects})
        IF(${project} MATCHES "build" )
            continue()
        ENDIF()
        message(${lecture})
        message(${project})

        string(REPLACE _lecture "" project_name ${lecture}_${project})

        add_subdirectory(
                "${PROJECT_SOURCE_DIR}/${lecture}/${project}"
                "${PROJECT_SOURCE_DIR}/build/${lecture}/${project}")
    ENDFOREACH()
ENDFOREACH()

