#include "SelectedText.h"
#include "Globals.h"
#include "AdvancedString.h"
#include "Source/Algorithms/Common/Common.h"
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Render/Shader/Shader.h"
#include "Events/EventListeners.h"
#include "Source/Loaders/Audio.h"

void Render::GUI::SelectedText::handleInvalidInput()
{
	// IDEAS:
	// Play a Sound
	// Make Cursor Blink Rapidly

	// Stop Playing the Exclaimation Sound or Program will Crash
	alSourceStop(exclaimation_source);

	// Play the Exclaimation Sound
	alSourcei(exclaimation_source, AL_BUFFER, 0);
	Source::Audio::loadAudioFile(exclaimation_buffer, std::string("../Resources/Sound/Notification/exclaimation.wav"));
	alSourcei(exclaimation_source, AL_BUFFER, exclaimation_buffer);
	alSourcePlay(exclaimation_source);

	// Initialize the Fast Cursor Timer With Time at Initially Fully Off
	cursor_idle_timer = 0.0f;
	cursor_idle_toggle = true;
	fast_cursor_toggle = true;
	fast_cursor_timer = CURSOR_FAST_LENGTH;
	cursor_timer = CURSOR_FAST_BLINK_TIMER;
	cursor_toggle = false;
}

void Render::GUI::SelectedText::updateSelectedText()
{
	// Offset for Cursor
	float offset = 0.0f;

	// Get the Distance Cursor is From Start of Text
	float distance = text->calculateSizeTillIndex(text_index);

	// If the Text Size is Less Than Box Size, Clear the Render Text
	if (string_width <= box_width)
	{
		// Clear the Render Values of Box
		text->clearRenderingData();
		box_x = 0.0f;
	}

	// Else, Determine How Text Should be Offset
	else
	{
		// Determine the Percent The Distance is in the Box
		float percent = (distance - box_x) / box_width;

		// If the Percent is Less Than 25%, Set Box X to be Distance - 25% of Box Width
		if (percent < 0.25f)
			box_x = distance - box_width * 0.25f;

		// If the Percent is Greater Than 75%, Set the Box X to be the Distance - 75% of Box Width
		else if (percent > 0.75f)
			box_x = distance - box_width * 0.75f;

		// Make Sure Box is Within Bounds of Text
		if (box_x < 0.0f)
			box_x = 0.0f;
		else if (box_x + box_width > string_width)
			box_x = string_width - box_width;

		// Update the Selected Text
		offset = text->updateRenderTextSelected(box_x);
	}

	// Determine Where the Cursor Should be Placed
	model = glm::translate(glm::mat4(1.0f), glm::vec3(text_pos.x + distance - box_x + text->getOffset() - offset, text_pos.y, 0.0f));
}

