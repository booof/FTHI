#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#else DLL_CODE
#include "C:\Users\ellio\source\repos\FTHI\Core\EngineLibs\Header\DLLHeader.h"
#endif

namespace Render::Camera
{
	// Camera Directions
	enum Directions : unsigned char
	{
		NORTH,
		SOUTH,
		EAST,
		WEST
	};

	class Camera
	{
	public:

		// Position of Camera
		glm::vec3 Position = glm::vec3(0.0f);

		// The Currently Activated Level
		glm::vec2 level;

		// Scales How Much the Camera is Zoomed 
		float zoomScale; // Default is 1 during normal gameplay

		// How Fast the Camera Moves
		float accelerationL;
		float accelerationR;
		float accelerationY;

		// Determines if the Camera Should Move or Not
		bool Stationary = false;

		// Variables Used to Stabilize Camera
		float constantX;

		// Determines if the Camera Should Fall
		bool fall;

		// View Matrix
		glm::mat4 view;

		// Determines if the Camera Moved
		bool moved = false;

#ifndef DLL_HEADER

		// Initialize Camera
		Camera(float initialX, float initialY, bool stationary);

#endif

		// Handles Directional Movement from Inputs
		void moveCamera(unsigned char direction);

#ifndef DLL_HEADER

		// Occours Whenever the Camera is Moved
		void updatePosition();

		// Only Use This Function While Debugging
		void testForCringe();

		// Update Positions for the Debugger
		void updateDebugPositions(bool override_move);

#endif

		// Set Camera Position
		void setPosition(glm::vec3 new_pos);
	};
}

#endif

