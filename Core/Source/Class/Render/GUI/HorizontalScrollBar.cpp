#include "HorizontalScrollBar.h"

#include "Globals.h"

#include "Vertices/Rectangle/RectangleVertices.h"

#include "Source\Events\EventListeners.h"

// Editor Options to get Scroll Speed
#include "Render\Editor\EditorOptions.h"

Render::GUI::HorizontalScrollBar::HorizontalScrollBar(float xPos, float yPos, float width, float height, float size, float per, int16_t bar_identifier)
{
	// Assign Default Bar Positions
	element_data.position.x = xPos;
	element_data.position.y = yPos;
	default_position = element_data.position.x;
	data.initial_percent = per;
	percent = per;
	data.size = size;
	element_data.element_type = Render::GUI::SCROLL_BAR;
	data.bar_identifier = bar_identifier;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Store the Size of the Background
	data.background_width = width;
	data.background_height = height;

	// Determine Minimum Position
	MaxPos = xPos;

	// Calculate Maximum Position the Bar can be
	MinPos = xPos - width;

	// Calculate the Size of the Bar
	bar_size = width * abs(width / size);
	size_difference = size - width;

	// Test if BarHeight is Greater than Object Height and Nullify Object
	if (bar_size >= width)
	{
		// Set Height of Bar to Full
		bar_size = width;

		// Nullify Percent Constant to Keep Percent at 0
		percent_constant = 0;

		// Nullify Bar Offset
		BarOffset = 0.0f;

		// Set Bar Pos to Max
		bar_pos = MaxPos;
	}

	// Calculate Normal Variables
	else
	{
		// Caclulate the Percentage Constant
		percent_constant = 100 / (element_data.position.x - (MinPos + bar_size));

		// Calculate Offset if Bar Has Been Used Before
		bar_pos = MaxPos - ((1 - percent) * 100) / percent_constant;

		// Calculate the Offset Created by the Bar
		BarOffset = size_difference * percent;
	}

	model_Background = glm::translate(model_Background, glm::vec3(xPos, yPos, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(bar_pos, yPos, 0.0f));

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
	Vertices::Rectangle::genRectColor(-bar_size / 2, 0.0f, -1.1f, bar_size, height, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices);

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

Render::GUI::HorizontalScrollBar::HorizontalScrollBar() { BackgroundVAO = NULL; BackgroundVBO = NULL; ScrollVAO = NULL; ScrollVBO = NULL; }

void Render::GUI::HorizontalScrollBar::moveElement(float newX, float newY)
{
	// Store New Background Positions
	element_data.position.x = newX;
	element_data.position.y = newY;

	// Calculate New Positon Values
	bar_pos = (bar_pos - default_position) + newX; // New X pos is the New X Position Plus the Offset the Bar is Currently at
	MaxPos -= default_position - newX;
	MinPos -= default_position - newX;
	default_position = newX;

	// Calculate New Model Matricies
	model_Background = glm::translate(glm::mat4(1.0f), glm::vec3(newX, newY, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(bar_pos, newY, 0.0f));
}

bool Render::GUI::HorizontalScrollBar::TestColloisions()
{
	// Test if MouseY is inside Bar
	if (Global::mouseY / Global::zoom_scale > (double)element_data.position.y - data.background_height / 2 && Global::mouseY / Global::zoom_scale < (double)element_data.position.y + data.background_height / 2)
	{
		// Test if MouseX is inside Bar
		if (Global::mouseX / Global::zoom_scale > (double)bar_pos - bar_size && Global::mouseX / Global::zoom_scale < bar_pos)
		{
			// If Left Click, Start Scrolling
			if (Global::LeftClick)
			{
				is_being_modified = true;
				was_modified = true;
				scroll_offset = Global::mouseX / Global::zoom_scale - bar_pos;
			}

			// Return True if Mouse is Inside Bar
			return true;
		}
	}

	// Return False if Mouse is Not Inside Bar
	return false;
}

bool Render::GUI::HorizontalScrollBar::testMouseCollisions(float x, float y)
{
	// Note: Position is Right X, Center Y
	if (x <= element_data.position.x && x >= element_data.position.x - data.background_width)
	{
		float half_distance = data.background_height * 0.5f;
		if (y <= element_data.position.y + half_distance && y >= element_data.position.y - half_distance) {
			glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);
			Global::scroll_bar = this;
			return true;
		}
	}

	return false;
}

float Render::GUI::HorizontalScrollBar::Scroll(float newX)
{
	// Set New Y Position
	bar_pos = newX;

	// Test if Bar Goes Past Maximum Value
	if (bar_pos - bar_size < MinPos)
		bar_pos = MinPos + bar_size;

	// Test if Bar Goes Past Minimum Value
	else if (bar_pos > MaxPos)
		bar_pos = MaxPos;

	// Recalulate Model Matrix
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(bar_pos, element_data.position.y, 0.0f));

	// Calcualte the Bar Percent
	percent = 1 - (MaxPos - bar_pos) * percent_constant / 100;

	// Calculate the Offset Created by the Bar
	BarOffset = size_difference * percent;

	// Write Value of Percent
	if (percent_ptr != nullptr)
		*percent_ptr = percent;

	return percent;
}

void Render::GUI::HorizontalScrollBar::ScrollHelper()
{
	Scroll((Global::mouseX / Global::zoom_scale) - scroll_offset);
}

void Render::GUI::HorizontalScrollBar::deltaScroll(float delta)
{
	Scroll(bar_pos - delta * Global::editor_options->option_scroll_speed * data.background_width / data.size);
}
