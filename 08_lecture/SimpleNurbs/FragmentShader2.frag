#version 420 core

in vec4 VertexColor;
out vec4 FragColor;

void main(void)
{
	FragColor = VertexColor;
}
