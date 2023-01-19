#include "Wire.h"

// Selector
#include "Render/Editor/Selector.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Globals.h"

Object::Physics::Soft::Wire::Wire(uint32_t& uuid_, ObjectData& data_, WireData& wire_)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_WIRE;

	// Store Data
	data = data_;
	wire = wire_;
	uuid = uuid_;

	// Store Storage Type
	storage_type = PHYSICS_COUNT;

	// Store Physics Base
	base = PHYSICS_BASES::SOFT_BODY;

	// Initialize Nodes and Springs
	node_count = wire.node_instances;
	spring_count = wire.node_instances - 1;
	nodes = new Node[node_count];
	springs = new Spring[spring_count];

	// Parameterize Object
	float mx = wire.position2.x - data.position.x;
	float my = wire.position2.y - data.position.y;
	float parameter_offset = 1.0f / spring_count;
	float rest_length = wire.total_rest_length / spring_count;
	float parameter;

	// Create Nodes
	for (int i = 0; i < node_count; i++)
	{
		parameter = i * parameter_offset;
		nodes[i] = Node(i, mx * parameter + data.position.x, my * parameter + data.position.y, wire.Mass, wire.Health, wire.Radius);
	}

	// Create Springs
	for (int i = 0; i < spring_count; i++)
	{
		springs[i] = Spring(i, i + 1, rest_length, wire.Stiffness, wire.Dampening, wire.break_distance);
	}

	// Initiailize Visualizer
	initializeVisualizer();

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

glm::vec2* Object::Physics::Soft::Wire::pointerToPosition()
{
	return &nodes[0].Position;
}

void Object::Physics::Soft::Wire::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(PHYSICS);
	object_file.put((uint8_t)PHYSICS_BASES::SOFT_BODY);
	object_file.put((uint8_t)SOFT_BODY_TYPES::WIRE);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(ObjectData));
	object_file.write((char*)&wire, sizeof(WireData));
	
	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Physics::Soft::Wire::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Information
	info(object_info, name, data, wire);

	// Set Selector to Active Highlight
	selector.activateHighlighter();

	// Selector Helper
	select2(selector);
}

bool Object::Physics::Soft::Wire::testMouseCollisions(float x, float y)
{
	// Parameterize Line
	float mx = wire.position2.x - data.position.x;
	float my = wire.position2.y - data.position.y;

	// Calculate Relative Y
	float t = (x - data.position.x) / mx;
	if (t > 1 || t < 0)
		return false;
	float localY = my * t + data.position.y;

	// Test for Collision
	if (y < localY && y > localY - 1.0f)
	{
		return true;
	}

	return false;
}

glm::vec2 Object::Physics::Soft::Wire::returnPosition()
{
	return data.position;
}

void Object::Physics::Soft::Wire::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, WireData& wire)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Wire", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &wire.position2.x, &wire.position2.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}
