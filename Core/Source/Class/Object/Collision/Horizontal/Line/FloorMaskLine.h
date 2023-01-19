#pragma once
#ifndef FLOOR_MASK_LINE_H
#define FLOOR_MASK_LINE_H

#ifndef DLL_HEADER
#include "Object/Collision/Horizontal/FloorMask.h"
#include "Object/Collision/Horizontal/Line/HorizotnalLine.h"
#endif

namespace Object::Mask
{
	// Class for Floor Masks With Shape of Horizontal Line
	class Floor::FloorMaskLine : public Floor::FloorMask, public HorizontalLine
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
		static void info(Editor::ObjectInfo& object_info, std::string& name, HorizontalLineData& data, bool platform);
			
#endif

		// Test Collisions of Object
		bool testCollisions(glm::vec2 test_position, float error, bool fallthrough);

		// Get the Left and Right Edge Vertices
		void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right);

		// Function to Return Material
		void returnMaterial(int& material);

	public:

		// Initialize Object
		FloorMaskLine(HorizontalLineData& data_, bool& platform_);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();
	};
}


#endif
