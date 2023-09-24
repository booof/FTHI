#include "HorizontalScrollBar.h"

#include "Globals.h"

#include "Vertices/Rectangle/RectangleVertices.h"

GUI::HorizontalScrollBar::HorizontalScrollBar(float xPos, float yPos, float width, float height, float size, float per)
{
	// Assign Default Bar Positions
	BarPosX = xPos;
	BarPosY = yPos;
	DefaultPosition = BarPosX;
	percent = per;

	// Determine Minimum Position
	MaxPos = xPos;

	// Calculate the Size of the Bar
	BarWidth = width * abs(width / size);
	BarHeight = height;
	size_difference = size - width;

	// Calculate Maximum Position the Bar can be
	MinPos = xPos - width;

	// Test if BarHeight is Greater than Object Height and Nullify Object
	if (BarWidth > width)
	{
		// Set Height of Bar to Full
		BarWidth = width;

		// Nullify Percent Constant to Keep Percent at 0
		percent_constant = 0;

		// Nullify Bar Offset
		BarOffset = 0.0f;
	}

	// Calculate Normal Variables
	else
	{
		// Caclulate the Percentage Constant
		percent_constant = 100 / (BarPosX - (MinPos + BarWidth));

		// Calculate Offset if Bar Has Been Used Before
		BarPosX = MaxPos - ((1 - percent) * 100) / percent_constant;

		// Calculate the Offset Created by the Bar
		BarOffset = size_difference * percent;
	}

	model_Background = glm::translate(model_Background, glm::vec3(xPos, yPos, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(BarPosX, BarPosY, 0.0f));

	// Temporary Vertices
	float vertices[42];

	// Create Background Object

	// Generate Background Vertices
	Vertices::Rectangle::genRectColor(-width / 2, 0.0f, -1.2f, width, height, glm::vec4(0.75f, 0.75f, 0.75f, 1.0f), vertices);

	// Initialize backgroundVBO and VAO Objects
	glGenVertexArrays(1, &BackgroundVAO);
	glGenBuffers(1, &BackgroundVBO);

	// Bind BackgroundVAO
	glBindVertexArray(BackgroundVAO);

	// Bind BackgroundVBO and Assign it to Vertices
	glBindBuffer(GL_ARRAY_BUFFER, BackgroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Send Position Data to Shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Shaders
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind BackgroundVBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind BackgroundVAO
	glBindVertexArray(0);

	// Create Scroll Object

	// Generate Scroll Vertices
	Vertices::Rectangle::genRectColor(-BarWidth / 2, 0.0f, -1.1f, BarWidth, height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices);

	// Initialize ScrollVBO and VAO objects
	glGenVertexArrays(1, &ScrollVAO);
	glGenBuffers(1, &ScrollVBO);

	// Bind ScrollVAO
	glBindVertexArray(ScrollVAO);

	// Bind ScrollVBO and Assign to Vertices
	glBindBuffer(GL_ARRAY_BUFFER, ScrollVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Send Position Data to Shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Shaders
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind ScrollVBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind ScrollVAO
	glBindVertexArray(0);
}

GUI::HorizontalScrollBar::HorizontalScrollBar() { BarPosX = NULL; BarPosY = NULL; MinPos = NULL; MaxPos = NULL; BarWidth = NULL; BarHeight = NULL; BackgroundVAO = NULL; BackgroundVBO = NULL; ScrollVAO = NULL; ScrollVBO = NULL; }

void GUI::HorizontalScrollBar::moveElement(float newX, float newY)
{
	// Calculate New Positon Values
	BarPosY = newY;
	BarPosX = (BarPosX - DefaultPosition) + newX; // New X pos is the New X Position Plus the Offset the Bar is Currently at
	MaxPos -= DefaultPosition - newX;
	MinPos -= DefaultPosition - newX;
	DefaultPosition = newX;

	// Calculate New Model Matricies
	model_Background = glm::translate(glm::mat4(1.0f), glm::vec3(newX, BarPosY, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(BarPosX, BarPosY, 0.0f));
}

bool GUI::HorizontalScrollBar::TestColloisions()
{
	// Test if MouseY is inside Bar
	if (Global::mouseY / Global::zoom_scale > (double)BarPosY - BarHeight / 2 && Global::mouseY / Global::zoom_scale < (double)BarPosY + BarHeight / 2)
	{
		// Test if MouseX is inside Bar
		if (Global::mouseX / Global::zoom_scale > (double)BarPosX - BarWidth && Global::mouseX / Global::zoom_scale < BarPosX)
		{
			// If Left Click, Start Scrolling
			if (Global::LeftClick)
			{
				is_being_modified = true;
				was_modified = true;
				scroll_offset = Global::mouseX / Global::zoom_scale - BarPosX;
			}

			// Return True if Mouse is Inside Bar
			return true;
		}
	}

	// Return False if Mouse is Not Inside Bar
	return false;
}

float GUI::HorizontalScrollBar::Scroll(float newX)
{
	// Set New Y Position
	BarPosX = newX;

	// Test if Bar Goes Past Maximum Value
	if (BarPosX - BarWidth < MinPos)
		BarPosX = MinPos + BarWidth;

	// Test if Bar Goes Past Minimum Value
	else if (BarPosX > MaxPos)
		BarPosX = MaxPos;

	// Recalulate Model Matrix
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(BarPosX, BarPosY, 0.0f));

	// Calcualte the Bar Percent
	percent = 1 - (MaxPos - BarPosX) * percent_constant / 100;

	// Calculate the Offset Created by the Bar
	BarOffset = size_difference * percent;

	// Write Value of Percent
	*percent_ptr = percent;

	return percent;
}
