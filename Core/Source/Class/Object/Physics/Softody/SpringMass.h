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
		std::string path;

		// Initialize Object
		SpringMass(uint32_t & uuid_, ObjectData& data_, std::string& file_name_);

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

#endif
	};
}

#endif
