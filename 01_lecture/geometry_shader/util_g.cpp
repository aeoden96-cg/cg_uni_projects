#include "util_g.hpp"
#include "util.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>
#include <stdlib.h>

bool linkShaders_g(GLuint &programID, GLuint vertexShaderID, GLuint fragmentShaderID, GLuint geometryShaderID) 
{

	// Link the program
	std::cout << "Linking program" << std::endl << std::flush;

	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glAttachShader(programID, geometryShaderID);
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
	glDetachShader(programID, geometryShaderID);

	return result==GL_TRUE ? true : false;

}


GLuint loadShaders_g(const char * geometry_file_path, GLuint vertexShaderID, GLuint fragmentShaderID) 
{

	std::cout << "Creating geometry shader... " << std::endl << std::flush;
	GLuint geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
	std::cout << "Going to read... " << std::endl << std::flush;
	char * geometryShaderCode = NULL;
	if(!readFile(&geometryShaderCode, geometry_file_path)) return 0;

	bool compiled;
	compiled = compileShader(geometry_file_path, geometryShaderID, geometryShaderCode);
	free(geometryShaderCode);
	
	GLuint programID;

	if(compiled) {
		compiled = linkShaders_g(programID, vertexShaderID, fragmentShaderID, geometryShaderID);
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(geometryShaderID);

	return !compiled ? 0 : programID;
}


