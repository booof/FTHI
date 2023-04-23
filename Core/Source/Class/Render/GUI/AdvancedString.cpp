#include "AdvancedString.h"
#include "Globals.h"

GUI::AdvancedString::AdvancedString(std::string string)
{
	// Store String
	data = string;
}

GUI::AdvancedString::AdvancedString(std::string string, GLfloat max_length_, GLfloat scale_, bool centered_)
{
	// Store String
	data = string;

	// Store the Values of the String
	max_length = max_length_;
	scale = scale_;
	centered = centered_;

	// Initialize the Render Text
	if (max_length != 0 || centered)
		updateRenderText();
}

void GUI::AdvancedString::updateRenderText()
{
	// The Character in Text
	std::string::const_iterator c;

	// Temporary Width of Decimal Points at End of Text
	float decimal_point_width = (Global::Current_Font[46].Advance >> 6) * scale * 2;

	// The Current Length of the Iterated Text
	float text_length = 0;

	// Reset the Render Text
	render_string = "";

	// Reset the Offset
	offset = 0.0f;

	// Iterate Between Each Character in Text to Test if it Passes Max Length
	for (c = data.begin(); c != data.end(); c++)
	{
		// Increase Length of Text
		text_length += (Global::Current_Font[*c].Advance >> 6) * scale;

		// Append Character to newText
		render_string += *c;

		// Prevent Text From Going Past Max Length
		if (max_length != 0.0f && text_length  + decimal_point_width >= max_length)
		{
			text_length += decimal_point_width;
			render_string += "..";
			break;
		}
	}

	// Shift x Coordinate if Centered
	if (centered)
		offset -= text_length / 2;
}

float GUI::AdvancedString::updateRenderTextSelected(float unused_space)
{
	// The Character in Text
	std::string::const_iterator c;

	// Temporary Width of Decimal Points at End of Text
	float decimal_point_width = (Global::Current_Font[46].Advance >> 6) * scale * 2;

	// The Current Length of the Iterated Text
	float text_length = 0;

	// Reset the Render Text
	render_string = "";

	// Reset the Offset
	offset = 0.0f;

	// Initial Offset
	float initial_offset = 0.0f;

	// Flag That Will Create Initial Offset After Unused Space Was Reached
	bool initial_offset_ = true;

	// Iterate Between Each Character in Text to Test if it Passes Max Length
	for (c = data.begin(); c != data.end(); c++)
	{
		// Increase Length of Text
		text_length += (Global::Current_Font[*c].Advance >> 6) * scale;

		// If Text Length is Less Than Unused Space, Don't Add Character
		if (text_length >= unused_space)
		{
			// Provide Initial Offset
			if (initial_offset_)
			{
				initial_offset = (text_length - unused_space) - (Global::Current_Font[*c].Advance >> 6) * scale;
				offset = initial_offset;
				initial_offset_ = false;
			}

			// Append Character to newText
			render_string += *c;

			// Prevent Text From Going Past Max Length
			if (max_length != 0.0f && text_length - unused_space + decimal_point_width >= max_length)
			{
				text_length += decimal_point_width;
				render_string += "..";
				break;
			}
		}
	}

	// Correct Text Length With Unused Space
	text_length -= unused_space;

	// Shift x Coordinate if Centered
	if (centered)
	{
		if (text_length > max_length)
			offset -= max_length * 0.5f;
		else
			offset -= text_length / 2;
	}

	return initial_offset;
}

float GUI::AdvancedString::calculateTextSize()
{
	// The Character in Text
	std::string::const_iterator c;

	// The Current Length of the Iterated Text
	float text_length = 0;

	// Add the Length of the Character at the Given Scale
	for (c = data.begin(); c != data.end(); c++)
		text_length += (Global::Current_Font[*c].Advance >> 6) * scale;

	// Add 3 Blank Spaces to The End of the Size
	text_length += 3 * (Global::Current_Font[' '].Advance >> 6) * scale;

	return text_length;
}

float GUI::AdvancedString::calculateCharacterWidth(char c)
{
	return (Global::Current_Font[c].Advance >> 6) * scale;
}

float GUI::AdvancedString::calculateCharacterWidth(int index)
{
	return calculateCharacterWidth(data.at(index));
}

float GUI::AdvancedString::calculateSizeTillIndex(int index)
{
	float text_length = 0;
	for (int i = 0; i < index; i++)
		text_length += (Global::Current_Font[data.at(i)].Advance >> 6) * scale;
	return text_length;
}

std::string& GUI::AdvancedString::getString()
{
	return data;
}

void GUI::AdvancedString::setString(std::string string)
{
	// Set the String
	data = string;

	// If Initial String Values are Set, Update Render Text
	if (max_length != 0 || centered)
		updateRenderText();
}

std::string& GUI::AdvancedString::getRenderString()
{
	// Return Data String if Values are Not Set
	if (max_length == 0.0f)
		return data;
	return render_string;
}

void GUI::AdvancedString::clearRenderingData()
{
	// Set Box Width to 0
	max_length = 0.0f;

	// Set the Offset to 0
	offset = 0.0f;

	// If Centering is Active, Set the Offset
	if (centered)
		updateRenderText();

	// Clear the Render String
	render_string = "";
}
void GUI::AdvancedString::setAdvancedValues(GLfloat max_length_, GLfloat scale_, bool centered_)
{
	// Store the Values of the String
	max_length = max_length_;
	scale = scale_;
	centered = centered_;

	// Initialize the Render Text
	if (max_length != 0 || centered)
		updateRenderText();
}

void GUI::AdvancedString::setScale(GLfloat scale_)
{
	// Store the New Scale
	scale = scale_;

	// Initialize the Render Text
	if (max_length != 0 || centered)
		updateRenderText();
}

float GUI::AdvancedString::getOffset()
{
	return offset;
}

float GUI::AdvancedString::getBoxSize()
{
	return max_length;
}

float GUI::AdvancedString::getScale()
{
	return scale;
}

int GUI::AdvancedString::getCharacterCount()
{
	return (int)data.size();
}

void GUI::AdvancedString::insertCharacter(char c, int index)
{
	if (index == data.size())
		data.push_back(c);
	else
		data.insert(data.begin() + index, c);
}

void GUI::AdvancedString::removeCharacter(int index)
{
	data.erase(data.begin() + index - 1);
}

