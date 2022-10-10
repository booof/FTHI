// Fragment Shader for Drawing Post-Processed Images
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
	vec3 temp_color = vec3(1.0) - exp(-color.xyz * exposure);
	float gray_color = (temp_color.x + temp_color.y + temp_color.z) / 3;
	FragColor = vec4(gray_color, gray_color, gray_color, color.w);
}
