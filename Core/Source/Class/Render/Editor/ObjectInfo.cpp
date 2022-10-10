#include "ObjectInfo.h"

#include "Globals.h"
#include "Render/Shader/Shader.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Algorithms/Common/Common.h"

#define DEFAULT_TEXT_HEIGHT 25
#define MAX_TEXT_SIZE 43.875f
#define TEXT_HEIGHT_OFFSET 2.0f

Editor::ObjectInfo::ObjectInfo()
{
	// Set Initial Values
	type_text_position = glm::vec2(0.0f, 0.0f);
	first_text_position = glm::vec2(0.0f, 0.0f);
	type_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));

	// Generate Vertex Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocate Memory
	glBufferData(GL_ARRAY_BUFFER, 336, NULL, GL_STATIC_DRAW);

	// Generate Outline Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(-1.0f, -1.0f, -1.0f, 50.0f, 80.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

	// Generate Background Vertices
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -0.9f, 49.0f, 79.0f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::ObjectInfo::drawInfo()
{
	// If Type Text is Empty, Do Nothing
	if (type_text == "" || !active)
		return;

	// If Should Restructure Flag is True, Determine How the Layout Should be Established
	if (should_restructure)
	{
		// Determine the Scale and Size of the Type Text Object
		type_scale = 0.25f;
		float type_size = Source::Fonts::getTextSize(type_text, 0.25f);
		if (type_size > MAX_TEXT_SIZE)
		{
			type_scale = (MAX_TEXT_SIZE * 0.25f) / type_size;
			type_size = MAX_TEXT_SIZE;
		}
		max_text_size = type_size + 1;

		// Determine the Current Width and Height of the Text
		float temp_size = 0.0f;
		current_height = DEFAULT_TEXT_HEIGHT * type_scale;
		for (int i = 0; i < text_objects.size(); i++)
		{
			temp_size = text_objects[i]->getTextSize() + 1;
			max_text_size = (max_text_size > temp_size) ? max_text_size : temp_size;
			current_height += DEFAULT_TEXT_HEIGHT * text_objects[i]->returnScale() + TEXT_HEIGHT_OFFSET;
		}

		// Set Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(113.0f - max_text_size, 82.0f - current_height, 0.0));

		// Set Position of Text
		type_text_position = glm::vec2((84.0f - (max_text_size - type_size) * type_scale - type_size), 41.9f);
		first_text_position = glm::vec2((90.0f - max_text_size), (41.0f - DEFAULT_TEXT_HEIGHT * type_scale));

		// Disable Flag
		should_restructure = false;
	}

	// Draw Boarder
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(Global::staticLocColor, 1);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
	glUniform1i(Global::staticLocColor, 0);

	// Bind Font Shader
	Global::fontShader.Use();

	// Draw Object Type
	Source::Fonts::renderText(type_text, type_text_position.x, type_text_position.y, type_scale, type_color, true);

	// Draw Text Objects
	float text_height = first_text_position.y;
	for (int i = 0; i < text_objects.size(); i++)
		text_height -= text_objects[i]->blitzText(first_text_position.x, text_height);
}

