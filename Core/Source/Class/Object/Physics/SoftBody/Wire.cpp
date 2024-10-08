#include "Wire.h"
#include "Render/Struct/DataClasses.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Globals.h"

Object::Physics::Soft::Wire::Wire(uint32_t& uuid_, ObjectData& data_, WireData& wire_, glm::vec2& offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_WIRE;

	// Store Data
	data = data_;
	wire = wire_;
	uuid = uuid_;
	data.position += offset;

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

void Object::Physics::Soft::Wire::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	for (int i = 0; i < node_count; i++)
		nodes[i].Position += glm::vec2(deltaX, deltaY);
}

glm::vec2 Object::Physics::Soft::Wire::returnPosition()
{
	return data.position;
}

Object::Object* DataClass::Data_Wire::genObject(glm::vec2& offset)
{
	return new Object::Physics::Soft::Wire(uuid, data, wire, offset);
}

void DataClass::Data_Wire::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&wire, sizeof(Object::Physics::Soft::WireData));
}

void DataClass::Data_Wire::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&wire, sizeof(Object::Physics::Soft::WireData));
}

void DataClass::Data_Wire::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	wire.position2.x += deltaX;
	wire.position2.y += deltaY;
}

DataClass::Data_Wire::Data_Wire(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::WIRE;
	object_identifier[3] = children_size;
}

void DataClass::Data_Wire::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Wire", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
	object_info.addPositionValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &wire.position2, false);
}

DataClass::Data_Object* DataClass::Data_Wire::makeCopy()
{
	return new Data_Wire(*this);
}

void DataClass::Data_Wire::generateInitialValues(glm::vec2& position, float& size)
{
	generateUUID();
	data.position = position;
	data.zpos = -1.0f;
	data.colors = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	wire.position2 = position + glm::vec2(size, 0.0f);
	wire.Mass = 1.0f;
	wire.Health = 10.0f;
	wire.Radius = 0.2f;
	wire.node_instances = 5;
	wire.total_rest_length = size;
	wire.break_distance = size * 2.0f;
	wire.Stiffness = 1.0f;
	wire.Dampening = 1.0f;
}

Object::Physics::Soft::WireData& DataClass::Data_Wire::getWireData()
{
	return wire;
}

void DataClass::Data_Wire::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Position2 is at Index 3
	*position2 = &wire.position2;
	index2 = 3;

	// Position 3 Not Important
	position3Null(index3, position3);
}
