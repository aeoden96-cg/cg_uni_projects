#version 330 core

// interpolirana vrijednost vrijednosti koja je poslana iz vertex shadera
in vec3 fragmentColor;

// Ouput data
out vec3 color;

void main()
{

	color = fragmentColor;

}
