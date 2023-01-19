#pragma once
#ifndef SPRING_MASS_H
#define SPRING_MASS_H

#ifndef DLL_HEADER
#include "SoftBody.h"
#endif

namespace Object::Physics::Soft
{
	// Soft Body Physics Object With Data Loaded From File
	class SpringMass : public SoftBody
	{
	public:

		// File Path
		std::string file_name = "NULL";
		std::string path = "";

		// The Map for the Nodes
		int16_t* node_map = nullptr;

		// The Skip Array for Nodes
		bool* skip_nodes = nullptr;

		// The Skip Array for Springs
		bool* skip_springs = nullptr;

		// Initialize Object
		SpringMass(uint32_t & uuid_, ObjectData& data_, std::string& file_name_);

		// Deconstructor
		~SpringMass();

		glm::vec2* pointerToPosition();

		// Read File
		void read();

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Secondary Selection Function
		void select2(Editor::Selector& selector);

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, std::string& file_name);

#endif
	};
}

#endif
