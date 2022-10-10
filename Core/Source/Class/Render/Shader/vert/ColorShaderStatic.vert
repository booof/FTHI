// Vertex Shader for Creating Colored Objects not Affected by Light
#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

uniform mat4 model;
uniform int Static;

out VS_OUT
{
	vec4 Color;
} vs_out;

void main()
{
	// Determine Projection Matrix to be Used
	mat4 projection = ((Static - 1) * -1 * project * view) + (projectStatic * Static);

	gl_Position = projection * model * vec4(position, 1.0);

	vs_out.Color = color;
}