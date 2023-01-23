#include "NPC.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::NPC::NPC(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, uint16_t& ai) : EntityBase(uuid_, entity_, data_)
{
	// Store AI Script Index
	ai_script = ai;

	// Link AI Script
}

void Object::Entity::NPC::updateObject()
{
	// Execute AI Script
}

Object::Object* DataClass::Data_NPC::genObject()
{
	return new Object::Entity::NPC(uuid, entity, data, ai);
}

void DataClass::Data_NPC::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&ai, sizeof(uint16_t));
}

void DataClass::Data_NPC::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&ai, sizeof(uint16_t));
}

DataClass::Data_NPC::Data_NPC()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_NPC;
	object_identifier[2] = 0;
}

void DataClass::Data_NPC::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("NPC", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_NPC::makeCopy()
{
	return new Data_NPC(*this);
}
