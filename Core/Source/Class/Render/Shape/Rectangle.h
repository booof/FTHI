#pragma once
#ifndef RECTANGLE_SHAPE_H
#define RECTANGLE_SHAPE_H

#ifndef DLL_HEADER
#include "Render/Shape/Shape.h"
#include "Macros.h"
#endif

class Shape::Rectangle : public Shape
{
	// Width of the Rectangle
	float width = 1.0f;

	// Height of the Rectangle
	float height = 1.0f;

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
	Rectangle() {}

	// Initialize New Data
	Rectangle(float width_, float height_);

	// Read from File
	Rectangle(std::ifstream& object_file);

	// Return Pointer to Width
	float* pointerToWidth();

	// Return pointer to Height
	float* pointerToHeight();

	// Print Name
	void printName();

	// Make a Copy of the Shape
	Shape* makeCopy();

#endif

};

#endif
