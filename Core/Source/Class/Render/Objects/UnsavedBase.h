#pragma once
#ifndef UNSAVED_BASE_H
#define UNSAVED_BASE_H

#include "Macros.h"
#include "ExternalLibs.h"

// This is a Base Class for All Unsaved Object Types.
//
// The Main Reason Why this Class Exists is to Allow the
// Unsaved List of the Change Controller Can Contain All
// Types of Unsaved Objects at Once. Another Reason is to
// Copy the Slave Stack Between All Groups to Reduce Code
// Duplication

namespace Editor
{
	class Selector;
}

namespace DataClass
{
	class Data_Object;
	class Data_Terrain;
}

namespace Shape
{
	class Shape;
}

namespace Object
{
	class Object;
}

namespace Render::Objects
{
	class UnsavedBase
	{

	protected:

		enum CHANGE_TYPES : uint8_t
		{
			ADD = 1,
			REMOVE
		};

		// Individual Object Modification
		struct Change
		{
			// The Type of Change
			uint8_t change_type;

			// Determines if Object Should Move With Parent
			bool move_with_parent = true;

			// Pointer to Data Object At Identifier is to be Replaced With
			DataClass::Data_Object* data;
		};

		// Instance Changes
		struct Changes
		{
			// Number of Changes Made
			uint16_t change_count = 0;

			// Changes Made
			std::vector<Change*> changes;
		};

		// Instance of Objects
		struct ObjectsInstance
		{
			// Number of Loaded Objects 
			// (Excluding Physics and Entities for Unsaved Levels)
			uint16_t number_of_loaded_objects = 0;

			// Array of Object Data
			std::vector<DataClass::Data_Object*> data_objects;
		};

		// Slave Stack
		class SlaveStack
		{
			// Size of Array
			uint8_t array_size = 10;

			// Items in Stack
			uint8_t stack_size = 0;

			// Tail of Stack
			uint8_t tail = 0;

			// Current Head of Stack
			uint8_t head = 0;

			// Current Index in Stack
			uint8_t stack_index = 0;

			// Array
			Changes** stack_array = nullptr;

		public:

			// Initialize Stack
			SlaveStack();

			// Add Instance to Stack
			uint8_t appendInstance(Changes* instance);

			// Get Instance From Stack
			Changes* returnInstance();

			// Returns True if Stack is Empty
			bool isEmpty();

			// Delete an Instance
			void deleteInstance(Changes* instance);

			// Delete Entire Stack
			void deleteStack();

			// Delete the Most Recent Instance
			void removeRecentInstance();

			// Delete the Most Recent Instance Without Deleting Objects
			void removeRecentInstanceWithoutDeletion();

			// Move Forward Through Change List
			void moveForward();

			// Move Backward Through Change List
			bool moveBackward();
		};

		// Stack to Add Children
		class AddChildrenStack
		{
			// Object That Combines a Data Object And Number of Children Left to Add
			struct AddChildrenObject {
				uint8_t number_of_children_left = 0;
				DataClass::Data_Object* object = nullptr;
			};

			// Size of the Array
			const uint8_t array_size = 10; // Should be a Constant Size

			// The Current Number of Objects in Stack
			uint8_t stack_size = 0;

			// The Object Stack Array
			AddChildrenObject stack_array[10]{ 0 };

		public:

			// Add Object to Stack
			bool addObject(DataClass::Data_Object* new_object, uint8_t number_of_children);

			// Add Child to Top Object in Stack
			bool addChild(DataClass::Data_Object* new_child_object);
		};

		// Slave Stack
		SlaveStack slave_stack;

		// Vertex Object for Unsaved Level State Outline
		GLuint VAO = 0, VBO = 0;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Determines if a Change List is Being Built
		bool making_changes = false;

		// Current Change List
		Changes* current_change_list = nullptr;

		// The Original Instance of Objects
		ObjectsInstance original_instance;

		// The Instance That Has Changes
		ObjectsInstance instance_with_changes;

		bool selected_unmodified = true;

		// Creates a New Change List
		void generateChangeList();

		// Change Vertex Colors to Modified
		void changeToModified();

		// Change Vertex Colors to Unmodified
		void changeToUnmodified();

		// Change Vertex Colors to Saved
		void changeToSaved();

		// Function to Add Objects While Transversing
		virtual void addWhileTraversing(DataClass::Data_Object* data_object, bool move_with_parent) = 0;

		// Function to Remove Objects While Traversing
		virtual void removeWhileTraversing(DataClass::Data_Object* data_object) = 0;

	private:

		// Change Vertex Colors
		void changeColors(float* color);

		// Make Changes Moving Forward Though Change List
		void moveForwardsThroughChanges(Changes* changes);

		// Make Inverse Changes Moving Backwards Through Change List
		void moveBackwardsThroughChanges(Changes* changes);

		// Function to Perform Object-Specific Changes After an Undo/Redo
		virtual void updatePostTraverse() = 0;

	public:

		// Index of Object in Change Controller List
		uint8_t unsaved_object_index = 0;

		// Determines if Object was Previously Saved
		bool saved = false;

		// Pointer to Active Objects Array
		Object::Object*** active_objects = nullptr;

		// Create a New Change by Appending a New Object
		void createChangeAppend(DataClass::Data_Object* data_object);

		// Create a New Change by Removing an Object
		void createChangePop(DataClass::Data_Object* data_object_to_remove);

		// Reset a Change List in the Event it is Canceled
		void resetChangeList();

		// Traverse through Slave Stack
		void traverseChangeList(bool backward);

		// Remove Instance in Change List
		virtual void removeChainListInstance() = 0;

		// Finalize a New Change List. Returns True if a Change List was Created
		bool finalizeChangeList();
	};
}

#endif
