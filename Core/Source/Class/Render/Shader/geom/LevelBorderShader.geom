// Geometry Shader for Drawing Level Borders
#version 450 core

// Layout
layout (points) in;
layout (line_strip, max_vertices = 10) out;

// Camera Location
uniform vec2 view_pos;

// Screen Width
uniform float screen_width;

// Screen Height
uniform float screen_height;

// Level Size 
const float level_width = 128;
const float level_height = 64;

// Z Position
const float z_pos = -0.99;

void main()
{
	// Determine Horizontal Distance of Border From Camera
	float border_remainder_width = view_pos.x;
	while (border_remainder_width >= level_width) {
		border_remainder_width -= level_width;
		}
	while (border_remainder_width < 0) {
		border_remainder_width += level_width;
		}

	// Determine Vertical Distance of Border From Camera
	float border_remainder_height = view_pos.y;
	while (border_remainder_height >= level_height) {
		border_remainder_height -= level_height;
		}
	while (border_remainder_height < 0) {
		border_remainder_height += level_height;
		}

	//border_remainder_width = view_pos.x;
	//border_remainder_height = view_pos.y;
	
	// Get Normalized Device Coordinates of Vertical Lines
	float positive_vertical_line_pos = border_remainder_width / screen_width;
	float negative_vertical_line_pos = (border_remainder_width - level_width) / screen_width;

	// Get Vertical Device Coordinates of Horizontal Lines
	float positive_horizontal_line_pos = border_remainder_height / screen_height;
	float negative_horizontal_line_pos = (border_remainder_height - level_height) / screen_height;

	// Create Positive Vertical Line
	if (positive_vertical_line_pos < 1.0)
	{
		gl_Position = vec4(positive_vertical_line_pos, -1.0, z_pos, 1.0);
		EmitVertex();

		gl_Position = vec4(positive_vertical_line_pos, 1.0, z_pos, 1.0);
		EmitVertex();

		EndPrimitive();
	}	

	// Create Negative Vertical Line
	if (negative_vertical_line_pos > -1.0)
	{
		gl_Position = vec4(negative_vertical_line_pos, -1.0, z_pos, 1.0);
		EmitVertex();

		gl_Position = vec4(negative_vertical_line_pos, 1.0, z_pos, 1.0);
		EmitVertex();

		EndPrimitive();
	}

	// Create Positive Horizontal Line
	if (positive_horizontal_line_pos < 1.0)
	{
		gl_Position = vec4(-1.0, positive_horizontal_line_pos, z_pos, 1.0);
		EmitVertex();

		gl_Position = vec4(1.0, positive_horizontal_line_pos, z_pos, 1.0);
		EmitVertex();

		EndPrimitive();
	}

	// Create Negative Horizontal Line
	if (negative_horizontal_line_pos > -1.0)
	{
		gl_Position = vec4(-1.0, negative_horizontal_line_pos, z_pos, 1.0);
		EmitVertex();

		gl_Position = vec4(1.0, negative_horizontal_line_pos, z_pos, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}
