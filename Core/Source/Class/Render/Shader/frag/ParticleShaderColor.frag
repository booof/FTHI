// Fragment Shader for Instanced Particles
#version 450 core

in FS_IN
{
	vec3 Color;
} fs_in;

out vec4 FragColor;

uniform float alpha;

void main()
{
	FragColor = vec4(fs_in.Color, alpha);
}
