// Fragment Shader for Creating Colored Objects Affected by Light
#version 450 core

// Textures
uniform sampler2D color_texture;
uniform sampler2D material_texture;
uniform sampler2D parallax_texture;

// View Position
uniform vec4 view_pos;

// Lighting Layer
uniform int layer;

// Direction Light Values
struct Directional
{
	// Light Direction
	vec4 light_direction;
	vec4 line_direction;

	// Light Properties
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// Endpoints
	vec2 endpoint1;
	vec2 endpoint2;
};

// Calculate Lighting For Directional Lighting
vec4 CalculateDirectionalLight(Directional light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess);

// Uniform Buffer For Directional Lights
layout (std430, binding = 2) buffer Directional_Lights
{
	int directional_count;
	Directional directional_lights[];
};

// Point Light Values
struct Point 
{
	// Light Position
	vec4 position;

	// Light Properties
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// Attenuation
	float linear;
	float quadratic;
};

// Calculate Lighting For Point Lighting
vec4 CalculatePointLight(Point light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess);

// Uniform Buffer For Point Lights
layout (std430, binding = 3) buffer Point_Lights
{
	int point_count;
	Point point_lights[];	
};

// Spot Light Values
struct Spot
{
	// Light Position
	vec4 position;

	// Direction Vertex
	vec4 direction;

	// Light Properties
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// Attenuation
	float linear;
	float quadratic;

	// Angle Variables
	float InnerCutoff;
	float OuterCutoff;
};

// Calculate Lighting For Spot Lighting
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess);

// Uniform Buffer For Spot Lights
layout (std430, binding = 4) buffer Spot_Lights
{
	int spot_count;
	Spot spot_lights[];
};

// Beam Light Values
struct Beam
{
	// Direction of Line
	vec4 line_direction;

	// Light Properties
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	// Attenuation
	float linear;
	float quadratic;

	// Endpoints
	vec2 endpoint1;
	vec2 endpoint2;
};

// Calcualte Lighting For Beam Lighting

// Uniform Buffer for Beam Lights
layout (std430, binding = 5) buffer Beam_Lights
{
	int beam_count;
	Beam beam_lights[];
};

// Data to be Processed
in VS_OUT
{
	vec4 FragPos;
	vec2 TexPos;
	flat vec4 Color;
	flat vec4 Normal_Static;
	flat vec4 Normal;
} fs_in;

// Output Fragment Color
out vec4 FragColor;

void main()
{
	// Discard if Alpha is Below a Threshold
	if (fs_in.Color.a < 0.1)
		discard;

	// Update Several Values
	//vec4 viewDir = normalize(view_pos - fs_in.FragPos);
	vec4 viewDir = vec4(normalize(view_pos.xyz - fs_in.FragPos.xyz), 0.0); 
	//viewDir = vec4(1, 1, 0, 0);

	// Get Texture Colors
	vec4 tex_color = texture(color_texture, fs_in.TexPos);

	// Get Material Values
	vec4 material_values = texture(material_texture, fs_in.TexPos);
	material_values.z = 1.0;

	//vec4 color_diffuse = vec4(fs_in.Color.xyz, 1.0);
	//vec4 color_ambient = color_diffuse * material_values.x;
	//vec4 color_specular = color_diffuse * material_values.y;
	vec4 color_diffuse = fs_in.Color;
	vec4 color_ambient = color_diffuse * 1.0;
	vec4 color_specular = color_diffuse * 1.0;

	// Result of Fragmentation
	vec4 result = vec4(0);

	// Directional Lighting
	for (int i = 0; i < directional_count; i++)
	{
		result += CalculateDirectionalLight(directional_lights[i], fs_in.Normal_Static, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular, material_values.z);
	}	

	// Point Lighting
	for (int i = 0; i < point_count; i++)
	{
		result += CalculatePointLight(point_lights[i], fs_in.Normal, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular, material_values.z);
	}

	// Spot Lighting
	for (int i = 0; i < spot_count; i++)
	{
		result += CalculateSpotLight(spot_lights[i], fs_in.Normal, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular, material_values.z);
	}	

	FragColor = vec4(result.xyz, fs_in.Color.w);
}

