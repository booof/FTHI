#ifndef DIRECTIONAL_H
#define DIRECTIONAL_H

#ifndef DLL_HEADER
#include "Class/Object/Lighting/LightBase.h"
#endif

namespace Object::Light::Directional
{
	// Struct for Directional Light Data
	struct DirectionalData
	{
		// Second Pair of Coords
		glm::vec2 position2;
	};

	// Light That Travels in A Direction from a Line and is not Affected by Attenuation
	class Directional : public LightBase
	{
		// Data Specific for Directoinal Light
		DirectionalData directional;

		// Direction of Light Rays
		glm::vec4 light_direction;
		glm::vec4 line_direction;
		float direction;

	public:

		// Initialize Object
		Directional(DirectionalData& directional_, LightData light_);

		// Load Light Into Buffer
		void loadLight();

		// Update the Object
		void updateObject();

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer();

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Wtire to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Slope and Y-Intercept of Line for Editing
		float Slope;
		float Intercept;

#endif

	};
}

#endif

