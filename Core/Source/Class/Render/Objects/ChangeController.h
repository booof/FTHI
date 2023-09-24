#pragma once
#ifndef CHANGE_CONTROLLER_H
#define CHANGE_CONTROLLER_H

#include "ExternalLibs.h"

// This is the Main Class for Undo/Redo Handling and Storing of Data Until it is Saved
// All Unsaved Level Instances Are Stored in this Object
// All Changes Made Will Appear in a Master Stack with Instances that Points to Unsaved Level Stack of that Change
// Undo/Redo Changes Using CTRL-Z and CTRL-Y respecivly
// Undo/Redo Simply Determines Which Stacks to Use
// Making a Change While In a Farther Back Stack in the Chain Deletes All Future Redos, Similar to Normal Text IDEs
// To Delete Future Redos, Start at Top of Stack and Move Backwards Again Until Current Index is Reached, Deleting all Instances in Other Stacks
// Make Stacks Circular In Case There Are Too Many Undos
// If an Instance in a Stack is Overwriten Due to Circular Indexing, Delete Corrisponding Instance in Unsaved Level Stack
// Stack Will Persist Until a Save is Made
// Each Stack Instance Includes a Level Coords and Camera Position At Change Location

namespace DataClass
{
	class Data_Object;
}

namespace Editor
{
	class Selector;
}

namespace Render::Objects
{
	class UnsavedBase;
	class UnsavedLevel;
	class UnsavedGroup;
	class UnsavedComplex;

	class Level;

	// Struct for Saved Level Identifier
	struct SavedIdentifier
	{
		int level_x = 0;
		int level_y = 0;
		int level_version = 0;
	};

	// Saved Identifier Overloader
	bool operator==(const SavedIdentifier& first, const SavedIdentifier& second);

	class ChangeController
	{
		// Struct for Chain Instance
		struct ChainMember
		{
			// Stack Index of Each Unsaved Object
			std::vector<UnsavedBase*> stack_indicies;

			// Position of Camera to move to
			glm::vec2 camera_pos;
		};

		// The Master Stack
		class MasterStack
		{
			// Size of Array
			const uint8_t max_master_stack_size = 100;

			// Items in Stack
			uint8_t stack_size = 0;

			// Tail of Stack
			uint8_t tail = 0;

			// Current Head of Stack
			uint8_t head = 0;

			// Index of Stack
			uint8_t stack_index = 0;

			// Stack Array
			ChainMember* stack_array;

			// Pointer to Unsaved Levels
			std::vector<UnsavedLevel*>* unsaved_level_pointer;

			// Delete an Instance
			void deleteInstance(uint8_t index);

		public:

			// Initialize Stack
			MasterStack();

			// Store Pointer to Unsaved Levels
			void storePointerToUnsavedLevels(std::vector<UnsavedLevel*>* pointer);

			// Traverse Backwards Through Stack (Undo)
			bool traverseForwards();

			// Traverse Forwards Through Stack (Redo)
			bool traverseBackwards();

			// Append New Instance to Stack
			void appendInstance();

			// Return Current Instance
			ChainMember* returnCurrentInstance();

			// Delete All Instances From Stack Index to Head
			void deleteFromIndexToHead();

			// Create Initial Instance
			void createInitialInstance();

			// Reset Stack
			void reset();

			// Return Size of Array
			int retrunArraySize();
		};

		// The Singleton Instance
		static ChangeController instance;

		// Vector of Unsaved Levels
		std::vector<UnsavedLevel*> unsaved_levels;

		// Vector of Unsaved Groups
		std::vector<UnsavedGroup*> unsaved_groups;

		// Vector of Unsaved Complex Objects
		std::vector<UnsavedComplex*> unsaved_complex;

		// Master Stack
		MasterStack* master_stack = nullptr;

		// Level Pointer
		Level* level = nullptr;

		// Vector of Saved Unsaved Levels
		std::vector<SavedIdentifier> saved_levels;

		// Initialize Object
		ChangeController() {}

		// Get Level Coords from World Position
		void updateLevelPos(glm::vec2 position, glm::vec2& level);

		// Test if Unsaved Level Has Been Saved Prevously
		bool testIfSaved(SavedIdentifier test_identifier);

		// Add Data Object to Unsaved
		void addToUnsaved(DataClass::Data_Object* data_object);

	public:

		// Remove the Copy Constructor
		ChangeController(const ChangeController&) = delete;

		// Initialize Object
		void initializeChangeController();

		// Return the Singleton
		static ChangeController* get();

		// Store Pointer to Level
		void storeLevelPointer(Level* level_);

		// Return Unsaved Level at Coords. If DNE, Create New Unsaved Level
		UnsavedLevel* getUnsavedLevel(int16_t x, int16_t y, int8_t z);

		// Returns Unsaved Level at WorldSpace Coords.
		UnsavedLevel* getUnsavedLevelObject(DataClass::Data_Object* object);

		// Generate a New Unsaved Level
		UnsavedLevel* generateUnsavedLevel(int16_t x, int16_t y, int8_t z);

		// Return Unsaved Complex of Corrisponding File. If DNE, Create New Unsaved Complex
		UnsavedComplex* getUnsavedComplex(std::string& file_path);

		// Increments the Removed Count of the Specified SubLevel
		void incrementRemovedCount(int16_t x, int16_t y, int8_t z);

		// Stores an Unsaved Group
		void storeUnsavedGroup(UnsavedGroup* new_group);

		// Transfer An Object Between Levels
		void transferObject(DataClass::Data_Object* data_object, int16_t x, int16_t y, int8_t z);

		// Handle an Object Retrun From Selector
		void handleSelectorReturn(Editor::Selector* selector);

		// Handle the Return of a Single Object From Selector
		void handleSingleSelectorReturn(DataClass::Data_Object* data_object, DataClass::Data_Object* original_object, Editor::Selector* selector, bool reload_all, bool keep_parent);

		// Simple Function to Generate a Real Object From a Returned Object from the Selector (Above Function No Work)
		void handleSelectorRealReturn(DataClass::Data_Object* data_object, Editor::Selector* selector);

		// Handle Deletion of an Object From Selector
		void handleSelectorDelete(Editor::Selector* selector);

		// Handle Cancelation of a Selection
		void handleSelectorCancelation(Editor::Selector* selector);

		// Undo
		void undo();

		// Redo
		void redo();

		// Reload Objects
		void reloadObjects();

		// Revert All Changes
		void revertAllChanges();

		// Save Changes
		void save();

		// Returns True if The Change Controller is Unsaved
		bool returnIfUnsaved();

		// Reset Change Controller
		void reset(bool reload);

		// Draw Visualizers for Unsaved Objects
		void drawVisualizers();
	};
}

#endif
