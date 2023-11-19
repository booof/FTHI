#include "Anchor.h"
#include "Render/Struct/DataClasses.h"

// Globals
#include "Globals.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

// Shaders
#include "Render/Shader/Shader.h"

Object::Physics::Hinge::Anchor::Anchor(uint32_t& uuid_, AnchorData& data_, glm::vec2& offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_ANCHOR;

	// Store Data
	data = data_;
	uuid = uuid_;
	data.position += offset;

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
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 0.5f, 0.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), vertices);
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

void Object::Physics::Hinge::Anchor::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
}

glm::vec2 Object::Physics::Hinge::Anchor::returnPosition()
{
	return data.position;
}

Object::Object* DataClass::Data_Anchor::genObject(glm::vec2& offset)
{
	return new Object::Physics::Hinge::Anchor(uuid, data, offset);
}

void DataClass::Data_Anchor::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
}

void DataClass::Data_Anchor::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
}

DataClass::Data_Anchor::Data_Anchor(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::HINGE_BASE;
	object_identifier[2] = (uint8_t)Object::Physics::HINGES::ANCHOR;
	object_identifier[3] = children_size;
}

void DataClass::Data_Anchor::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Anchor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
}

DataClass::Data_Object* DataClass::Data_Anchor::makeCopy()
{
	return new Data_Anchor(*this);
}

void DataClass::Data_Anchor::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	updateSelectedPositionsHelper(deltaX, deltaY, update_real);
}

int& DataClass::Data_Anchor::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Anchor::getPosition()
{
	return data.position;
}

void DataClass::Data_Anchor::generateInitialValues(glm::vec2& position)
{
	generateUUID();
	data.position = position;
	data.script = 0;
}

Object::Physics::Hinge::AnchorData& DataClass::Data_Anchor::getAnchorData()
{
	return data;
}

void DataClass::Data_Anchor::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}
