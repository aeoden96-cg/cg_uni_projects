#ifndef UTIL_T_LOADER_HPP
#define UTIL_T_LOADER_HPP

#include <GL/glew.h>
#include <GL/gl.h>

bool linkShaders_t(GLuint &programID, GLuint vertexShaderID, GLuint fragmentShaderID);
GLuint loadShaders_t(const char * vertex_file_path, const char * fragment_file_path, const char * tess_cont_file_path, const char * tess_eval_file_path);

#endif

