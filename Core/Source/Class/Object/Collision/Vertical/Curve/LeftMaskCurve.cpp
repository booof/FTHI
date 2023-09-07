#include "LeftMaskCurve.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Left::LeftMaskCurve::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Left::LeftMaskCurve::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between Y Values
	if ((modified_coordinates.y < test_position.y && y_top > test_position.y) || (modified_coordinates.y > test_position.y && y_top < test_position.y))
	{
		// Calculated X-Value
		float localX = -(log((test_position.y - modified_coordinates.y) / slope) / log(amplitude) + x_offset - modified_coordinates.x - data.width);

		// Check if Object X intercepts Wall X
		if (localX - error < test_position.x && localX + 0.01f > test_position.x)
		{
			returned_value = localX;
			returned_angle = 4.7124f;
			return true;
		}
	}

    return false;
}

void Object::Mask::Left::LeftMaskCurve::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.4f, 0.0f, 1.0f), -1);
}

void Object::Mask::Left::LeftMaskCurve::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Left::LeftMaskCurve::LeftMaskCurve(CurveData& data_, glm::vec2& offset) : Curve(data_, offset)
{
	// Store Type
	type = VERTICAL_CURVE;

	// Store Storage Type
	storage_type = LEFT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.4f, 0.0f, 1.0f), -1);
	number_of_vertices = 22;

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

glm::vec2* Object::Mask::Left::LeftMaskCurve::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Left::LeftMaskCurve::getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom)
{
	top = glm::vec2((log((y_top - modified_coordinates.y) / slope) / log(amplitude) + x_offset - modified_coordinates.x), y_top);
	bottom = glm::vec2(data.position.x, modified_coordinates.y);
}

Object::Object* DataClass::Data_LeftMaskCurve::genObject(glm::vec2& offset)
{
	return new Object::Mask::Left::LeftMaskCurve(data, offset);
}

void DataClass::Data_LeftMaskCurve::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::CurveData));
}

void DataClass::Data_LeftMaskCurve::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::CurveData));
}

DataClass::Data_LeftMaskCurve::Data_LeftMaskCurve(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::LEFT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_CURVE;
	object_identifier[3] = children_size;
}

void DataClass::Data_LeftMaskCurve::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Left Wall Mask Curve", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_LeftMaskCurve::makeCopy()
{
	return new Data_LeftMaskCurve(*this);
}
