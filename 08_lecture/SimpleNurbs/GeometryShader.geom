#version 330

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;

in vec4 VertexColor[];
flat out vec4 PatchColor;

void main() {
int i;
for (i=0; i<gl_in.length(); i++)
{
	gl_Position = gl_in[i].gl_Position;
	PatchColor = VertexColor[i];
	EmitVertex();
}
EndPrimitive();
}
