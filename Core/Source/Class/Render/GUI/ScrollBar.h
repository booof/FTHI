#ifndef SCROLL_BAR_H
#define SCROLL_BAR_H

#include "ExternalLibs.h"

// Allows GUIs to Scroll
class ScrollBar
{
public:

	// Value of yPos of Bar Over Max yPos
	float percent = 0;

	// Positions of Bar
	float BarPosX, BarPosY;

	// The Bar Offset
	float BarOffset = 0.0f;

	ScrollBar(float xPos, float yPos, float width, float height, float size, float per);

	// Default Constructor
	ScrollBar();

	// Moves Scroll Bar if GUI is Moved
	void Move(float newX, float newY);

	// Test Collisions
	bool TestColloisions();

	// Move Bar
	float Scroll(float newY);

	// Update Bar
	void Update();

private:

	// Default Y Position of Bar
	float DefaultY = 0;

	// Min Corrdinate Positions
	float MinPos;

	// Max Coordinate Positions
	float MaxPos;

	// Size of Bar
	float BarWidth, BarHeight;

	// Percentage Constant
	float percent_constant = 0;

	// The Height Difference
	float height_difference = 0.0f;

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

#endif
