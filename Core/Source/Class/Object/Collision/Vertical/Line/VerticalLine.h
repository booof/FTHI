#pragma once
#ifndef VERTICAL_LINE_H
#define VERTICAL_LINE_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Object::Mask
{
	// Struct for Vertical Line Data
	struct VerticalLineData
	{
		// Position of Object
		glm::vec2 position;

		// Height of Line
		float height;

		// Material of Object
		int material;

		// Script of Object
		int script;
	};

	// Vertical Line Collision Shape
	class VerticalLine
	{
	protected:

		// Data for Line
		VerticalLineData data;

		// Y Positions of Object
		float y_top, y_bottom;

		// Initialize Vertical Line
		VerticalLine(VerticalLineData data_);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color);

#endif

	};
}

#endif

