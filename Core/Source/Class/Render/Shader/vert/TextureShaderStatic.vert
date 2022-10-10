// Vertex Shader for Creating Textured Objects not Affected by Light
#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texPos;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

uniform mat4 model;
uniform int Static;
uniform int direction;

out VS_OUT
{
	vec2 TexPos;
} vs_out;

void main()
{
	// Determine Projection Matrix to be Used
	mat4 projection = ((Static - 1) * -1 * project * view) + (projectStatic * Static);
	//mat4 projection = project * view;

	gl_Position = projection * model * vec4(position, 1.0);

	vs_out.TexPos = vec2(texPos.x, (1.0 - texPos.y) * direction);
	//vs_out.TexPos = vec2(texPos.x, 1.0 - texPos.y);
}