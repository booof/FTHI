// Fragment Shader For Drawing Objects of a Static Color
#version 450 core

// Static Color Value
uniform vec4 color;

// Fragment Color
out vec4 FragColor;

void main()
{
	// Set Fragment color
	FragColor = color;
}