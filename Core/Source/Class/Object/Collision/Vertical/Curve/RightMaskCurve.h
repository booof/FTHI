#ifndef RIGHT_MASK_CURVE_H
#define RIGHT_MASK_CURVE_H

#ifndef DLL_HEADER
#include "Class/Object/Collision/Vertical/RightMask.h"
#include "Class/Object/Collision/Vertical/Curve/Curve.h"
#endif

namespace Object::Mask
{
	// Class for Right Wall Masks With Shape of Logarithmic Function
	class Right::RightMaskCurve : public Right::RightMask, public Curve
	{
	public:

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, CurveData& data);

#endif

	private:

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		RightMaskCurve(CurveData& data_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Top and Bottom Edge Vertices
		void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom);
	};
}

#endif
