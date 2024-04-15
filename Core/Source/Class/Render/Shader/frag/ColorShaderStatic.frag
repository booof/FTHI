// Fragment Shader for Creating Colored Object not Affected by Light
#version 450 core

in VS_OUT
{
	vec4 Color;
} fs_in;

out vec4 FragColor;

uniform vec4 brightness;

void main()
{
	//FragColor = brightness + fs_in.Color;
	FragColor = fs_in.Color;
	if (FragColor.a < 0.1)
		discard;
}