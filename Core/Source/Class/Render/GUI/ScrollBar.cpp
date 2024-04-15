#include "ScrollBar.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Globals
#include "Globals.h"

#include "Render\Struct\DataClasses.h"

#include "Render\Editor\ObjectInfo.h"

#include "VerticalScrollBar.h"
#include "HorizontalScrollBar.h"

bool Render::GUI::ScrollBar::updateElement()
{
	// Determine if Bar Should Try to be Modified
	if (!(modified_by_user ^ is_being_modified))
	{
		// If Left Click is Not Held, Disable Scrolling
		if (!Global::LeftClick)
			is_being_modified = false;

		// If Bar is Currently Being Modified, Scroll the Bar
		else if (is_being_modified)
		{
			was_modified = true;
			ScrollHelper();
			return true;
		}

		// If Not Being Modified, Test Collisions
		if (!is_being_modified)
			return TestColloisions();
	}

	return false;
}

void Render::GUI::ScrollBar::linkValue(void* value_ptr)
{
	// Reinterpret Value Pointer as the Percent Pointer
	percent_ptr = static_cast<float*>(value_ptr);
}

void Render::GUI::ScrollBar::blitzElement()
{
	// Send Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model_Background));

	// Send Nullified Brightness Vector to Shader
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glBindVertexArray(BackgroundVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Send Other Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model_Scroll));

	glBindVertexArray(ScrollVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Render::GUI::ScrollBar::blitzElement2(glm::mat4& matrix)
{
	// A Matrix Used to Store the Combined Master and Bar Matricies
	glm::mat4 temp_matrix;

	// Send Model Matrix to Shader
	temp_matrix = matrix * model_Background;
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp_matrix));

	// Send Nullified Brightness Vector to Shader
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glBindVertexArray(BackgroundVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Send Other Model Matrix to Shader
	temp_matrix = matrix * model_Scroll;
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp_matrix));

	glBindVertexArray(ScrollVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

float Render::GUI::ScrollBar::getPercent()
{
	return percent;
}

float Render::GUI::ScrollBar::getOffset()
{
	return BarOffset;
}

void Render::GUI::ScrollBar::resetBar()
{
	// Set Percent to 0
	percent = 0;
}

float* Render::GUI::ScrollBar::getOffsetPointer()
{
	return &BarOffset;
}

bool Render::GUI::ScrollBar::isBeingUsed()
{
	return is_being_modified;
}

int16_t Render::GUI::ScrollBar::getIdentifier()
{
	return data.bar_identifier;
}

Object::Object* DataClass::Data_ScrollBarElement::genObject(glm::vec2& offset)
{
	if (object_identifier[2] == Render::GUI::VERTICAL)
		return new Render::GUI::VerticalScrollBar(element_data.position.x, element_data.position.y, data.background_width, data.background_height, data.size, data.initial_percent, data.bar_identifier);
	else
		return new Render::GUI::HorizontalScrollBar(element_data.position.x, element_data.position.y, data.background_width, data.background_height, data.size, data.initial_percent, data.bar_identifier);
}

void DataClass::Data_ScrollBarElement::writeObjectData(std::ofstream& object_file)
{
	// Write Element and Scroll Data
	object_file.write((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.write((char*)&data, sizeof(Render::GUI::ScrollData));
}

void DataClass::Data_ScrollBarElement::readObjectData(std::ifstream& object_file)
{
	// Read Element and Scroll Data
	object_file.read((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.read((char*)&data, sizeof(Render::GUI::ScrollData));
}

DataClass::Data_ScrollBarElement::Data_ScrollBarElement(uint8_t bar_type, uint8_t children_size)
{
	// Store Scroll Bar Identifier
	object_identifier[0] = Object::ELEMENT;
	object_identifier[1] = Render::GUI::SCROLL_BAR;

	// Store Type Identifier and Children Size
	object_identifier[2] = bar_type;
	object_identifier[3] = children_size;

	// Store Element Type
	element_data.element_type = Render::GUI::SCROLL_BAR;
}

void DataClass::Data_ScrollBarElement::info(Editor::ObjectInfo& object_info)
{
	// Strings of Different Bar Modes
	static std::string bar_mode_strings[] = { "Vertical Bar", "Horizontal Bar" };

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Scroll Bar", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addTextValue("Type: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &bar_mode_strings[object_identifier[2]], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &element_data.position, false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.background_width, &data.background_height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Scroll Plane Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.size, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), false);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_ScrollBarElement::makeCopy()
{
	return new DataClass::Data_ScrollBarElement(*this);
}

Render::GUI::ScrollData& DataClass::Data_ScrollBarElement::getScrollData()
{
	return data;
}

void DataClass::Data_ScrollBarElement::generateInitialValues(glm::vec2 initial_position, glm::vec2 initial_size)
{
	// Store Initial Position
	element_data.position = initial_position;

	// If a Vertical Bar, Use the Height to Make the Bar
	if (object_identifier[2] == Render::GUI::VERTICAL)
	{
		data.background_height = initial_size.y;
		data.background_width = 1.0f;
		data.size = initial_size.y * 1.5f;
	}

	// If Horizontal, Use Width
	else
	{
		data.background_width = initial_size.x;
		data.background_height = 1.0f;
		data.size = initial_size.x * 1.5f;
	}
}

glm::vec4 DataClass::Data_ScrollBarElement::calculateDefaultBar()
{
	// Clamp Initial Percent, If Necessary
	if (data.initial_percent < 0.0f)
		data.initial_percent = 0.0f;
	else if (data.initial_percent > 1.0f)
		data.initial_percent = 1.0f;

	// Vertical Bar
	if (object_identifier[2] == Render::GUI::VERTICAL)
	{
		// If Bar Size is More Than Height, Same as Background
		float bar_size = data.background_height * abs(data.background_height / data.size);
		if (bar_size >= data.background_height)
			return glm::vec4(0.0f, -data.background_height * 0.5f, data.background_width, data.background_height);

		// Else, Determine Position of Bar
		float offset = -data.initial_percent * (element_data.position.y - ((element_data.position.y - data.background_height) + bar_size));
		return glm::vec4(0.0f, offset - bar_size * 0.5f, data.background_width, bar_size);
	}

	// Horizontal Bar

	// If Bar Size is More Than Width, Same as Background
	float bar_size = data.background_width * abs(data.background_width / data.size);
	if (bar_size >= data.background_width)
		return glm::vec4(-data.background_width * 0.5f, 0.0f, data.background_width, data.background_height);

	// Else, Determine Position of Bar
	float offset = data.initial_percent * (element_data.position.x - ((element_data.position.x - data.background_width) + bar_size));
	return glm::vec4(offset + bar_size * 0.5f - data.background_width, 0.0f, bar_size, data.background_height);
}
