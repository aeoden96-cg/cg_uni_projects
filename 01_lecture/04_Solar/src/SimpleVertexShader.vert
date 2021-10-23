#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertex_in;

// Output data for Tessellation shader.
out vec3 vCenter;
out float vRadius;

void main(){

    vCenter = vertex_in.xyz;
	vRadius = vertex_in.w;
    
    gl_Position = vertex_in;

}

