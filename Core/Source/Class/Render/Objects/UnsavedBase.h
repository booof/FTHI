#pragma once
#ifndef UNSAVED_BASE_H
#define UNSAVED_BASE_H

#include "Macros.h"
#include "ExternalLibs.h"
#include "Render\Container\Container.h"

// This is a Base Class for All Unsaved Object Types.
//
// The Main Reason Why this Class Exists is to Allow the
// Unsaved List of the Change Controller Can Contain All
// Types of Unsaved Objects at Once. Another Reason is to
// Copy the Slave Stack Between All Groups to Reduce Code
// Duplication



// Ideas for the Data File Standardization
// This Will be the Same for Both .DAT and .EDT Files
// File Header Should be Usable as a Structure
// Usable For Both Level and Complex Files

// HEADER:
// 3 Bytes: File Signiture (Will be "DAT" for .DAT or "EDT" for .EDT)
// 3 Bytes: File Mode (LVL for Levels, CPX for Group Objects, HNG for Hinges, SBD for Soft Bodies)
// 1 Byte:  Development Mode (D for Development, R for Release, Release and Development Files Will Probably Differ)
// 5 Bytes: Project Hash (Will be a Char and Floating Point Value. Mostly for LVL Types)
// 4 Bytes: int, number of Objects in the File
// 4 Bytes: int, number of Bytes in the File
// 4 Bytes: short, Checksum (Will Probably Implement, Used to Prevent Tampering of File)

// DATA:
// N Bytes Determined by "Number of Bytes in the File" Value

// TAIL:
// 4 Bytes: ENDF

// Idea for Project Hash
// First Byte is the Number of Characters in the Project Name
// Next Values are a Floating Point Hash Value of the Project Name
// Value Will be Determined by a Hash Function
// Should Try to Make the Hash Unique to Each Project Name
// If a Project Encounters a Mismatched Hash in a File, That File Will
// Be Marked as Invalid, and the User Will be Prompted to 

// Note: I Believe Making a Single Dynamic Memory Call With the Number
// Of Bytes in the File Is More Efficient Than Allocating Dynamic Memory
// For Each Individual Object, If my 357 Knowledge is Sufficient. In Doing
// So, The Call Will Return a Single Buffer That Can Fit All Objects in
// the File. When Reading from the File, Instead of Calling New on Each
// Object, Each Object is Created on This Buffer at the Next Available 
// Object Space, Until All Objects are Read. A Counter Will be Used to
// Place These Objects, Incremented After Each Objects using sizeof()
// 
// Should Attempt the Same for Sublevels and Generating of Objects

// Checksum:
// Will Essentially be the All of the Bytes, Except the Checksum, Added
// Together, With Overflow Enabled. Will Definitely be Checked in Development
// Files, Maybe Not in Release Files. Invalid Checksums Result in Immediate
// Deallocation of Resources and Throws an Error

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
	struct Active;
	class Object;
}

namespace Render::Objects
{
	// File Header
	struct DataHeader
	{
		char         signiture[3];
		char         mode[3];
		char         development;
		uint8_t      project_size;
		float        project_hash;
		unsigned int object_count;
		unsigned int byte_count;
		unsigned int checksum;
	};

	// Unsaved Base Object
	class UnsavedBase
	{

	public:

		// The Two Types of Changes That Can Occour
		// Describes the Process that Caused the Change, NOT What Happens During an Undo
		enum CHANGE_TYPES : uint8_t
		{
			NULCHANGE = 0,
			ADD,
			REMOVE
		};

		// Individual Object Modification
		struct Change
		{
			// Pointer to Data Object At Identifier is to be Replaced With
			DataClass::Data_Object* data = nullptr;

			// The Total Movement this Object Had in Real Coordinates During a Change
			// Will Replace the "New Object Pos - Old Object Pos" Method of Moving Children
			// Using Real Coordinates in Case of Changes That Result in Movement In/Out of Groups
			glm::vec2 change_offset = glm::vec2(0.0f, 0.0f);

			// The Type of Change
			uint8_t change_type = 0;
		};

	protected:

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

			// Get Instance from Specified Direction
			Changes* returnInstanceDirectionSpecified(bool backwards);

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

		// The Lambdas that Generate and Return a Data Class Based on Object Identifier
		DataClass::Data_Object* lambdaDataObject(uint8_t object_identifier[4]);

		// Creates a New Change List
		void generateChangeList();

		// Change Vertex Colors to Modified
		void changeToModified();

		// Change Vertex Colors to Unmodified
		void changeToUnmodified();

		// Change Vertex Colors to Saved
		void changeToSaved();

		// Function to Add Objects While Transversing
		virtual void addWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset) = 0;

		// Function to Remove Objects While Traversing
		virtual void removeWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset) = 0;

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

		// Create a New Change by Appending a New Object
		void createChangeAppend(DataClass::Data_Object* data_object, glm::vec2 final_offset);

		// Create a New Change by Removing an Object
		void createChangePop(DataClass::Data_Object* data_object_to_remove, Object::Object* real_object);

		// Reset a Change List in the Event it is Canceled
		void resetChangeList();

		// Prepare All Modified Objects in Change Prior to a Redo/Undo
		void prepareChangeTraversal(bool backwards);

		// Traverse through Slave Stack
		void traverseChangeList(bool backward);

		// Clear Flags of Modified Objects after a Redo/Undo
		void endChangeTraversal(bool backwards);

		// Remove Instance in Change List
		virtual void removeChainListInstance() = 0;

		// Tests if an Object can be Successfully Added to the Object
		virtual bool testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child) = 0;

		// Finalize a New Change List. Returns True if a Change List was Created
		bool finalizeChangeList();

		// Get the Current Change List
		std::vector<Change*>* getChanges();

		// Remove a DataObject From an Instance With Changes
		void yeetObjectFromInstance(DataClass::Data_Object* object);

		// Returns True if an Object Exisits in Current Instance
		bool testObjectExists(DataClass::Data_Object* test_object);
	};
}

#endif
