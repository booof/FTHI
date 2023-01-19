#pragma once
#ifndef ANCHOR_H
#define ANCHOR_H

#ifndef DLL_HEADER
#include "Object/Physics/PhysicsBase.h"
#endif

namespace Object::Physics::Hinge
{
	// Anchor Data
	struct AnchorData
	{
		// Position of Anchor
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Script Index
		int script = 0;
	};

	// The Unmoving Anchor Class That Is Used in Conjunction With Hinges
	class Anchor : public PhysicsBase
	{
		// Vertex Object
		GLuint VAO, VBO;

		// Model Matrix
		glm::mat4 model;

	public:

		// Anchor Data
		AnchorData data;

		// Initialize Object
		Anchor(uint32_t& uuid_, AnchorData& data_);

		// Update Object
		void updateObject();

		// Draw Object
		void drawObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, AnchorData& data);

#endif
	};
}

#endif
