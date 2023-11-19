#include "FloorMaskSlant.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Floor::FloorMaskSlant::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Floor::FloorMaskSlant::testCollisions(glm::vec2 test_position, float error, bool fallthrough)
{
	// Test if Fallthough Occoured
	if (fallthrough && platform)
		return false;

	// Check if Object is Between X-Values
	if (data.position.x < test_position.x && test_position.x < data.position2.x)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = slope * test_position.x + intercept;

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - 1.0f) < posY && localY > posY)
		if (localY - error < test_position.y && localY + 0.01 > test_position.y)
		{
			returned_value = localY;
			return true;
		}
	}

	return false;
}

void Object::Mask::Floor::FloorMaskSlant::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	data.position2.x += deltaX;
	data.position2.y += deltaY;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
}

void Object::Mask::Floor::FloorMaskSlant::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = data.position;
	right = data.position2;
}

void Object::Mask::Floor::FloorMaskSlant::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Floor::FloorMaskSlant::FloorMaskSlant(SlantData& data_, bool& platform_, glm::vec2& offset) : Slant(data_, offset)
{
	// Store Type
	type = HORIZONTAL_SLANT;
	returned_angle = Algorithms::Math::angle_from_vector(data.position2 - data.position);
	platform = std::move(platform_);

	// Store Storage Type
	storage_type = FLOOR_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Floor::FloorMaskSlant::pointerToPosition()
{
	return &data.position;
}

Object::Object* DataClass::Data_FloorMaskSlant::genObject(glm::vec2& offset)
{
	return new Object::Mask::Floor::FloorMaskSlant(data, platform, offset);
}

void DataClass::Data_FloorMaskSlant::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskSlant::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskSlant::Data_FloorMaskSlant(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLANT;
	object_identifier[3] = children_size;
}

void DataClass::Data_FloorMaskSlant::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Floor Mask Slant", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
	object_info.addPositionValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position2, false);
	object_info.addBooleanValue("Platform? ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &platform, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
}

DataClass::Data_Object* DataClass::Data_FloorMaskSlant::makeCopy()
{
	return new Data_FloorMaskSlant(*this);
}

