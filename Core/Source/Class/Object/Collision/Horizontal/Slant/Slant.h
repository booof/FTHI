#ifndef SLANT_H
#define SLANT_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Object::Mask
{
	// Struct for Slant Data
	struct SlantData
	{
		// Position of Slant
		glm::vec2 position;

		// Second Position of Slant
		glm::vec2 position2;

		// Material of Object
		int material;

		// Script of Object
		int script;
	};

	// Slant Collision Shape
	class Slant
	{
	protected:

		// Data for Slant
		SlantData data;

		// Slope of Slant
		float slope;
	
		// Y-Intercept of Slope
		float intercept;

		// Initilize Shape
		Slant(SlantData& data_);

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color);

#endif

	};
}

#endif
