#pragma once
#ifndef HORIZONTAL_SCROLL_BAR_H
#define HORIZONTAL_SCROLL_BAR_H

#include "ScrollBar.h"

// A Child of the ScrollBar Class that Scroll Horizontally
// Percent Increses from Left to Right

namespace GUI
{
	// Horizontal ScrollBar Class
	class HorizontalScrollBar : public ScrollBar
	{
	private:
		
		// Default X Position of Bar
		float defaultX = 0.0f;

	public:

		// Constructor for Horizontally Scrolling Scroll Bar
		HorizontalScrollBar(float xPos, float yPos, float width, float height, float size, float per);

		// Default Constructor
		HorizontalScrollBar();

		// Moves Scroll Bar if GUI is Moved
		void moveElement(float newX, float newY);

		// Test Collisions
		bool TestColloisions();

		// Move Bar
		float Scroll(float newY);
	};
}

#endif