import os
import re


directory = os.fsencode(".")

for file in os.listdir(directory):
    lecture_name = os.fsdecode(file)
    if os.path.isdir(lecture_name) and re.search("^[0-9][0-9]_lecture$", lecture_name):
        print(lecture_name)

        for file2 in os.listdir("./" + lecture_name):
            example_name = os.fsdecode(file2)
            print("    ", example_name)

            list_of_shaders = []
            hpp_exists = False
            for file3 in os.listdir(f"./{lecture_name}/{example_name}"):
                fil = os.fsdecode(file3)
                print("        ", fil)
                if fil.endswith(".frag") or fil.endswith(".vert") or fil.endswith(".tese") or \
                        fil.endswith(".tesc") or fil.endswith(".geom"):
                    list_of_shaders.append(fil)
                if fil == "main.hpp" or fil == "main.h":
                    hpp_exists = fil
            try:
                os.remove(f'{lecture_name}/{example_name}/CMakeLists2.txt')
            except:
                pass

            with open(f'{lecture_name}/{example_name}/CMakeLists.txt', 'w') as f:
                f.write('cmake_minimum_required(VERSION 3.21)\n')
                f.write('project(sol_'+example_name+')\n')
                f.write('set(CMAKE_CXX_STANDARD 17)\n')

                f.write('add_subdirectory(../../opengl_libs ${CMAKE_CURRENT_BINARY_DIR}/opengl_libs)\n')

                f.write('set(SHADER_LIST \n')
                for shader in list_of_shaders:
                    f.write(f'    "{shader}" \n')
                f.write(f')\n\n')
                f.write('foreach(shader ${SHADER_LIST}) \n')
                f.write('   configure_file(${shader} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)\n')
                f.write('endforeach()\n\n')

                if hpp_exists:
                    f.write(f'set(files_for_compile main.cpp {hpp_exists})\n')
                else:
                    f.write('set(files_for_compile main.cpp)\n')
                f.write('ADD_EXECUTABLE(sol_' +example_name +' ${files_for_compile})\n')
                f.write('target_link_libraries(sol_'+example_name+' SHADER  -lGLEW -lGL -lGLU -lglut -lpthread -lm)')

