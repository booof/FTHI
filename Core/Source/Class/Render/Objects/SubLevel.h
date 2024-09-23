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
		std::string path = "";

		// Coordinates of Level
		int16_t level_x = 0;
		int16_t level_y = 0;
		int16_t level_version = 0;

		// Header Data
		//LevelHeader header;
		uint32_t number_of_loaded_objects = 0;

		// The Array of All Active Objects
		Object::Active* active_objects = nullptr;

		// The Number of New Active Objects
		int16_t new_active_objects = 0;
		int16_t removed_count = 0;

		// The Offset of the Sublevel Objects When Building Sublevel
		glm::vec2 object_offsets = glm::vec2(0.0f, 0.0f);

		// Default Constructor for the Object
		SubLevel() {}

#ifdef EDITOR

		// Initialize SubLevel for Level Container
		SubLevel(std::string& level_data_path, glm::i16vec4 pos, glm::vec2& wrapping_offset);

		// Initialize SubLevel for GUI Container
		SubLevel(std::string& gui_path);

		// Read Level
		void readLevel(uint16_t& index_in_object_array);

		// Pointer to Unsaved Level
		UnsavedLevel* unsaved_level;

		// Read Headers
		void readHeaders();

		// Draw Level Visualizer
		void drawVisualizer();

		// Update Model Matrix of Unsaved Level
		void updateModelMatrix();

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
		void* deactivateObjects();

		// Include New Objects Into Active Object Array
		void includeNewActives(Object::Active* new_actives, int new_active_count, Level* level);

		// Resets the Counts of the Object
		void resetCounts();

		// Delete Level
		void deleteSubLevel();

		// Reload Actives Pointer
		void reloadActivePointer();
	};
}

#endif
