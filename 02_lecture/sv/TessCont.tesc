#version 430 core

in float vRadius[],vSlices[],vStacks[];

patch out float tcRadius;

layout (vertices = 1)  out;

void main( )
{
	gl_out[gl_InvocationID].gl_Position = gl_in[0].gl_Position;

	tcRadius = vRadius[0];
	
	gl_TessLevelOuter[0] = 2.;
	gl_TessLevelOuter[1] = vStacks[0];
	gl_TessLevelOuter[2] = 2.;
	gl_TessLevelOuter[3] = vStacks[0];
	gl_TessLevelInner[0]  = vStacks[0];
	gl_TessLevelInner[1]  = vSlices[0];
}
