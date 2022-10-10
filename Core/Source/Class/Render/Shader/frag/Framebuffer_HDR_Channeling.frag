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

// Channels
uniform float Red;
uniform float Green;
uniform float Blue;

void main()
{
	// Exposure
	const float exposure = 1.0;

	// Calculate Fragment Color
	vec4 color = texture(screenTexture, fs_in.TexPos);
	vec3 calculated_color = vec3(1.0) - exp(-color.xyz * exposure);
	calculated_color *= vec3(Red, Green, Blue);
	FragColor = vec4(calculated_color, color.w);
}
