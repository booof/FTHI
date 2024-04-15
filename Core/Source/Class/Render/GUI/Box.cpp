#include "Box.h"
#include "Render\Struct\DataClasses.h"

#include "Class/Render/GUI/ToggleGroup.h"
#include "Class\Render\Editor\ObjectInfo.h"

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

// Shaders
#include "Render\Shader\Shader.h"

// Transformations for Clipping
#include "Source\Algorithms\Transformations\Transformations.h"

void Render::GUI::Box::storePositions()
{
	// Get Lower Left Coordinates
	lower_left.x = element_data.position.x - data.width * 0.5f;
	lower_left.y = element_data.position.y - data.height * 0.5f;

	// Store Position of Text
	if (data.centered)
		text_x = element_data.position.x;
	else
		text_x = lower_left.x + 0.5f;
	text_y = lower_left.y + 1.0f;
}

void Render::GUI::Box::initializeVertices()
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Vertices
	float vertices[42];

	// Set Data for Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 3, NULL, GL_STATIC_DRAW);

	// Get Outline Vertices
	Vertices::Rectangle::genRectColor(element_data.position.x, element_data.position.y, data.zpos, data.width + 0.6f, data.height + 0.6f, data.outline_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Get Background Vertices
	Vertices::Rectangle::genRectColor(element_data.position.x, element_data.position.y, data.zpos + 0.01f, data.width, data.height, data.background_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(vertices), vertices);

	// Get Highlighter Vertices
	Vertices::Rectangle::genRectColor(element_data.position.x, element_data.position.y, data.zpos + 0.02f, data.width - 0.1f, data.height - 0.2f, data.highlight_color, vertices);
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

void Render::GUI::Box::updateText()
{
	// Update Text if Needed
	if (data.mode >= 4 && data.mode <= 9 && !texting)
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

	// If Editing Text, Reset Offset
	if (texting)
		data.button_text.updateRenderText();
}

Render::GUI::Box::Box(ElementData& data1, BoxData& data2)
{
	// Store Box Data
	element_data = data1;
	data = data2;

	// Store Data in Advanced String
	data.button_text.setAdvancedValues(data.width - 1.5f, 0.1f, data.centered);

	// Store Positions of Object
	storePositions();

	// Generate Vertices
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Create Vertices
	initializeVertices();
}

Render::GUI::Box::Box() {}

void Render::GUI::Box::moveElement(float new_x, float new_y)
{
	// Store New Coordinates
	element_data.position = glm::vec2(new_x, new_y);

	// Store Positions of Object
	storePositions();

	// Update Position in Buffer Data
	initializeVertices();
}

void Render::GUI::Box::linkValue(void* value)
{
	// Function Box
	if (data.mode == FUNCTION_BOX)
		setFunctionPointer(*static_cast<std::function<void()>*>(value));

	// Data Box
	else
		setDataPointer(value);
}

void Render::GUI::Box::setDataPointer(void* data_pointer_)
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
	if (data.mode == GENERAL_TEXT_BOX || data.mode == ALPHABETICAL_TEXT_BOX)
	{
		std::string* string_pointer = static_cast<std::string*>(data_pointer);
		data.button_text.setString(*string_pointer);
	}
}

void Render::GUI::Box::setFunctionPointer(std::function<void()> funct_)
{
	funct = funct_;
}

void Render::GUI::Box::groupBox(ToggleGroup* grouper_, uint8_t index)
{
	grouper = grouper_;
	group_index = index;
}

void Render::GUI::Box::setGroupFalse()
{
	if (data.mode == GROUPED_BOX)
	{
		state = false;
	}
}

