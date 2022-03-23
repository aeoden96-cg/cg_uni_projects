#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;

// Output data; will be interpolated for each fragment.
out vec4 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec4 clr;

void main(){

	gl_Position = MVP * vertexPosition_modelspace;  //vec4(vertexPosition_modelspace, 0, 1);
	fragmentColor = clr;
}
