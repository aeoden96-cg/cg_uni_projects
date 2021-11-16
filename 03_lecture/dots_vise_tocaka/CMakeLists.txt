set(additional_temp_sources util_t.cpp util_t.hpp util.cpp util.hpp)
set(shader_location DEFAULT)
set(compiled_libs SHADER)

PROJECT_INIT(${additional_temp_sources} ${shader_location} ${compiled_libs})
