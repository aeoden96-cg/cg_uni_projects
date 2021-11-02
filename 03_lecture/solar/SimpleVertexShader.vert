#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Output data for Tessellation shader.
out vec3 vCenter;
out float vRadius;

void main(){

    vCenter = vertexPosition_modelspace.xyz;
	vRadius = vertexPosition_modelspace.w;
    
    gl_Position = vertexPosition_modelspace; 

}

