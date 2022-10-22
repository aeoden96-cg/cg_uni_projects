#version 420 core

flat in vec4 PatchColor;
out vec4 FragColor;

void main(void)
{
	FragColor = PatchColor;
}
