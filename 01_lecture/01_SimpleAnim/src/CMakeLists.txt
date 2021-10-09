# add_library(Sqrt SHARED sqrt.cpp ../include/sqrt.h)
# add_library(Sqrt STATIC sqrt.cpp ../include/sqrt.h)

#kreiraj lib
add_library(Util util.cpp util.hpp)

find_library(GLEW glew32.lib ../../../ext/glew-2.1.0/lib/Release/x64)

#kreiraj program
add_executable(ex3prog main.cpp main.hpp)

#linkanje s novo izradenom lib
target_link_libraries(ex3prog Util)
target_link_libraries(ex3prog GLEW)


set_target_properties(ex3prog
					PROPERTIES
					CXX_STANDARD 17
					CXX_EXTENSIONS ON
					CXX_STANDARD_REQUIRED ON
					)
