#pragma once
#ifndef FLOOR_MASK_SLANT_H
#define FLOOR_MASK_SLANT_H

#ifndef DLL_HEADER
#include "Object/Collision/Horizontal/FloorMask.h"
#include "Object/Collision/Horizontal/Slant/Slant.h"
#endif

namespace Object::Mask
{
	// Class for Floor Mask With Shape of Sloped Line
	class Floor::FloorMaskSlant : public Floor::FloorMask, public Slant
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
		
		// Get the Left and Right Edge Vertices
		void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		FloorMaskSlant(SlantData& data_, bool& platform_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();
	};
}

#endif
