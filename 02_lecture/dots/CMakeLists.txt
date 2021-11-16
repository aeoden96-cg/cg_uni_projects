set(shader_location GIVEN)

#will be removed when Shader class is finished
set(compiled_libs "")
set(additional_temp_sources util.cpp util.hpp)


PROJECT_INIT(${additional_temp_sources} ${shader_location} ${compiled_libs})