bool Render::GUI::Box::updateElement()
{
	// Stop Updating if Another Object is Being Updated
	if (modified_by_user)
		return false;

	// Default Highligher is False
	highlighted = false;

	// Get Mouse Positions
	float mouseX = gui_mouse_position.x;
	float mouseY = gui_mouse_position.y;

	// Test Drop Down Bar First
	if (data.mode == BOX_MODES::DROP_DOWN_BOX && Box::DropDownActivePtr == this && Box::DropDownBar.isBeingUsed())
	{
		float og_mouseY = Global::mouseY;
		Global::mouseY = gui_mouse_position.y * Global::zoom_scale;
		modified_by_user = true;
		bool temp = DropDownBar.updateElement();
		Global::mouseY = og_mouseY;
		return temp;
	}

	// Test if Mouse is Inside Box
	else if (mouseX > lower_left.x && mouseX < lower_left.x + data.width)
	{
		// Inside Normal Box
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

		// Inside Menu Box
		// IDEA: Move The Selection of the Menu to Another Function Only Accessable Through the Master Element
		else if (Box::DropDownActivePtr == this)
		{
			// Reset Selected Menu Index to -1, In Case Nothing is Selected
			Box::DropDownMousedItem = -1;

			// Temporarily Modify MouseY For Testing Scroll Bar
			// No Longer Needed Once Master Elements are Finished
			float og_mouseY = Global::mouseY;
			Global::mouseY = gui_mouse_position.y * Global::zoom_scale;

			// Test if Moused Over Scroll Bar
			if (Box::DropDownBar.updateElement())
			{
				Global::mouseY = og_mouseY;
				return true;
			}

			// Test if Y is Inside Menu
			else if (mouseY < lower_left.y && mouseY > lower_left.y - drop_down_data->max_menu_height)
			{
				Global::mouseY = og_mouseY;

				// TODO: Add Variable to Master Element of the Currently Highlighted Bar, So
				// That Bar Can be Scrolled Using the Mouse Wheel.
				// When in Menu, Scrolling Scrolls the Menu

				// Get the Index the Mouse is Selecting
				Box::DropDownMousedItem = (uint8_t)floor((lower_left.y - mouseY + Box::DropDownBar.getOffset()) / drop_down_data->item_height);

				// Prevent Index From Going Past Item Count
				if (Box::DropDownMousedItem >= drop_down_data->item_count)
					Box::DropDownMousedItem = -1;

				// If Left Click, Set to False
				if (Global::LeftClick)
				{
					// Disable Left Click
					Global::LeftClick = false;

					// Set to False
					setFalse();
				}

				return true;
			}
			Global::mouseY = og_mouseY;
		}
	}

	return false;
}

void Render::GUI::Box::setTrue()
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
		selected_text->assignText(&data.button_text, glm::vec2(text_x, text_y), data.mode - 4, this);
		state = true;
		texting = true;
		break;
	}
	// Toggle Box
	case TOGGLE_BOX:
	{
		state = true;
		if (data_pointer != nullptr)
		{
			bool* bool_pointer = static_cast<bool*>(data_pointer);
			*bool_pointer = true;
		}
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
	// Drop Down Box
	case DROP_DOWN_BOX:
	{
		accessDropDown();
		state = true;
		break;
	}
	}
}

void Render::GUI::Box::setFalse()
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
		if (data_pointer != nullptr)
		{
			std::string* string_pointer = static_cast<std::string*>(data_pointer);
			*string_pointer = data.button_text.getString();
		}
		break;
	}
	// Alphabetical Text Box
	case ALPHABETICAL_TEXT_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data_pointer != nullptr)
		{
			std::string* string_pointer = static_cast<std::string*>(data_pointer);
			*string_pointer = data.button_text.getString();
		}
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
		if (data_pointer != nullptr)
		{
			float* number_pointer = static_cast<float*>(data_pointer);
			*number_pointer = Source::Algorithms::Common::convertStringToFloat(data.button_text.getString());
		}
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
		if (data_pointer != nullptr)
		{
			float* number_pointer = static_cast<float*>(data_pointer);
			*number_pointer = Source::Algorithms::Common::convertStringToFloat(data.button_text.getString());
		}
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
		if (data_pointer != nullptr)
		{
			int* int_pointer = static_cast<int*>(data_pointer);
			*int_pointer = Source::Algorithms::Common::convertStringToInt(data.button_text.getString());
		}
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
		if (data_pointer != nullptr)
		{
			int* int_pointer = static_cast<int*>(data_pointer);
			*int_pointer = Source::Algorithms::Common::convertStringToInt(data.button_text.getString());
		}
		break;
	}
	// File Path Box
	case FILE_PATH_BOX:
	{
		state = false;
		texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		if (data_pointer != nullptr)
		{
			std::string* string_pointer = static_cast<std::string*>(data_pointer);
			*string_pointer = data.button_text.getString();
		}
		break;
	}
	// Toggle Box
	case TOGGLE_BOX:
	{
		state = false;
		if (data_pointer != nullptr)
		{
			bool* bool_pointer = static_cast<bool*>(data_pointer);
			*bool_pointer = false;
		}
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
	// Drop Down Box
	case DROP_DOWN_BOX:
	{
		// Set the Active Drop Down Box to NULL and Update Selected Item
		Box::DropDownActivePtr = nullptr;
		if (Box::DropDownMousedItem != -1 && data_pointer != nullptr) {
			int* int_pointer = static_cast<int*>(data_pointer);
			*int_pointer = Box::DropDownMousedItem;
			data.button_text = std::string(drop_down_data->text[Box::DropDownMousedItem]);
		}
		state = false;
		break;
	}
	}

	// If Not a Function Box, And Closer is Available, Activate Closer
	if (funct != nullptr && data.mode != FUNCTION_BOX)
		funct();
}

