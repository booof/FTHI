#include "CeilingMaskSlant.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Globals.h"

#ifdef EDITOR

glm::vec2 Object::Mask::Ceiling::CeilingMaskSlant::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Ceiling::CeilingMaskSlant::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between X-Values
	if (data.position.x < test_position.x && test_position.x < data.position2.x)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = slope * test_position.x + intercept;

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - 1.0f) < posY && localY > posY)
		if (localY + error > test_position.y && localY - 0.01 < test_position.y)
		{
			returned_value = localY;
			return true;
		}
	}

	return false;
}

void Object::Mask::Ceiling::CeilingMaskSlant::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Ceiling::CeilingMaskSlant::CeilingMaskSlant(SlantData& data_) : Slant(data_)
{
	// Store Type
	type = HORIZONTAL_SLANT;
	returned_angle = Algorithms::Math::angle_from_vector(data.position2 - data.position) + 3.14159f;

	// Store Storage Type
	storage_type = CEILING_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.28f, 0.0f, 0.34f, 1.0f));
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

glm::vec2* Object::Mask::Ceiling::CeilingMaskSlant::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Ceiling::CeilingMaskSlant::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = data.position;
	right = data.position2;
}

Object::Object* DataClass::Data_CeilingMaskSlant::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskSlant(data);
}

void DataClass::Data_CeilingMaskSlant::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
}

void DataClass::Data_CeilingMaskSlant::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlantData));
}

DataClass::Data_CeilingMaskSlant::Data_CeilingMaskSlant()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLANT;
}

void DataClass::Data_CeilingMaskSlant::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Ceiling Mask Slant", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position2.x, &data.position2.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_CeilingMaskSlant::makeCopy()
{
	return new Data_CeilingMaskSlant(*this);
}
