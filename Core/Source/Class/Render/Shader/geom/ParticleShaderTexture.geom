// Geometry Shader that Creates Instances of Particles
#version 450 core

// Layout
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Color Data
in VS_OUT
{
	vec2 Texture;
	vec2 Animation;
} gs_in[];

// Data To Fragment Shader
out FS_IN
{
	vec2 Texture;
	vec2 Animation;
} gs_out;

void main()
{
	// Pass Vertex 1 Data to Fragment Shader
	gl_Position = gl_in[0].gl_Position;
	gs_out.Texture = gs_in[0].Texture;
	gs_out.Animation = gs_in[0].Animation;
	EmitVertex();

	// Pass Vertex 2 Data to Fragment Shader
	gl_Position = gl_in[1].gl_Position;
	gs_out.Texture = gs_in[1].Texture;
	gs_out.Animation = gs_in[1].Animation;
	EmitVertex();

	// Pass Vertex 3 Data to Fragment Shader
	gl_Position = gl_in[2].gl_Position;
	gs_out.Texture = gs_in[2].Texture;
	gs_out.Animation = gs_in[2].Animation;
	EmitVertex();

	// End Shape
	EndPrimitive();
}
