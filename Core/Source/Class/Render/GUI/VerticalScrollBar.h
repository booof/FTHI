#pragma once
#ifndef VERTICAL_SCROLL_BAR_H
#define VERTICAL_SCROLL_BAR_H

#include "ScrollBar.h"

// A Child of the ScrollBar Class that Scroll Vertically
// Percent Increses from Left to Right

namespace Render::GUI
{
	// Horizontal ScrollBar Class
	class VerticalScrollBar : public ScrollBar
	{
	private:

		// Default X Position of Bar
		//float defaultY = 0.0f;

	public:

		// Constructor for Vertically Scrolling Scroll Bar
		VerticalScrollBar(float xPos, float yPos, float width, float height, float size, float per, int16_t bar_identifier);

		// Default Constructor
		VerticalScrollBar();

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