bool Render::GUI::SelectedText::parseCharacter(char c)
{
	// Determine if Character is Usable Through Current Mode
	switch (mode)
	{

	// General Text, All Characters
	case TEXTMODES::GENERAL_TEXT: return true;

	// Aplhabetical Text, Only Letters
	case TEXTMODES::ALPHABETICAL_TEXT:
	{
		// Test if Character is a Captital Or Lowercase Letter
		if ((c > 64 && c < 91) || (c > 96 && c < 123) || c == 32)
			return true;

		// Else Throw Error, Return False
		handleInvalidInput();
		return false;
	}

	// Numerical Text Box, Numbers, Decimal, and Sign
	case TEXTMODES::NUMERICAL_TEXT:
	{
		// If Character is a Sign, Perform Sign Toggle
		if (c == '-')
		{
			toggleNegative();
			return false;
		}
	}

	// Absolute Numerical Text Box, Numbers and Decimal
	case TEXTMODES::ABSOLUTE_NUMERICAL_TEXT:
	{
		// Only Add Decimal if It Does Not Already Exist
		if (c == '.')
		{
			if (!Source::Algorithms::Common::testIN(c, text->getString().c_str()))
				return true;

			handleInvalidInput();
			return false;
		}

		// Test for a Number
		if (c > 47 && c < 58)
			return true;

		// Else, Handle Invalid Input, Return False
		handleInvalidInput();
		return false;
	}

	// Integer Text Box, Numbers and Sign
	case TEXTMODES::INTEGER_TEXT:
	{
		// If Character is a Sign, Perform Sign Toggle
		if (c == '-')
		{
			toggleNegative();
			return false;
		}
	}

	// Absolute Integer Text Box, Only Numbers
	case TEXTMODES::ABSOLUTE_INTEGER_TEXT:
	{
		// Test for a Number
		if (c > 47 && c < 58)
			return true;

		// Else, Handle Invalid Input, Return False
		handleInvalidInput();
		return false;
	}

	// Path Text Box, Most Characters Surprisingly Valid, Slash Direction is Determined by Macro
	case TEXTMODES::PATH_TEXT:
	{
		// Invalid Characters Can be Found on Microsoft's Dev Pages
		if (c == 34 || c == 42 || c == 60 || c == 62 || c == 63 || c == 124)
		{
			// Else Throw Error, Return False
			handleInvalidInput();
			return false;
		}

		// Test if Forwards Slash Should Change to a Backwards Slash
		if (c == 47 && USE_BACKSLASH)
			c = 92;

		// Test if Backwards Slash Should Change to a Forwrads Slash
		else if (c == 92 && !USE_BACKSLASH)
			c = 47;
		
		// Characters Should All be Valid
		return true;
	}

	}

	// Code Should Never Reach Here
	return false;
}

void Render::GUI::SelectedText::toggleNegative()
{
	// Determine if the First Character is Already Negative
	bool is_negaive = character_count != 0 && text->getString().at(0) == '-';

	// If Already Negative, Erase the Negative Sign
	if (is_negaive)
	{
		// Decrement Text Width by Character Being Removed
		string_width -= text->calculateCharacterWidth((char)'-');

		// Erase Character at Given Position
		text->removeCharacter(1);

		// Decrement the Text Index
		text_index--;

		// Decrement Character Count
		character_count--;
	}

	// Else, Add the Negative Sign
	else
	{
		// Increment Text Size Based on Size of Character
		string_width += text->calculateCharacterWidth((char)'-');

		// Insert the Character
		text->insertCharacter('-', 0);

		// Increment the Text Index
		text_index++;

		// Increment the Character Count
		character_count++;
	}

	// Request to Update the Selected Text
	updateSelectedText();
}

void Render::GUI::SelectedText::setCursorHeight(float new_scale)
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// The Height is the Height of Pipe Times Scale
	// Upper Portion is 75% of Total Height, Lower is 25%
	float new_height = Global::Current_Font[124].Size.y * new_scale * 1.2;
	float new_upper = new_height * 0.80f;
	float new_lower = -new_height * 0.20f;

	// Change the Lower Y-Value
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT), sizeof(GL_FLOAT), &new_lower);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GL_FLOAT), sizeof(GL_FLOAT), &new_lower);
	glBufferSubData(GL_ARRAY_BUFFER, 36 * sizeof(GL_FLOAT), sizeof(GL_FLOAT), &new_lower);

	// Change the Upper Y-Value
	for (int i = 0, j = 15 * sizeof(GL_FLOAT); i < 3; i++, j += 7 * sizeof(GL_FLOAT))
		glBufferSubData(GL_ARRAY_BUFFER, j, sizeof(GL_FLOAT), &new_upper);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::GUI::SelectedText::initializeSelectedText()
{
	// Generate the Vertex Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind the Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Generate Initial Data for Cursor
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 3.5f, -0.9f, CURSOR_WIDTH, 7.0f, glm::vec4(0.3f, 0.3f, 0.3f, 0.8f), vertices);

	// Store the Initial Cursor Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate Notification Buffer
	alGenBuffers(1, &exclaimation_buffer);

	// Generate Notification Source
	alGenSources(1, &exclaimation_source);
	alSourcef(exclaimation_source, AL_PITCH, 1);
	alSourcef(exclaimation_source, AL_GAIN, 1.0f);
	alSource3f(exclaimation_source, AL_POSITION, 0, 0, 0);
	alSource3f(exclaimation_source, AL_VELOCITY, 0, 0, 0);
	alSourcei(exclaimation_source, AL_LOOPING, AL_FALSE);
}

