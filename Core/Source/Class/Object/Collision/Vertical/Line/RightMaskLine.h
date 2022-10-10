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

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		RightMaskLine(VerticalLineData& data_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Top and Bottom Edge Vertices
		void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom);
	};
}

#endif
