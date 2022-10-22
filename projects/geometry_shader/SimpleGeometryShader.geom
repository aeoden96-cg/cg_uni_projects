#version 430

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

out vec3 fragmentColor;

// Uniform matrices
uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 NORMAL;

// Uniform vector to store the color
uniform vec3 outColor;

// Uniform to store the length of the visualized normals
uniform float normal_length;

vec3 ab = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
vec3 ac = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
vec3 face_normal = normalize(vec3(NORMAL*vec4(cross(ab, ac),0.0)));

vec3 lightpos = vec3(-1.0, 1.0, 5.0);

void main(){

	int i;
	for (i=0; i<gl_in.length(); i++)
	{
		gl_Position = MVP * gl_in[i].gl_Position;
		float light = dot(normalize(lightpos-vec3(MV*gl_in[i].gl_Position)),face_normal);
		light = abs(light); 
		fragmentColor = outColor*light; 
		EmitVertex();
	}
	EndPrimitive();


}
