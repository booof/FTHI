// Fragment Shader for Creating Colored Objects Affected by Light
#version 450 core

// Texture Layout:

// Color Texture (4 bytes per channel) (float)
// byte 1: red
// byte 2: green
// byte 3: blue
// byte 4: alpha

// Material Texture (4 bytes per channel) (float)
// byte 1: ambient multiplier
// byte 2: specular multiplier
// byte 3: shininess (int)
// byte 4: color multiplier (how much color affects texture, 0 = none, 255 = replace, use mix function)

// Parallax Texture (4 bytes per channel) (normalized)
// bytes 1-3: normals
// byte 4: unkown, should probably be unused

// Material of Object Being Drawn
struct Material
{
	vec4 viewPos;
	float ambient;
	float specular;
	int shininess;
};

uniform Material material;

in mat4 view_mat;

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
vec4 CalculatePointLight(Point light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular);

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
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 FragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular);

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
	flat int texture_index;
	flat int mapping_index;
} fs_in;

// Output Fragment Color
out vec4 FragColor;

void main()
{
	// Discard if Alpha is Below a Threshold
	if (fs_in.Color.a < 0.1)
		discard;

	// Update Several Values
	//vec4 viewDir = normalize(material.viewPos - fs_in.FragPos);
	vec4 viewDir = vec4(normalize(material.viewPos.xyz - fs_in.FragPos.xyz), 0.0); 

	//vec4 color_diffuse = vec4(fs_in.Color.xyz, 0);
	vec4 color_diffuse = fs_in.Color;
	//vec4 color_ambient = color_diffuse * material.ambient;
	//vec4 color_specular = color_diffuse * material.specular;
	vec4 color_ambient = color_diffuse * 1.0;
	vec4 color_specular = color_diffuse * 1.0;

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
	FragColor = vec4(result.xyz, fs_in.Color.w);
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
	vec4 halfwayDirection = vec4(normalize(light.light_direction.xyz + viewDirection.xyz), 0.0);
	//float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);
	float spec = pow(max(dot(normal.xyz, halfwayDirection.xyz), 0.0), 1.0);

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
	vec4 lightDir = vec4(normalize(light.position.xyz - fragPos.xyz), 0.0);

	// Diffuse Lighting
	float diff = max(dot(normal.xyz, lightDir.xyz), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = vec4(normalize(lightDir.xyz + viewDirection.xyz), 0.0);
	//float spec = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);
	float spec = pow(max(dot(normal.xyz, halfwayDirection.xyz), 0.0), 1.0);

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
vec4 CalculateSpotLight(Spot light, vec4 normal, vec4 fragPos, vec4 viewDirection, vec4 color_ambient, vec4 color_diffuse, vec4 color_specular)
{
	// Normalize Light Direction
	vec4 lightDir = vec4(normalize(light.position.xyz - fragPos.xyz), 0.0);

	// Diffuse Lighting
	float diff = max(dot(normal.xyz, lightDir.xyz), 0.0);

	// Specular Lighting
	vec4 halfwayDirection = vec4(normalize(lightDir.xyz + viewDirection.xyz), 0.0);
	float spec = pow(max(dot(normal.xyz, halfwayDirection.xyz), 0.0), 1.0);

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