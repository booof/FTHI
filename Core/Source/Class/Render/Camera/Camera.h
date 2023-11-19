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

		// Scales How Much the Camera is Zoomed 
		float zoomScale; // Default is 1 during normal gameplay

		// The Currently Activated Level
		glm::vec2 level;

		// How Fast the Camera Moves
		float accelerationL;
		float accelerationR;
		float accelerationY;

		// Variables Used to Stabilize Camera
		float constantX;

		// Determines if the Camera Should Move or Not
		bool Stationary = false;

		// Determines if the Camera Should Fall
		bool fall;

		// Determines if the Camera Moved
		bool moved = false;

		// Determines if Camera Should Wrap
		bool wrap = false;

		// Determines if the Camera Actually Wrapped
		bool wrapped = false;

		// View Matrix
		glm::mat4 view;

		// The Min/Max Values in Each Direction
		// -Horizontal, +Horizontal, -Vertical, +Vertical
		glm::vec4 scene_bounderies = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		// The Distance the Camera Can See In Each Direction From Center
		// Center Position to Position at Edge of Screen
		glm::vec2 camera_radius = glm::vec2(0.0f, 0.0f);

#ifndef DLL_HEADER

		// Initialize Camera
		Camera(float initialX, float initialY, bool stationary, bool wrap_, glm::vec4 scene_bounderies_);

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

