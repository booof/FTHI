#pragma once
#ifndef CIRCLE_H
#define CIRCLE_H

#ifndef DLL_HEADER
#include "Class/Render/Shape/Shape.h"
#endif

class Shape::Circle : public Shape
{
	// Radius of Circle
	float radius = 1.0f;

	// Inner Radius of Circle
	float radius_inner = 0.0f;

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
	Circle() {}
	
	// Initialize New Data
	Circle(float radius1, float radius2);

	// Read from File
	Circle(std::ifstream& object_file);

	// Return Pointer to Radius
	float* pointerToRadius();

	// Return Pointer to Inner Radius
	float* pointerToRadiusInner();

	// Print Name
	void printName();

#endif

};

#endif