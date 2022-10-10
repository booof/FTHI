// Geometry Shader for Textured Objects Affected by Light
#version 450 core

// Layout
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Data From Vertex Shader
in VS_OUT
{
	vec4 FragPos;
	vec2 TexPos;
	vec4 Normal_Static;
	vec4 Normal;
} gs_in[];

// Data To Fragment Shader
out FS_IN
{
	vec4 FragPos;
	vec2 TexPos;
	vec4 Normal_Static;
	vec4 Normal;
} gs_out;

void main()
{
	// Pass Vertex 1 Data to Fragment Shader
	gl_Position = gl_in[0].gl_Position;
	gs_out.FragPos = gs_in[0].FragPos;
	gs_out.TexPos = gs_in[0].TexPos;
	gs_out.Normal_Static = gs_in[0].Normal_Static;
	gs_out.Normal = gs_in[0].Normal;
	EmitVertex();

	// Pass Vertex 2 Data to Fragment Shader
	gl_Position = gl_in[1].gl_Position;
	gs_out.FragPos = gs_in[1].FragPos;
	gs_out.TexPos = gs_in[1].TexPos;
	gs_out.Normal_Static = gs_in[1].Normal_Static;
	gs_out.Normal = gs_in[1].Normal;
	EmitVertex();

	// Pass Vertex 3 Data to Fragment Shader
	gl_Position = gl_in[2].gl_Position;
	gs_out.FragPos = gs_in[2].FragPos;
	gs_out.TexPos = gs_in[2].TexPos;
	gs_out.Normal_Static = gs_in[2].Normal_Static;
	gs_out.Normal = gs_in[2].Normal;
	EmitVertex();

	// End Shape
	EndPrimitive();
}