#include "Class/Object/Object.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Globals.h"

void Object::Object::initializeScript(int script)
{
	// Access User Project and Retrieve Script at Index
	Global::bindFunctionPointer(script, this);
}

void Object::Object::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Data Object in Selector
	selector.highlighted_object = data_object;

	// Store Object Information
	selector.highlighted_object->info(object_info);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

void Object::Object::debug_funct()
{
	std::cout << object_index << " i\n";
}

void DataClass::Data_Object::readEditorData(std::ifstream& editor_file)
{
	editor_file.read((char*)&editor_data, sizeof(EditorData));
	name.resize(editor_data.name_size);
	editor_file.read(&name[0], editor_data.name_size);
}

void DataClass::Data_Object::writeEditorData(std::ofstream& editor_file)
{
	editor_file.write((char*)&editor_data, sizeof(EditorData));
	editor_file.write(name.c_str(), name.size());
}

DataClass::Data_Object::Data_Object()
{
	object_index = Global::object_index_counter;
	Global::object_index_counter++;
}

void DataClass::Data_Object::writeObject(std::ofstream& object_file, std::ofstream& editor_file)
{
	object_file.write((char*)object_identifier, 3);
	writeObjectData(object_file);
	writeEditorData(editor_file);
}

void DataClass::Data_Object::readObject(std::ifstream& object_file, std::ifstream& editor_file)
{
	readObjectData(object_file);
	readEditorData(editor_file);
}

Object::Object* DataClass::Data_Object::generateObject()
{
	Object::Object* new_object = genObject();
	new_object->name = name;
	new_object->clamp = editor_data.clamp;
	new_object->lock = editor_data.lock;
	new_object->object_index = object_index;
	return new_object;
}


uint8_t* DataClass::Data_Object::getObjectIdentifier()
{
	return object_identifier;
}

uint32_t DataClass::Data_Object::getObjectIndex()
{
	return object_index;
}

DataClass::EditorData& DataClass::Data_Object::getEditorData()
{
	return editor_data;
}

std::string& DataClass::Data_Object::getName()
{
	return name;
}

DataClass::Data_Object* DataClass::Data_Object::makeCopyUnique()
{
	// Make Copy
	Data_Object* unique_copy = makeCopy();

	// Assign New Object Index
	unique_copy->object_index = Global::object_index_counter;
	Global::object_index_counter++;

	// Assign New UUID, if It Has One
	if (object_identifier[0] == 3)
	{
		if (object_identifier[1] == 0)
			static_cast<Data_UUID*>(static_cast<Data_RigidBody*>(unique_copy))->generateUUID();

		else if (object_identifier[1] == 1)
		{
			if (object_identifier[2] == 0)
				static_cast<Data_UUID*>(static_cast<Data_SpringMass*>(unique_copy))->generateUUID();

			if (object_identifier[2] == 1)
				static_cast<Data_UUID*>(static_cast<Data_Wire*>(unique_copy))->generateUUID();
		}

		else if (object_identifier[1] == 2)
		{
			if (object_identifier[2] == 0)
				static_cast<Data_UUID*>(static_cast<Data_Anchor*>(unique_copy))->generateUUID();

			if (object_identifier[2] == 1)
				static_cast<Data_UUID*>(static_cast<Data_Hinge*>(unique_copy))->generateUUID();
		}
	}

	else if (object_identifier[0] == 4)
		static_cast<Data_UUID*>(static_cast<Data_Entity*>(unique_copy))->generateUUID();

	return unique_copy;
}

Object::ObjectData& DataClass::Data_SubObject::getObjectData()
{
	return data;
}

int& DataClass::Data_SubObject::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_SubObject::getPosition()
{
	return data.position;
}

Shape::Shape* DataClass::Data_Shape::getShape()
{
	return shape;
}

uint32_t& DataClass::Data_UUID::getUUID()
{
	return uuid;
}

void DataClass::Data_UUID::generateUUID()
{
	// Increment UUID
	Global::uuid_counter++;

	// Store Change in File
	std::ofstream uuid_file;
	uuid_file.open(Global::project_resources_path + "/Data/CommonData/UUID.dat", std::ios::binary);
	uuid_file.write((char*)&Global::uuid_counter, sizeof(uint32_t));
	uuid_file.close();

	// Save New UUID
	uuid = Global::uuid_counter;
}
