#pragma once
#ifndef SPOT_H
#define SPOT_H

#ifndef DLL_HEADER
#include "Class/Object/Lighting/LightBase.h"
#endif

namespace Object::Light::Spot
{
	// Struct for Spot Light Data
	struct SpotData
	{
		// Angle1
		float angle1;

		// Angle2
		float angle2;

		// Linear Coefficient
		float linear;

		// Quadratic Coefficient
		float quadratic;

		// Direction of Light
		glm::vec4 direction;
	};

	// Light that Travels in a Direction and Follows Attenuation
	class Spot : public LightBase
	{
	public:

		// Data Specific for Spots
		SpotData spot;

		// Initialize Object
		Spot(SpotData& spot_, LightData& light_, glm::vec2& offset);

		// Load Light Into Shader
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

#endif

	};
}

#endif

