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
		FORMERGROUND
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
		int instance = 0;

		// Initialize Object
		TerrainBase(Shape::Shape* shape_, ObjectData data_);

		void write(std::ofstream& object_file, std::ofstream& editor_file);

		void updateObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

	public:

		// Layer of Object
		unsigned char layer = 0;

		// Z-Position of Object
		float zpos = 0.0f;

		// Initialize Vertices
		void initializeTerrain(int& offset_, int& instance_, int& instance_index_);

		bool testMouseCollisions(float x, float y);

#ifdef EDITOR

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

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

	};

	// Terrain Objects
	class Foreground;
	class Formerground;
	class Background;
	class Backdrop;
}

#endif
