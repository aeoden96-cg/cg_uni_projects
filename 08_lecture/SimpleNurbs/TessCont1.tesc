#version 430 core

layout (vertices = 16) out;

uniform int U,V;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = U; 
        gl_TessLevelInner[1] = V; 
        gl_TessLevelOuter[0] = V; 
        gl_TessLevelOuter[1] = U; 
        gl_TessLevelOuter[2] = V; 
        gl_TessLevelOuter[3] = U; 
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
