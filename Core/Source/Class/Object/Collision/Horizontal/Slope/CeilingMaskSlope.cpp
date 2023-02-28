#include "CeilingMaskSlope.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Ceiling::CeilingMaskSlope::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Ceiling::CeilingMaskSlope::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_left)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = half_height * atan(slope * (test_position.x - data.position.x)) + data.position.y;

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - Angle / 2.0f * HeightSign) < posY && localY > posY)
		if ((localY + error + data.height * 0.25f * height_sign) > test_position.y && localY - 0.01 < test_position.y)
		{
			// Calculate Derivative
			float derivative = (half_height * slope) / (1 + (float)glm::pow(slope * (test_position.x - data.position.x), 2));
			returned_angle = atan(derivative);

			returned_value = localY;
			return true;
		}
	}

	return false;
}

void Object::Mask::Ceiling::CeilingMaskSlope::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Ceiling::CeilingMaskSlope::CeilingMaskSlope(SlopeData& data_) : Slope(data_)
{
	// Store Type
	type = HORIZONTAL_SLOPE;

	// Store Storage Type
	storage_type = CEILING_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 0.0f, 0.45f, 1.0f));
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

glm::vec2* Object::Mask::Ceiling::CeilingMaskSlope::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Ceiling::CeilingMaskSlope::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(data.position.x - abs(data.width), data.position.y - data.height);
	right = glm::vec2(data.position.x + abs(data.width), data.position.y + data.height);
}

Object::Object* DataClass::Data_CeilingMaskSlope::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskSlope(data);
}

void DataClass::Data_CeilingMaskSlope::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlopeData));
}

void DataClass::Data_CeilingMaskSlope::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlopeData));
}

DataClass::Data_CeilingMaskSlope::Data_CeilingMaskSlope()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLOPE;
}

void DataClass::Data_CeilingMaskSlope::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Ceiling Mask Slope", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_CeilingMaskSlope::makeCopy()
{
	return new Data_CeilingMaskSlope(*this);
}
