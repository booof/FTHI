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
vec4 CalculateDirectionalLight(Directional light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular);

// Uniform Buffer For Directional Lights
layout (std430, binding = 1) buffer Directional_Lights
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
vec4 CalculatePointLight(Point light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular);

// Uniform Buffer For Point Lights
layout (std430, binding = 2) buffer Point_Lights
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
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular);

// Uniform Buffer For Spot Lights
layout (std430, binding = 3) buffer Spot_Lights
{
	int spot_count;
	Spot spot_lights[];
};

// Uniforms and Processed Data
in FS_IN
{
	vec4 FragPos;
	vec3 Color;
	vec4 Normal_Static;
	vec4 Normal;
} fs_in;
uniform float Alpha;

// Output Fragment Color
out vec4 FragColor;

void main()
{
	// Discard if Alpha is Below a Threshold
	if (Alpha < 0.1)
		discard;

	// Update Several Values
	vec4 viewDir = normalize(material.viewPos - fs_in.FragPos);
	vec4 color_diffuse = vec4(fs_in.Color, 0);
	vec4 color_ambient = color_diffuse * material.ambient;
	vec4 color_specular = color_diffuse * material.specular;

	// Result of Fragmentation
	vec4 result = vec4(0);

	// Directional Lighting
	for (int i = 0; i < directional_count; i++)
	{
		result += CalculateDirectionalLight(directional_lights[i], fs_in.Normal_Static, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular);
	}	

	// Point Lighting
	for (int i = 0; i < point_count; i++)
	{
		result += CalculatePointLight(point_lights[i], fs_in.Normal, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular);
	}

	// Spot Lighting
	for (int i = 0; i < spot_count; i++)
	{
		result += CalculateSpotLight(spot_lights[i], fs_in.Normal, fs_in.FragPos, viewDir, color_ambient, color_diffuse, color_specular);
	}	

	// Return Fragment
	result.w = Alpha;
	FragColor = result;
}

// Calculate Lighting for Directional Lighting
vec4 CalculateDirectionalLight(Directional light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular)
{
	// Diffuse Lighting
	float x = min((-normal.y + light.light_direction.x) / light.light_direction.x, 1.0);
	float y = min((-normal.x + light.light_direction.y) / light.light_direction.y, 1.0);
	float z = min((normal.z + light.light_direction.z) / light.light_direction.z, 1.0);
	float diff = max(x * y * z, 0.0);

	// Specular Lighting
	vec4 halfwayDirection = normalize(light.light_direction + viewDirection);
	float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

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
vec4 CalculatePointLight(Point light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular)
{
	// Normalize Light Direction
	vec4 lightDir = normalize(light.position - fragPos);

	// Diffuse Lighting
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = normalize(lightDir + viewDirection);
	float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

	// Calculate Attenuation
	float dist = length(light.position.xy - fragPos.xy);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * (dist * dist));

	// Calculate Lighting
	vec4 ambient = color_ambient * light.ambient * attenuation;
	vec4 diffuse = color_diffuse * light.diffuse * diff * attenuation;
	vec4 specular = color_specular * light.specular * spec * attenuation;

	// Return Calculated Light
	return (ambient + diffuse + specular);
}

// Calculate Lighting for Spot Lighting
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular)
{
	// Normalize Light Direction
	vec4 lightDir = normalize(light.position - fragPos);

	// Diffuse Lighting
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = normalize(lightDir + viewDirection);
	float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);

	// Calculate Attenuation
	float dist = length(light.position.xy - fragPos.xy);
	float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * (dist * dist));

	// Calculate Cutoff
	float theta = dot(normalize(normal - lightDir), light.direction);
	float epsilon = light.OuterCutoff - light.InnerCutoff;
	float intensity = clamp((theta - light.OuterCutoff) / epsilon, 0.0, 1.0);

	// Calculate Lighting
	vec4 ambient = color_ambient * light.ambient * attenuation * intensity;
	vec4 diffuse = color_diffuse * light.diffuse * diff * attenuation * intensity;
	vec4 specular = color_specular * light.specular * spec * attenuation * intensity;

	// Return Calculated light
	return (ambient + diffuse + specular);
}