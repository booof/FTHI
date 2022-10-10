#include "Anchor.h"

// Globals
#include "Globals.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Selector
#include "Render/Editor/Selector.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

// Shaders
#include "Render/Shader/Shader.h"

Object::Physics::Hinge::Anchor::Anchor(uint32_t& uuid_, AnchorData& data_)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_ANCHOR;

	// Store Data
	data = data_;
	uuid = uuid_;

	// Store Storage Type
	storage_type = PHYSICS_COUNT;

	// Store Physics Base
	base = PHYSICS_BASES::HINGE_BASE;

	// Generate Vertex Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Store Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 0.5f, 0.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

#ifdef EDITOR

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

void Object::Physics::Hinge::Anchor::updateObject() {}

void Object::Physics::Hinge::Anchor::drawObject()
{
	// Bind Static Color Shader
	Global::colorShaderStatic.Use();

	// Bind Model Matrix
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Return to Object Shader
	Global::objectShader.Use();
}

glm::vec2* Object::Physics::Hinge::Anchor::pointerToPosition()
{
	return &data.position;
}

void Object::Physics::Hinge::Anchor::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(PHYSICS);
	object_file.put((uint8_t)PHYSICS_BASES::HINGE_BASE);
	object_file.put((uint8_t)HINGES::ANCHOR);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(AnchorData));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Physics::Hinge::Anchor::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = PHYSICS;
	selector.object_identifier[1] = (uint8_t)PHYSICS_BASES::HINGE_BASE;
	selector.object_identifier[2] = (uint8_t)HINGES::ANCHOR;

	// Store Editing Values
	selector.uuid = uuid;
	selector.anchor_data = data;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Anchor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

bool Object::Physics::Hinge::Anchor::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 0.5f && x < data.position.x + 0.5f)
	{
		if (y > data.position.y - 0.5f && y < data.position.y + 0.5f)
		{
			return true;
		}
	}

	return false;
}

glm::vec2 Object::Physics::Hinge::Anchor::returnPosition()
{
	return data.position;
}
