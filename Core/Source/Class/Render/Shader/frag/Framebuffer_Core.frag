// Fragment Shader for Drawing Finalized Window Object
#version 450 core

// Tex Coordinates
in FS_IN
{
	vec2 TexPos;
} fs_in;

// Fragment Color
out vec4 FragColor;

// Window Texture
uniform sampler2D screenTexture;

// Bloom Texture
uniform sampler2D bloomTexture;

void main()
{
	// Gama
	const float gama = 1.2;

	// Calculate Fragment Color
	vec4 HDRcolor = texture(screenTexture, fs_in.TexPos);
	vec4 Bloomcolor = texture(bloomTexture, fs_in.TexPos);
	vec4 color = HDRcolor + Bloomcolor;
	//color = Bloomcolor;
	FragColor.xyz = pow(color.xyz, vec3(1.0 / gama));
	FragColor.w = color.w;

	// Calculate Fragment Color
	//FragColor = texture(screenTexture, fs_in.TexPos);
}
