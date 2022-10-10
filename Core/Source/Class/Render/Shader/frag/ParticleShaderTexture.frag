// Fragment Shader for Instanced Particles
#version 450 core

in FS_IN
{
	vec2 Texture;
	vec2 Animation;
} fs_in;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2DArray texture2;
uniform float alpha;

void main()
{
	vec4 color = (texture(texture1, fs_in.Texture) * int(fs_in.Animation.x == 0)) + (texture(texture2, vec3(fs_in.Texture, int(fs_in.Animation.y) - 1)) * int(fs_in.Animation.x));
	color.w *= alpha;
	FragColor = color;
}