// Calculate Lighting for Directional Lighting
vec4 CalculateDirectionalLight(Directional light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess)
{
	// Diffuse Lighting
	float x = min((-normal.y + light.light_direction.x) / light.light_direction.x, 1.0);
	float y = min((-normal.x + light.light_direction.y) / light.light_direction.y, 1.0);
	float z = min((normal.z + light.light_direction.z) / light.light_direction.z, 1.0);
	float diff = max(x * y * z, 0.0);

	// Specular Lighting
	vec4 halfwayDirection = vec4(normalize(light.light_direction.xyz + viewDirection.xyz), 0.0);
	float spec = pow(max(dot(normal.xyz, halfwayDirection.xyz), 0.0), shininess);

	// Calculate Cutoff
	float y_sign = 1 - 2 * int(normal.x < 0);
	float light_sign = 1 - 2 * int(light.light_direction.x > 0);
	vec2 endpointVector1 = normalize(fragPos.xy - light.endpoint1) * light_sign;
	vec2 endpointVector2 = normalize(light.endpoint2 - fragPos.xy) * light_sign;
	float endpointTheta1 = dot(endpointVector1, light.line_direction.xy) * y_sign;
	float endpointTheta2 = dot(endpointVector2, light.line_direction.xy) * y_sign;
	int cutoff = int(0 < endpointTheta1 && endpointTheta1 < 1.1 && 0 < endpointTheta2 && endpointTheta2 < 1.1);

	// Calculate Lighting
	vec4 ambient = color_ambient * light.ambient * cutoff;
	vec4 diffuse = color_diffuse * light.diffuse * diff * cutoff;
	vec4 specular = color_specular * light.specular * spec * cutoff;

	// Return Calculated Light
	return (ambient + diffuse + specular);
}

// Calculate Lighting for Point Lighting
vec4 CalculatePointLight(Point light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess)
{
	// Normalize Light Direction
	vec4 lightDir = vec4(normalize(light.position.xyz - fragPos.xyz), 0.0);

	// Diffuse Lighting
	float diff = max(dot(normal.xyz, lightDir.xyz), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = normalize(lightDir + viewDirection);
	float spec = pow(max(dot(normal, halfwayDirection), 0.0), 1.0);

	// Calculate Attenuation
	float dist = length(light.position.xyz - fragPos.xyz);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * (dist * dist));

	// Calculate Lighting
	vec4 ambient = color_ambient * light.ambient * attenuation;
	vec4 diffuse = color_diffuse * light.diffuse * diff * attenuation;
	vec4 specular = color_specular * light.specular * spec * attenuation;

	// Return Calculated Light
	return (ambient + diffuse + specular);
}

// Calculate Lighting for Spot Lighting
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular, float shininess)
{
	// Normalize Light Direction
	vec4 lightDir = vec4(normalize(light.position.xyz - fragPos.xyz), 0.0);

	// Diffuse Lighting
	float diff = max(dot(normal.xyz, lightDir.xyz), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = vec4(normalize(lightDir.xyz + viewDirection.xyz), 0.0);
	float spec = pow(max(dot(normal.xyz, halfwayDirection.xyz), 0.0), shininess);

	// Calculate Attenuation
	float dist = length(light.position.xyz - fragPos.xyz);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * (dist * dist));

	// Calculate Cutoff
	float theta = dot(normalize(normal.xyz - lightDir.xyz), light.direction.xyz);
	float epsilon = light.InnerCutoff - light.OuterCutoff;
	float intensity = smoothstep(0.0, 1.0, (light.OuterCutoff - theta) / epsilon);

	// Calculate Lighting
	vec4 ambient = color_ambient * light.ambient * attenuation * intensity;
	vec4 diffuse = color_diffuse * light.diffuse * diff * attenuation * intensity;
	vec4 specular = color_specular * light.specular * spec * attenuation * intensity;

	// Return Calculated light
	return (ambient + diffuse + specular);
}