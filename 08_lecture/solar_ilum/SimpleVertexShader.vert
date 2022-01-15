#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Output data; will be interpolated for each fragment.
out float vRadius,vSlices,vStacks;

void main(){

	vRadius = vertexPosition_modelspace.x;
	vSlices = vertexPosition_modelspace.y;
	vStacks = vertexPosition_modelspace.z;

	gl_Position = vertexPosition_modelspace;

}

