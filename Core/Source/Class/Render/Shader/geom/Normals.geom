// Geometry Shader That Draws a Visualized Normal Vector
#version 450 core

// Layout
layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

// Data From Vertex Shader
in VS_OUT
{
	vec4 Normal;
	mat4 Model;
	mat4 View;
	mat4 Project;
} gs_in[];

void main()
{
	// Calculate Starting Position of Vector
	float average_x = (gl_in[0].gl_Position.x + gl_in[2].gl_Position.x) / 2;
	float average_y = (gl_in[0].gl_Position.y + gl_in[2].gl_Position.y) / 2;
	vec4 Start = vec4(average_x, average_y, gl_in[1].gl_Position.z + 0.01, 1.0);

	// Calculate Offsets of Line
	vec4 End = Start + gs_in[1].Normal;
	float angle = 1.57f - atan(gs_in[1].Normal.y / gs_in[1].Normal.x);
	vec4 simplifiedVector = vec4(-cos(angle) / 4, sin(angle) / 4, 0.0, 0.0);
	mat4 matrix = gs_in[1].Project * gs_in[1].View * gs_in[1].Model;

	// Start 1 of Line
	gl_Position =  matrix * (Start - simplifiedVector);
	EmitVertex();

	// Start 2 of Line
	gl_Position = matrix * (Start + simplifiedVector);
	EmitVertex();

	// End 1 of Line
	gl_Position = matrix * (End - simplifiedVector);
	EmitVertex();

	// End 2 of Line
	gl_Position = matrix * (End + simplifiedVector);
	EmitVertex();

	// End Shape
	EndPrimitive();
}