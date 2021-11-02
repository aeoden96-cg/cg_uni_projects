#version 430 core

uniform mat4 PMatrix,MVMatrix,NMatrix;

layout( quads, equal_spacing, ccw)  in;

patch in float tcRadius;

out vec4 Position;
out vec3 Normal;

const float PI = 3.14159265;

void main( )
{
	vec3 p = gl_in[0].gl_Position.xyz;
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	
	float phi = PI * ( u - .5 );
	float theta = 2. * PI * ( v - .5 );
	
	float cosphi = cos(phi);
	Normal = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );	
	
	Position = MVMatrix * vec4(tcRadius*Normal, 1.0f);
	Normal = vec3(NMatrix * vec4(Normal, 1.0f));
	gl_Position = PMatrix * Position;
}
