// Fragment Shader for Creating Textured Objects not Affected by Light
#version 450 core

in VS_OUT
{
	vec2 TexPos;
} fs_in;

out vec4 FragColor;

// Texture Values
uniform int texture_layer;
uniform sampler2D texture1;
uniform sampler2DArray texture2;

void main()
{
	// Bools for Texture
	bool animated = (texture_layer > 0);
	bool not_animated = !animated;

	// Calculate Texture Fragment
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	//FragColor = (texture(texture1, fs_in.TexPos) * int(not_animated)) + (texture(texture2, vec3(fs_in.TexPos, texture_layer - 1)) * int(animated));

	FragColor = texture(texture1, fs_in.TexPos);
	if (FragColor.a < 0.1)
		discard;
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}