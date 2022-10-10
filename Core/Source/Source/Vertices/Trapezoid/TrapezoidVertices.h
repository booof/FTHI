#ifndef TRAPEZOID_VERTICES_H
#define TRAPEZOID_VERTICES_H

#include "Vertices/Vertices.h"

namespace Vertices::Trapezoid
{
	// Object Trapezoid Vertices
	// Vertices = 30f
	void genTrapObject(glm::vec2 position, float width, float height, float width_offset, float height_offset, int data_index, float* vertices);

	// Object Trapezoid Vertices With Color and Texture
	// Vertices = 72f
	void genTrapObjectFull(glm::vec2 position, float zPos, float width, float height, float width_offset, float height_offset, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generate Vertices Used for Colors
	// Vertices = 42f
	void genTrapColor(float xPos, float yPos, float zPos, float width, float height, float width_offset, float height_offset, glm::vec4 color, float* vertices);

	// Generate Vertices Used for Colors With Normalized Vectors
	// Vertices = 60f
	void genTrapColorNormal(float xPos, float yPos, float zPos, float width, float height, float width_offset, float height_offset, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generate Vertices Used for Textures
	// Vertices = 30f
	void genTrapTexture(float xPos, float yPos, float zPos, float width, float height, float width_offset, float height_offset, float* vertices);

	// Generate Vertices Used for Textures With Normalized Vectors
	// Vertices = 48f
	void genTrapTextureNormal(float xPos, float yPos, float zPos, float width, float height, float width_offset, float height_offset, glm::vec3 normal, float* vertices);

	// Generate Vertices that Highlight the Object
	// Vertices = 56f
	void genTrapHighlighter(float xPos, float yPos, float zPos, float width, float height, float width_offset, float height_offset, float* vertices);
}

#endif
