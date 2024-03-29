#pragma once
#ifndef POINT_H
#define POINT_H
	
#ifndef DLL_HEADER
#include "Class/Object/Lighting/LightBase.h"
#endif

namespace Object::Light::Point
{
	// Struct for Point Light Data
	struct PointData
	{
		// Linear Coefficient
		float linear;

		// Quadratic Coefficient
		float quadratic;
	};

	// Light That Travels in All Directions and Follows Attenuation
	class Point : public LightBase
	{
		// Data Specific for Points
		PointData point;

	public:

		// Initialize Object 
		Point(PointData& point_, LightData& light_, glm::vec2& offset);

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
