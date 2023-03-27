#pragma once
#ifndef UNSAVED_GROUP_H
#define UNSAVED_GROUP_H

#include "UnsavedBase.h"

// This File Will Handle the Storage of Changes Made to Objects
// That Have a Parent-Child Relationship. Since These Parent-Child
// Objects Are Declared in the Level File, They Will be Seperate From
// The Complex Physics Objects. A Parent-Child Relationship Will be
// Layed Out Similar to Java Inheritance, Where A Child Can Only Have
// 1 Parent, But a Parent Can Have Multiple Children and A Child Can
// Can Have Children As Well.
// 
// In the Level File, an Extra Byte Will be Put After the Identifier 
// That Determines the Number of Children To Be Added to the Object.
// If That Byte is Non-Zero, the Currently Read  Object Will be Added
// to a Stack and Any Further Objects Read Will be  Added as a Child 
// to That Object Until All Allocated Memory for Children Is Fullfilled. 
//
// For the Selector, Collisions Must be Recursively Checked With the Mouse
// in order to Select the Children of an Object. When Editing the Object,
// If the Object is a Parent, All Children and Children of Children Will Be 
// Moved with the Parent, but Will Not be Resized With the Parent (Probably).
// Any Modifications to Children Will NOT Affect the Parent. In the Editor
// Window, Child Objects Will be Easily Accessible from Parent Objects.
//
// There Will be Two Methods to Create Parent-Child Relationships. The First
// Will be to Use a Key to Bind an Object to another Object, Probably Using a
// Modified Left-Click or Right-Click. The Relationship Can be Undone Using the
// Same Process. The Other Method will be to Utilize the yet-to-be-created List 
// of Objects in the Editor Window, Where the User can Drag an Object Onto Another
// Objec to Create the Parent-Child Relationship.
//
// IDEA: For Some Parent-Child Objects, There Should be an Option to Save the Group
// To a File, Allowing the Objects to be Reusable. This Will Probably Happen Utilizing
// A New Object Type, (Probably Will be Called a "Group Object"), That Will Utilize
// The Same Process to Create a Parent-Child Relationship, However, the Created
// Relationship Will be Saved to an External File Instead of the Level File. There
// Could Also be an Option to "Inline" These Files Into the Level Files When a Game 
// is Ready to be Deployed.
//
// IDEA 2: Expand All Object Identifiers to 4 bytes Where the Last Byte is the Child Count.
//
// IDEA 3: Use a Similar Outline to Determine The State of a Group as Used in the Unsaved
// Level, However, the Dimensions of the Outline Will be Determined Similarly to the
// Outline of a Selected Group in the Selector. This Outline Will also Recursively be
// Determined by the Children of Children

namespace Render::Objects
{
	class UnsavedGroup : public UnsavedBase
	{

	private:

		// A Simple Queue to Store Updated Parent Objects So Children Can be Moved Properly
		// Will Be a Singleton, Technically
		class ParentQueue
		{

		private:

			// Constant Size for Parents Array
			#define PARENTS_ARRAY_SIZE 10

			// Array for Parents 
			DataClass::Data_Object* parents_array[PARENTS_ARRAY_SIZE];

			// The Number of Parents Queued
			uint8_t queue_amount = 0;

		public:

			// Enqueue a Parent to the Queue
			void enqueueParent(DataClass::Data_Object* parent);

			// Perform Move Operations on Queued Parents
			void moveParents();
		};

		// Instance of the Parent Queue for All Group Objects
		static ParentQueue parent_queue;

		// The Pointer to the Parent of Children
		// Should be Updated With Every Selector Operation and Change Traversal
		DataClass::Data_Object* parent_pointer = nullptr;

		// Set Group Layer of a Child and Sub-Children
		void setChildLayer(DataClass::Data_Object* data_object, uint8_t new_layer);

		// Function to Add Objects While Transversing
		void addWhileTraversing(DataClass::Data_Object* data_object, bool move_with_parent);

		// Function to Remove Objects While Traversing
		void removeWhileTraversing(DataClass::Data_Object* data_object);

		// Function to Perform Object-Specific Changes After an Undo/Redo
		void updatePostTraverse();

		// Remove Instance in Change List
		void removeChainListInstance();

	public:

		// Contructor for Object
		UnsavedGroup(uint8_t initial_size);

		// Get the Number of Children in Group
		uint8_t getNumberOfChildren();

		// Get the Children Vector
		std::vector<DataClass::Data_Object*>& getChildren();

		// Function to Add a Child Object to Group
		void addChild(DataClass::Data_Object* new_child);

		// Recursively Set the Group Layer
		void recursiveSetGroupLayer(uint8_t layer);

		// Make All Children Orphans
		void makeOrphans();

		// Set Parent Object
		void setParent(DataClass::Data_Object* new_parent, bool move);

		// Function to Set the Parent Pointer of All Children
		void updateParentofChildren();

		// Disable Move With Parent of Objects
		void disableMoveWithParent(DataClass::Data_Object* data_object);

		// Add Parent From Level to Parent Queue
		static void enqueueLevelParent(DataClass::Data_Object* data_object);

		// Finalize Movement With Parent
		static void finalizeParentMovement();
	};
}

#endif
