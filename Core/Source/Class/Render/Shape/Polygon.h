#ifndef POLYGON_H
#define POLYGON_H

#ifndef DLL_HEADER
#include "Class/Render/Shape/Shape.h"
#include "Macros.h"
#endif

class Shape::Polygon : public Shape
{
	// Radius of Object
	float radius = 1.0f;

	// Inner Radus
	float radius_inner = 0.0f;

	// Number of Sides
	unsigned char number_of_sides = 5;

	// Angle Offset
	float angle_offset = 0.0f;
	
	void initializeShape(int& number_of_vertices);

	// Initialize Verties
	void initializeVertices(Object::ObjectData& data, int& offset, int instance);

#ifdef EDITOR

	// Select Shape Data
	void selectShape(Editor::Selector& selector);

	// Select Shape Info
	void selectInfo(Editor::ObjectInfo& object_info);

	// Test Mouse Collisions
	bool testMouseCollisions(float x, float y, float object_x, float object_y);

public:

	// Defualt Constructor
	Polygon() {}

	// Initialize New Data
	Polygon(float radius1, float radius2, unsigned char sides, float a_offset);

	// Return Pointer to Radius
	float* pointerToRadius();

	// Return Pointer to Inner Radius
	float* pointerToRaidusInner();

	// Return Pointer to Number of Sides
	unsigned char* pointerToNumberOfSides();

	// Return Pointer to Angle Offset
	float* pointerToAngleOffset();

	// Print Name
	void printName();

#endif

};

#endif