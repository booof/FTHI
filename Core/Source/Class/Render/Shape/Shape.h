#ifndef SHAPE_H
#define SHAPE_H

#ifndef DLL_HEADER
#include "Macros.h"
#endif

namespace Editor
{
	class Selector;
	class ObjectInfo;
}

namespace Object
{
	struct ObjectData;
}

namespace Shape
{
	// List of Shapes
	enum SHAPES : unsigned char
	{
		RECTANGLE, 
		TRAPEZOID,
		TRIANGLE,
		CIRCLE,
		POLYGON
	};

	// Interface Shape Class
	class Shape
	{
	public:

		// Currently Used Shape
		unsigned char shape = 0;
		
		// Initialize Shape
		virtual void initializeShape(int& number_of_vertices) = 0;

		// Initialize Vertices
		virtual void initializeVertices(Object::ObjectData& data, int& offset, int instance) = 0;

#ifdef EDITOR

		// Select Shape Data
		virtual void selectShape(Editor::Selector& selector) = 0;

		// Select Shape Info
		virtual void selectInfo(Editor::ObjectInfo& object_info) = 0;

		// Test Mouse Collisions
		virtual bool testMouseCollisions(float x, float y, float object_x, float object_y) = 0;

		// Prints the Object Name
		virtual void printName() = 0;

#endif

	};

	// List of Rendering Shapes
	class Rectangle;
	class Trapezoid;
	class Triangle;
	class Circle;
	class Polygon;
}

#endif
