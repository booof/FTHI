// Vertex Shader For Drawing Objects of a Static Color
#version 450 core

// Position Values
layout (location = 0) in vec2 position;

// Core Matrices
layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Other Uniforms
uniform mat4 model;

void main()
{
	// Set Vertices Position
	gl_Position = project * view * model * vec4(position.x, position.y, -2.0, 1.0);
}