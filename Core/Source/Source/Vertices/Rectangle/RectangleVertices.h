#pragma once
#ifndef RECTANGLE_VERTICES_H
#define RECTANGLE_VERTICES_H

#include "Vertices/Vertices.h"

namespace Vertices::Rectangle
{
	// Object Rectangle Vertices
	// Vertices = 30f
	void genRectObject(glm::vec2 position, float width, float height, int data_index, float* vertices);

	// Object Rectangle Vertices With Color and Texture
	// Vertices = 72f
	void genRectObjectFull(glm::vec2 position, float zPos, float width, float height, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generates Vertices Used for Colors
	// Vertices = 42f
	void genRectColor(float xPos, float yPos, float zPos, float width, float height, glm::vec4 color, float* vertices);

	// Generates Vertices Used for Colors With Normalized Vectors
	// Vertices = 60f
	void genRectColorNormal(float xPos, float yPos, float zPos, float width, float height, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Generates Vertices Used for Textures
	// Vertices = 30f
	void genRectTexture(float xPos, float yPos, float zPos, float width, float height, float* vertices);

	// Generate Vertices Used for Textures With Normalized Vectors
	// Vertices = 48f
	void genRectTextureNormal(float xPos, float yPos, float zPos, float width, float height, glm::vec3 normal, float* vertices);

	// Generate Vertices that Highlight an Object
	// Vertices = 56f
	void genRectHilighter(float xPos, float yPos, float zPos, float width, float height, float* vertices);
}

#endif
