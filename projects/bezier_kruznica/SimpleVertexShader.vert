#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// Output data; will be interpolated for each fragment.
out vec3 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

    gl_Position = MVP * vertexPosition_modelspace;
    
    fragmentColor = vertexColor;
    
}

