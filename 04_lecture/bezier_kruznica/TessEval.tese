#version 430 core

layout (isolines, equal_spacing) in;

uniform mat4 PP;

void main(void)
{
    vec4 P[3];
    float t=gl_TessCoord.x;
    int i,j;
    
    for (i = 0; i < 3; i++)
    {
        P[i] = gl_in[i].gl_Position;
    }
    
    for(j=1; j<3; ++j)
		for(i=2; i>=j; --i)
			P[i]=mix(P[i-1],P[i],t);
    gl_Position = PP*P[2];
}
