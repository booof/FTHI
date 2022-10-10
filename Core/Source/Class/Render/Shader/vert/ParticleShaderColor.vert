// Vertex Shader for Instancing Particles
#version 450 core

// Vertex Inputs
in layout (location = 0) vec3 position;
in layout (location = 1) vec3 color;

// Matrix Block
layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Offset of Particles in Position Data
uniform int Particles_Offset;

// Uniform Buffer for Particle Positions
layout (std140, binding = 4) buffer Particles
{
	vec4 particles[];
};

uniform mat4 model;
uniform int Static;

out VS_OUT
{
	vec3 Color;
} vs_out;

void main()
{
	// Determine Projection Matrix to be Used
	mat4 projection = ((Static - 1) * -1 * project * view) + (projectStatic * Static);

	// Compute Particle Offset
	vec4 new_position = vec4(position.xy + particles[Particles_Offset + gl_InstanceID].xy, position.z, 1.0);

	// Calcultate Position
	gl_Position = project * view * model * new_position;

	vs_out.Color = color;
}

