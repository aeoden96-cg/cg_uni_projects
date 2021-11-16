#version 330 core

// interpolirana vrijednost vrijednosti koja je poslana iz vertex shadera
in vec4 fragmentColor;

// Ouput data
out vec4 color;

void main()
{

	color = fragmentColor;

}
