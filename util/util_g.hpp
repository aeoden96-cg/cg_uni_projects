#ifndef UTIL_G_LOADER_HPP
#define UTIL_G_LOADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>

bool linkShaders_g(GLuint &programID, GLuint fragmentShaderID, GLuint geometryShaderID);
GLuint loadShaders_g(const char * geometry_file_path, GLuint vertexShaderID, GLuint fragmentShaderID); 

#endif

