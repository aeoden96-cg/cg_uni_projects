# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/02_SimpleAnimIdle.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/02_SimpleAnimIdle.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/02_SimpleAnimIdle.dir/flags.make

CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o: CMakeFiles/02_SimpleAnimIdle.dir/flags.make
CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o -c /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/main.cpp

CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/main.cpp > CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.i

CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/main.cpp -o CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.s

CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o: CMakeFiles/02_SimpleAnimIdle.dir/flags.make
CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o: ../src/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o -c /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/util.cpp

CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/util.cpp > CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.i

CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/src/util.cpp -o CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.s

# Object files for target 02_SimpleAnimIdle
02_SimpleAnimIdle_OBJECTS = \
"CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o" \
"CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o"

# External object files for target 02_SimpleAnimIdle
02_SimpleAnimIdle_EXTERNAL_OBJECTS =

02_SimpleAnimIdle: CMakeFiles/02_SimpleAnimIdle.dir/src/main.cpp.o
02_SimpleAnimIdle: CMakeFiles/02_SimpleAnimIdle.dir/src/util.cpp.o
02_SimpleAnimIdle: CMakeFiles/02_SimpleAnimIdle.dir/build.make
02_SimpleAnimIdle: /usr/lib/libOpenGL.so
02_SimpleAnimIdle: /usr/lib/libGLX.so
02_SimpleAnimIdle: /usr/lib/libGLU.so
02_SimpleAnimIdle: /usr/lib/libglut.so
02_SimpleAnimIdle: /usr/lib/libXmu.so
02_SimpleAnimIdle: /usr/lib/libXi.so
02_SimpleAnimIdle: /usr/lib/libGLEW.so
02_SimpleAnimIdle: CMakeFiles/02_SimpleAnimIdle.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable 02_SimpleAnimIdle"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/02_SimpleAnimIdle.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/02_SimpleAnimIdle.dir/build: 02_SimpleAnimIdle
.PHONY : CMakeFiles/02_SimpleAnimIdle.dir/build

CMakeFiles/02_SimpleAnimIdle.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/02_SimpleAnimIdle.dir/cmake_clean.cmake
.PHONY : CMakeFiles/02_SimpleAnimIdle.dir/clean

CMakeFiles/02_SimpleAnimIdle.dir/depend:
	cd /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug /home/mateo/Desktop/projects/cg/01_lecture/01_SimpleAnim/cmake-build-debug/CMakeFiles/02_SimpleAnimIdle.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/02_SimpleAnimIdle.dir/depend

