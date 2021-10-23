#version 430 core

uniform float  uScale;
uniform mat4 MVP;

layout( quads, equal_spacing, ccw)  in;

patch in float tcuRadius,tcvRadius,tcuDetail,tcvDetail;


const float PI = 3.14159265;

void main( )
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;
	
	float phi = 2. * PI * u;
	float theta = 2. * PI * v;
	
	float costheta = cos(theta);
	vec3 xyz = vec3(
		cos(phi)*(tcvRadius*costheta+tcuRadius),
		sin(phi)*(tcvRadius*costheta+tcuRadius),
		tcvRadius*sin(theta) );
	
	gl_Position = MVP * vec4( xyz,1. );
}
