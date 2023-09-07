#pragma once
#ifndef RIGHT_MASK_LINE_H
#define RIGHT_MASK_LINE_H

#ifndef DLL_HEADER
#include "Object/Collision/Vertical/RightMask.h"
#include "Object/Collision/Vertical/Line/VerticalLine.h"
#endif

namespace Object::Mask
{
	// Class for Right Wall Masks With Shape of Vertical Line
	class Right::RightMaskLine : public Right::RightMask, public VerticalLine
	{
	public:

#ifdef EDITOR

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

	private:

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error);

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		RightMaskLine(VerticalLineData& data_, glm::vec2& offset);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Top and Bottom Edge Vertices
		void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom);
	};
}

#endif