Render::GUI::SelectedText* Render::GUI::SelectedText::get()
{
	return &instance;
}

void Render::GUI::SelectedText::deleteSelectedText()
{
	// Delete Objects
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	alDeleteSources(1, &exclaimation_buffer);
	alDeleteBuffers(1, &exclaimation_source);
}

void Render::GUI::SelectedText::assignText(AdvancedString* text_, glm::vec2 text_pos_, uint8_t mode_, Box* box_)
{
	// Store Pointer to Text
	text = text_;

	// Store Text Position
	text_pos = text_pos_;

	// Store the Text Mode
	mode = mode_;

	// Store the Box Pointer
	box = box_;

	// Get the Backup of the String
	backup = text->getString();

	// Set Object to be Active
	active = true;

	// Obtain Information About Text
	character_count = text->getCharacterCount();
	text_index = character_count;
	string_width = text->calculateTextSize();
	box_width = text->getBoxSize();

	// Update the Height of the Cursor Based on Font Size
	setCursorHeight(text->getScale());

	// Calculate the Initial Render Text
	updateSelectedText();

	// Set Event Mode to Type
	glfwSetKeyCallback(Global::window, Source::Listeners::TypeCallback);

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::forceUpdateText(std::string& new_string)
{
	// Only Execute if Active
	if (active)
	{
		// Insert Text Into Advanced Text
		text->setString(new_string);

		// Obtain Information About Text
		character_count = text->getCharacterCount();
		text_index = character_count;
		string_width = text->calculateTextSize();
		box_width = text->getBoxSize();

		// Calculate the Initial Render Text
		updateSelectedText();
	}
}

void Render::GUI::SelectedText::stopSelecting()
{
	// Set Object to be Inactive
	active = false;

	// Reset the Box, If it Exists
	if (box != nullptr)
		box->setFalse();

	// Call the Closer Function, If it Exists
	if (closing_func != nullptr)
		closing_func();

	// Clear the Closing Function
	closing_func = nullptr;

	// Clear the Box Position
	box_x = 0.0f;

	// Set Event Mode to Editor
	glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);

	// Reset Render Text
	if (text != nullptr)
		text->updateRenderText();
}

