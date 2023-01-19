#pragma once
#ifndef CIRCLE_VERTICES_H
#define CIRCLE_VERTICES_H

#include "Vertices/Vertices.h"

namespace Vertices::Circle
{
	// Object Circle Vertices
	// Vertices = (number_of_sides * 15)f
	void genCircleObject(glm::vec2 position, float radius, int number_of_sides, int data_index, float* vertices);

	// Object Circle Vertices With Color and Texture
	// Vertices = (number_of_sides * 36)f
	void genCircleObjectFull(glm::vec2 position, float zPos, float radius, int number_of_sides, glm::vec4 color, glm::vec3 normal, float* vertices);

	// Creates a Full Circle with a Solid Color
	// Vertices = (number_of_sides * 21)f
	void genCircleColorFull(float xPos, float yPos, float zPos, float radius, int number_of_sides, glm::vec4 color, float* vertices);

	// Creates a Full Circle with a Texture
	void genCircleTextureFull();

	// Creates a Hollow Circle with a Solid Color
	void genCircleColorHollow();

	// Creates a Full Circle Based off of a Color Wheel
	// Vertices = (number_of_sides * 21)f
	void genCircleColorWheel(float xPos, float yPos, float zPos, float radius, int number_of_sides, float* vertices);

	// Generate Vertices that Highlight an Object
	// Vertices = (number_of_sides * 14)f
	void genCircleHighlighter(float xPos, float yPos, float zPos, float radius, int number_of_sides, float* vertices);
}

#endif