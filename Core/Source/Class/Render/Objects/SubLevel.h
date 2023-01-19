#pragma once
#ifndef SUB_LEVEL_H
#define SUB_LEVEL_H

#include "ExternalLibs.h"
#include "Macros.h"
#include "Render/Struct/List.h"

namespace Render::Objects
{
	class ChangeController;
	class UnsavedLevel;
}

namespace Object
{
	class Object;
	namespace Physics { class PhysicsBase; }
	namespace Entity { class EntityBase; }
}

namespace Render::Objects
{
	struct ObjectCount;

	// Header Data Struct
	struct LevelHeader
	{
		int number_of_loaded_objects = 0;
		short floor_count = 0;
		short left_count = 0;
		short right_count = 0;
		short ceiling_count = 0;
		short trigger_count = 0;
		short terrain_count = 0;
		short directional_count = 0;
		short point_count = 0;
		short spot_count = 0;
		short beam_count = 0;
	};

	// Container for Chunk of the World
	class SubLevel
	{
	public:

		// Determines if Level is Initialized
		bool initialized = false;

		// Path to Level File
		std::string path;

		// Coordinates of Level
		int level_x;
		int level_y;
		int level_version;

		// Header Data
		LevelHeader header;

		// The Array of All Active Objects
		Object::Object** active_objects;

#ifdef EDITOR

		// Initialize SubLevel
		SubLevel(int x, int y);

		// Read Level
		void readLevel(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities);

		// Pointer to Unsaved Level
		UnsavedLevel* unsaved_level;

		// Read Headers
		void readHeaders();

		// Draw Level Visualizer
		void drawVisualizer();

#else

		// Initialize SubLevel
		SubLevel(int x, int y);

		// Read Level
		void readLevel(Object::Object** objects);

		// Read Saved Data For Level
		void readSaveData();

		// Read Headers
		void readHeaders();

		// Write To File
		void writeLevel();

#endif

		// Add Header
		void addHeader(ObjectCount& object_count);

		// Subtract Header
		void subtractHeader(ObjectCount& object_count);

		// Delete Level
		~SubLevel();
	};
}

#endif
