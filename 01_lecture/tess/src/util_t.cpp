#include "util_t.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>
#include <stdlib.h>

bool readFile(char **lines, const char* filename) 
{

  FILE * f = fopen(filename, "rb");
  if(f==NULL) return false;
  fseek(f, 0, SEEK_END);
  int pos = ftell(f);
  fseek(f, 0, SEEK_SET);

  char* buf = (char*)malloc(pos+1);
  if(buf==NULL) {
    fclose(f);
    return false;
  }

  bool success = pos>0 ? 1 == fread(buf, pos, 1, f) : true;
  buf[pos] = '\0';

  fclose(f);

  if(!success) {
    free(buf);
    return false;
  }

  *lines = buf;

  return true;
}

bool compileShader(const char *path, GLuint shaderID, char *source_code) {
	std::cout << "Compiling shader : " << path << std::endl << std::flush;
	char const * src = source_code;
	glShaderSource(shaderID, 1, &src, NULL);
	glCompileShader(shaderID);

	// Check Shader Compilation Status
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> shaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);
		std::cout << (char*)(&shaderErrorMessage[0]) << std::endl;
	}

	return result==GL_TRUE ? true : false;
}

bool linkShaders(GLuint &programID, GLuint vertexShaderID, GLuint fragmentShaderID, GLuint tesscontShaderID, GLuint tessevalShaderID) 
{

	// Link the program
	std::cout << "Linking program" << std::endl << std::flush;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glAttachShader(programID, tesscontShaderID);
	glAttachShader(programID, tessevalShaderID);
	glLinkProgram(programID);

	// Check the program
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> programErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
		std::cout << (char*)(&programErrorMessage[0]) << std::endl;
	}

	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDetachShader(programID, tesscontShaderID);
	glDetachShader(programID, tessevalShaderID);

	return result==GL_TRUE ? true : false;

}


GLuint loadShaders(const char * vertex_file_path, const char * fragment_file_path, const char * tess_cont_file_path, const char * tess_eval_file_path) 
{

	std::cout << "Creating vertex shader... " << std::endl << std::flush;
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "Going to read... " << std::endl << std::flush;
	char * vertexShaderCode = NULL;
	if(!readFile(&vertexShaderCode, vertex_file_path)) return 0;

	std::cout << "Creating fragment shader... " << std::endl << std::flush;
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout << "Going to read... " << std::endl << std::flush;
	char * fragmentShaderCode = NULL;
	if(!readFile(&fragmentShaderCode, fragment_file_path)) {
		free(vertexShaderCode);
		return 0;
	}
	
	std::cout << "Creating tessellation control shader... " << std::endl << std::flush;
	GLuint tesscontShaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
	std::cout << "Going to read... " << std::endl << std::flush;
	char * tesscontShaderCode = NULL;
	if(!readFile(&tesscontShaderCode, tess_cont_file_path)) {
		free(vertexShaderCode);
		free(fragmentShaderCode);
		return 0;
	}
	
	std::cout << "Creating evaluation control shader... " << std::endl << std::flush;
	GLuint tessevalShaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
	std::cout << "Going to read... " << std::endl << std::flush;
	char * tessevalShaderCode = NULL;
	if(!readFile(&tessevalShaderCode, tess_eval_file_path)) {
		free(vertexShaderCode);
		free(fragmentShaderCode);
		free(tesscontShaderCode);
		return 0;
	}

	bool compiled;
	compiled = compileShader(vertex_file_path, vertexShaderID, vertexShaderCode);
	if (compiled) {
		std::cout << "Uspjelo kompajliranje!!!" << std::endl << std::flush;
	};
	compiled = compileShader(fragment_file_path, fragmentShaderID, fragmentShaderCode) && compiled;
	if (compiled) {
		std::cout << "Uspjelo kompajliranje!!!" << std::endl << std::flush;
	};
	compiled = compileShader(tess_cont_file_path, tesscontShaderID, tesscontShaderCode) && compiled;
	if (compiled) {
		std::cout << "Uspjelo kompajliranje!!!" << std::endl << std::flush;
	};
	compiled = compileShader(tess_eval_file_path, tessevalShaderID, tessevalShaderCode) && compiled;
	
	if (compiled) {
		std::cout << "Uspjelo kompajliranje!!!" << std::endl << std::flush;
	};

	free(vertexShaderCode);
	free(fragmentShaderCode);
	free(tesscontShaderCode);
	free(tessevalShaderCode);

	GLuint programID;

	if(compiled) {
		compiled = linkShaders(programID, vertexShaderID, fragmentShaderID, tesscontShaderID, tessevalShaderID);
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return !compiled ? 0 : programID;
}


