// Framebuffer Shader for Drawing Post-Processed Images
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

void main()
{
	// Exposure
	const float exposure = 1.0;

	// Calculate Fragment Color
	vec4 color = texture(screenTexture, fs_in.TexPos);
	FragColor.xyz = exp(-color.xyz * exposure);
	FragColor.w = color.w;
}
