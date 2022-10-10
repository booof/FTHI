#ifndef SLOPE_H
#define SLOPE_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Object::Mask
{
	// Struct for Slope Data
	struct SlopeData
	{
		// Position of Object (Midpoint of Shape)
		glm::vec2 position;

		// Width of Object
		float width;

		// Height of Object
		float height;

		// Material of Object
		int material;

		// Script of Object
		int script;
	};

	// Slope Collisions Shape
	class Slope
	{
	protected:

		// Data for Slope
		SlopeData data;

		// Left X Position
		float x_left;

		// Right X Position
		float x_right;

		// Slope of Curve
		float slope;

		// Sign of Curve
		char height_sign;

		// Half Height of Curve
		float half_height;

		// Initialize Slope
		Slope(SlopeData& data_);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color);

#endif

	};
}

#endif