bool Render::GUI::Box::getState()
{
	return state;
}

std::string Render::GUI::Box::getText()
{
	return data.button_text.getString();
}

void Render::GUI::Box::blitzElement()
{
	// Bind Vertex Object
	glBindVertexArray(VAO);

	// Draw Object
	glDrawArrays(GL_TRIANGLES, 0, 12 + 6 * (highlighted || state));

	// Unbind Vertex Object
	glBindVertexArray(0);
}

void Render::GUI::Box::blitzText(float offset)
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderText(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y + offset, 2.4f, data.text_color, true);
}

void Render::GUI::Box::blitzOffsetText()
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderTextOffset(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y, 2.4f, data.text_color, element_data.is_static);
}

void Render::GUI::Box::blitzGlobalText()
{
	// Update Text if Needed
	updateText();

	// Draw Text
	Source::Fonts::renderTextGlobal(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y, 2.4f, data.text_color);
}

bool Render::GUI::Box::testMouseCollisions(float x, float y)
{
	// Get Half Width and Height
	float half_width = data.width * 0.5f;
	float half_height = data.height * 0.5f;

	// Test if Inside X-Axis
	if (element_data.position.x - half_width < x && element_data.position.x + half_width > x)
	{
		// Test if Inside Y-Axis
		if (element_data.position.y - half_height < y && element_data.position.y + half_height > y)
		{
			return true;
		}
	}

	return false;
}

void Render::GUI::Box::storeDropDownData(DropDownData* new_drop_down_data)
{
	// Store Data
	drop_down_data = new_drop_down_data;

	// Update the Current Text
	data.button_text = std::string(new_drop_down_data->text[new_drop_down_data->current_index]);

	// If This Box is Active, Redo Menu
	if (Box::DropDownActivePtr == this)
	{
		Box::DropDownActivePtr = nullptr;
		accessDropDown();
	}
}

