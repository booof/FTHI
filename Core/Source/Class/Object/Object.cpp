#include "Class/Object/Object.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Render/Objects/UnsavedGroup.h"
#include "Globals.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Terrain/TerrainBase.h"

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

void Object::Object::drawGroupVisualizer()
{
	data_object->drawGroupVisualizer();
}

void DataClass::Data_Object::updateSelectedPositionsHelper(float deltaX, float deltaY)
{
	// If Group is Not Null, Perform Position Update on Children
	if (group_object != nullptr)
	{
		for (DataClass::Data_Object* child : group_object->getChildren())
		{
			if (child->move_with_parent)
				child->updateSelectedPosition(deltaX, deltaY);
			else
				child->move_with_parent = true;
		}
	}

	// If Object is Not Null, Perform Position Update on Object Pointer
	if (object_pointer != nullptr)
		object_pointer->updateSelectedPosition(deltaX, deltaY);
}

void DataClass::Data_Object::readEditorData(std::ifstream& editor_file)
{
	editor_file.read((char*)&editor_data, sizeof(EditorData));
	name.resize(editor_data.name_size);
	editor_file.read(&name[0], editor_data.name_size);
}

void DataClass::Data_Object::writeEditorData(std::ofstream& editor_file)
{
	editor_data.name_size = name.size();
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
	// Write Object
	object_file.write((char*)object_identifier, 4);
	writeObjectData(object_file);
	writeEditorData(editor_file);

	// Write Children
	if (group_object != nullptr)
	{
		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
		for (DataClass::Data_Object* child : children)
			child->writeObject(object_file, editor_file);
	}
}

void DataClass::Data_Object::readObject(std::ifstream& object_file, std::ifstream& editor_file)
{
	readObjectData(object_file);
	readEditorData(editor_file);
}

void DataClass::Data_Object::addChild(DataClass::Data_Object* data_object)
{
	// NOTE: Don't Have to Create a List In This Object for Children, as the Pointer to
	// Children Will be Taken Directly from the Unsaved Group Object

	// If Group Has Not Been Initialized, Allocate Memory and Create Group
	if (group_object == nullptr)
	{
		group_object = new Render::Objects::UnsavedGroup(object_identifier[3]);
		group_object->setParent(this, false);
	}

	// Store the Object in Group Object
	group_object->addChild(data_object);
}

void DataClass::Data_Object::addChildViaSelection(DataClass::Data_Object* data_object)
{
	// If Group Has Not Been Initialized, Allocate Memory and Create Group
	if (group_object == nullptr)
	{
		group_object = new Render::Objects::UnsavedGroup(object_identifier[3]);
		group_object->setParent(this, false);
	}

	// Create a New Append Change
	group_object->createChangeAppend(data_object);
}

void DataClass::Data_Object::drawGroupVisualizer()
{
	if (group_object != nullptr)
	{
		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
		for (DataClass::Data_Object* child : children)
		{
			Vertices::Visualizer::visualizeLine(getPosition(), child->getPosition(), 1.0f, visualizer_colors[group_layer]);
			Vertices::Visualizer::visualizePoint(child->getPosition(), 2.0f, visualizer_colors[group_layer + 1]);
		}
	}
}

void DataClass::Data_Object::drawParentConnection()
{
	if (parent != nullptr)
	{
		Vertices::Visualizer::visualizeLine(parent->getPosition(), getPosition(), 1.0f, visualizer_colors[group_layer - 1]);
		Vertices::Visualizer::visualizePoint(getPosition(), 2.0f, visualizer_colors[group_layer]);
	}
}

Render::Objects::UnsavedGroup* DataClass::Data_Object::getGroup()
{
	return group_object;
}

void DataClass::Data_Object::setParent(Data_Object* new_parent)
{
	parent = new_parent;
}

DataClass::Data_Object* DataClass::Data_Object::getParent()
{
	return parent;
}

void DataClass::Data_Object::disableMoveWithParent()
{
	move_with_parent = false;
}

void DataClass::Data_Object::enableMoveWithParent()
{
	move_with_parent = true;
}

bool DataClass::Data_Object::getMoveWithParent()
{
	return move_with_parent;
}

void DataClass::Data_Object::setGroupLayer(uint8_t new_layer)
{
	group_layer = new_layer;
}

uint8_t& DataClass::Data_Object::getGroupLayer()
{
	return group_layer;
}

Object::Object* DataClass::Data_Object::getObject()
{
	return object_pointer;
}

Object::Object* DataClass::Data_Object::generateObject()
{
	object_pointer = genObject();
	object_pointer->name = name;
	object_pointer->clamp = editor_data.clamp;
	object_pointer->lock = editor_data.lock;
	object_pointer->object_index = object_index;
	return object_pointer;
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

DataClass::Data_Object* DataClass::Data_Object::makeCopySelected(Editor::Selector& selector)
{
	// Make Copy
	Data_Object* selected_copy = makeCopy();

	// If Object Has a Group, Update Parent
	if (group_object != nullptr)
	{
		// Set Parent for Objects In the Group
		group_object->setParent(selected_copy, false);

		// For Any Group Objects Currently Selected, Also Update Their Parents
		selector.updateParentofSelected(selected_copy);
	}

	// Return Copy
	return selected_copy;
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

void DataClass::Data_SubObject::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;

	if ((object_identifier[0] == Object::TERRAIN ||
		(object_identifier[0] == Object::PHYSICS && object_identifier[1] == (uint8_t)Object::Physics::PHYSICS_BASES::RIGID_BODY))
		&& object_identifier[2] == Shape::SHAPES::TRIANGLE)
		static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(this)->getShape())->updateSelectedPosition(deltaX, deltaY);

	if (object_identifier[0] == Object::TERRAIN)
		static_cast<Object::Terrain::TerrainBase*>(object_pointer)->updateModel();

	updateSelectedPositionsHelper(deltaX, deltaY);
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

void Object::SubObject::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position += glm::vec2(deltaX, deltaY);
}
