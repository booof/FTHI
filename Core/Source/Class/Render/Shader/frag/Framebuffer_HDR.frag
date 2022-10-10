// Fragment Shader for Drawing Post-Processed Images
#version 450 core

// Tex Coordinates
in FS_IN
{
	vec2 TexPos;
} fs_in;

// Fragment Colors
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

// Window Texture 
uniform sampler2D screenTexture;

void main()
{
	// Exposure
	const float exposure = 0.9;
	
	// Gamma
	const float gamma = 0.9;

	// Calculate Fragment Color
	//vec4 color = texture(screenTexture, fs_in.TexPos);
	//FragColor.xyz = vec3(1.0) - exp(-color.xyz * exposure);
	//FragColor.w = color.w;
	
	// Store Fragement Color
	//FragColor = texture(screenTexture, fs_in.TexPos);
	
	// reinhard tone mapping
	vec3 hdrColor = texture(screenTexture, fs_in.TexPos).rgb;
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	mapped = pow(mapped, vec3(1 / gamma));
	FragColor = vec4(mapped, 1.0);

	// Calculate Bloom
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	BloomColor = FragColor * int(brightness >= 1.0);
	//BloomColor.w = color.w;
	BloomColor.w = 1.0;
}