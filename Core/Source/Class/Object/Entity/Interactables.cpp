#include "Interactables.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::Interactables::Interactables(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, InteractableData& interactable_, glm::vec2& offset) : EntityBase(uuid_, entity_, data_, offset)
{
	// Store Interactable Data
	interactable = interactable_;

	// Link Function
}

void Object::Entity::Interactables::updateObject()
{
	// Perform Collision Detection

	// If Collision Detection, Execute Function
}

Object::Object* DataClass::Data_Interactable::genObject(glm::vec2& offset)
{
	return new Object::Entity::Interactables(uuid, entity, data, interactable, offset);
}

void DataClass::Data_Interactable::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&interactable, sizeof(Object::Entity::InteractableData));
}

void DataClass::Data_Interactable::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&interactable, sizeof(Object::Entity::InteractableData));
}

DataClass::Data_Interactable::Data_Interactable(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_INTERACTABLE;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_Interactable::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Interactable Entity", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
}

DataClass::Data_Object* DataClass::Data_Interactable::makeCopy()
{
	return new Data_Interactable(*this);
}
