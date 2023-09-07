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
	struct Active;
	namespace Physics { class PhysicsBase; }
	namespace Entity { class EntityBase; }
}

namespace Render::Objects
{
	// Declaration for Level Object
	class Level;

	// Container for Chunk of the World
	class SubLevel
	{
	public:

		// Determines if Level is Initialized
		bool initialized = false;

		// Path to Level File
		std::string path;

		// Coordinates of Level
		int16_t level_x;
		int16_t level_y;
		int16_t level_version;

		// Header Data
		//LevelHeader header;
		uint32_t number_of_loaded_objects = 0;

		// The Array of All Active Objects
		Object::Active* active_objects;

		// The Number of New Active Objects
		int16_t new_active_objects = 0;
		int16_t removed_count = 0;

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
		void addHeader(uint32_t& object_count);

		// Subtract Header
		void subtractHeader(uint32_t& object_count);

		// Deactivate All Objects Belonging To This Level
		void deactivateObjects();

		// Include New Objects Into Active Object Array
		void includeNewActives(Object::Active* new_actives, int new_active_count, Level* level);

		// Resets the Counts of the Object
		void resetCounts();

		// Delete Level
		~SubLevel();
	};
}

#endif
