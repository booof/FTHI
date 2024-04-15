#include "Macros.h"
#ifdef EDITOR

#pragma once
#ifndef UNSAVED_LEVEL_H
#define UNSAVED_LEVEL_H

#include "UnsavedBase.h"
#include "SubLevel.h"

// This is a Class That Acts as a Storage Container for SubLevel Objects
// Similar Containers to Main Level, However Are Specific to Static Objects From a Sub Level File
// All Changes Made in Editor Will Not Affect Sub Levels, But Will Instead Affect Contents From This Contianer
// Each Container Object Will Contain a Stack That Contains All Changes Made While Editing
// This Stack is Used for Undo/Redo Chains
// A New Item to the Stack is Only Updated if The Selector Deselects or Deletes an Object in Corrisponding SubLevel
// All Stacks for All Instances an Unsaved Level Container is Controlled by a Master Stack in Another Class Specifically for handling the Undo/Redo Chain
// This Container Will Persist Until The Levels Are Saved
// Even if the Corrisponding SubLevel Becomes Unloaded, This Container Will Persist
// If a Corrisponding SubLevel is Needed to Load Again During Editing, This Container Will Load in Its Place
// Upon Loading a Level for the First Time, Data Will be Stored in Memory Seperate From Stack

namespace Render
{
	class Container;
}

namespace Render::Objects
{
	class UnsavedLevel : public UnsavedBase
	{
		// Possible Change to Method of Switching Between Instances (More Memory / Performance Friendly)
		// Since Each Instance Can Only be Accessed From the Undo / Redo Chain, Each Instance Only Needs the Changes Made From The Previous/Next Instance
		// Each Unsaved Level Only Has two Full Instances: The Unmodified Data and The Current Instance Data
		// Unmodied Data is Created Upon First Initialization of Unsaved Level, Current Instance is Immediatly Copied After
		// Instances Should Remian the Same With the Same Data and Arrays
		// To Get To Another Instance, Parse Through Corrisponding Change Array and Either Add, Modify, or Remove Objects From the Current Instance
		// If Moving Backwards Through Undo Chain (Undo), Use Previous Change Data. If Moving Forwards Through Undo Chain (Redo), Use Next Change Data
		// If Unmodified Data is To be Added to the Stack, Find and Store All Neccessary Changes to Make to Convert Current Instance to Unmodified Instance, Then Copy Unmodified to Current
		// The Change Controller Should Remain Unchanged
		// Doing This Should Allow Less Memory to be Used and Allow Switching Between Instances to be Easier and More Efficient
		// This Could Allow The Amount of Instances in the Undo/Redo Chain to be Increased Drastically

	private:

		// IDEA: If Selected Unmodified is False, Create Yellow Border Around Level to Signify It was Changed
		//       If Selected Unmodified is True and There are Instances in Stack, Create Blue Border around Level
		//       If Selected Unmodified is True and The Stack is Empty, Show No Level Border
		//       If Currently No Border Criteria is met, and Level was Saved, Show Green Border
		//       These Colors Should be Similar to Visual Studio Colors in Respect to Making Changes in a File

		// Path to Object Data
		std::string object_path = "";

		// Path to Editor Configuration Data
		std::string editor_path = "";

		// Construct Unmodified Data Helper
		void constructUnmodifiedDataHelper(ObjectsInstance& instance);

		// Build Objects In Level Helper
		void buildObjectsLevelHelper(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, ObjectsInstance& instance, glm::vec2& object_offset);

		// Build Objects In GUI Helper
		void buildObjectsGUIHelper(Object::Object** objects, ObjectsInstance& instance);

		// Generator to Build Objects
		void buildObjectsGeneratorLevel(std::vector<DataClass::Data_Object*>& data_object_array, Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, Object::Active* active_array, uint16_t& active_index, Object::Object* parent, glm::vec2 position_offset);

		// Generator to Build Objects
		void buildObjectsGeneratorGUI(std::vector<DataClass::Data_Object*>& data_object_array, Object::Object** objects, uint16_t& index, Object::Object* parent, glm::vec2 position_offset);

		// Return Pointer to Shape Data
		Shape::Shape* getShapePointer(Editor::Selector* selector);

		// Function to Add Objects While Transversing
		void addWhileTraversing(DataClass::Data_Object* data_object, MOVE_WITH_PARENT move_with_parent);

		// Function to Remove Objects While Traversing
		void removeWhileTraversing(DataClass::Data_Object* data_object);

		// Function to Perform Object-Specific Changes After an Undo/Redo
		// Currently Not Doing Anything Right Now, Might Change Later
		void updatePostTraverse();

		// Remove Instance in Change List
		void removeChainListInstance();

	public:

		// Coordinates of Level
		int16_t level_x = 0;
		int16_t level_y = 0;
		int8_t level_version = 0;

		// Pointer to the Container Object
		Container* main_container = nullptr;

		// Initialize Object
		UnsavedLevel(glm::vec2& sizes, Container* level);

		// Deconstructor
		~UnsavedLevel();

		// Construct Unmodified Data As a Sublevel in a Level
		void constructUnmodifiedDataLevel(int16_t x, int16_t y, uint8_t z, float width, float height, std::string level_data_path, std::string editor_level_data_path);

		// Construct Unmodified Data As a Single GUI
		void contructUnmodifiedDataGUI(std::string gui_path);

		// Build Objects to Main Level
		void buildObjectsLevel(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, glm::vec2& object_offset);

		// Build Objects to GUI
		void buildObjectsGUI(Object::Object** objects);

		// Write Current Instance to File
		void write(bool& save);

		// Transfer An Object From Another Unsaved Level
		void transferObject(DataClass::Data_Object* data_object);

		// Return Masks of Desired Type
		void returnMasks(DataClass::Data_Object*** masks, int& masks_size, uint8_t type, DataClass::Data_Object* match);

		// Return Terrain Objects At Layer
		void returnTerrainObjects(DataClass::Data_Terrain*** terrain_objects, int& terrain_objects_size, uint8_t layer, DataClass::Data_Object* match);

		// Return Object Header
		uint16_t returnObjectHeader();

		// Return Object Header Helper
		void returnObjectHeaderHelper(DataClass::Data_Object* dobject);

		// Tests if an Object can be Successfully Added to the Object
		bool testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child);

		// Draw Object State Visualizer
		void drawVisualizer();

		// Update the Outline Model In Case of Wrapping
		void updateModelMatrix();
	};
}

#endif

#endif
