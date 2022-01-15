#version 430 core

layout (isolines, equal_spacing) in;

uniform mat4 MVP;

void main(void)
{
    vec4 H[3],H1[2],H2;
    float t=gl_TessCoord.x;
    int i;
    
    for (i = 0; i < 3; i++)
    {
        H[i] = gl_in[i].gl_Position;
    }
    
	for(i=0; i<=1; i++)
		H1[i]=mix(H[i],H[i+1],t);
	H2=mix(H1[0],H1[1],t);
	gl_Position = MVP*H2;
}
