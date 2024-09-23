#pragma once
#ifndef UNSAVED_COLLECTION_H
#define UNSAVED_COLLECTION_H

#include "UnsavedBase.h"

// This is an Abstracted Class to Reduce Code
// Duplication and Provide and Interface For
// Unsaved Complex and Unsaved Group Objects

namespace Render::Objects
{
	// Types of Unsaved Collection
	enum class UNSAVED_COLLECTIONS : uint8_t
	{
		GROUP,
		COMPLEX
	};

	class UnsavedCollection : public UnsavedBase
	{

	public:

		// Recursively Set the Group Layer
		virtual void recursiveSetGroupLayer(int8_t layer) = 0;

		// Get the Number of Children in Group
		uint8_t getNumberOfChildren();

		// Get the Children Vector
		std::vector<DataClass::Data_Object*>& getChildren();

		// Function to Add a Child Object to Group
		void addChild(DataClass::Data_Object* new_child);

		// Disable Move With Parent of Objects
		void disableMoveWithParent(DataClass::Data_Object* data_object);

		// Make All Children Orphans
		void makeOrphans(Editor::Selector* selector, std::vector<DataClass::Data_Object*>& orphan_list);

		// Determines What the Collection Type is
		virtual UNSAVED_COLLECTIONS getCollectionType() = 0;

		// Set Parent During a Redo/Undo Without Moving Children
		virtual void setParentTraverseChangeNoMove(DataClass::Data_Object* new_parent) = 0;

		// Test if Object Is Not Added to a Higher Parent
		static bool preventCircularGroups(DataClass::Data_Object* parent, DataClass::Data_Object* test_child);
	};
}

#endif
