#version 430 core

uniform float  uScale;
uniform mat4 MVP;

layout( quads, equal_spacing, ccw)  in;

patch in float tcRadius;
patch in vec3 tcCenter;

const float PI = 3.14159265;

void main( )
{
	vec3 p = gl_in[0].gl_Position.xyz;
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	
	float phi = PI * ( u - .5 );
	float theta = 2. * PI * ( v - .5 );
	
	float cosphi = cos(phi);
	vec3 xyz = vec3( cosphi*cos(theta), sin(phi), cosphi*sin(theta) );
	
	xyz *= ( uScale * tcRadius );
	xyz += tcCenter;
	gl_Position = MVP * vec4( xyz,1. );
}
