// Fragment Shader for Applying the Bloom Effect
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

// Current Bloom Iteration
int iteration;

// How Much a Fragment Contributes to Color
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
	// Calculate Texels
	vec2 tex_offset = 1.0 / textureSize(screenTexture, 0);
	vec4 color = texture(screenTexture, fs_in.TexPos) * weight[0];

	// Calculate Direction of Blur
	bool x_qualifier = bool(iteration % 2);
	bool y_qualifier = !x_qualifier;
	vec2 offset = vec2(tex_offset.x * int(x_qualifier), tex_offset.y * int(y_qualifier));

	// Apply Blur
	for (int i = 1; i < 5; ++i)
	{
		color += texture(screenTexture, fs_in.TexPos + offset * i) * weight[i];
		color += texture(screenTexture, fs_in.TexPos - offset * i) * weight[i];
	}

	// Save Fragment Color
	FragColor = color;
}