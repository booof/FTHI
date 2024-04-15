#pragma once
#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "Element.h"

// IDEA: The Percent Paremeter in the Constructor Should be a Pointer to the Percent Variable
// The Percent Variable Should be Replaced Entirely by the Percent Pointer

namespace Render::GUI
{
	// Identifiers for Vertical and Horizontal Bars
	enum SCROLL_TYPES : uint8_t
	{
		VERTICAL = 0,
		HORIZONTAL
	};

	// Data for ScrollBars
	struct ScrollData
	{
		// Size of Bar Background
		float background_width = 0.0f, background_height = 0.0f;

		// Size of the Plane the Bar Scrolls
		float size = 0.0f;

		// Initial Percent of Bar (Utilized on Startup)
		float initial_percent = 0;

		// The Identifier of the Scroll Bar
		int16_t bar_identifier = -1;
	};

	// Allows GUIs to Scroll
	class ScrollBar : public Element
	{
	public:

		// Update Function
		bool updateElement();

		// Link Value Through a Pointer
		void linkValue(void* value_ptr);

		// Update Bar
		void blitzElement();

		// Temp Function for Drawing Until Master Element Model Matrix Interactions are Implemented
		void blitzElement2(glm::mat4& matrix);

		// Test Collisions
		virtual bool TestColloisions() = 0;

		// Move Bar
		virtual float Scroll(float newY) = 0;

		// Helper Function to Move Bar
		virtual void ScrollHelper() = 0;

		// Get the Percent of the Bar
		float getPercent();

		// Get the GUI Position Offset from Scroll Bar
		float getOffset();

		// Reset Bar to Defualt Position
		void resetBar();

		// Provide Pointer for Bar (For Master Element Only)
		float* getOffsetPointer();

		// Move the Bar With a Change in Position
		virtual void deltaScroll(float delta) = 0;

		// Returns if Being Used
		bool isBeingUsed();

		// Get the Identifier of the Bar
		int16_t getIdentifier();

	protected:

		// The Data for the Bar
		ScrollData data;

		// The Percent of the Bar That Has Been Scrolled
		float percent = 0.0f;

		// The Bar Offset
		float BarOffset = 0.0f;

		// Determines if Current Object is Being Modified by User
		bool is_being_modified = false;

		// The Pointer to the Value Connected to Percent
		float* percent_ptr = nullptr;

		// Min Corrdinate Positions
		float MinPos;

		// Max Coordinate Positions
		float MaxPos;

		// Percentage Constant
		float percent_constant = 0;

		// The Size Difference
		float size_difference = 0.0f;

		// The Resting Position of the Bar (When Percent is 0)
		float default_position = 0.0f;

		// The Size of the Actual Bar Object That Moves
		float bar_size = 0.0f;

		// The Current Position of the Bar
		float bar_pos = 0.0f;

		// Model Matrix for Background
		glm::mat4 model_Background = glm::mat4(1.0f);

		// Model Matrix for ScrollBar
		glm::mat4 model_Scroll = glm::mat4(1.0f);

		// Nullified View Matrix for GUI
		glm::mat4 view_null = glm::mat4(1.0f);

		// Array Objects
		GLuint BackgroundVAO, BackgroundVBO;
		GLuint ScrollVAO, ScrollVBO;
	};

	// The Offset for Scrolling
	inline float scroll_offset = 0.0f;
}

#endif
