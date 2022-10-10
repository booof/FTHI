// Geometry Shader for Static Texture Objects
#version 450 core

// Layout
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Data From Vertex Shader
in VS_OUT
{
	vec2 TexPos;
} gs_in[];

// Data To Fragment Shader
out FS_IN
{
	vec2 TexPos;
} gs_out;

void main()
{
	// Pass Vertex 1 Data to Fragment Shader
	gl_Position = gl_in[0].gl_Position;
	gs_out.TexPos = gs_in[0].TexPos;
	EmitVertex();

	// Pass Vertex 2 Data to Fragment Shader
	gl_Position = gl_in[1].gl_Position;
	gs_out.TexPos = gs_in[1].TexPos;
	EmitVertex();

	// Pass Vertex 3 Data to Fragment Shader
	gl_Position = gl_in[2].gl_Position;
	gs_out.TexPos = gs_in[2].TexPos;
	EmitVertex();

	// End Shape
	EndPrimitive();
}