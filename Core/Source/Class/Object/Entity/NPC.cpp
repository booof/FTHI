#include "NPC.h"
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

#ifdef EDITOR

void Object::Entity::NPC::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(ENTITY);
	object_file.put(ENTITY_NPC);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(EntityData));
	object_file.write((char*)&data, sizeof(ObjectData));
	object_file.write((char*)&ai_script, sizeof(uint16_t));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Entity::NPC::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = ENTITY;
	selector.object_identifier[1] = ENTITY_NPC;

	// Store Editing Values
	selector.uuid = uuid;
	selector.entity_data = entity;
	selector.object_data = data;
	selector.npc_ai = ai_script;
	selector.enable_resize = false;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("NPC", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

#endif
