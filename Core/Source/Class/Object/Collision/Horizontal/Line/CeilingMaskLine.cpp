#include "CeilingMaskLine.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Ceiling::CeilingMaskLine::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Ceiling::CeilingMaskLine::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_right)
	{
		// Check if Object Y Intercepts Floor Y
		if (data.position.y + error > test_position.y && data.position.y - 0.01f < test_position.y)
		{
				return true;
		}
	}

	return false;
}

void Object::Mask::Ceiling::CeilingMaskLine::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Ceiling::CeilingMaskLine::CeilingMaskLine(HorizontalLineData& data_) : HorizontalLine(data_)
{
	// Store Data
	type = HORIZONTAL_LINE;
	returned_value = data.position.y;
	returned_angle = 3.1459f;

	// Store Storage Type
	storage_type = CEILING_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Ceiling::CeilingMaskLine::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Ceiling::CeilingMaskLine::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(x_left, data.position.y);
	right = glm::vec2(x_right, data.position.y);
}

Object::Object* DataClass::Data_CeilingMaskHorizontalLine::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskLine(data);
}

void DataClass::Data_CeilingMaskHorizontalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::HorizontalLineData));
}

void DataClass::Data_CeilingMaskHorizontalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::HorizontalLineData));
}

DataClass::Data_CeilingMaskHorizontalLine::Data_CeilingMaskHorizontalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_LINE;
}

void DataClass::Data_CeilingMaskHorizontalLine::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Ceiling Mask Horizontal Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Width: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.width, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_CeilingMaskHorizontalLine::makeCopy()
{
	return new Data_CeilingMaskHorizontalLine(*this);
}
