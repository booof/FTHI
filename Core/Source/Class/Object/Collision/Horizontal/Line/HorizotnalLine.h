#ifndef HORIZONTAL_LINE_H
#define HORIZONTAL_LINE_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Object::Mask
{
	// Struct for Horizontal Line Data
	struct HorizontalLineData
	{
		// Position of Line
		glm::vec2 position;

		// Width of the Line
		float width;

		// Material of Object
		int material;

		// Script of Object
		int script;
	};

	// Horizontal Line Collision Shape
	class HorizontalLine
	{
	protected:

		// Data for Line
		HorizontalLineData data;

		// Edge X Positions
		float x_left, x_right;

		// Initilize Horizontal Line
		HorizontalLine(HorizontalLineData& data_);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color);

#endif

	};
}

#endif
