#include "TriggerMask.h"

// Selector
#include "Class/Render/Editor/Selector.h"

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
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -3.0f, data.width, data.height, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), vertices);

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

void Object::Mask::Trigger::TriggerMask::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Information
	info(object_info, name, data);

	// Selector Helper
	select2(selector);
}

glm::vec2* Object::Mask::Trigger::TriggerMask::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Trigger::TriggerMask::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(TRIGGER);

	// Write Data
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

glm::vec2 Object::Mask::Trigger::TriggerMask::returnPosition()
{
	return data.position;
}

void Object::Mask::Trigger::TriggerMask::info(Editor::ObjectInfo& object_info, std::string& name, TriggerData& data)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Trigger Mask", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
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

Object::Mask::Trigger::TriggerMask::TriggerMask(TriggerData& data_)
{
	// Store Data
	data = std::move(data_);

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

