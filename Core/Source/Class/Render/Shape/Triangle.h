#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#ifndef DLL_HEADER
#include "Class/Render/Shape/Shape.h"
#include "ExternalLibs.h"
#include "Macros.h"
#endif

class Shape::Triangle : public Shape
{
	// Second Vertex Position
	glm::vec2 position2 = glm::vec2(0.0f, 0.0f);

	// Third Vertex Position
	glm::vec2 position3 = glm::vec2(0.0f, 0.0f);

	// Initialize Shape
	void initializeShape(int& number_of_vertices);

	// Initialize Vertices
	void initializeVertices(Object::ObjectData& data, int& offset, int instance);

#ifdef EDITOR

	// Write Shape to File
	void writeShape(std::ofstream& object_file);

	// Select Shape Info
	void selectInfo(Editor::ObjectInfo& object_info);

	// Test Mouse Collisions
	bool testMouseCollisions(float x, float y, float object_x, float object_y);

public:

	// Default Constructor
	Triangle() {}

	// Initialize New Data
	Triangle(glm::vec2 pos2, glm::vec2 pos3);

	// Read from File
	Triangle(std::ifstream& object_file);

	// Return Pointer to Second Vertex
	glm::vec2* pointerToSecondPosition();

	// Return Pointer to Third Vertex
	glm::vec2* pointerToThirdPosition();

	// Print Name
	void printName();

	// Make a Copy of the Shape
	Shape* makeCopy();

	// Update the Selected Position of the Shape
	void updateSelectedPosition(float deltaX, float deltaY);

#endif

};

#endif
