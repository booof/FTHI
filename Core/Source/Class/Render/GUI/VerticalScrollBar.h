#ifndef VERTICAL_SCROLL_BAR_H
#define VERTICAL_SCROLL_BAR_H

#include "ScrollBar.h"

// A Child of the ScrollBar Class that Scroll Vertically
// Percent Increses from Left to Right

namespace GUI
{
	// Horizontal ScrollBar Class
	class VerticalScrollBar : public ScrollBar
	{
	private:

		// Default X Position of Bar
		//float defaultY = 0.0f;

	public:

		// Constructor for Vertically Scrolling Scroll Bar
		VerticalScrollBar(float xPos, float yPos, float width, float height, float size, float per);

		// Default Constructor
		VerticalScrollBar();

		// Moves Scroll Bar if GUI is Moved
		void moveElement(float newX, float newY);

		// Test Collisions
		bool TestColloisions();

		// Move Bar
		float Scroll(float newY);
	};
}

#endif
