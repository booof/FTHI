#pragma once
#ifndef FLOOR_MASK_H
#define FLOOR_MASK_H

#ifndef DLL_HEADER
#include "Object/Collision/CollisionMask.h"
#endif

namespace Object::Mask::Floor
{
	// Class for Floor Collision Masks
	class FloorMask : public CollisionMask
	{
	protected:

		// Type of Object
		unsigned char type;

		// Determines if Object is a Platform
		bool platform;

	public:

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error);
		virtual bool testCollisions(glm::vec2 test_position, float error, bool fallthrough) = 0;

		// Test Collisions at Edge of Object
		bool testEdgeCollisions(glm::vec2& left_vertex, glm::vec2& right_vertex, float error, glm::vec2& returned_edge_pos);

		// Get the Left and Right Edge Vertices
		virtual void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right) = 0;
	};

	// Floor Mask Shapes
	class FloorMaskLine;
	class FloorMaskSlant;
	class FloorMaskSlope;
}

#endif
