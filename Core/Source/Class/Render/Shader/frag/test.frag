// Fragment Shader for Creating Colored Objects Affected by Light
#version 450 core

// Material of Object Being Drawn
struct Material
{
	vec4 viewPos;
	float ambient;
	float specular;
	int shininess;
};

uniform Material material;

// Data to be Processed
in VS_OUT
{
//	vec4 FragPos;
//	vec2 TexPos;
	vec4 Color;
//	vec4 Normal_Static;
//	vec4 Normal;
//	flat int texture_index;
//	flat int mapping_index;
} fs_in;

// Output Fragment Color
out vec4 FragColor;

void main()
{
//	FragColor = vec4(0, 0, 0, 1);
//	FragColor.x = fs_in.FragPos.x;
//	FragColor.x *= 0.5;
//	FragColor.y = fs_in.FragPos.y;
//	FragColor.y *= 0.5;

	FragColor = vec4(0, 0, 0, 1);
	FragColor.x = fs_in.Color.y;
	FragColor.x *= 0.5;
	FragColor.y = fs_in.Color.x;
	FragColor.y *= 0.5;

//FragColor = fs_in.Color;

	//if (fs_in.FragPos.x < 0 || fs_in.FragPos.y < 0)
	//if (gl_FragCoord.x < 500)
	//{
	//	discard;
//	}

//FragColor = fs_in.Color;
}
