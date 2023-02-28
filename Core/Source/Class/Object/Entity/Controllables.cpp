#include "Controllables.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::Controllables::Controllables(uint32_t& uuid_, EntityData& entity_, ObjectData& data_) : EntityBase(uuid_, entity_, data_)
{

}

void Object::Entity::Controllables::updateObject()
{
}

Object::Object* DataClass::Data_Controllable::genObject()
{
	return new Object::Entity::Controllables(uuid, entity, data);
}

void DataClass::Data_Controllable::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
}

void DataClass::Data_Controllable::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
}

DataClass::Data_Controllable::Data_Controllable()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_CONTROLLABLE;
	object_identifier[2] = 0;
}

void DataClass::Data_Controllable::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Controllable Entity", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_Controllable::makeCopy()
{
	return new Data_Controllable(*this);
}
