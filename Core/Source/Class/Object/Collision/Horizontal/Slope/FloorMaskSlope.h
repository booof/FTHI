#pragma once
#ifndef FLOOR_MASK_SLOPE_H
#define FLOOR_MASK_SLOPE_H

#ifndef DLL_HEADER
#include "Class/Object/Collision/Horizontal/FloorMask.h"
#include "Class/Object/Collision/Horizontal/Slope/Slope.h"
#endif

namespace Object::Mask
{
	// Class for Floor Mask With Shape of Inverse Tangent Function
	class Floor::FloorMaskSlope : public Floor::FloorMask, public Slope
	{
	public:

#ifdef EDITOR

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

	private:

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error, bool fallthrough);

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Get the Left and Right Edge of Vertices
		void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		FloorMaskSlope(SlopeData& data_, bool& platform_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();
	};
}

#endif
