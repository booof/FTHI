#include "Controllables.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::Controllables::Controllables(uint32_t& uuid_, EntityData& entity_, ObjectData& data_) : EntityBase(uuid_, entity_, data_)
{

}

void Object::Entity::Controllables::updateObject()
{
}

void Object::Entity::Controllables::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(ENTITY);
	object_file.put(ENTITY_CONTROLLABLE);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(EntityData));
	object_file.write((char*)&data, sizeof(ObjectData));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Entity::Controllables::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Information
	info(object_info, name, data);

	// Selector Helper
	select2(selector);
}

void Object::Entity::Controllables::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Controllable Entity", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}
