#pragma once
#ifndef LEFT_MASK_LINE_H
#define LEFT_MASK_LINE_H

#ifndef DLL_HEADER
#include "Class/Object/Collision/Vertical/LeftMask.h"
#include "Class/Object/Collision/Vertical/Line/VerticalLine.h"
#endif

namespace Object::Mask
{
	// Class for Left Wall Masks With Shape of Vertical Line
	class Left::LeftMaskLine : public Left::LeftMask, public VerticalLine
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
		LeftMaskLine(VerticalLineData& data_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Top and Bottom Edge Vertices
		void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom);
	};
}

#endif
