#include "Box.h"

#include "Class/Render/GUI/ToggleGroup.h"

// Globals
#include "Globals.h"

// Functions
#include "Source/Algorithms/Common/Common.h"
#include "Source/Events/EventListeners.h"
#include "Source/Loaders/Fonts.h"

// Selected Text Object
#include "SelectedText.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

void GUI::Box::storePositions()
{
	// Get Lower Left Coordinates
	lower_left.x = data.position.x - data.width / 2.0f;
	lower_left.y = data.position.y - data.height / 2.0f;

	// Store Position of Text
	if (data.centered)
		text_x = data.position.x;
	else
		text_x = lower_left.x + 0.5f;
	text_y = lower_left.y + 1.0f;
}

void GUI::Box::initializeVertices()
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Vertices
	float vertices[42];

	// Set Data for Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 3, NULL, GL_STATIC_DRAW);

	// Get Outline Vertices
	Vertices::Rectangle::genRectColor(data.position.x, data.position.y, data.zpos, data.width + 0.6f, data.height + 0.6f, data.outline_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Get Background Vertices
	Vertices::Rectangle::genRectColor(data.position.x, data.position.y, data.zpos + 0.01f, data.width, data.height, data.background_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(vertices), vertices);

	// Get Highlighter Vertices
	Vertices::Rectangle::genRectColor(data.position.x, data.position.y, data.zpos + 0.02f, data.width - 0.1f, data.height - 0.2f, data.highlight_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) * 2, sizeof(vertices), vertices);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GUI::Box::updateText()
{
	// Update Text if Needed
	if (data.mode < 6 && !texting)
	{
		switch (data.mode)
		{

			// Basic and Alphabetical Text Boxes
		case GENERAL_TEXT_BOX:
		case ALPHABETICAL_TEXT_BOX:
		{
			std::string* string_pointer = static_cast<std::string*>(data_pointer);
			data.button_text.setString(*string_pointer);
			break;
		}

		// Numerical Text Boxes
		case NUMERICAL_TEXT_BOX:
		case ABSOLUTE_NUMERICAL_TEXT_BOX:
		{
			float* float_pointer = static_cast<float*>(data_pointer);
			data.button_text.setString(Source::Algorithms::Common::removeTrailingZeros(std::to_string(*float_pointer)));
			break;
		}

		// Integer Text Box
		case INTEGER_TEXT_BOX:
		case ABSOLUTE_INTEGER_TEXT_BOX:
		{
			int* int_pointer = static_cast<int*>(data_pointer);
			data.button_text.setString(std::to_string(*int_pointer));
			break;
		}

		}
	}
}

GUI::Box::Box(BoxData& data_)
{
	// Store Box Data
	data = data_;

	// Store Data in Advanced String
	data.button_text.setAdvancedValues(data.width - 1.5f, 0.1f, data.centered);

	// Store Positions of Object
	storePositions();

	// Generate Vertices
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Create Vertices
	initializeVertices();
}

GUI::Box::Box() {}

void GUI::Box::moveElement(float new_x, float new_y)
{
	// Store New Coordinates
	data.position = glm::vec2(new_x, new_y);

	// Store Positions of Object
	storePositions();

	// Update Position in Buffer Data
	initializeVertices();
}

void GUI::Box::linkValue(void* value)
{
	// Function Box
	if (data.mode == FUNCTION_BOX)
		setFunctionPointer(*static_cast<std::function<void()>*>(value));

	// Data Box
	else
		setDataPointer(value);
}

void GUI::Box::setDataPointer(void* data_pointer_)
{
	// Set Data Pointer
	data_pointer = data_pointer_;

	// If Toggle Box, Set Initial State
	if (data.mode == TOGGLE_BOX)
	{
		bool* bool_pointer = static_cast<bool*>(data_pointer);
		state = *bool_pointer;
	}

	// If Text Box, Set Initial Text
	if (data.mode < 2)
	{
		std::string* string_pointer = static_cast<std::string*>(data_pointer);
		data.button_text.setString(*string_pointer);
	}
}

