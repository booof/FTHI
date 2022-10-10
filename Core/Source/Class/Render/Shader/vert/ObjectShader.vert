// Vertex Shder for Creating Colored Objects Affected by Light
#version 450 core

// Eplaination for the Strange Lighting Glitch
// There is a Lighting Glitch Where There is a Dark Circle In the Direction the Camera is Moving and Circle of the Expected Color with Higher Intensity in the Opposite Direction
// This Glitch Occours When The Camera Moves, Resulting in a Different View Matrix
// If a Matrix that exist in both gl_Position and vs_out.FragPos is Changed, There is no Visaul Glitch
// If a Matrix Only Exists in One of These Equations and is Changed, Such as a Changing View Matrix in gl_Position, There Will be a Visual Glitch
// The Problem is Estimated to be Caused by the Rasterizer being Absolutly Fucking Retarded and Using Data From Previous Frames to Shade the FragPos Variable
// This Error Seems to have Varying Levels of Intensity on Differing Hardware, as Both the Actual Program and Videos of the Program Show Different Results on Different Hardware
// Why is this so fucking retarded

// Vertex Data
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_pos;
layout (location = 2) in float instance;

// Instance Structure
struct Instance
{
	// Model Matrix 
	mat4 model;

	// Colors
	vec4 colors;

	// Normals
	vec3 normals;

	// Texture Index
	int texture_index;

	// Mapping Index
	int mapping_index;

	// Z Position
	float zpos;
};

layout (std430, binding = 1) buffer Instances
{
	Instance instances[];
};

// Several Core Matrices
layout (std140) uniform Matrices
{
	mat4 view;
	mat4 project;
	mat4 projectStatic;
};

// Matrix
uniform mat4 matrix;
//uniform mat4 view;
out mat4 view_mat;

// Data to be Processed
out VS_OUT
{
	vec4 FragPos;
	vec2 TexPos;
	flat vec4 Color;
	flat vec4 Normal_Static;
	flat vec4 Normal;
	flat int texture_index;
	flat int mapping_index;
} vs_out;

void main()
{
	// Get instance
	Instance current_instance = instances[int(instance)];

	// Get Vertex Position
	vs_out.FragPos = current_instance.model * vec4(position, current_instance.zpos, 1.0);
	gl_Position = project * view * current_instance.model * vec4(position, current_instance.zpos, 1.0);

	// Get Texture Coords
	vs_out.TexPos = tex_pos;

	// Get Normals
	vs_out.Normal_Static = vec4(current_instance.normals, 1.0);
	vs_out.Normal = vec4(normalize(mat3(transpose(inverse(current_instance.model))) * current_instance.normals), 0.0);

	// Get Colors
	vs_out.Color = current_instance.colors;

	// Get Texture Indicies
	vs_out.texture_index = current_instance.texture_index;
	vs_out.mapping_index = current_instance.mapping_index;
}