#include "FloorMaskSlope.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Floor::FloorMaskSlope::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Floor::FloorMaskSlope::testCollisions(glm::vec2 test_position, float error, bool fallthrough)
{
	// Test if Fallthough Occoured
	if (fallthrough && platform)
		return false;

	//Vertices::Visualizer::visualizePoint(glm::vec2(x_left, data.position.y), 0.4f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//Vertices::Visualizer::visualizePoint(glm::vec2(x_right, data.position.y), 0.4f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	//Vertices::Visualizer::visualizePoint(data.position, 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_right)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = half_height * atan(slope * (test_position.x - data.position.x)) + data.position.y;

		//Vertices::Visualizer::visualizePoint(glm::vec2(test_position.x, localY), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - Angle / 2.0f * HeightSign) < posY && localY > posY)
		if ((localY - error - data.height * 0.25f * height_sign) < test_position.y && localY + 0.01 > test_position.y)
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

void Object::Mask::Floor::FloorMaskSlope::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
}

void Object::Mask::Floor::FloorMaskSlope::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(data.position.x - abs(data.width), data.position.y - data.height);
	right = glm::vec2(data.position.x + abs(data.width), data.position.y + data.height);
}

void Object::Mask::Floor::FloorMaskSlope::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Floor::FloorMaskSlope::FloorMaskSlope(SlopeData& data_, bool& platform_, glm::vec2& offset) : Slope(data_, offset)
{
	// Store Type
	type = HORIZONTAL_SLOPE;
	platform = std::move(platform_);

	// Store Storage Type
	storage_type = FLOOR_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.04f, 0.24f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Floor::FloorMaskSlope::pointerToPosition()
{
	return &data.position;
}

Object::Object* DataClass::Data_FloorMaskSlope::genObject(glm::vec2& offset)
{
	return new Object::Mask::Floor::FloorMaskSlope(data, platform, offset);
}

void DataClass::Data_FloorMaskSlope::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskSlope::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlopeData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskSlope::Data_FloorMaskSlope(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLOPE;
	object_identifier[3] = children_size;
}

void DataClass::Data_FloorMaskSlope::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Floor Mask Slope", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addBooleanValue("Platform? ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &platform, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
}

DataClass::Data_Object* DataClass::Data_FloorMaskSlope::makeCopy()
{
	return new Data_FloorMaskSlope(*this);
}
