#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 ulaz_position;
layout(location = 1) in vec3 ulaz_color;

// Output data; will be interpolated for each fragment.
out vec3 fragment_color;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){

    gl_Position = MVP * ulaz_position;

    // U fragmentColor samo zapisi primljeni vertexColor; interpoliranu vrijednost te varijable cemo
    // potom primiti u fragmentshaderu pod istim imenom (tamo je moramo tako deklarirati)
    fragment_color = ulaz_color;
}

