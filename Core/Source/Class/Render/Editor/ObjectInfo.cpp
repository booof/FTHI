#include "ObjectInfo.h"

#include "Globals.h"
#include "Render/Shader/Shader.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Algorithms/Common/Common.h"
#include "EditorOptions.h"

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
	Vertices::Rectangle::genRectColor(-0.1f, -0.1f, -0.9f, 50.0f, 80.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.7f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

	// Generate Background Vertices
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -0.8f, 49.0f, 79.0f, glm::vec4(0.4f, 0.4f, 0.4f, 0.7f), vertices);
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

	// Set Color Value Count to 0
	TextColor::resetColorCount();

	// Generate the Text Color Vertex Objects
	glGenVertexArrays(1, &TextColor::VAO);
	glGenBuffers(1, &TextColor::VBO);

	// Bind Text Color Vertex Objects
	glBindVertexArray(TextColor::VAO);
	glBindBuffer(GL_ARRAY_BUFFER, TextColor::VBO);

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
	if (type_text == "" || !active || Global::editor_options->option_object_info_max_width_percent == 0 || Global::editor_options->option_object_info_text_size_percent == 0)
		return;

	// If Should Restructure Flag is True, Determine How the Layout Should be Established
	if (should_restructure)
	{
		//std::cout << "should restructure\n";

		// Determine Size of Text
		max_width = MAX_TEXT_SIZE * Global::editor_options->option_object_info_max_width_percent;
		max_height = DEFAULT_TEXT_HEIGHT * Global::editor_options->option_object_info_text_size_percent;
		max_scale = 0.143f * Global::editor_options->option_object_info_text_size_percent;

		// Determine the Scale and Size of the Type Text Object
		type_scale = 0.25f * Global::editor_options->option_object_info_text_size_percent;
		float type_size = Source::Fonts::getTextSize(type_text, type_scale);
		if (type_size > max_width)
		{
			type_scale = (max_width * type_scale) / type_size;
			type_size = max_width;
		}
		max_text_size = type_size + 1;

		// Determine the Current Width and Height of the Text
		float temp_size = 0.0f;
		float type_height_offset = (32.0f + TEXT_HEIGHT_OFFSET) * type_scale * 1.1f;
		current_height = type_height_offset;
		for (int i = 0; i < text_objects.size(); i++)
		{
			text_objects[i]->setScale(max_scale);
			temp_size = text_objects[i]->getTextSize(max_width) + 1;
			max_text_size = (max_text_size > temp_size) ? max_text_size : temp_size;
			current_height += text_objects[i]->returnHeightOffset(max_height);
		}

		// Set Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(113.0f - max_text_size, 88.0f - current_height, 0.0));

		// Set Position of Text
		type_text_position = glm::vec2(89.5f - (max_text_size + type_size) * 0.5f, 49.9f - 32.0f * type_scale);
		first_text_position = glm::vec2((90.0f - max_text_size), (49.9f - type_height_offset));

		// Generate the Color Vertices
		generateColorVertices();

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
		text_objects[i]->blitzText(first_text_position.x, text_height, max_height);
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

void Editor::ObjectInfo::addColorValue(std::string identifier, glm::vec4 identifier_color, glm::vec4* value, bool generate)
{
	// Generate and Store Text Object
	text_objects.push_back(new TextColor(identifier, identifier_color, value));

	// If Generate is Set, Generate Vertices for All Instances
	if (generate)
	{
		// Bind Vertex Object
		glBindVertexArray(TextColor::VAO);
		glBindBuffer(GL_ARRAY_BUFFER, TextColor::VBO);

		// Allocate Vertex Data
		// Each Color Has 2 Rectangles; 1 for the Color, and 1 for
		// a Black Outline. Colors Have Same Transparency as Value
		// and Width the Current Width of the Object. All Colors
		// Will Be at 0,0 and Dynamically Modified Via a Model Matrix
		glBufferData(GL_ARRAY_BUFFER, 84 * sizeof(GL_FLOAT) * TextColor::getColorCount(), 0, GL_STATIC_DRAW);

		// Vertices are Generated Along With All Other Objects

		// Unbind Vertex Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Enable Flag
	should_restructure = true;
}

void Editor::ObjectInfo::editColorValue(int index, glm::vec4* value)
{
	// Return if Index is Greater or Equal to Size of Text Array
	if (index > text_objects.size())
		return;

	// Get Reference to Object
	TextColor& object = *static_cast<TextColor*>(text_objects[index]);

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

	// Set Color Value Count to 0
	TextColor::resetColorCount();
}

void Editor::ObjectInfo::generateColorVertices()
{
	// Bind Vertex Object
	glBindVertexArray(TextColor::VAO);
	glBindBuffer(GL_ARRAY_BUFFER, TextColor::VBO);

	// Iterate Through Each Color Object and Generate Their Vertices
	int offset = 0, triangle_offset = 0, text_width;
	float width = 0.0f, height = 0.0f, x = 0.0f, y = 0.0f;
	float vertices[42];
	for (TextMaster* instance : text_objects)
	{
		if (instance->getTextType() == TEXT_OBJECTS::TEXT_COLOR)
		{
			// Get Reference to Instance
			TextColor* color_instance = static_cast<TextColor*>(instance);

			// Store the Offset
			color_instance->setVAOOffset(triangle_offset);
			triangle_offset += 12;

			// Calculate the Width and Height of the Boxes
			text_width = color_instance->getTextSize(max_width);
			width = (max_width - text_width) * 0.9f;
			height = (color_instance->returnScale() / 0.143f) * 3.5;

			// Calculate the X/Y-Position of the Boxes
			x = first_text_position.x + text_width * 1.3 + width * 0.5f - 1;
			y = color_instance->returnScale() * 10;

			// Set Black Outline
			Vertices::Rectangle::genRectColor(x, y, -0.7f, width, height, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(GL_FLOAT), sizeof(vertices), vertices);
			offset += 42;

			// Set Color Outline
			Vertices::Rectangle::genRectColor(x, y, -0.6f, width - 0.4f, height - 0.4f, color_instance->getColor(), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(GL_FLOAT), sizeof(vertices), vertices);
			offset += 42;
		}
	}

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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

void Editor::ObjectInfo::TextMaster::setScale(float new_scale)
{
	text_scale = new_scale;
}

float Editor::ObjectInfo::TextMaster::returnHeightOffset(float max_height)
{
	return (max_height + TEXT_HEIGHT_OFFSET) * text_scale * 1.3f;
}

Editor::ObjectInfo::TextString::TextString(std::string identifier_, glm::vec4 identifier_color_, std::string* value_, glm::vec4 value_color_)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	value_string = value_;
	color = value_color_;
}

Editor::ObjectInfo::TEXT_OBJECTS Editor::ObjectInfo::TextString::getTextType()
{
	return TEXT_OBJECTS::TEXT_STRING;
}

void Editor::ObjectInfo::TextString::blitzText(float x, float& y, float max_height)
{
	// Render Text
	y -= returnHeightOffset(max_height);
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(*value_string, x, y, text_scale, color, true);
}

float Editor::ObjectInfo::TextString::getTextSize(float max_width)
{
	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + *value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > max_width)
	{
		text_scale = (max_width * text_scale) / text_size;
		text_size = max_width;
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
	interpret_as_int = is_int;

	// Set Values
	setValue(value_);
}

Editor::ObjectInfo::TEXT_OBJECTS Editor::ObjectInfo::TextSingleValue::getTextType()
{
	return TEXT_OBJECTS::TEXT_SINGLE_VALUE;
}

void Editor::ObjectInfo::TextSingleValue::blitzText(float x, float& y, float max_height)
{
	// Render Text
	y -= returnHeightOffset(max_height);
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(value_string, x, y, text_scale, color, true);
}

float Editor::ObjectInfo::TextSingleValue::getTextSize(float max_width)
{
	// Convert Value Into String
	if (interpret_as_int)
		value_string = std::to_string(*static_cast<int*>(value));
	else
	{
		std::stringstream value_stream;
		value_stream << std::fixed << std::setprecision(4) << *static_cast<float*>(value);
		value_string = value_stream.str();
		//value_string = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value)));
	}

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > max_width)
	{
		text_scale = (max_width * text_scale) / text_size;
		text_size = max_width;
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
	interpret_as_int = is_int;

	// Set Values
	setValues(value1_, value2_);
}

Editor::ObjectInfo::TEXT_OBJECTS Editor::ObjectInfo::TextDoubleValue::getTextType()
{
	return TEXT_OBJECTS::TEXT_DOUBLE_VALUE;
}

void Editor::ObjectInfo::TextDoubleValue::blitzText(float x, float& y, float max_height)
{
	// Render Text
	y -= returnHeightOffset(max_height);
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	x = Source::Fonts::renderText(secondary_identifier_1, x, y, text_scale, secondary_identifier_color_1, true);
	x = Source::Fonts::renderText(value_string_1, x, y, text_scale, value_color, true);
	x = Source::Fonts::renderText(secondary_identifier_2, x, y, text_scale, secondary_identifier_color_2, true);
	Source::Fonts::renderText(value_string_2, x, y, text_scale, value_color, true);
}

float Editor::ObjectInfo::TextDoubleValue::getTextSize(float max_width)
{
	// Convert Value Into String
	if (interpret_as_int)
	{
		value_string_1 = std::to_string(*static_cast<int*>(value1));
		value_string_2 = std::to_string(*static_cast<int*>(value2));
	}
	else
	{
		std::stringstream value_stream1, value_stream2;
		value_stream1 << std::fixed << std::setprecision(4) << *static_cast<float*>(value1);
		value_string_1 = value_stream1.str();
		value_stream2 << std::fixed << std::setprecision(4) << *static_cast<float*>(value2);
		value_string_2 = value_stream2.str();
		//value_string_1 = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value1)));
		//value_string_2 = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*static_cast<float*>(value2)));
	}

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + secondary_identifier_1 + value_string_1 + secondary_identifier_2 + value_string_2, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > max_width)
	{
		text_scale = (max_width * text_scale) / text_size;
		text_size = max_width;
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

Editor::ObjectInfo::TEXT_OBJECTS Editor::ObjectInfo::TextBoolean::getTextType()
{
	return TEXT_OBJECTS::TEXT_BOOL;
}

void Editor::ObjectInfo::TextBoolean::blitzText(float x, float& y, float max_height)
{
	// Render Text
	y -= returnHeightOffset(max_height);
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);
	Source::Fonts::renderText(value_string, x, y, text_scale, color, true);
}

float Editor::ObjectInfo::TextBoolean::getTextSize(float max_width)
{
	// Get Text
	if (*value)
		value_string = "True";
	else
		value_string = "False";

	// Get Size of Text
	text_size = Source::Fonts::getTextSize(identifier + value_string, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > max_width)
	{
		text_scale = (max_width * text_scale) / text_size;
		text_size = max_width;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextBoolean::setValue(bool* value_)
{
	// Store Value
	value = value_;
}

Editor::ObjectInfo::TextColor::TextColor(std::string identifier_, glm::vec4 identifier_color_, glm::vec4* value_)
{
	// Store Values
	identifier = identifier_;
	identifier_color = identifier_color_;
	value = value_;

	// Increment Color Count
	color_count++;
}

void Editor::ObjectInfo::TextColor::blitzText(float x, float& y, float max_height)
{
	// Render Text
	y -= returnHeightOffset(max_height);
	x = Source::Fonts::renderText(identifier, x, y, text_scale, identifier_color, true);

	// Generate Model Matrix
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y, 0.0f));

	// Render Color
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glUniform1i(Global::staticLocColor, 1);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, vao_offset, 12);
	glBindVertexArray(0);
	glUniform1i(Global::staticLocColor, 0);
	Global::fontShader.Use();
}

float Editor::ObjectInfo::TextColor::getTextSize(float max_width)
{
	// Get Size of Identifier
	text_size = Source::Fonts::getTextSize(identifier, text_scale);

	// Clamp Size and Scale, If Needed
	if (text_size > max_width)
	{
		text_scale = (max_width * text_scale) / text_size;
		text_size = max_width;
	}

	// Return Size of Text
	return text_size;
}

void Editor::ObjectInfo::TextColor::setValue(glm::vec4* value_)
{
	value = value_;
}

bool Editor::ObjectInfo::TextColor::containsColor()
{
	return color_count == 0;
}

void Editor::ObjectInfo::TextColor::resetColorCount()
{
	color_count = 0;
}

uint8_t Editor::ObjectInfo::TextColor::getColorCount()
{
	return color_count;
}

glm::vec4& Editor::ObjectInfo::TextColor::getColor()
{
	return *value;
}

void Editor::ObjectInfo::TextColor::setVAOOffset(int new_offset)
{
	vao_offset = new_offset;
}

Editor::ObjectInfo::TEXT_OBJECTS Editor::ObjectInfo::TextColor::getTextType()
{
	return TEXT_OBJECTS::TEXT_COLOR;
}

uint8_t Editor::ObjectInfo::TextColor::color_count;
GLuint Editor::ObjectInfo::TextColor::VAO;
GLuint Editor::ObjectInfo::TextColor::VBO;
