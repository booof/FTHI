#include "Dynamics.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"

Object::Entity::Dynamics::Dynamics(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, DynamicData& dynamic_) : EntityBase(uuid_, entity_, data_)
{
	// Store Dynamic Data
	dynamic = dynamic_;
}

void Object::Entity::Dynamics::updateObject()
{
}

void Object::Entity::Dynamics::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(ENTITY);
	object_file.put(ENTITY_DYNAMIC);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(EntityData));
	object_file.write((char*)&data, sizeof(ObjectData));
	object_file.write((char*)&dynamic, sizeof(DynamicData));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Entity::Dynamics::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = ENTITY;
	selector.object_identifier[1] = ENTITY_DYNAMIC;

	// Store Editing Values
	selector.uuid = uuid;
	selector.entity_data = entity;
	selector.object_data = data;
	selector.dynamic_data = dynamic;
	selector.enable_resize = false;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

void Object::Entity::Dynamics::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Interactable Entity", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}