void Render::GUI::Box::accessDropDown()
{
	// Determines if the Drop Down Menu Has Been Initialized
	static bool drop_down_initialized = false;

	// If Drop Down Menu Has Not Been Initialized, Do So
	if (!drop_down_initialized)
	{
		// Set Flag
		drop_down_initialized = true;

		// Generate Vertex Object
		glGenVertexArrays(1, &Box::DropDownVAO);
		glGenBuffers(1, &Box::DropDownVBO);

		// Bind the Vertex Object
		glBindVertexArray(Box::DropDownVAO);
		glBindBuffer(GL_ARRAY_BUFFER, Box::DropDownVBO);

		// Allocate 3 Rectangles for Menu
		// TODO: Add 2 More Rectangles When Scroll Bar VAO is Merged With Main GUI VAO
		// TODO: Possibly Add 1 Rectangle Per Item to Make Menu Look Better
		glBufferData(GL_ARRAY_BUFFER, 504, nullptr, GL_STATIC_DRAW);

		// Bind Position Vertices
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Bind Color Vertices
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		// Determine the Ratio for the Bar Offset
	}

	// If the Currently Active Box is This, Do Nothing
	else if (Box::DropDownActivePtr == this)
		return;

	// Vertex Buffer to Hold 1 Rectangle
	float vertices[42] = { 0 };

	// Calculate the Base Y-Position of Menu
	float menu_y = element_data.position.y - (data.height + drop_down_data->max_menu_height) * 0.5f;
	
	// Bind the Vertex Object
	glBindVertexArray(Box::DropDownVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Box::DropDownVBO);

	// Generate Menu Outline
	Vertices::Rectangle::genRectColor(element_data.position.x, menu_y + 0.3f, data.zpos, data.width + 0.6f, drop_down_data->max_menu_height + 0.3f, data.outline_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

	// Generate Menu Background
	Vertices::Rectangle::genRectColor(element_data.position.x, menu_y, data.zpos + 0.01f, data.width, drop_down_data->max_menu_height, drop_down_data->menu_background_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

	// Generate Menu Outline
	Vertices::Rectangle::genRectColor(element_data.position.x, element_data.position.y - (data.height + drop_down_data->item_height) * 0.5f, data.zpos + 0.02f, (data.width - drop_down_data->bar_width) * 0.9f, drop_down_data->item_height * 0.9f, drop_down_data->menu_highlight_color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 336, 168, vertices);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate the Scroll Bar
	Box::DropDownBar = VerticalScrollBar(element_data.position.x + data.width * 0.5f - drop_down_data->bar_width * 0.5f,
		element_data.position.y - data.height * 0.5f - 0.3f,
		drop_down_data->bar_width,
		drop_down_data->max_menu_height,
		drop_down_data->item_count * drop_down_data->item_height,
		0, -1);

	// Clear the Item List
	Box::DropDownStrings.clear();

	// Resize Item List
	Box::DropDownStrings.resize(drop_down_data->item_count);

	// Generate the Advanced Strings to be Rendered
	for (int i = 0; i < drop_down_data->item_count; i++) 
	{
		Box::DropDownStrings[i] = std::string(drop_down_data->text[i]);
		Box::DropDownStrings.at(i).setAdvancedValues(data.width - 1.0f - drop_down_data->bar_width, drop_down_data->item_height * 0.48f, data.centered);
	}

	// Set the Initial Moused Index to -1
	Box::DropDownMousedItem = -1;

	// Generate the Corners of the Clipping Window
	Box::DropDownClipCorners.x = Source::Algorithms::Transformations::transformStaticScreenCoordsX(lower_left.x);
	Box::DropDownClipCorners.y = Source::Algorithms::Transformations::transformStaticScreenCoordsY(menu_y - drop_down_data->max_menu_height * 0.5f);
	Box::DropDownClipCorners.z = Source::Algorithms::Transformations::transformStaticScreenWidth(data.width);
	Box::DropDownClipCorners.w = Source::Algorithms::Transformations::transformStaticScreenHeight(drop_down_data->max_menu_height);

	// Set This as the Active Drop Down Box
	Box::DropDownActivePtr = this;
}

void Render::GUI::Box::drawDropDownHelper()
{
	// Temporary Way to Get Vertical Offset (Will Change Once Master Element is Finished)
	float v_offset = Global::mouseY / Global::zoom_scale - gui_mouse_position.y;

	// Bind the Vertex Object
	glBindVertexArray(Box::DropDownVAO);

	// Draw the Menu
	glDrawArrays(GL_TRIANGLES, 0, 12);

	// Begin Clipping
	glScissor(Box::DropDownClipCorners.x, Box::DropDownClipCorners.y + Source::Algorithms::Transformations::transformStaticScreenHeight(v_offset), Box::DropDownClipCorners.z, Box::DropDownClipCorners.w);

	// Get Where the First Element Should Start Based on Scroll Bar

	// Generate Model Matrix for Item Box

	// Draw Item Boxes (Once Added)

	// Get the Current Model Matrix for the Box
	glm::mat4 temp;
	glGetUniformfv(Global::colorShaderStatic.Program, Global::modelLocColorStatic, glm::value_ptr(temp));
	glm::mat4 temp2 = glm::translate(temp, glm::vec3(0.0f, Box::DropDownBar.getOffset(), 0.0f));

	// If an Item is Highlighted, Draw the Highlighter
	if (Box::DropDownMousedItem != -1)
	{
		// Generate Model Matrix for Highlighter
		glm::mat4 highligher_matrix = glm::translate(temp2, glm::vec3(0.0f, -drop_down_data->item_height * Box::DropDownMousedItem, 0.0f));

		// Send Model Matrix
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highligher_matrix));

		// Draw the Highlighter
		glDrawArrays(GL_TRIANGLES, 12, 6);
	}

	// Unbind Vertex Array
	glBindVertexArray(0);

	// Draw the Scroll Bar
	Box::DropDownBar.blitzElement2(temp);

	// End Clipping
	glScissor(0, 0, (GLuint)Global::screenWidth, (GLuint)Global::screenHeight);
}

