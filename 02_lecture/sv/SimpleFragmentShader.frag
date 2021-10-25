#version 330 core

// interpolirana vrijednost vrijednosti koja je poslana iz vertex shadera

uniform vec3 clr;

// Ouput data
out vec3 color;

void main()
{

	color = clr;

}
 
