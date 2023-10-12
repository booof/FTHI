// Vertex Shader for Drawing Borders Between Levels
#version 450 core

// Vertex Data for Lower-Left Corner of Each Sublevel
layout (location = 0) in vec2 pos;

void main()
{
	// Position is Center of Screen
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
}
