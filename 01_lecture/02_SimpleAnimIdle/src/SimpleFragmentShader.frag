#version 330 core

// interpolirana vrijednost vrijednosti koja je poslana iz vertex shadera
in vec3 fragment_color;

// Ouput data
out vec3 color;

void main()
{

	color = fragment_color;

}
