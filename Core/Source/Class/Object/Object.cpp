#include "Class/Object/Object.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Render/Objects/UnsavedGroup.h"
#include "Render/Objects/UnsavedComplex.h"
#include "Globals.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Terrain/TerrainBase.h"

void Object::Object::initializeScript(int script)
{
	// Access User Project and Retrieve Script at Index
	Global::bindFunctionPointer(script, this);
}

Object::Object::~Object()
{
	// Need to Find a "super" Equivalent to Java to Call This
	// Update: This Super Equivalent Happens Automatically
	// This Also Messes up Temp Objects

	// Delete the Children Array
	if (children != nullptr && storage_type != STORAGE_TYPES::NULL_TEMP)
		delete[] children;

	// Remove Self from DataObject List
	data_object->removeObject(this);
}

bool Object::Object::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Determine the Offset for the Highlighter
	glm::vec2 highlight_offset = glm::vec2(0.0f, 0.0f);
	Object* current_parent = parent;
	while (current_parent != nullptr)
	{
		// If Parent is Complex, Add Position To Offset
		if (current_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		{
			// If Parent is Temporary, Don't Highlight
			//if (current_parent->storage_type == NULL_TEMP)
			if (static_cast<Render::Objects::UnsavedComplex*>(current_parent->group_object)->testForSelectedInstance() && !(selector.selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])))
				return false;

			// Provide Offset
			if (current_parent->storage_type == STORAGE_TYPES::NULL_TEMP)
				highlight_offset += *static_cast<TempObject*>(current_parent)->pointerToSelectedPosition();
			else
				highlight_offset += *current_parent->pointerToPosition();
		}

		// Test Next Parent
		current_parent = current_parent->parent;
	}

	// Store Data Object in Selector
	selector.highlighted_object = data_object;

	// Store Object Information
	selector.highlighted_object->info(object_info);

	// Set Selector to Active Highlight
	selector.activateHighlighter(highlight_offset);

	// Successfull Highlight
	return true;
}

void Object::Object::updateSelectedComplexPosition(float deltaX, float deltaY)
{
	// Update Position of This Object
	updateSelectedPosition(deltaX, deltaY);

	// Update Positions of Children
	for (int i = 0; i < children_size; i++)
		children[i]->updateSelectedComplexPosition(deltaX, deltaY);
}

void Object::Object::debug_funct()
{
	std::cout << object_index << " i\n";
}

void Object::Object::drawGroupVisualizer()
{
	// This Function Has Been Changed to be Recursive
	if (parent == nullptr)
		data_object->drawGroupVisualizer(glm::vec2(0.0f, 0.0f));
}

Object::TempObject::TempObject(Object* object, glm::vec2* new_position_ptr)
{
	// Copy Data from Old Object to Current Object
	position = object->returnPosition();
	data_object = object->data_object;
	data_object->getObjects().push_back(this);
	object_index = object->object_index;
	parent = object->parent;
	children = object->children;
	children_size = object->children_size;
	group_object = object->group_object;
	storage_type = NULL_TEMP;
	selected_position = new_position_ptr;
}

Object::TempObject::~TempObject()
{
	// Remove This Object From Complex Instances, If Temp Object is a Group Object
	if (data_object->getGroup() != nullptr && data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		static_cast<Render::Objects::UnsavedComplex*>(data_object->getGroup())->removeComplexInstance(this);
}

glm::vec2* Object::TempObject::pointerToPosition()
{
	return &position;
}

bool Object::TempObject::testMouseCollisions(float x, float y)
{
	// This Object Should NEVER be Selected
	return false;
}

glm::vec2 Object::TempObject::returnPosition()
{
	return position;
}

void Object::TempObject::updateSelectedPosition(float deltaX, float deltaY)
{
	position += glm::vec2(deltaX, deltaY);
}

glm::vec2* Object::TempObject::pointerToSelectedPosition()
{
	return selected_position;
}

void DataClass::Data_Object::updateSelectedPositionsHelper(float deltaX, float deltaY, bool update_real)
{
	// If Group is Not Null, Perform Position Update on Children
	if (group_object != nullptr)
	{
		for (DataClass::Data_Object* child : group_object->getChildren())
		{
			if (child->move_with_parent)
				child->updateSelectedPosition(deltaX, deltaY, update_real);
			else
				child->move_with_parent = true;
		}
	}

	// Perform Position Update on All Object Pointers
	if (update_real)
	{
		for (std::vector<Object::Object*>::iterator it = object_pointers->begin(); it != object_pointers->end(); it++)
			(*it)->updateSelectedPosition(deltaX, deltaY);
	}
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
		Render::Objects::UnsavedGroup* new_unsaved_group = new Render::Objects::UnsavedGroup(object_identifier[3]);
		new_unsaved_group->setParent(this, false);
		group_object = new_unsaved_group;
	}

	// Store the Object in Group Object
	group_object->addChild(data_object);
}

