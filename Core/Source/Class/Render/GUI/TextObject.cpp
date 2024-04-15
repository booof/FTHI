#include "TextObject.h"
#include "Render\Struct\DataClasses.h"

// Functions
#include "Source/Loaders/Fonts.h"

#include "Render\Editor\ObjectInfo.h"

Render::GUI::TextObject::TextObject(ElementData& data1, TextData& data2)
{
	// Store Text Data
	element_data = data1;
	data = data2;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}
}

void Render::GUI::TextObject::blitzText()
{
	// Draw Text
	Source::Fonts::renderText(data.text, element_data.position.x, element_data.position.y, data.scale, data.color, element_data.is_static);
}

void Render::GUI::TextObject::blitzOffsetText()
{
	// Draw Text
	Source::Fonts::renderTextOffsetAdvanced(data.text, element_data.position.x, element_data.position.y, data.scale, data.color, 0, false, element_data.is_static);
}

void Render::GUI::TextObject::blitzGlobalText()
{
	// Draw Text
	Source::Fonts::renderTextGlobal(data.text, element_data.position.x, element_data.position.y, data.scale, data.color);
}

void Render::GUI::TextObject::swapText(std::string new_text)
{
	data.text = new_text;
}

bool Render::GUI::TextObject::testMouseCollisions(float x, float y)
{
	// Test if Mouse is Within Text Width
	if (x >= element_data.position.x && x <= element_data.position.x + static_cast<DataClass::Data_TextElement*>(data_object)->getCalculatedWidth())
	{
		// Test if Mouse is Within Text Height
		if (y >= element_data.position.y && y <= element_data.position.y + data.scale)
		{
			// Collision Detected, Return True
			return true;
		}
	}

	return false;
}

bool Render::GUI::TextObject::updateElement()
{
	// If a Pointer is Linked, This is When the Text Should Change (Only When Text is Different)

	// IDEA: Create a Special Struct in "Advanced String" for the Pointer That Contains a String and Other Data

	// Always Return False Because it Cannot be Interracted With

	// Another Idea: Additional Value in Text_Data For What Font to Use
	// Index Similar to Textures and Scripts
	return false;
}

void Render::GUI::TextObject::blitzElement()
{
	// TODO: Possibly Find a Way to Blitz the Correct Text Type
}

void Render::GUI::TextObject::linkValue(void* value_ptr)
{
	// Store String Object That Will be Dynamically Rendered by Text Object
}

void Render::GUI::TextObject::moveElement(float newX, float newY)
{
	// Only Need to Update the Position
	element_data.position.x = newX;
	element_data.position.y = newY;
}

// Function to Read Data and Create an Object
Object::Object* DataClass::Data_TextElement::genObject(glm::vec2& offset)
{
	return new Render::GUI::TextObject(element_data, data);
}

void DataClass::Data_TextElement::writeObjectData(std::ofstream& object_file)
{
	// Update the Size of the Text, In Case it Changed
	data.text_size = data.text.size();

	// Write Text Object Data
	object_file.write((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.write((char*)&data, sizeof(Render::GUI::TextData));

	// Write the Current Text
	object_file.write(data.text.c_str(), data.text_size);
}

void DataClass::Data_TextElement::readObjectData(std::ifstream& object_file)
{
	// Read Text Object Data
	object_file.read((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.read((char*)&data, sizeof(Render::GUI::TextData));

	// Allocate Space for Text
	data.text.resize(data.text_size);

	// Caculate Initial Text Width
	forceWidthRecalculation();

	// Read the Initial Text
	object_file.read(&data.text[0], data.text_size);
}

DataClass::Data_TextElement::Data_TextElement(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::ELEMENT;
	object_identifier[1] = Render::GUI::TEXT;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
	element_data.element_type = Render::GUI::TEXT;
}

void DataClass::Data_TextElement::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Text", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &element_data.position, false);
	object_info.addTextValue("Value: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.text, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addSingleValue("Scale: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), false);
	object_info.addColorValue("Text Color: ", glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), &data.color, true);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_TextElement::makeCopy()
{
	return new Data_TextElement(*this);
}

Render::GUI::TextData& DataClass::Data_TextElement::getTextData()
{
	return data;
}

void DataClass::Data_TextElement::generateInitialValues(glm::vec2 initial_position)
{
	element_data.position = initial_position;
	data.text = "New Text";
	data.scale = 1.0f;
	data.text_size = data.text.size();
}

void DataClass::Data_TextElement::forceWidthRecalculation()
{
	// Store Recalculated Width
	calculated_width = Source::Fonts::getTextSize(data.text, data.scale);
}

float& DataClass::Data_TextElement::getCalculatedWidth()
{
	return calculated_width;
}

void DataClass::Data_TextElement::renderText()
{
	// Assuming Offset Text
	Source::Fonts::renderTextOffsetAdvanced(data.text, element_data.position.x, element_data.position.y, data.scale, data.color, 0, false, element_data.is_static);
}
