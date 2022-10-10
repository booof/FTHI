// Geometry Shader for Drawing Objects of a Static Color
#version 450 core

// Vertex Data
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

void main()
{
	// Bottom Left Vertex
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
	EmitVertex();

	// Bottom Right Vertex
	gl_Position = gl_in[0].gl_Position + vec4(0.1, -0.1, 0.0, 0.0);
	EmitVertex();

	// Top Right Vertex
	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.1, 0.0, 0.0);
	EmitVertex();

	// Top Left Vertex
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.1, 0.0, 0.0);
	EmitVertex();

	// End Object
	EndPrimitive();
}