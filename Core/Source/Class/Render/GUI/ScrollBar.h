#pragma once
#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "Element.h"

// IDEA: The Percent Paremeter in the Constructor Should be a Pointer to the Percent Variable
// The Percent Variable Should be Replaced Entirely by the Percent Pointer

namespace GUI
{
	// Allows GUIs to Scroll
	class ScrollBar : public Element
	{
	public:

		// Value of yPos of Bar Over Max yPos
		float percent = 0;

		// Positions of Bar
		float BarPosX, BarPosY;

		// The Bar Offset
		float BarOffset = 0.0f;

		// Update Function
		bool updateElement();

		// Link Value Through a Pointer
		void linkValue(void* value_ptr);

		// Update Bar
		void blitzElement();

		// Test Collisions
		virtual bool TestColloisions() = 0;

		// Move Bar
		virtual float Scroll(float newY) = 0;

	protected:

		// Determines if Current Object is Being Modified by User
		bool is_being_modified = false;

		// The Pointer to the Value Connected to Percent
		float* percent_ptr = nullptr;

		// Default Position of Bar
		float DefaultPosition = 0;

		// Min Corrdinate Positions
		float MinPos;

		// Max Coordinate Positions
		float MaxPos;

		// Size of Bar
		float BarWidth, BarHeight;

		// Percentage Constant
		float percent_constant = 0;

		// The Size Difference
		float size_difference = 0.0f;

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
