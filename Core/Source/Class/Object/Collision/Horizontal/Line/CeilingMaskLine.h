#pragma once
#ifndef CEILING_MASK_LINE_H
#define CEILING_MASK_LINE_H

#ifndef DLL_HEADER
#include "Object/Collision/Horizontal/CeilingMask.h"
#include "Object/Collision/Horizontal/Line/HorizotnalLine.h"
#endif

namespace Object::Mask
{
	// Class for Ceiling Masks with Shape of Vertical Line
	class Ceiling::CeilingMaskLine : public Ceiling::CeilingMask, public HorizontalLine
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
		CeilingMaskLine(HorizontalLineData& data_, glm::vec2& offset);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Left and Right Edge Vertices
		void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right);
	};
}

#endif
