#ifndef TRIANGLE_VERTICES_H
#define TRIANGLE_VERTICES_H

#include "Vertices/Vertices.h"

namespace Vertices::Triangle
{
	// Object Triangle Vertices
	// Vertices = 15f
	void genTriObject(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, int data_index, float* vertices);

	// Object Triangle Vertices With Color and Texture
	// Vertices = 36f
	void genTriObjectFull(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generate Vertices Used for Colors
	// Vertices = 21f
	void genTriColor(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, float* vertices);

	// Generate Vertices used for Colors With Normalized Vectors
	// Vertices = 30f
	void genTriColorNormal(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generate Vertices Used for Textures
	// Vertices = 15f
	void genTriTexture(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, float* vertices);

	// Generate Vertices Used for Textures With Normalized Vectors
	// Vertices = 24f
	void genTriTextureNormal(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec3 normal, float* vertices);

	// Generate Vertices that Highlight an Object
	// Vertices = 42f
	void genTriHighlighter(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, float* vertices);
}

#endif
