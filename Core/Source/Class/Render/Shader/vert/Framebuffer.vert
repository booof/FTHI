// Vertex Shader for Drawing Window Framebuffer
#version 450 core

// Vertex Data
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcords;

out VS_OUT
{
	vec2 TexPos;
} vs_out;

void main()
{
	// Texture Values
	vs_out.TexPos = texcords;

	// Position Values
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}