void Render::GUI::Box::drawDropDownTextHelper()
{
	// Temporary Way to Get Vertical Offset (Will Change Once Master Element is Finished)
	float v_offset = Global::mouseY / Global::zoom_scale - gui_mouse_position.y;

	// Begin Clipping
	glScissor(Box::DropDownClipCorners.x, Box::DropDownClipCorners.y + Source::Algorithms::Transformations::transformStaticScreenHeight(v_offset), Box::DropDownClipCorners.z, Box::DropDownClipCorners.w);

	// Get Where the Text Should Start Based on Scroll Bar
	//float offset = -data.height * 0.5f - drop_down_data->item_height * 0.2f;
	float offset = -drop_down_data->item_height * 0.9f + Box::DropDownBar.getOffset();

	// Draw All Text
	for (Render::GUI::AdvancedString& string : Box::DropDownStrings)
	{
		Source::Fonts::renderText(string.getRenderString(), text_x + string.getOffset(), text_y + offset, string.getScale(), data.text_color, true);
		offset -= drop_down_data->item_height;
	}

	// End Clipping
	glScissor(0, 0, (GLuint)Global::screenWidth, (GLuint)Global::screenHeight);
}

void Render::GUI::Box::drawDropDownText()
{
	if (Box::DropDownActivePtr != nullptr)
		Box::DropDownActivePtr->drawDropDownTextHelper();
}

void Render::GUI::Box::drawDropDown()
{
	if (Box::DropDownActivePtr != nullptr)
		Box::DropDownActivePtr->drawDropDownHelper();
}

// Function to Read Data and Create an Object
Object::Object* DataClass::Data_BoxElement::genObject(glm::vec2& offset)
{
	Render::GUI::Box* new_box = new Render::GUI::Box(element_data, data);
	if (data.mode == Render::GUI::BOX_MODES::DROP_DOWN_BOX)
		new_box->storeDropDownData(drop_down_data);
	return new_box;
}