void GUI::Box::setFunctionPointer(std::function<void()> funct_)
{
	funct = funct_;
}

void GUI::Box::groupBox(ToggleGroup* grouper_, uint8_t index)
{
	grouper = grouper_;
	group_index = index;
}

void GUI::Box::setGroupFalse()
{
	if (data.mode == GROUPED_BOX)
	{
		state = false;
	}
}

bool GUI::Box::updateElement()
{
	// Stop Updating if Another Object is Being Updated
	if (modified_by_user)
		return false;

	// Default Highligher is False
	highlighted = false;

	// Get Mouse Positions
	float mouseX = gui_mouse_position.x;
	float mouseY = gui_mouse_position.y;

	// Test if Mouse is Inside Box
	if (mouseX > lower_left.x && mouseX < lower_left.x + data.width)
	{
		if (mouseY > lower_left.y && mouseY < lower_left.y + data.height)
		{
			// Set Highlighter to True
			highlighted = true;

			// Set Selected Mouse Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If Left-Click, Set Flag to True
			if (Global::LeftClick)
			{
				// Disable Left Click
				Global::LeftClick = false;

				// If Null Box, Return
				if (data.mode == NULL_BOX)
					return true;

				// If This Object is Texting, Return
				if (texting)
				{
					// If File Box, Open File Explorer if Double Click
					if (data.mode == FILE_PATH_BOX && Global::DoubleClick)
					{
						// Disable Double Click
						Global::DoubleClick = false;

						// Generate Open File Dialogue
						OPENFILENAME file_dialogue = { 0 };
						TCHAR szFile[260] = { 0 };
						TCHAR szTitle[260] = { 0 };
						file_dialogue.lStructSize = sizeof(OPENFILENAME);
						file_dialogue.lpstrFile = szFile;
						file_dialogue.nMaxFile = sizeof(szFile);
						file_dialogue.lpstrFilter = ("Engine Data File\0*.DAT");
						file_dialogue.nFilterIndex = 1;
						file_dialogue.lpstrFileTitle = szTitle;
						file_dialogue.lpstrTitle = "Select A Valid Data File (.dat)";
						file_dialogue.nMaxFileTitle = sizeof(szTitle);
						file_dialogue.lpstrInitialDir = selected_text->getString().c_str();
						file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

						// Open the File Explorer
						if (GetSaveFileName(&file_dialogue) == TRUE)
						{
							// Get the File Path Without Extension
							std::string new_path = file_dialogue.lpstrFile;
							Source::Algorithms::Common::eraseFileExtension(new_path);

							// Update Selected Text
							selected_text->forceUpdateText(new_path);
						}
					}

					return true;
				}

				// Reset Text
				if (selected_text->isActive())
					selected_text->stopSelecting();

				// Perform State Change
				if (!state) { setTrue(); }
				else { setFalse(); }

				// Return True if State Changed
				return true;
			}
		}
	}

	return false;
}

void GUI::Box::setTrue()
{
	// Set Box to True
	switch (data.mode)
	{

	// Basic Text Box
	case GENERAL_TEXT_BOX:
	// Alphabetical Text Box
	case ALPHABETICAL_TEXT_BOX:
	// Numerical Box
	case NUMERICAL_TEXT_BOX:
	// Absolute Numerical Box
	case ABSOLUTE_NUMERICAL_TEXT_BOX:
	// Integer Box
	case INTEGER_TEXT_BOX:
	// Absolute Integer Box
	case ABSOLUTE_INTEGER_TEXT_BOX:
	// File Box
	case FILE_PATH_BOX:
	{
		glfwSetKeyCallback(Global::window, Source::Listeners::TypeCallback);
		selected_text->assignText(&data.button_text, glm::vec2(text_x, text_y), data.mode, this);
		state = true;
		texting = true;
		break;
	}
	// Toggle Box
	case TOGGLE_BOX:
	{
		state = true;
		bool* bool_pointer = static_cast<bool*>(data_pointer);
		*bool_pointer = true;
		break;
	}
	// Multiple Toggle Box
	case GROUPED_BOX:
	{
		grouper->changeState(group_index);
		state = true;
		break;
	}
	// Function Box
	case FUNCTION_BOX:
	{
		funct();
		break;
	}
	}
}

