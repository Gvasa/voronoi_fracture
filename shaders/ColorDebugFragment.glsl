#version 330 core

// Ouput data
out vec4 fragColor;

in vec4 outColor;


void main()
{

	// Output color = red 
	fragColor = outColor;

}