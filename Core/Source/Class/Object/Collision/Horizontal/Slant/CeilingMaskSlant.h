#ifndef CEILING_MASK_SLANT_H
#define CEILING_MASK_SLANT_H

#ifndef DLL_HEADER
#include "Object/Collision/Horizontal/CeilingMask.h"
#include "Object/Collision/Horizontal/Slant/Slant.h"
#endif

namespace Object::Mask
{
	// Class for Ceiling Mask With Shape of Sloped Line
	class Ceiling::CeilingMaskSlant : public Ceiling::CeilingMask, public Slant
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
		CeilingMaskSlant(SlantData& data_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Get the Left and Right Edge Vertices
		void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right);
	};
}

#endif
