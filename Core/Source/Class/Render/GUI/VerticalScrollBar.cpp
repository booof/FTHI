#include "VerticalScrollBar.h"

#include "Globals.h"

#include "Vertices/Rectangle/RectangleVertices.h"

#include "Source\Events\EventListeners.h"

// Editor Options to get Scroll Speed
#include "Render\Editor\EditorOptions.h"

Render::GUI::VerticalScrollBar::VerticalScrollBar(float xPos, float yPos, float width, float height, float size, float per, int16_t bar_identifier)
{
	// Assign Default Bar Positions
	element_data.position.x = xPos;
	element_data.position.y = yPos;
	default_position = element_data.position.y;
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
	MaxPos = yPos;

	// Calculate Maximum Position the Bar can be
	MinPos = yPos - height;

	// Calculate the Size of the Bar
	bar_size = height * abs(height / size);
	size_difference = size - height;

	// Test if BarHeight is Greater than Object Height and Nullify Object
	if (bar_size >= height)
	{
		// Set Height of Bar to Full
		bar_size = height;

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
		percent_constant = 100 / (element_data.position.y - (MinPos + bar_size));

		// Calculate Offset if Bar Has Been Used Before
		bar_pos = MaxPos - (percent * 100) / percent_constant;

		// Calculate the Offset Created by the Bar
		BarOffset = size_difference * percent;
	}

	model_Background = glm::translate(model_Background, glm::vec3(xPos, yPos, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, bar_pos, 0.0f));

	// Temporary Vertices
	float vertices[42];

	// Create Background Object

	// Generate Background Vertices
	Vertices::Rectangle::genRectColor(0.0f, -height / 2, -1.2f, width, height, glm::vec4(0.75f, 0.75f, 0.75f, 1.0f), vertices);

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
	Vertices::Rectangle::genRectColor(0.0f, -bar_size / 2, -1.1f, width, bar_size, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices);

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

Render::GUI::VerticalScrollBar::VerticalScrollBar() { BackgroundVAO = NULL; BackgroundVBO = NULL; ScrollVAO = NULL; ScrollVBO = NULL; }

void Render::GUI::VerticalScrollBar::moveElement(float newX, float newY)
{
	// Store New Background Positions
	element_data.position.x = newX;
	element_data.position.y = newY;

	// Calculate New Bar Positon Values
	bar_pos = (bar_pos - default_position) + newY; // New Y pos is the New Y Position Plus the Offset the Bar is Currently at
	MaxPos -= default_position - newY;
	MinPos -= default_position - newY;
	default_position = newY;

	// Calculate New Model Matricies
	model_Background = glm::translate(glm::mat4(1.0f), glm::vec3(newX, newY, 0.0f));
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(newX, bar_pos, 0.0f));
}

bool Render::GUI::VerticalScrollBar::TestColloisions()
{
	// Test if MouseX is inside Bar
	if (Global::mouseX / Global::zoom_scale > (double)element_data.position.x - data.background_width / 2 && Global::mouseX / Global::zoom_scale < (double)element_data.position.x + data.background_width / 2)
	{
		// Test if MouseY is inside Bar
		if (Global::mouseY / Global::zoom_scale > (double)bar_pos - bar_size && Global::mouseY / Global::zoom_scale < bar_pos)
		{
			// If Left Click, Start Scrolling
			if (Global::LeftClick)
			{
				is_being_modified = true;
				was_modified = true;
				scroll_offset = Global::mouseY / Global::zoom_scale - bar_pos;
			}

			// Return True if Mouse is Inside Bar
			return true;
		}
	}

	// Return False if Mouse is Not Inside Bar
	return false;
}

bool Render::GUI::VerticalScrollBar::testMouseCollisions(float x, float y)
{
	// Note: Position is Center X, Top Y
	if (y <= element_data.position.y && y >= element_data.position.y - data.background_height)
	{
		float half_distance = data.background_width * 0.5f;
		if (x <= element_data.position.x + half_distance && x >= element_data.position.x - half_distance) {
			glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);
			Global::scroll_bar = this;
			return true;
		}
	}

	return false;
}

float Render::GUI::VerticalScrollBar::Scroll(float newY)
{
	// Set New Y Position
	bar_pos = newY;

	// Test if Bar Goes Past Maximum Value
	if (bar_pos - bar_size < MinPos)
		bar_pos = MinPos + bar_size;

	// Test if Bar Goes Past Minimum Value
	else if (bar_pos > MaxPos)
		bar_pos = MaxPos;

	// Recalulate Model Matrix
	model_Scroll = glm::translate(glm::mat4(1.0f), glm::vec3(element_data.position.x, bar_pos, 0.0f));

	// Calcualte the Bar Percent
	percent = (MaxPos - bar_pos) * percent_constant / 100;

	// Calculate the Offset Created by the Bar
	BarOffset = size_difference * percent;

	// Write Value of Percent
	if (percent_ptr != nullptr)
		*percent_ptr = percent;

	return percent;
}

void Render::GUI::VerticalScrollBar::ScrollHelper()
{
	Scroll((Global::mouseY / Global::zoom_scale) - scroll_offset);
}

void Render::GUI::VerticalScrollBar::deltaScroll(float delta)
{
	Scroll(bar_pos + delta * Global::editor_options->option_scroll_speed * data.background_height / data.size);
}
