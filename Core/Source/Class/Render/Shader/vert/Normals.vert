// Vertex Shader That Draws a Visualized Normal Vector
#version 450 core

// Only Take Position and Normal From Layout
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

// Several Core Vertices
layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Model Matrix
uniform mat4 model;

// Output Normal Vector
out VS_OUT
{
	vec4 Normal;
	mat4 Model;
	mat4 View;
	mat4 Project;
} vs_out;

void main()
{
	// Calculate New Normal
	float magnitude = pow(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z, 0.5) * 5;
	vs_out.Normal = vec4(normalize(normal), 0.0) * magnitude;
	vs_out.Normal.z = 0.0;
	vs_out.Model = model;
	vs_out.View = view;
	vs_out.Project = project;

	// Calculate Position
	gl_Position = vec4(position, 1.0);
}