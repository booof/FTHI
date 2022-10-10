// Vertex Shder for Creating Colored Objects Affected by Light
#version 450 core

// Vertex Data
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec4 colors;
layout (location = 3) in vec2 tex_pos;

// Matrix
uniform mat4 matrix;
uniform mat4 model;

layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Data to be Processed
out VS_OUT
{
	vec4 FragPos;
	vec2 TexPos;
	flat vec4 Color;
	flat vec4 Normal_Static;
	flat vec4 Normal;
} vs_out;

void main()
{
	// Get Vertex Position
	mat4 projection = project * view;
	gl_Position = projection * model * vec4(position, 1.0);
	vs_out.FragPos = model * vec4(position, 1.0);

	// Get Texture Coords
	vs_out.TexPos = tex_pos;

	// Get Normals
	vs_out.Normal_Static = vec4(normals, 1.0);
	vs_out.Normal = vec4(normalize(mat3(transpose(inverse(model))) * normals), 0.0);

	// Get Colors
	vs_out.Color = colors;
}