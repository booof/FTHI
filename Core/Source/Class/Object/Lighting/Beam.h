#ifndef BEAM_H
#define BEAM_H

#ifndef DLL_HEADER
#include "LightBase.h"
#endif

namespace Object::Light::Beam
{
	// Struct for Beam Light Data
	struct BeamData
	{
		// Second Pair of Coords
		glm::vec2 position2;

		// Linear Coefficient
		float linear;

		// Quadratic Coefficient
		float quadratic;
	};

	// Directional Light That Follows Attenuation, Similar to a Laser
	class Beam : public LightBase
	{
		// Data Specific for Beam Light
		BeamData beam;

		// Normalized Direction Vector of Beam
		glm::vec2 direction;

	public:

		// Initialize Object
		Beam(BeamData& beam_, LightData& light_);

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

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, LightData& data, BeamData& beam);

		// Slope and Y-Intercept of Line for Editing
		float Slope;
		float Intercept;

#endif

	};
}

#endif
