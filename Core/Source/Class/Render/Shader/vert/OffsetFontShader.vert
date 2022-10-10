// Basic Fragmentation to Render Characters
#version 450 core

layout (location = 0) in vec4 vertex;

uniform mat4 project;
uniform mat4 model;

out VS_OUT
{
	vec2 TexPos;
} vs_out;

void main()
{
	gl_Position = project * model * vec4(vertex.xy, -0.1f, 1.0f);
	vs_out.TexPos = vertex.zw;
}