void Render::GUI::SelectedText::insertCharacter(char character)
{
	// Parse Character Based on the Mode of the Text
	if (!parseCharacter(character))
		return;

	// If Code Reaches Here, The Character is a Normal, Valid Character

	// Increment Text Size Based on Size of Character
	string_width += text->calculateCharacterWidth((char)character);

	// Insert the Character
	text->insertCharacter(character, text_index);

	// Increment the Text Index
	text_index++;

	// Increment the Character Count
	character_count++;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::removeCharacter()
{
	// If Currently At the 0th Index, Send Error
	if (text_index == 0)
	{
		handleInvalidInput();
		return;
	}

	// Decrement Text Width by Character Being Removed
	string_width -= text->calculateCharacterWidth((int)(text_index - 1));

	// Erase Character at Given Position
	text->removeCharacter(text_index);

	// Decrement the Text Index
	text_index--;

	// Decrement Character Count
	character_count--;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::moveLeft()
{
	// If At Furthest Left, Error
	if (text_index == 0)
	{
		handleInvalidInput();
		return;
	}

	// Decrement Text Index
	text_index--;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::moveRight()
{
	// If at Furthest Right, Error
	if (text_index == character_count)
	{
		handleInvalidInput();
		return;
	}

	// Increment Text Index
	text_index++;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::moveToTop()
{
	// If at Furthest Right, Error
	if (text_index == character_count)
	{
		handleInvalidInput();
		return;
	}

	// Set Text to Furthest Right
	text_index = character_count;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::moveToBottom()
{
	// If At Furthest Left, Error
	if (text_index == 0)
	{
		handleInvalidInput();
		return;
	}

	// Set Text to Furthest Left
	text_index = 0;

	// Update the Text
	updateSelectedText();

	// Set the Cursor Idle Timer to Full
	cursor_idle_timer = CURSOR_IDLE_TIMER;
	cursor_idle_toggle = false;
}

void Render::GUI::SelectedText::renderCursor()
{
	// Early Return if the Object is Not Active
	if (!active)
		return;

	// If Blinking Cursor is Enabled, Determine if the Cursor Should be Rendered
	if (cursor_idle_toggle)
	{
		// Decrement the Cursor Timer
		cursor_timer -= Global::deltaTime;

		// If Fast Blink is Enabled, Blink the Timer Quickly
		if (fast_cursor_toggle)
		{
			// Decrement the Fast Cursor Timer
			fast_cursor_timer -= Global::deltaTime;

			// If Fast Cursor Timer Reaches 0, Set Cursor Timer to be Fully On
			if (fast_cursor_timer <= 0)
			{
				// Disable Fast Timer
				fast_cursor_timer = 0.0;
				fast_cursor_toggle = false;

				// Set Blink to be Initially Fully Off
				cursor_timer = CURSOR_BLINK_TIMER;
				cursor_toggle = false;
			}

			// If Cursor Timer Reaches 0, Toggle Cursor With Fast Timer
			if (cursor_timer <= 0)
			{
				cursor_timer = CURSOR_FAST_BLINK_TIMER;
				cursor_toggle = !cursor_toggle;
			}
		}
		
		// Else, Blink the Timer Slowly
		else
		{
			// If Cursor Timer Reaches 0, Toggle Cursor With Normal Timer
			if (cursor_timer <= 0)
			{
				cursor_timer = CURSOR_BLINK_TIMER;
				cursor_toggle = !cursor_toggle;
			}
		}

		// If Cursor Toggle is False, Don't Render Cursor Via Early Return
		if (!cursor_toggle)
			return;
	}

	// Else, Decrement the Cursor Idle Toggle
	else
	{
		// Decrement Idle Timer
		cursor_idle_timer -= Global::deltaTime;
		
		// If Idle Timer Goes Below 0, Enable Cursor Toggle
		if (cursor_idle_timer <= 0.0)
		{
			// Disable Idle Timer
			cursor_idle_timer = 0.0;
			cursor_idle_toggle = true;
			
			// Set Blink to be Initially Fully Off
			cursor_timer = CURSOR_BLINK_TIMER;
			cursor_toggle = false;
		}
	}

	// Bind Static Color Shader
	Global::colorShaderStatic.Use();

	// Bind Model Matrix
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Object is Static
	glUniform1i(Global::staticLocColor, 1);

	// Draw the Cursor
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Unstatic the Object
	glUniform1i(Global::staticLocColor, 0);
}

void Render::GUI::SelectedText::assignCloser(std::function<void()> closer)
{
	if (!active)
		closing_func = closer;
}

std::string& Render::GUI::SelectedText::getString()
{
	return text->getString();
}

bool Render::GUI::SelectedText::isActive()
{
	return active;
}

void Render::GUI::SelectedText::clearText()
{
	// Only Execute if Active
	if (active)
	{
		// Clear the String
		text->getString().clear();

		// Set String Information to Null
		character_count = 0;
		text_index = 0;
		string_width = 0;

		// Calculate the Initial Render Text
		updateSelectedText();
	}
}

void Render::GUI::SelectedText::setBackup()
{
	forceUpdateText(backup);
}

std::string& Render::GUI::SelectedText::getBackup()
{
	return backup;
}

Render::GUI::Box* Render::GUI::SelectedText::getBox()
{
	return box;
}

Render::GUI::SelectedText Render::GUI::SelectedText::instance;
