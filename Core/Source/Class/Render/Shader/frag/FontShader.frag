// Optomized to Render Characters
#version 450 core

in FS_IN
{
	vec2 TexPos;
} fs_in;

uniform sampler2D texture1;
uniform vec3 texColor;

out vec4 color;

void main()
{
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(texture1, fs_in.TexPos).r);
	color = vec4(texColor, 1.0f) * sampled;
}