void DataClass::Data_Object::addChildViaSelection(DataClass::Data_Object* data_object, bool disable_move)
{
	// If Group Has Not Been Initialized, Allocate Memory and Create Group
	if (group_object == nullptr)
	{
		Render::Objects::UnsavedGroup* new_unsaved_group = new Render::Objects::UnsavedGroup(object_identifier[3]);
		new_unsaved_group->setParent(this, false);
		group_object = new_unsaved_group;
	}

	// Create a New Append Change
	group_object->createChangeAppend(data_object, disable_move);
}

void DataClass::Data_Object::drawGroupVisualizer(glm::vec2 current_offset)
{
	if (group_object != nullptr)
	{
		// Offset is Causes by Complex Objects
		glm::vec2 new_offset = current_offset;
		if (group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			new_offset += getPosition();

		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
		for (DataClass::Data_Object* child : children)
		{
			// Draw Visualizers for All Children
			Vertices::Visualizer::visualizeLine(getPosition() + current_offset, child->getPosition() + new_offset, 1.0f, visualizer_colors[group_layer]);
			Vertices::Visualizer::visualizePoint(child->getPosition() + new_offset, 2.0f, visualizer_colors[group_layer + 1]);

			// Recursively Draw Visualizers
			child->drawGroupVisualizer(new_offset);
		}
	}
}

void DataClass::Data_Object::drawSelectedGroupVisualizer(glm::vec2 new_offset)
{
	if (group_object != nullptr)
	{
		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
		for (DataClass::Data_Object* child : children)
		{
			// Draw Visualizers for All Children
			Vertices::Visualizer::visualizeLine(getPosition(), child->getPosition() + new_offset, 1.0f, visualizer_colors[group_layer]);
			Vertices::Visualizer::visualizePoint(child->getPosition() + new_offset, 2.0f, visualizer_colors[group_layer + 1]);

			// Recursively Draw Visualizers
			child->drawGroupVisualizer(new_offset);
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

Render::Objects::UnsavedCollection* DataClass::Data_Object::getGroup()
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

std::vector<Object::Object*>& DataClass::Data_Object::getObjects()
{
	return *object_pointers;
}

void DataClass::Data_Object::removeObject(Object::Object* object)
{
	for (std::vector<Object::Object*>::iterator it = object_pointers->begin(); it != object_pointers->end(); it++)
	{
		if (*it == object)
		{
			object_pointers->erase(it);
			return;
		}
	}
}

void DataClass::Data_Object::clearObjects()
{
	object_pointers->clear();
}

Object::Object* DataClass::Data_Object::generateObject()
{
	Object::Object* new_pointer = genObject();
	new_pointer->data_object = this;
	new_pointer->name = name;
	new_pointer->clamp = editor_data.clamp;
	new_pointer->lock = editor_data.lock;
	new_pointer->object_index = object_index;
	if (object_pointers == nullptr)
		object_pointers = new std::vector<Object::Object*>;
	object_pointers->push_back(new_pointer);
	return new_pointer;
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
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
	{
		// Set Parent for Objects In the Group
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->setParent(selected_copy, false);

		// For Any Group Objects Currently Selected, Also Update Their Parents
		selector.updateParentofSelected(selected_copy);
	}

	// If Object Belongs to a Parent, Remove Offset if A Parent is Selected
	DataClass::Data_Object* root_parent = parent;
	while (root_parent != nullptr)
	{
		// If Current Parent is a Complex Parent, Stop Iteration
		if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			break;

		// If From a Group, and Is a Temp Object, Remove Offset if Original Moved
		if (root_parent->getObjects().at(0)->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
		{
			// Test if Root Parent Belongs to a Complex Group
			DataClass::Data_Object* complex_root = root_parent;
			while (complex_root != nullptr)
			{
				if (complex_root->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
					break;
				complex_root = complex_root->parent;
			}

			// If From a Group Object, Use Offset From Only Available Temp Object
			if (complex_root == nullptr)
			{
				Object::TempObject* temp = static_cast<Object::TempObject*>(root_parent->getObjects().at(0));
				glm::vec2 offset = *temp->pointerToPosition() - *temp->pointerToSelectedPosition();
				getPosition() += offset;
			}

			// If From a Complex Object, Use Offset of Currently Selected Group
			else
			{
				Object::Object* test_parent = static_cast<DataClass::Data_ComplexParent*>(static_cast<Render::Objects::UnsavedComplex*>(complex_root->getGroup())->getComplexParent())->getRootParent();
				Object::Object* complex_parent = nullptr;
				for (Object::Object* object : root_parent->getObjects())
				{
					// Get the Complex Parent of This Instance
					complex_parent = object->parent;
					while (complex_parent->group_object->getCollectionType() != Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
						complex_parent = complex_parent->parent;
					
					// If Complex Parent Matches the Selected Parent of Group, Use Offset of This Object
					if (complex_parent == test_parent)
					{
						Object::TempObject* temp = static_cast<Object::TempObject*>(object);
						glm::vec2 offset = *temp->pointerToPosition() - *temp->pointerToSelectedPosition();
						getPosition() += offset;
						break;
					}
				}
			}

			break;
		}

		// Get Next Parent
		root_parent = root_parent->getParent();
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

	// Assign New Real Object List
	object_pointers = new std::vector<Object::Object*>;

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

void DataClass::Data_Object::genChildrenRecursive(Object::Object*** object_list, int& list_size, Object::Object* parent, glm::vec2& offset)
{
	// Only Execute if Group is Not Nullptr Not Complex
	if (group_object != nullptr && group_object->getCollectionType() != Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
	{
		// Get Vector of Children
		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();

		// Only Execute if There Are Children
		if (children.size() > 0)
		{
			// Generate the Children Array in Object
			parent->children = new Object::Object*[children.size()];
			parent->children_size = 0;

			// Generate a New Array to Include Children
			int new_size = list_size + children.size();
			Object::Object** new_list = new Object::Object*[new_size];

			// Copy Old Objects Into New Array
			int i = 0;
			for (i = 0; i < list_size; i++)
				new_list[i] = (*object_list)[i];
			int children_begin = i;

			// Generate Children Into New Array
			for (DataClass::Data_Object* child : children)
			{
				Object::Object* new_object = child->generateObject();
				new_object->parent = parent;
				*new_object->pointerToPosition() += offset;
				parent->children[parent->children_size] = new_object;
				parent->children_size++;
				new_list[i] = new_object;
				i++;
			}

			// Delete the Old Array
			delete[] (*object_list);

			// Store New Array Values
			*object_list = new_list;
			list_size = new_size;

			// Recursively Generate Children
			for (int j = children_begin; j < new_size; j++)
				(*object_list)[j]->data_object->genChildrenRecursive(object_list, list_size, (*object_list)[j], offset);
		}
	}
}

bool DataClass::Data_Object::testIsParent(DataClass::Data_Object* parent)
{
	// If parent is a Complex Object, Test if Index is Located in Group Object Instances
	if (parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		return static_cast<Render::Objects::UnsavedComplex*>(parent->getGroup())->testForInstance(object_index);

	// Else, Test if Object Indices Match
	return object_index == parent->getObjectIndex();
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

void DataClass::Data_SubObject::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	data.position.x += deltaX;
	data.position.y += deltaY;

	if ((object_identifier[0] == Object::TERRAIN ||
		(object_identifier[0] == Object::PHYSICS && object_identifier[1] == (uint8_t)Object::Physics::PHYSICS_BASES::RIGID_BODY))
		&& object_identifier[2] == Shape::SHAPES::TRIANGLE)
		static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(this)->getShape())->updateSelectedPosition(deltaX, deltaY);

	updateSelectedPositionsHelper(deltaX, deltaY, update_real);
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
	// Update Model Matrix of Any Terrain Objects
	if (data_object->getObjectIdentifier()[0] == TERRAIN)
		static_cast<Terrain::TerrainBase*>(this)->updateModel();

	// Update Position of Data Object
	data.position += glm::vec2(deltaX, deltaY);
}

