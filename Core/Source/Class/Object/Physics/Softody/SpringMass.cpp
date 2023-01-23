#include "SpringMass.h"
#include "Render/Struct/DataClasses.h"

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
	path = Global::project_resources_path + "/Models/SoftBodies/" + file_name;
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

void Object::Physics::Soft::SpringMass::select3(Editor::Selector& selector)
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

Object::Object* DataClass::Data_SpringMass::genObject()
{
	return new Object::Physics::Soft::SpringMass(uuid, data, file_name);
}

void DataClass::Data_SpringMass::writeObjectData(std::ofstream& object_file)
{
	uint16_t file_name_size = file_name.size();
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&file_name_size, sizeof(uint16_t));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write(file_name.c_str(), file_name_size);
}

void DataClass::Data_SpringMass::readObjectData(std::ifstream& object_file)
{
	uint16_t file_name_size;
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&file_name_size, sizeof(uint16_t));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	file_name.resize(file_name_size);
	object_file.read(&file_name[0], file_name_size);
}

DataClass::Data_SpringMass::Data_SpringMass()
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS;
}

void DataClass::Data_SpringMass::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("SpringMass", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addTextValue("File: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &file_name, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
}

DataClass::Data_Object* DataClass::Data_SpringMass::makeCopy()
{
	return new Data_SpringMass(*this);
}

std::string& DataClass::Data_SpringMass::getFile()
{
	return file_name;
}

void DataClass::Data_SpringMass::generateInitialValues(glm::vec2& position)
{
	generateUUID();
	data.position = position;
	data.zpos = -1.0f;
	data.colors = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	file_name = "NULL";
}
