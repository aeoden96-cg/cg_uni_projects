#version 330

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in vec3 geomColor[3];
flat out vec3 fragmentColor;

void main(){

	int i;
	for (i=0; i<gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		fragmentColor = geomColor[i]; // svi fragmenti u primitivi ce biti boje geomColor[2];
		EmitVertex();
	}
	EndPrimitive();
}