void Editor::ObjectInfo::setObjectType(std::string type, glm::vec4 color)
{
	// Store Text and Color
	type_text = type;
	type_color = color;

	// Determine the Size of Text
	float temp_size = Source::Fonts::getTextSize(type_text, 0.25f) + 1.0f;
	max_text_size = (max_text_size > temp_size) ? max_text_size : temp_size;

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::addTextValue(std::string identifier, glm::vec4 identifier_color, std::string* value, glm::vec4 value_color)
{
	// Generate and Store Text Object
	text_objects.push_back(new TextString(identifier, identifier_color, value, value_color));

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::editSingleValue(int index, std::string* value)
{
	// Return if Index is Greater or Equal to Size of Text Array
	if (index > text_objects.size())
		return;

	// Get Reference to Object
	TextString& object = *static_cast<TextString*>(text_objects[index]);

	// Store Data
	object.setValue(value);
}

void Editor::ObjectInfo::addSingleValue(std::string identifier, glm::vec4 identifier_color, void* value, glm::vec4 value_color, bool is_int)
{
	// Generate and Store Text Object
	text_objects.push_back(new TextSingleValue(identifier, identifier_color, value, value_color, is_int));

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::editSingleValue(int index, void* value)
{
	// Return if Index is Greater or Equal to Size of Text Array
	if (index > text_objects.size())
		return;

	// Get Reference to Object
	TextSingleValue& object = *static_cast<TextSingleValue*>(text_objects[index]);

	// Store Data
	object.setValue(value);
}

void Editor::ObjectInfo::addDoubleValue(std::string identifier, glm::vec4 identifier_color, std::string secondary_identifier_1, glm::vec4 secondary_color_1,
	std::string secondary_identifier_2, glm::vec4 secondary_color_2, void* value1, void* value2, glm::vec4 value_color, bool is_int)
{
	// Generate and Store Text Object
	text_objects.push_back(new TextDoubleValue(identifier, identifier_color, secondary_identifier_1, secondary_color_1, secondary_identifier_2, secondary_color_2, value1, value2, value_color, is_int));

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::editDoubleValue(int index, void* value1, void* value2)
{
	// Return if Index is Greater or Equal to Size of Text Array
	if (index > text_objects.size())
		return;

	// Get Reference to Object
	TextDoubleValue& object = *static_cast<TextDoubleValue*>(text_objects[index]);

	// Store Data
	object.setValues(value1, value2);
}

void Editor::ObjectInfo::addBooleanValue(std::string identifier, glm::vec4 identifier_color, bool* value, glm::vec4 value_color)
{
	// Generate and Store Text Object
	text_objects.push_back(new TextBoolean(identifier, identifier_color, value, value_color));

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::editBooleanValue(int index, bool* value)
{
	// Return if Index is Greater or Equal to Size of Text Array
	if (index > text_objects.size())
		return;

	// Get Reference to Object
	TextBoolean& object = *static_cast<TextBoolean*>(text_objects[index]);

	// Store Data
	object.setValue(value);
}

void Editor::ObjectInfo::removeValueAtIndex(int index)
{
	// If Index is Out of Range, Disregard
	if (index < 0 || index > text_objects.size())
		return;

	// Delete Object at Index
	delete text_objects[index];

	// Erase Value at Index
	text_objects.erase(text_objects.begin() + index, text_objects.begin() + index + 1);

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::clearAll()
{
	// Delete All Objects in Text Array
	for (int i = 0; i < text_objects.size(); i++)
		delete text_objects[i];

	// Clear Text Array
	text_objects.clear();

	// Reset Type String
	type_text = "";
}

void Editor::ObjectInfo::forceResize()
{
	should_restructure = true;
}

void Editor::ObjectInfo::TextMaster::setSizeScale(float size, float scale)
{
	// Set Values
	text_size = size;
	text_scale = scale;
}

float Editor::ObjectInfo::TextMaster::returnSize()
{
	return text_size;
}

float Editor::ObjectInfo::TextMaster::returnScale()
{
	return text_scale;
}

Editor::ObjectInfo::TextString::TextString(std::string identifier_, glm::vec4 identifier_color_, std::string* value_, glm::vec4 value_color_)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	value_string = value_;
	color = value_color_;
}

float Editor::ObjectInfo::TextString::blitzText(float x, float y)
{
	// Render Text
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(*value_string, x, y, text_scale, color, true);

	// Return Offset Created From Rendering
	return DEFAULT_TEXT_HEIGHT * text_scale + TEXT_HEIGHT_OFFSET;
}

float Editor::ObjectInfo::TextString::getTextSize()
{
	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + *value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > MAX_TEXT_SIZE)
	{
		text_scale = (MAX_TEXT_SIZE * text_scale) / text_size;
		text_size = MAX_TEXT_SIZE;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextString::setValue(std::string* value_)
{
	// Set Value
	value_string = value_;
}

Editor::ObjectInfo::TextSingleValue::TextSingleValue(std::string identifier_, glm::vec4 identifier_color_, void* value_, glm::vec4 value_color_, bool is_int)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	color = value_color_;

	// Set Values
	setValue(value_);
}

float Editor::ObjectInfo::TextSingleValue::blitzText(float x, float y)
{
	// Render Text
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(value_string, x, y, text_scale, color, true);

	// Return Offset Created From Rendering
	return DEFAULT_TEXT_HEIGHT * text_scale + TEXT_HEIGHT_OFFSET;
}

float Editor::ObjectInfo::TextSingleValue::getTextSize()
{
	// Convert Value Into String
	if (interpret_as_int)
		value_string = std::to_string(*static_cast<int*>(value));
	else
		value_string = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value)));

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > MAX_TEXT_SIZE)
	{
		text_scale = (MAX_TEXT_SIZE * text_scale) / text_size;
		text_size = MAX_TEXT_SIZE;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextSingleValue::setValue(void* value_)
{
	// Store Value
	value = value_;
}

Editor::ObjectInfo::TextDoubleValue::TextDoubleValue(std::string identifier_, glm::vec4 identifier_color_, std::string secondary_identifier_1_, glm::vec4 secondary_color_1_,
	std::string secondary_identifier_2_, glm::vec4 secondary_color_2_, void* value1_, void* value2_, glm::vec4 value_color_, bool is_int)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	secondary_identifier_1 = secondary_identifier_1_;
	secondary_identifier_2 = secondary_identifier_2_;
	secondary_identifier_color_1 = secondary_color_1_;
	secondary_identifier_color_2 = secondary_color_2_;
	value_color = value_color_;

	// Set Values
	setValues(value1_, value2_);
}

float Editor::ObjectInfo::TextDoubleValue::blitzText(float x, float y)
{
	// Render Text
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	x = Source::Fonts::renderText(secondary_identifier_1, x, y, text_scale, secondary_identifier_color_1, true);
	x = Source::Fonts::renderText(value_string_1, x, y, text_scale, value_color, true);
	x = Source::Fonts::renderText(secondary_identifier_2, x, y, text_scale, secondary_identifier_color_2, true);
	Source::Fonts::renderText(value_string_2, x, y, text_scale, value_color, true);

	// Return Offset Created From Rendering
	return DEFAULT_TEXT_HEIGHT * text_scale + TEXT_HEIGHT_OFFSET;
}

float Editor::ObjectInfo::TextDoubleValue::getTextSize()
{
	// Convert Value Into String
	if (interpret_as_int)
	{
		value_string_1 = std::to_string(*static_cast<int*>(value1));
		value_string_2 = std::to_string(*static_cast<int*>(value2));
	}
	else
	{
		value_string_1 = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value1)));
		value_string_2 = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value2)));
	}

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + secondary_identifier_1 + value_string_1 + secondary_identifier_2 + value_string_2, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > MAX_TEXT_SIZE)
	{
		text_scale = (MAX_TEXT_SIZE * text_scale) / text_size;
		text_size = MAX_TEXT_SIZE;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextDoubleValue::setValues(void* value1_, void* value2_)
{
	// Store Values
	value1 = value1_;
	value2 = value2_;
}

Editor::ObjectInfo::TextBoolean::TextBoolean(std::string identifier_, glm::vec4 identifier_color_, bool* value_, glm::vec4 value_color_)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	value = value_;
	color = value_color_;
}

float Editor::ObjectInfo::TextBoolean::blitzText(float x, float y)
{
	// Render Text
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(value_string, x, y, text_scale, color, true);

	// Return Offset Created From Rendering
	return DEFAULT_TEXT_HEIGHT * text_scale + TEXT_HEIGHT_OFFSET;
}

float Editor::ObjectInfo::TextBoolean::getTextSize()
{
	// Get Text
	if (*value)
		value_string = "True";
	else
		value_string = "False";

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > MAX_TEXT_SIZE)
	{
		text_scale = (MAX_TEXT_SIZE * text_scale) / text_size;
		text_size = MAX_TEXT_SIZE;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextBoolean::setValue(bool* value_)
{
	// Store Value
	value = value_;
}
