#pragma once
#ifndef TRIGGER_MASK_H
#define TRIGGER_MASK_H

#ifndef DLL_HEADER
#include "Class/Object/Collision/CollisionMask.h"
#endif

// This Mask is 

namespace Object::Mask::Trigger
{
	// Trigger Check Types
	enum CHECK_TYPE
	{
		NONE,
		ENTITY_ONLY,
		SPECIFIC_ENTITY_ONLY,
		PHYSICS_ONLY,
		SPECIFIC_PHYSICS_ONLY,
		ALL,
		SPECIFIC_ALL
	};

	// Struct for Trigger Mask Data
	struct TriggerData
	{
		// Position of Object (Lower Left Corner)
		glm::vec2 position;

		// Width of Rectangle
		float width;

		// Height of Rectangle
		float height;

		// Type of Object That Mask Checks For
		CHECK_TYPE check_type = CHECK_TYPE::NONE;

		// The Identifier of Specific Object to Check For (If Specific Object is Check Type)
		// Leave as 0 if Checking for General Objects
		int specific_identifier = 0;

		// Function That Gets Invoked
		int function;

		// Script
		int script;
	};

	// Collision Mask That Executes a Trigger Function
	class TriggerMask : public CollisionMask
	{
		// Trigger Data
		TriggerData data;

		// Right X Value
		float x_right;

		// Top Y Value
		float y_top;

		// Pointer to Function
		void (*trigger)(SubObject* object);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer();

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

#endif

		// Test Basic Collisions
		bool testCollisions(glm::vec2 test_position, float error);

		// Retrun Null Material
		void returnMaterial(int& material);

		// Test Trigger Event
		void testTrigger(glm::vec2 test_position, SubObject* object);

	public:

		// Initialize Object
		TriggerMask(TriggerData& data_);

#ifdef EDITOR

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, TriggerData& data);

#endif

		// Return Pointer to Position
		glm::vec2* pointerToPosition();
	};
}

#endif
