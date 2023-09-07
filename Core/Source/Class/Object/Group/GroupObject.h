#pragma once
#ifndef GROUP_H
#define GROUP_H

#include "Object/Object.h"

// A Group Object That Can be Read From and Written to
// an External File. Allows for Groups of Objects to Be
// Stored Safely and Easily Reusable

namespace Render::Objects
{
	class UnsavedComplex;
}

namespace Object::Group
{
	struct GroupData
	{
		// Position of Group
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Script
		int script = 0;

		// Size of File Name
		uint8_t file_path_size = 0;
	};

	class GroupObject : public Object
	{
		// Group Data
		GroupData data;

		// The File of the Group
		std::string file_name = "NULL";
		std::string path = "";

		// Vertex Objects
		GLuint VAO, VBO;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		void initializeVisualizer();

		void updateObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

	public:

		GroupObject(GroupData& data_, std::string file_name_, Render::Objects::UnsavedComplex* complex_object, glm::vec2& offset);

		~GroupObject();

		bool testMouseCollisions(float x, float y);

#ifdef EDITOR

		// Return Position of Object
		glm::vec2 returnPosition();

		// Return the Group Object
		Render::Objects::UnsavedComplex* getComplexGroup();

#endif

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Draw Object
		void drawObject();

		// Update the Model Matrix for Visualizer
		void updateModelMatrix();
	};
}

#endif
