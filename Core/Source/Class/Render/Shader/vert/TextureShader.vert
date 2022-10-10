// Vertex Shader for Creating Textured Objects Affected by Light
#version 450 core

// Vertex Data
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texPos;
layout (location = 2) in vec3 normal;

// Several Core Matrices
layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Model Matrix
uniform mat4 model;

// Data to be Processed
out VS_OUT
{
	vec4 FragPos;
	vec2 TexPos;
	vec4 Normal_Static;
	vec4 Normal;
} vs_out;

void main()
{
	// Texture Values
	vs_out.FragPos = model * vec4(position, 1.0);
	vs_out.TexPos = vec2(texPos.x, 1.0 - texPos.y);

	// Normal Values
	vs_out.Normal_Static = vec4(normal, 0.0);
	vs_out.Normal = vec4(normalize(mat3(transpose(inverse(model))) * normal), 0);

	// Position Values
	gl_Position = project * view * vs_out.FragPos;
}
