#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Output data; will be interpolated for each fragment.
out vec4 fragmentColor;

// Values that stay constant for all vertices.
uniform mat4 MVP;
uniform vec4 clr;

void main(){

    gl_Position = MVP * vertexPosition_modelspace; 



    // U vragmentColor samo zapisi primljeni vertexColor; interpoliranu vrijednost te varijable cemo
    // potom primiti u fragmentshaderu pod istim imenom (tamo je moramo tako deklarirati)
    fragmentColor = clr;
}

