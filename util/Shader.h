//
// Created by mateo on 13. 11. 2021..
//

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <initializer_list>
#include <vector>
#include <string>
#include <iterator>
#include <set>



class Shader
{
public:
    unsigned int ID{};

    Shader();
    unsigned int load_shaders(std::initializer_list<std::string> l);


    // activate the shader
    // ------------------------------------------------------------------------
    void use(){glUseProgram(ID);}
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const{
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const{
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    static void checkCompileErrors(unsigned int shader, const std::string& type);

};


#endif //SHADER_H