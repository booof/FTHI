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
		Wire(uint32_t& uuid_, ObjectData& data_, WireData& wire_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

	};
}

#endif
