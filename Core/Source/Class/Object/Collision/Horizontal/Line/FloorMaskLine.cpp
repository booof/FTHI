#include "FloorMaskLine.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Floor::FloorMaskLine::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Floor::FloorMaskLine::testCollisions(glm::vec2 test_position, float error, bool fallthrough)
{
	// Test if Fallthough Occoured
	if (fallthrough && platform)
		return false;

	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_right)
	{
		// Check if Object Y Intercepts Floor Y
		if (data.position.y - error < test_position.y && data.position.y + 0.01f > test_position.y)
		{
			return true;
		}
	}

	return false;
}

void Object::Mask::Floor::FloorMaskLine::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(x_left, data.position.y);
	right = glm::vec2(x_right, data.position.y);
}

void Object::Mask::Floor::FloorMaskLine::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Floor::FloorMaskLine::FloorMaskLine(HorizontalLineData& data_, bool& platform_) : HorizontalLine(data_)
{
	// Store Type
	type = HORIZONTAL_LINE;
	returned_value = data.position.y;
	returned_angle = 0;
	platform = std::move(platform_);

	// Store Storage Type
	storage_type = FLOOR_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	number_of_vertices = 2;

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif

}

glm::vec2* Object::Mask::Floor::FloorMaskLine::pointerToPosition()
{
	return &data.position;
}

Object::Object* DataClass::Data_FloorMaskHorizontalLine::genObject()
{
	return new Object::Mask::Floor::FloorMaskLine(data, platform);
}

void DataClass::Data_FloorMaskHorizontalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::HorizontalLineData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskHorizontalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::HorizontalLineData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskHorizontalLine::Data_FloorMaskHorizontalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_LINE;
}

void DataClass::Data_FloorMaskHorizontalLine::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Floor Mask Horizontal Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Width: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.width, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
	object_info.addBooleanValue("Platform? ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &platform, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
}

DataClass::Data_Object* DataClass::Data_FloorMaskHorizontalLine::makeCopy()
{
	return new Data_FloorMaskHorizontalLine(*this);
}
