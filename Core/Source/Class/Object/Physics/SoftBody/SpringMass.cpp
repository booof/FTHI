#include "SpringMass.h"
#include "Render/Struct/DataClasses.h"

// Common Functions
#include "Source/Algorithms/Common/Common.h"

#include "Render/Editor/Selector.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Globals.h"

Object::Physics::Soft::SpringMass::SpringMass(uint32_t& uuid_, ObjectData& data_, std::string& file_name_, DataClass::Data_SpringMass* data_object_)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_SPRING_MASS;

	// Store Data
	data = data_;
	file_name = file_name_;
	path = Global::project_resources_path + "/Models/SoftBodies/" + file_name;
	uuid = uuid_;
	data_object = data_object_;

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
			data_nodes = new DataClass::Data_SpringMassNode*[node_count];
			for (int i = 0; i < node_count; i++)
				data_nodes[i] = new DataClass::Data_SpringMassNode(0);
			skip_nodes = new bool[node_count] {0};
		}
		file.read((char*)&spring_count, 1);
		if (spring_count)
		{
			springs = new Spring[spring_count];
			data_springs = new DataClass::Data_SpringMassSpring*[spring_count];
			for (int i = 0; i < spring_count; i++)
				data_springs[i] = new DataClass::Data_SpringMassSpring(0);
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
				data_nodes[node_index]->readObjectData(file);
				data_nodes[node_index]->storeParent(static_cast<DataClass::Data_SpringMass*>(data_object));
				nodes[node_index] = Node(data_nodes[node_index]->genNode());
				//nodes[node_index].Position += data.position;
				node_index++;
			}

			// Read Spring
			else
			{
				data_springs[spring_index]->readObjectData(file);
				data_springs[spring_index]->storeParent(static_cast<DataClass::Data_SpringMass*>(data_object));
				springs[spring_index] = Spring(data_springs[spring_index]->genSpring());
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

void Object::Physics::Soft::SpringMass::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	for (int i = 0; i < node_count; i++)
		nodes[i].Position += glm::vec2(deltaX, deltaY);
}

glm::vec2 Object::Physics::Soft::SpringMass::returnPosition()
{
	return data.position;
}

void Object::Physics::Soft::SpringMass::select3(Editor::Selector& selector)
{
	// Update Node Map
	node_map[static_cast<DataClass::Data_SpringMassNode*>(selector.highlighted_object)->getNodeData().name] = node_count;

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
	return new Object::Physics::Soft::SpringMass(uuid, data, file_name, this);
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

DataClass::Data_SpringMass::Data_SpringMass(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS;
	object_identifier[3] = children_size;
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

Object::Object* DataClass::Data_SpringMassNode::genObject()
{
	return nullptr;
}

int& DataClass::Data_SpringMassNode::getScript()
{
	return parent->getScript();
}

glm::vec2& DataClass::Data_SpringMassNode::getPosition()
{
	return parent->getPosition();
}

void DataClass::Data_SpringMassNode::info(Editor::ObjectInfo& object_info)
{
}

DataClass::Data_Object* DataClass::Data_SpringMassNode::makeCopy()
{
	return this;
}

void DataClass::Data_SpringMassNode::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	node_data.position.x += deltaX;
	node_data.position.y += deltaY;
}

DataClass::Data_SpringMassNode::Data_SpringMassNode(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS;
	object_identifier[3] = children_size;
}

void DataClass::Data_SpringMassNode::writeObjectData(std::ofstream& object_file)
{

}

void DataClass::Data_SpringMassNode::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&node_data, sizeof(node_data));
}

Object::Physics::Soft::Node DataClass::Data_SpringMassNode::genNode()
{
	return Object::Physics::Soft::Node(node_data);
}

void DataClass::Data_SpringMassNode::storeParent(Data_SpringMass* springmass_object)
{
	parent = springmass_object;
	node_data.position += springmass_object->getPosition();
}

DataClass::Data_SpringMass* DataClass::Data_SpringMassNode::getParent()
{
	return parent;
}

Object::Physics::Soft::NodeData& DataClass::Data_SpringMassNode::getNodeData()
{
	return node_data;
}

Object::Object* DataClass::Data_SpringMassSpring::genObject()
{
	return nullptr;
}

int& DataClass::Data_SpringMassSpring::getScript()
{
	return parent->getScript();
}

glm::vec2& DataClass::Data_SpringMassSpring::getPosition()
{
	return parent->getPosition();
}

void DataClass::Data_SpringMassSpring::info(Editor::ObjectInfo& object_info)
{
}

DataClass::Data_Object* DataClass::Data_SpringMassSpring::makeCopy()
{
	return this;
}

void DataClass::Data_SpringMassSpring::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
}

DataClass::Data_SpringMassSpring::Data_SpringMassSpring(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS;
	object_identifier[3] = children_size;
}

void DataClass::Data_SpringMassSpring::writeObjectData(std::ofstream& object_file)
{

}

void DataClass::Data_SpringMassSpring::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&spring_data, sizeof(spring_data));
}

Object::Physics::Soft::Spring DataClass::Data_SpringMassSpring::genSpring()
{
	return spring_data;
}

void DataClass::Data_SpringMassSpring::storeParent(Data_SpringMass* springmass_object)
{
	parent = springmass_object;
}

DataClass::Data_SpringMass* DataClass::Data_SpringMassSpring::getParent()
{
	return parent;
}

Object::Physics::Soft::Spring& DataClass::Data_SpringMassSpring::getSpringData()
{
	return spring_data;
}
