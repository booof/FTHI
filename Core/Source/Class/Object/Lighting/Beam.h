#pragma once
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
		Beam(BeamData& beam_, LightData& light_, glm::vec2& offset);

		// Load Light Into Buffer
		void loadLight();

		// Update the Object
		void updateObject();

#ifdef EDITOR

		// Initialize Visualizer
		void initializeVisualizer();

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Slope and Y-Intercept of Line for Editing
		float Slope;
		float Intercept;

#endif

	};
}

#endif
