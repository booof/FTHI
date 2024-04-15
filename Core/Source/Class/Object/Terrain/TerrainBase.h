#pragma once
#ifndef TERRAIN_BASE_H
#define TERRAIN_BASE_H

#ifndef DLL_HEADER
#include "Object/Object.h"
#include "Render/Shape/Shape.h"
#endif

namespace Object::Terrain
{
	// Terrain Layers
	enum LAYERS : unsigned char
	{
		BACKDROP,
		BACKGROUND_3,
		BACKGROUND_2,
		BACKGROUND_1,
		FOREGROUND,
		FORMERGROUND,
		STATIC
	};

	// Base Terrain Class
	class TerrainBase : public SubObject
	{
	protected:

		// Shape of Object
		Shape::Shape* shape;

		// Offset in Vertex Object
		int offset = 0;

		// Instance of Object
		int instance = -1;

		// Initialize Object
		TerrainBase(Shape::Shape* shape_, ObjectData data_, glm::vec2& offset);

		void updateObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

	public:

		// Layer of Object
		unsigned char layer = 0;

		// Z-Position of Object
		float zpos = 0.0f;

		// Deconstructor
		~TerrainBase();

		// Initialize Vertices
		void initializeTerrain(int& offset_, int& instance_, int& instance_index_);

		bool testMouseCollisions(float x, float y);

#ifdef EDITOR

		// Return Position of Object
		glm::vec2 returnPosition();

		// Return Shape of Object
		Shape::Shape* returnShapePointer();

#endif

		// Returns Reference to Object Data
		ObjectData& returnObjectData();

		// Returns Pointer to Shape
		Shape::Shape* returnShapeData();

		// Update Model Matrix
		void updateModel();

		// Reset Vertices, If Needed
		void resetVertices();

		// Override of Recursive Terrain Generation
		void genTerrainRecursively(int& offset_static, int& offset_dynamic, int& instance, int& instance_index);
	};

	// Terrain Objects
	class Foreground;
	class Formerground;
	class Background;
	class Backdrop;
}

#endif
