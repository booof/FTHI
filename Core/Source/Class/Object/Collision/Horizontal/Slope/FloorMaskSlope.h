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

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, SlopeData& data, bool platform);

#endif

	private:

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error, bool fallthrough);

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
