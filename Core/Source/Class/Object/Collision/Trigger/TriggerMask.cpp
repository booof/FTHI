#include "TriggerMask.h"
#include "Render/Struct/DataClasses.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

#ifdef EDITOR

void Object::Mask::Trigger::TriggerMask::initializeVisualizer()
{
	// Initialize Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x + data.width / 2.0f, data.position.y + data.height / 2.0f, 0.0f));

	// Create Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Generate Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.7f, data.width, data.height, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), vertices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::vec2* Object::Mask::Trigger::TriggerMask::pointerToPosition()
{
	return &data.position;
}

glm::vec2 Object::Mask::Trigger::TriggerMask::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Trigger::TriggerMask::testCollisions(glm::vec2 test_position, float error)
{
	// Test if X Value is Inside Rectangle
	if (test_position.x > data.position.x && test_position.x < x_right)
	{
		// Test if Y Value is Inside Rectangle
		if (test_position.y > data.position.y && test_position.y < y_top)
		{
			return true;
		}
	}

	return false;
}

void Object::Mask::Trigger::TriggerMask::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	initializeVisualizer();
}

void Object::Mask::Trigger::TriggerMask::returnMaterial(int& material)
{
	// Do Absolutly Nothing
}

void Object::Mask::Trigger::TriggerMask::testTrigger(glm::vec2 test_position, SubObject* object)
{
	// Test Collisions
	if (!testCollisions(test_position, NULL))
		return;

	// Invoke Function
	trigger(object);
}

Object::Mask::Trigger::TriggerMask::TriggerMask(TriggerData& data_, glm::vec2& offset)
{
	// Store Data
	data = std::move(data_);
	data.position += offset;

	// Store Storage Type
	storage_type = TRIGGER_COUNT;

	// Get Opposite Coordinates
	x_right = data.position.x + data.width;
	y_top = data.position.y + data.height;

	// Get Trigger Function

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer();
	number_of_vertices = 6;

#endif

	// Script Initializer
	initializeScript(data.script);
}

Object::Object* DataClass::Data_TriggerMask::genObject(glm::vec2& offset)
{
	return new Object::Mask::Trigger::TriggerMask(data, offset);
}

void DataClass::Data_TriggerMask::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::Trigger::TriggerData));
}

void DataClass::Data_TriggerMask::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::Trigger::TriggerData));
}

DataClass::Data_TriggerMask::Data_TriggerMask(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::TRIGGER;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_TriggerMask::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Trigger Mask", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_TriggerMask::makeCopy()
{
	return new Data_TriggerMask(*this);
}

void DataClass::Data_TriggerMask::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	updateSelectedPositionsHelper(deltaX, deltaY, update_real);
}

Object::Mask::Trigger::TriggerData& DataClass::Data_TriggerMask::getTriggerData()
{
	return data;
}

int& DataClass::Data_TriggerMask::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_TriggerMask::getPosition()
{
	return data.position;
}

void DataClass::Data_TriggerMask::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size * 2.0f;
	data.height = size;
	data.check_type = Object::Mask::Trigger::CHECK_TYPE::NONE;
	data.script = 0;
}

void DataClass::Data_TriggerMask::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}