void DataClass::Data_BoxElement::writeObjectData(std::ofstream& object_file)
{
	// Write Normal Box Data
	object_file.write((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.write((char*)&data, sizeof(Render::GUI::BoxData));

	// If Drop Down Box, Write Drop Down Data
	if (data.mode == Render::GUI::BOX_MODES::DROP_DOWN_BOX)
	{
		// Update Character Count, If Needed
		int count = 0;
		for (int i = 0, j = 0; i < drop_down_data->item_count; i++)
		{
			for (j = 0; drop_down_data->text[i][j] != 0; j++)
				count++;
		}
		drop_down_data->character_count = count;

		// Write Header
		object_file.write((char*)drop_down_data, 64);

		// Write All Text
		// Note: 0th Index is of Text PTR is First Text, All Text is Next To Eachother
		object_file.write(*drop_down_data->text, drop_down_data->character_count);
	}
}

void DataClass::Data_BoxElement::readObjectData(std::ifstream& object_file)
{
	// Static Variable to Hold New Drop Down Data
	static Render::GUI::DropDownData temp_drop_data;

	// Read the Normal Box Data
	object_file.read((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.read((char*)&data, sizeof(Render::GUI::BoxData));

	// TODO: Test the Below Code

	// If Box Mode is Drop Down Menu, Get the Drop Down Menu Data
	if (data.mode == Render::GUI::BOX_MODES::DROP_DOWN_BOX)
	{
		// Read the Base Drop Down Data
		// 72 - 8 = 64 for Without the String List Pointer
		object_file.read((char*)&temp_drop_data, 64);

		// Allocate Memory for Full Drop Down Data
		// Size = 72 bytes for Struct, Item Count * Pointer Size for Item List, and Full Character List
		char* allocated_data = new char[72 + 8 * temp_drop_data.item_count + temp_drop_data.character_count];
		drop_down_data = (Render::GUI::DropDownData*)allocated_data;

		// Copy Read Drop Down Data
		*drop_down_data = temp_drop_data;
		drop_down_data->text = (char**)(allocated_data + 72);

		// Get Where the Text Starts and Read All Text
		char* text_start = allocated_data + 72 + 8 * temp_drop_data.item_count;
		object_file.read(text_start, temp_drop_data.character_count);
		drop_down_data->text[0] = text_start;

		// Parse Text and Get Pointers to Start of Each Item
		// End of An Item = 0
		uint16_t text_index = 0;
		uint8_t item_index = 1;
		for (; item_index < temp_drop_data.item_count; text_index++)
		{
			if (text_start[text_index] == 0)
			{
				drop_down_data->text[text_index] = text_start + text_index;
				item_index++;
			}
		}
	}
}

DataClass::Data_BoxElement::Data_BoxElement(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::ELEMENT;
	object_identifier[1] = Render::GUI::BOX;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
	element_data.element_type = Render::GUI::BOX;
}

void DataClass::Data_BoxElement::info(Editor::ObjectInfo& object_info)
{
	// Strings of Different Box Modes
	static std::string box_mode_strings[] = { "Null Box", "Toggle Box", "Function Box", "Drop Down Box", "General Text Box", "Alphabetical Text Box", "Numerical Text Box", "Absolute Numerical Text Box", "Integer Text Box", "Absolute Integer Text Box", "File Path Box", "Grouped Box" };

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Box", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addTextValue("Type: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &box_mode_strings[data.mode], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &element_data.position, false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addColorValue("Outline Color: ", glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), &data.outline_color, true);
	object_info.addColorValue("Background Color: ", glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), &data.background_color, true);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_BoxElement::makeCopy()
{
	return new Data_BoxElement(*this);
}

Render::GUI::BoxData& DataClass::Data_BoxElement::getBoxData()
{
	return data;
}

Render::GUI::DropDownData* DataClass::Data_BoxElement::getDropDownData()
{
	return drop_down_data;
}

void DataClass::Data_BoxElement::generateInitialValues(glm::vec2 initial_position, glm::vec2 initial_size)
{
	element_data.position = initial_position;
	data.width = initial_size.x;
	data.height = initial_size.y;
	data.button_text = std::string("New Button");
	data.mode = Render::GUI::BOX_MODES::NULL_BOX;
	element_data.is_static = false;
	data.centered = true;
	data.text_color = glm::vec4(0.0f, 0.0, 0.0f, 1.0f);
}

void DataClass::Data_BoxElement::renderText()
{
	// Text Calculated Positions
	float text_x, text_y;

	// Lower Left Box Position
	glm::vec2 lower_left;

	lower_left.x = element_data.position.x - data.width * 0.5f;
	lower_left.y = element_data.position.y - data.height * 0.5f;

	// Get Position of Text
	if (data.centered)
		text_x = element_data.position.x;
	else
		text_x = lower_left.x + 0.5f;
	text_y = lower_left.y + 1.0f;

	// Update Button Text
	data.button_text.setAdvancedValues(data.width - 1.5f, 0.1f, data.centered);

	// Assuming Offset Text
	Source::Fonts::renderTextOffset(data.button_text.getRenderString(), text_x + data.button_text.getOffset(), text_y, 2.4f, data.text_color, element_data.is_static);
}

// Initialize the Static Stuff
GLuint Render::GUI::Box::DropDownVAO;
GLuint Render::GUI::Box::DropDownVBO;
Render::GUI::VerticalScrollBar Render::GUI::Box::DropDownBar;
std::vector<Render::GUI::AdvancedString> Render::GUI::Box::DropDownStrings;
Render::GUI::Box* Render::GUI::Box::DropDownActivePtr;
uint8_t Render::GUI::Box::DropDownMousedItem;
glm::ivec4 Render::GUI::Box::DropDownClipCorners;
float Render::GUI::Box::DropDownBarRatio;
