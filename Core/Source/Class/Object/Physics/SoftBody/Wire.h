#pragma once
#ifndef WIRE_H
#define WIRE_H

#ifndef DLL_HEADER
#include "SoftBody.h"
#endif

namespace Object::Physics::Soft
{
	// Wire Data
	struct WireData
	{
		// Second Position
		glm::vec2 position2;

		// Number of Nodes
		int node_instances;

		// Node Mass
		float Mass;

		// Node Health
		float Health;

		// Node Radius
		float Radius;

		// Spring Stiffness
		float Stiffness;

		// Spring Dampening
		float Dampening;

		// Rest Length of Entire Wire
		float total_rest_length;

		// Distance Needed to Break Segment of Wire
		float break_distance;
	};

	// Soft Body Physics Object With Data Loaded From File
	class Wire : public SoftBody
	{
		WireData wire;

	public:

		// Initialize Object
		Wire(uint32_t& uuid_, ObjectData& data_, WireData& wire_, glm::vec2& offset);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

#ifdef EDITOR

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

	};
}

#endif