void GUI::Box::setFalse()
{
	// Turn Off Highlihting
	highlighted = false;

	// Set Box to False
	switch (data.mode)
	{
	// Basic Text Box
	case GENERAL_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		std::string* string_pointer = static_cast<std::string*>(data_pointer);
		*string_pointer = data.button_text.getString();
		break;
	}
	// Alphabetical Text Box
	case ALPHABETICAL_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		std::string* string_pointer = static_cast<std::string*>(data_pointer);
		*string_pointer = data.button_text.getString();
		break;
	}
	// Numerical Box
	case NUMERICAL_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data.button_text.getString() == "" || data.button_text.getString() == "-" || data.button_text.getString() == ".")
			data.button_text.getString() = "0";
		float* number_pointer = static_cast<float*>(data_pointer);
		*number_pointer = Source::Algorithms::Common::convertStringToFloat(data.button_text.getString());
		break;
	}
	// Absolute Numerical Box
	case ABSOLUTE_NUMERICAL_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data.button_text.getString() == "" || data.button_text.getString() == ".")
			data.button_text.getString() = "0";
		float* number_pointer = static_cast<float*>(data_pointer);
		*number_pointer = Source::Algorithms::Common::convertStringToFloat(data.button_text.getString());
		break;
	}
	// Integer Box
	case INTEGER_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data.button_text.getString() == "" || data.button_text.getString() == "-")
			data.button_text.getString() = "0";
		int* int_pointer = static_cast<int*>(data_pointer);
		*int_pointer = Source::Algorithms::Common::convertStringToInt(data.button_text.getString());
		break;
	}
	// Absolute Integer Box
	case ABSOLUTE_INTEGER_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data.button_text.getString() == "")
			data.button_text.getString() = "0";
		int* int_pointer = static_cast<int*>(data_pointer);
		*int_pointer = Source::Algorithms::Common::convertStringToInt(data.button_text.getString());
		break;
	}
	// File Path Box
	case FILE_PATH_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		std::string* string_pointer = static_cast<std::string*>(data_pointer);
		*string_pointer = data.button_text.getString();
		break;
	}
	// Toggle Box
	case TOGGLE_BOX:
	{
		state = false;
		bool* bool_pointer = static_cast<bool*>(data_pointer);
		*bool_pointer = false;
		break;
	}
	// Multiple Toggle Box
	case GROUPED_BOX:
	{
		// Dont't Do Anything
		break;
	}
	// Function Toggle Box
	case FUNCTION_BOX:
	{
		// Dont't Do Anything
		break;
	}
	}
}

bool GUI::Box::getState()
{
	return state;
}

std::string GUI::Box::getText()
{
	return data.button_text.getString();
}

void GUI::Box::blitzElement()
{
	// Bind Vertex Object
	glBindVertexArray(VAO);

	// Draw Object
	glDrawArrays(GL_TRIANGLES, 0, 12 + 6 * (highlighted || state));

	// Unbind Vertex Object
	glBindVertexArray(0);
}

void GUI::Box::blitzText(float offset)
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderText(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y + offset, 0.1f, data.text_color, true);
}

void GUI::Box::blitzOffsetText()
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderTextOffset(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y, 0.1f, data.text_color);
}

void GUI::Box::blitzGlobalText()
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderTextGlobal(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y, 0.1f, data.text_color);
}
