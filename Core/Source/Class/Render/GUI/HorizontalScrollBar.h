#pragma once
#ifndef HORIZONTAL_SCROLL_BAR_H
#define HORIZONTAL_SCROLL_BAR_H

#include "ScrollBar.h"

// A Child of the ScrollBar Class that Scroll Horizontally
// Percent Increses from Left to Right

namespace Render::GUI
{
	// Horizontal ScrollBar Class
	class HorizontalScrollBar : public ScrollBar
	{
	private:
		
		// Default X Position of Bar
		float defaultX = 0.0f;

	public:

		// Constructor for Horizontally Scrolling Scroll Bar
		HorizontalScrollBar(float xPos, float yPos, float width, float height, float size, float per, int16_t bar_identifier);

		// Default Constructor
		HorizontalScrollBar();

		// Moves Scroll Bar if GUI is Moved
		void moveElement(float newX, float newY);

		// Test Collisions
		bool TestColloisions();

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Move Bar
		float Scroll(float newY);

		// Helper Function to Move Bar
		void ScrollHelper();

		// Move the Bar With a Change in Position
		void deltaScroll(float delta);
	};
}

#endif