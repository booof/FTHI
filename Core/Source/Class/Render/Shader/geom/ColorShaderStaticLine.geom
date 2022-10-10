// Geometry Shader for Static Color Objects
#version 450 core

// Layout
layout (lines) in;
layout (line_strip, max_vertices = 2) out;

// Data From Vertex Shader
in VS_OUT
{
	vec3 Color;
} gs_in[];

// Data To Fragment Shader
out FS_IN
{
	vec3 Color;
} gs_out;

void main()
{
	// Pass Vertex 1 Data to Fragment Shader
	gl_Position = gl_in[0].gl_Position;
	gs_out.Color = gs_in[0].Color;
	EmitVertex();

	// Pass Vertex 2 Data to Fragment Shader
	gl_Position = gl_in[1].gl_Position;
	gs_out.Color = gs_in[1].Color;
	EmitVertex();

	// End Shape
	EmitVertex();
}