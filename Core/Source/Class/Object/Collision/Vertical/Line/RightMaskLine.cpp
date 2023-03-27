#include "RightMaskLine.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Right::RightMaskLine::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Right::RightMaskLine::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between Y Values
	if (y_bottom < test_position.y && y_top > test_position.y)
	{
		// Check if Object is Between X Values
		if (data.position.x + error > test_position.x && data.position.x - 0.01f < test_position.x)
		{
			return true;
		}
	}

	return false;
}

void Object::Mask::Right::RightMaskLine::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

void Object::Mask::Right::RightMaskLine::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Right::RightMaskLine::RightMaskLine(VerticalLineData& data_) : VerticalLine(data_)
{
	// Store Type
	type = VERTICAL_LINE;
	returned_value = data.position.x;
	returned_angle = 1.5708f;

	// Store Storage Type
	storage_type = RIGHT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Right::RightMaskLine::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Right::RightMaskLine::getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom)
{
	top = glm::vec2(data.position.x, y_top);
	bottom = data.position;
}

Object::Object* DataClass::Data_RightMaskVerticalLine::genObject()
{
	return new Object::Mask::Right::RightMaskLine(data);
}

void DataClass::Data_RightMaskVerticalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

void DataClass::Data_RightMaskVerticalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

DataClass::Data_RightMaskVerticalLine::Data_RightMaskVerticalLine(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::RIGHT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_LINE;
	object_identifier[3] = children_size;
}

void DataClass::Data_RightMaskVerticalLine::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Right Wall Mask Vertical Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Height: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.height, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_RightMaskVerticalLine::makeCopy()
{
	return new Data_RightMaskVerticalLine(*this);
}
