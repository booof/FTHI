#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#ifndef DLL_HEADER
#include "Class/Render/Shape/Shape.h"
#include "Macros.h"
#endif

class Shape::Trapezoid : public Shape
{
	// Width of Trapezoid
	float width = 1.0f;

	// Height of Trapezoid
	float height = 1.0f;

	// Width Offset of Trapezoid
	float width_offset = 0.0f;

	// Height Offset of Trapezoid
	float height_offset = 0.0f;

	// Initialize Shape
	void initializeShape(int& number_of_vertices);

	// Initialize Vertices
	void initializeVertices(Object::ObjectData& data, int& offset, int instance);

#ifdef EDITOR

	// Write Shape to File
	void writeShape(std::ofstream& object_file);

	// Select Shape Data
	void selectShape(Editor::Selector& selector);

	// Select Shape Info
	void selectInfo(Editor::ObjectInfo& object_info);

	// Test Mouse Collisions
	bool testMouseCollisions(float x, float y, float object_x, float object_y);

public:

	// Default Constructor
	Trapezoid() {}

	// Initialize New Data
	Trapezoid(float width_, float height_, float w_offest, float h_offset);

	// Read from File
	Trapezoid(std::ifstream& object_file);

	// Return Pointer to Width
	float* pointerToWidth();

	// Return Pointer to Height
	float* pointerToHeight();

	// Return Pointer to Width Offset
	float* pointerToWidthOffset();

	// Return Pointer to Height Offset
	float* pointerToHeightOffset();

	// Print Name
	void printName();

#endif

};

#endif
