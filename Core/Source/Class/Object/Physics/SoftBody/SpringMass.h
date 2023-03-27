#pragma once
#ifndef SPRING_MASS_H
#define SPRING_MASS_H

#ifndef DLL_HEADER
#include "SoftBody.h"
#endif

namespace DataClass
{
	class Data_SpringMass;
	class Data_SpringMassNode;
	class Data_SpringMassSpring;
}

namespace Object::Physics::Soft
{
	// Soft Body Physics Object With Data Loaded From File
	class SpringMass : public SoftBody
	{
	public:

		// File Path
		std::string file_name = "NULL";
		std::string path = "";

		// The Lists of Data Objects
		DataClass::Data_SpringMassNode** data_nodes;
		DataClass::Data_SpringMassSpring** data_springs;

		// The Map for the Nodes
		int16_t* node_map = nullptr;

		// The Skip Array for Nodes
		bool* skip_nodes = nullptr;

		// The Skip Array for Springs
		bool* skip_springs = nullptr;

		// Initialize Object
		SpringMass(uint32_t & uuid_, ObjectData& data_, std::string& file_name_, DataClass::Data_SpringMass* data_object_);

		// Deconstructor
		~SpringMass();

		glm::vec2* pointerToPosition();

		// Read File
		void read();

#ifdef EDITOR

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Secondary Selection Function
		void select3(Editor::Selector& selector);

#endif
	};
}

#endif
