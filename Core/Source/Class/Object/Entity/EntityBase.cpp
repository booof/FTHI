#include "EntityBase.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Loaders/Textures.h"
#include "Globals.h"

Object::Entity::EntityBase::EntityBase(uint32_t& uuid_, EntityData& entity_, ObjectData& data_)
{
	// Store Information
	entity = entity_;
	data = data_;
	uuid = uuid_;

	stats.Force = 20.0f;

	// Store Storage Type
	storage_type = ENTITY_COUNT;

	// Store Position
	state.position = data.position;

	// Collision Stuff
	vertices[0] = glm::vec2(entity.half_collision_width, entity.half_collision_height);
	vertices[1] = glm::vec2(-entity.half_collision_width, entity.half_collision_height);
	vertices[2] = -vertices[0];
	vertices[3] = -vertices[1];
	angle_offsets[0] = Named_Node(0.7854f, 2);
	angle_offsets[1] = Named_Node(2.3562f, 3);
	angle_offsets[2] = Named_Node(3.927f, 0);
	angle_offsets[3] = Named_Node(5.4978f, 1);

	// Generate Vertex Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Generate and Store Vertex Data
	float vertices[30];
	Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -1.0f, entity.half_width * 2.0f, entity.half_height * 2.0f, vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Get Texture
	Source::Textures::loadSingleTexture("../Resources/Textures/Entity/Player/Player.tex/egg.jpg", texture);

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

void Object::Entity::EntityBase::updateEntity()
{
	// 
}

void Object::Entity::EntityBase::blitzEntity()
{
	// Bind Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(state.position.x, state.position.y, 0.0f));
	glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Bind Texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture.texture);

	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

glm::vec2* Object::Entity::EntityBase::pointerToPosition()
{
	return &data.position;
}

bool Object::Entity::EntityBase::testMouseCollisions(float x, float y)
{
	if (x >= data.position.x - entity.half_collision_width && x <= data.position.x + entity.half_collision_width)
	{
		if (y >= data.position.y - entity.half_collision_height && y <= data.position.y + entity.half_collision_height)
			return true;
	}

	return false;
}

glm::vec2 Object::Entity::EntityBase::returnPosition()
{
	return data.position;
}
