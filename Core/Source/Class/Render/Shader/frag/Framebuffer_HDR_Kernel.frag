// Fragment Shader for Drawing Post-Processed Images
#version 450 core

// Tex Coordinates
in FS_IN
{
	vec2 TexPos;
} fs_in;

// Fragment Color
out vec4 FragColor;

// Texture Offsets
uniform vec2 offsets[9];

uniform float kernel[9];

// Window Texture 
uniform sampler2D screenTexture;

void main()
{
	// Exposure
	const float exposure = 1.0;

	// Calculate Texture Samples
	vec3 color = vec3(0.0);
	float alpha_samples = 0;
	for (int i = 0; i < 9; i++)
	{
		vec4 new_sample = texture(screenTexture, fs_in.TexPos + offsets[i]);
		color += new_sample.xyz * kernel[i];
		alpha_samples += new_sample.w;
	}
	alpha_samples /= 9;

	// Calculate Fragment Color
	FragColor.xyz = vec3(1.0) - exp(-color * exposure);
	FragColor.w = alpha_samples;
}
