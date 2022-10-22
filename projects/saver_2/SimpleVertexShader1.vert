#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 MV;

void main(){

    gl_Position = MV * vertexPosition_modelspace;

}

