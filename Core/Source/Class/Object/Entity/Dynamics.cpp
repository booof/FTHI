#include "Dynamics.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::Dynamics::Dynamics(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, DynamicData& dynamic_, glm::vec2& offset) : EntityBase(uuid_, entity_, data_, offset)
{
	// Store Dynamic Data
	dynamic = dynamic_;
}

void Object::Entity::Dynamics::updateObject()
{
}

Object::Object* DataClass::Data_Dynamic::genObject(glm::vec2& offset)
{
	return new Object::Entity::Dynamics(uuid, entity, data, dynamic, offset);
}

void DataClass::Data_Dynamic::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&dynamic, sizeof(Object::Entity::DynamicData));
}

void DataClass::Data_Dynamic::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&dynamic, sizeof(Object::Entity::DynamicData));
}

DataClass::Data_Dynamic::Data_Dynamic(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_DYNAMIC;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_Dynamic::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Interactable Entity", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
}

DataClass::Data_Object* DataClass::Data_Dynamic::makeCopy()
{
	return new Data_Dynamic(*this);
}
