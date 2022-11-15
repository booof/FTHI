#include "SpringMass.h"

// Common Functions
#include "Source/Algorithms/Common/Common.h"

// Selector
#include "Render/Editor/Selector.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Globals.h"

Object::Physics::Soft::SpringMass::SpringMass(uint32_t& uuid_, ObjectData& data_, std::string& file_name_)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_SPRING_MASS;

	// Store Data
	data = data_;
	file_name = file_name_;
	path = "../Resources/Models/Soft Bodies/" + file_name;
	uuid = uuid_;

	// Store Storage Type
	storage_type = PHYSICS_COUNT;

	// Store Physics Base
	base = PHYSICS_BASES::SOFT_BODY;

	// Read File
	read();

	// Initiailize Visualizer
	initializeVisualizer();

#ifdef EDITOR

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		data.script = 0;
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

Object::Physics::Soft::SpringMass::~SpringMass()
{
	// Delete Node Map
	if (node_count)
		delete[] node_map;

	// Delete Skip Node Array
	if (node_count)
		delete[] skip_nodes;

	// Delete Skip Spring Array
	if (spring_count)
		delete[] skip_springs;
}

glm::vec2* Object::Physics::Soft::SpringMass::pointerToPosition()
{
	return &data.position;
}

void Object::Physics::Soft::SpringMass::read()
{
	// If Data is Currently Loaded, Delete Data
	if (node_count)
		delete[] nodes;
	if (spring_count)
		delete[] springs;

	// Build Object if Using a Valid File
	if (file_name != "NULL")
	{
		// If Editor File is Empty, Load Empty Headers
		std::filesystem::path temp = path;
		std::error_code ec;
		if ((int)std::filesystem::file_size(temp, ec) == 0)
		{
			return;
		}
		if (ec)
		{
			return;
		}

		// Access File
		std::ifstream file;
		file.open(path, std::ios::binary);

		// Get Header of File
		uint8_t byte = 0;
		file.read((char*)&node_count, 1);
		if (node_count)
		{
			nodes = new Node[node_count + 1]; // +1 is in event selected node is Being Edited
			skip_nodes = new bool[node_count] {0};
		}
		file.read((char*)&spring_count, 1);
		if (spring_count)
		{
			springs = new Spring[spring_count];
			skip_springs = new bool[spring_count] {0};
		}

		// Read Rest of File
		NodeData node_data;
		Spring spring_data;
		int node_index = 0;
		int spring_index = 0;
		int max_node_name = 0;
		while (!file.eof())
		{
			// Read Type
			file.read((char*)&byte, 1);

			if (file.eof())
				break;

			// Read Node
			if (byte == 0)
			{
				file.read((char*)&node_data, sizeof(node_data));
				nodes[node_index] = Node(node_data);
				nodes[node_index].Position += data.position;
				node_index++;
			}

			// Read Spring
			else
			{
				file.read((char*)&spring_data, sizeof(spring_data));
				springs[spring_index] = Spring(spring_data);
				max_node_name = (spring_data.Node1 > max_node_name) ? spring_data.Node1 : max_node_name;
				max_node_name = (spring_data.Node2 > max_node_name) ? spring_data.Node2 : max_node_name;
				spring_index++;
			}
		}

		// Construct the Node Name Map
		node_map = new int16_t[max_node_name + 1]{0};
		for (int i = 0; i < max_node_name + 1; i++)
			node_map[i] = 0x7FFF;
		for (int i = 0; i < node_count; i++)
			node_map[nodes[i].Name] = i;

		// Link Springs to Correct Node
		for (int i = 0; i < spring_count; i++)
		{
			springs[i].Node1 = node_map[springs[i].Node1];
			springs[i].Node2 = node_map[springs[i].Node2];
		}
	}
}

void Object::Physics::Soft::SpringMass::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(PHYSICS);
	object_file.put((uint8_t)PHYSICS_BASES::SOFT_BODY);
	object_file.put((uint8_t)SOFT_BODY_TYPES::SPRING_MASS);

	// Write Object Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	uint16_t file_name_size = (uint16_t)file_name.size();
	object_file.write((char*)&file_name_size, sizeof(uint16_t));
	object_file.write((char*)&data, sizeof(ObjectData));
	object_file.write((char*)&file_name[0], file_name_size);

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Physics::Soft::SpringMass::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = PHYSICS;
	selector.object_identifier[1] = (uint8_t)PHYSICS_BASES::SOFT_BODY;
	selector.object_identifier[2] = (uint8_t)SOFT_BODY_TYPES::SPRING_MASS;

	// Store Editing Values
	selector.uuid = uuid;
	selector.object_data = data;
	selector.file_name = file_name;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data, file_name);

	// Reset Some Values
	selector.springmass_node_modified = false;
	selector.springmass_spring_modified = false;

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

bool Object::Physics::Soft::SpringMass::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 1.0f && x < data.position.x + 1.0f)
	{
		if (y > data.position.y - 1.0f && y < data.position.y + 1.0f)
		{
			return true;
		}
	}

	return false;
}

glm::vec2 Object::Physics::Soft::SpringMass::returnPosition()
{
	return data.position;
}

void Object::Physics::Soft::SpringMass::select2(Editor::Selector& selector)
{
	// Update Node Map
	node_map[selector.node_data.name] = node_count;

	// Store Pointer to Node
	selector.node_pointer = &nodes[node_count];

	// Link Springs to Correct Node
	for (int i = 0; i < spring_count; i++)
	{
		if (springs[i].Node1 == 0x7FFF)
			springs[i].Node1 = node_count;
		if (springs[i].Node2 == 0x7FFF)
			springs[i].Node2 = node_count;
	}
}

void Object::Physics::Soft::SpringMass::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, std::string& file_name)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("SpringMass", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addTextValue("File: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &file_name, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
}
