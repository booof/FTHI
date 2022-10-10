#ifndef CURVE_H
#define CURVE_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Object::Mask
{
	// Struct for Curve Data
	struct CurveData
	{
		// Posiiton of Curve (Midpoint)
		glm::vec2 position;

		// Width of Curve
		float width;

		// Height of Curve
		float height;

		// Material of Object
		int material;

		// Script of Object
		int script;
	};

	// Curve Collision Shape
	class Curve
	{
	protected:

		// Curve Data
		CurveData data;

		// Modified Coordinates of Object
		glm::vec2 modified_coordinates;

		// Top Y Position of Object
		float y_top;

		// Slope of Curve
		float slope;

		// Amplitude of Curve
		float amplitude;

		// X Offset of Curve
		float x_offset;

		// Initialize Curve
		Curve(CurveData& data_);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color, char sign);

#endif

	};
}

#endif
