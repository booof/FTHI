#include "Class/Object/Object.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Render/Objects/UnsavedGroup.h"
#include "Render/Objects/UnsavedComplex.h"
#include "Globals.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Terrain/TerrainBase.h"
#include "Render/Editor/ObjectInfo.h"
#include "Render/Objects/ChangeController.h"

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

	// Mark Object is Dead
	if (active_ptr != nullptr)
	{
		// If False, Object is Considered Dead And Will be Skipped in Iteration
		active_ptr->alive = false;

		// Tell the Object's Level That It was Removed
		change_controller->incrementRemovedCount(active_ptr->level_pos.x, active_ptr->level_pos.y, 0);
	}
}

bool Object::Object::select(Editor::Selector& selector, Editor::ObjectInfo& object_info, bool add_children)
{
	// Determine the Offset for the Highlighter
	glm::vec2 highlight_offset = glm::vec2(0.0f, 0.0f);
	Object* current_parent = parent;
	while (current_parent != nullptr)
	{
		// If Parent is Complex, Add Position To Offset
		if ((current_parent->data_object->getGroup() != nullptr && current_parent->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			|| (current_parent->group_object != nullptr && current_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX))
		{
			// Provide Offset
			if (current_parent->storage_type == STORAGE_TYPES::NULL_TEMP)
				highlight_offset += *static_cast<TempObject*>(current_parent)->pointerToSelectedPosition();
			else
				highlight_offset += *current_parent->pointerToPosition();

			// If Parent is Temporary, Highlight With a Red Color
			if (static_cast<Render::Objects::UnsavedComplex*>(current_parent->group_object)->testForSelectedInstance() && !(selector.selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])))
			{
				object_info.clearAll();
				selector.highlighted_object = data_object;
				selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::INVALID_SELECTION);
				return false;
			}

			break;
		}

		// Test Next Parent
		current_parent = current_parent->parent;
	}

	// Highlight Previously Selected Objects with a Green Color
	// DO NOT SELECT
	if (!data_object->isSelectable())
	{
		object_info.clearAll();
		selector.highlighted_object = data_object;
		selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::PREVIOUS_SELECTION);
		return false;
	}

	// Test if Further Complex Parent is Temporary
	while (current_parent != nullptr)
	{
		if (((current_parent->data_object->getGroup() != nullptr && current_parent->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			|| (current_parent->group_object != nullptr && current_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX))
			&& static_cast<Render::Objects::UnsavedComplex*>(current_parent->group_object)->testForSelectedInstance() && !(selector.selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])))
		{
			object_info.clearAll();
			selector.highlighted_object = data_object;
			selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::INVALID_SELECTION);
			return false;
		}
		current_parent = current_parent->parent;
	}

	// If Object is Complex and is Active, Don't Select
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX
		&& static_cast<DataClass::Data_ComplexParent*>(static_cast<Render::Objects::UnsavedComplex*>(group_object)->getComplexParent())->isActive())
	{
		object_info.clearAll();
		selector.highlighted_object = data_object;
		selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::INVALID_SELECTION);
		return false;
	}

	// Store Data Object in Selector
	selector.highlighted_object = data_object;

	// Store Object Information
	selector.highlighted_object->info(object_info);

	// Set Selector to Active Highlight
	if (add_children)
		selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::ADD_CHILDREN);
	else
		selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::SELECTABLE);

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

Object::TempObject::TempObject(Object* object, glm::vec2* new_position_ptr, bool original_)
{
	// Copy Data from Old Object to Current Object
	position = object->returnPosition();
	original = original_;
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

bool Object::TempObject::isOriginal()
{
	// Update: Need to Check Storage Type First in in List With Normal Objects
	return storage_type == NULL_TEMP && original;
}

void DataClass::Data_Object::updateSelectedPositionsHelper(float deltaX, float deltaY, bool update_real)
{
	// If Group is Not Null, Perform Position Update on Children
	if (group_object != nullptr)
	{
		for (DataClass::Data_Object* child : group_object->getChildren())
		{
			if (child->move_with_parent == Render::Objects::MOVE_WITH_PARENT::MOVE_ENABLED)
				child->updateSelectedPosition(deltaX, deltaY, update_real);
			else
				child->move_with_parent = Render::Objects::MOVE_WITH_PARENT::MOVE_ENABLED;
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

Shape::Shape* DataClass::Data_Object::readNewShape(std::ifstream& object_file, uint8_t index)
{
	// Lambdas to Read a Shape
	static std::function<Shape::Shape* (std::ifstream& object_file)> shapes[5] = {
		[](std::ifstream& object_file_)->Shape::Shape* {return new Shape::Rectangle(object_file_); },
		[](std::ifstream& object_file_)->Shape::Shape* {return new Shape::Trapezoid(object_file_); },
		[](std::ifstream& object_file_)->Shape::Shape* {return new Shape::Triangle(object_file_); },
		[](std::ifstream& object_file_)->Shape::Shape* {return new Shape::Circle(object_file_); },
		[](std::ifstream& object_file_)->Shape::Shape* {return new Shape::Polygon(object_file_); }
	};

	return shapes[index](object_file);
}

void DataClass::Data_Object::position23Null(int& index2, int& index3, glm::vec2** position2, glm::vec2** position3)
{
	// Set Position 2 to the Dummy Vector
	*position2 = &Global::dummy_vec2;
	index2 = -1;

	// Save Code Reuse by Using the Null Position 3 Function
	position3Null(index3, position3);
}

void DataClass::Data_Object::position3Null(int& index3, glm::vec2** position3)
{
	// Set Position 3 to the Dummy Vector
	*position3 = &Global::dummy_vec2;
	index3 = -1;
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
		new_unsaved_group->setParent(this, Render::Objects::MOVE_WITH_PARENT::MOVE_DISSABLED);
		group_object = new_unsaved_group;
	}

	// Store the Object in Group Object
	group_object->addChild(data_object);
}

void DataClass::Data_Object::addChildViaSelection(DataClass::Data_Object* data_object, Render::Objects::MOVE_WITH_PARENT disable_move)
{
	// If Group Has Not Been Initialized, Allocate Memory and Create Group
	if (group_object == nullptr)
	{
		Render::Objects::UnsavedGroup* new_unsaved_group = new Render::Objects::UnsavedGroup(object_identifier[3]);
		new_unsaved_group->setParent(this, Render::Objects::MOVE_WITH_PARENT::MOVE_ENABLED);
		group_object = new_unsaved_group;
	}

	// Create a New Append Change
	group_object->createChangeAppend(data_object, disable_move);
}

void DataClass::Data_Object::drawGroupVisualizerHelper(glm::vec2& left_offset, glm::vec2& right_offset, glm::vec2& point_offset, glm::vec2 new_offset)
{
	// Draw With Normal Children
	std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
	for (DataClass::Data_Object* child : children)
	{
		// Draw Visualizers for All Children
		Vertices::Visualizer::visualizeLine(getPosition() + left_offset, child->getPosition() + right_offset, 0.5f, returnLineColor(group_layer));
		Vertices::Visualizer::visualizePoint(child->getPosition() + point_offset, 1.0f, returnLineColor(group_layer + 1));

		// Recursively Draw Visualizers
		child->drawGroupVisualizer(new_offset);
	}
}

void DataClass::Data_Object::drawGroupVisualizer(glm::vec2 current_offset)
{
	if (group_object != nullptr)
	{
		// Offset is Causes by Complex Objects
		glm::vec2 new_offset = current_offset;
		if (group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			new_offset += getPosition();

		// Draw Lines and Points With the Offset
		drawGroupVisualizerHelper(current_offset, new_offset, new_offset, new_offset);
	}
}

void DataClass::Data_Object::drawSelectedGroupVisualizer(glm::vec2 new_offset)
{
	static glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);

	if (group_object != nullptr)
		drawGroupVisualizerHelper(null_vec, new_offset, new_offset, new_offset);
}

void DataClass::Data_Object::drawSelectedGroupVisualizerOffset(glm::vec2 new_offset, glm::vec2 new_offset2)
{
	glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);
	if (group_object != nullptr)
		drawGroupVisualizerHelper(new_offset2, new_offset, new_offset, new_offset);
}

void DataClass::Data_Object::drawParentConnection()
{
	if (parent != nullptr)
	{
		Vertices::Visualizer::visualizeLine(parent->getPosition(), getPosition(), 0.5f, returnLineColor(group_layer - 1));
		Vertices::Visualizer::visualizePoint(getPosition(), 1.0f, returnLineColor(group_layer));
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

void DataClass::Data_Object::disableMoveWithParent(Render::Objects::MOVE_WITH_PARENT mode)
{
	move_with_parent = mode;
}

void DataClass::Data_Object::enableMoveWithParent()
{
	move_with_parent = Render::Objects::MOVE_WITH_PARENT::MOVE_ENABLED;
}

Render::Objects::MOVE_WITH_PARENT DataClass::Data_Object::getMoveWithParent()
{
	return move_with_parent;
}

void DataClass::Data_Object::setGroupLayer(int8_t new_layer)
{
	group_layer = new_layer;
}

int8_t& DataClass::Data_Object::getGroupLayer()
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

Object::Object* DataClass::Data_Object::generateObject(glm::vec2& offset)
{
	// Generate Object and Copy Data Object Values
	Object::Object* new_pointer = genObject(offset);
	new_pointer->data_object = this;
	new_pointer->name = name;
	new_pointer->clamp = editor_data.clamp;
	new_pointer->lock = editor_data.lock;
	new_pointer->object_index = object_index;
	new_pointer->group_object = group_object;
	std::cout << "\n\ngenerating object: " << object_index << "  at: " << new_pointer->returnPosition().x << " " << new_pointer->returnPosition().y << "\n\n\n";

	// If Object Pointers is Null, Generate the Vector
	if (object_pointers == nullptr)
		object_pointers = new std::vector<Object::Object*>;

	// Add the Newly Created Object to the List of Real Objects of This Data Object
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

	// If Object Has a Group, Update Parent for Each Child to the Copy
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
	{
		// Set Parent for Objects In the Group
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->setParent(selected_copy, Render::Objects::MOVE_WITH_PARENT::MOVE_DISSABLED);

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
				DataClass::Data_ComplexParent* complex_parent_data_object = static_cast<DataClass::Data_ComplexParent*>(static_cast<Render::Objects::UnsavedComplex*>(complex_root->getGroup())->getComplexParent());
				Object::Object* test_parent = complex_parent_data_object->getRootParent();
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

void DataClass::Data_Object::genChildrenRecursive(Object::Object*** object_list, int& list_size, Object::Object* parent, glm::vec2& offset, Editor::Selector* selector, bool test_groups)
{
	// Only Execute if Group is Not Nullptr Not Complex
	if (group_object != nullptr)
	{
		// Determine if This is the First Instance
		bool first_instance = selectable;

		// Mark Object as Selected
		if (selectable)
			selectable = !test_groups;

		// Get Vector of Children
		std::vector<DataClass::Data_Object*>& children = group_object->getChildren();

		// Calculate the Children Size
		int children_size = children.size();

		// Only Execute if There Are Children
		if (children_size)
		{
			// Generate the Children Array in Object
			parent->children = new Object::Object*[children_size];
			parent->children_size = 0;

			// Generate a New Array to Include Children
			int new_size = list_size + children_size;
			Object::Object** new_list = new Object::Object*[new_size];

			// Copy Old Objects Into New Array
			int i = 0;
			for (i = 0; i < list_size; i++)
				new_list[i] = (*object_list)[i];
			int children_begin = i;

			// If Object is Complex, Increment the Offset of the Object
			if (group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				offset = getPosition();

			// Generate Children Into New Array if Group Object
			for (DataClass::Data_Object* child : children)
			{
				Object::Object* new_object = child->generateObject(offset);
				new_object->parent = parent;
				parent->children[parent->children_size] = new_object;
				parent->children_size++;
				new_list[i] = new_object;
				i++;
			}

			// Delete the Old Array
			delete[](*object_list);

			// Store New Array Values
			*object_list = new_list;
			list_size = new_size;

			// Recursively Generate Children
			for (int j = children_begin; j < new_size; j++)
				(*object_list)[j]->data_object->genChildrenRecursive(object_list, list_size, (*object_list)[j], offset, selector, true);

			// Note: Need to Prevent This From Executing Further in Due to Offsets Changing
			if (test_groups && first_instance && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			{
				// Get Instances
				std::vector<Object::Object*>& instances = static_cast<Render::Objects::UnsavedComplex*>(group_object)->getInstances();

				// Gnerate Children for Each Instance
				for (Object::Object* instance : instances)
				{
					// Only Modify Selectable Instances, Selectable Instances Have Already Been Added
					if (instance->data_object->isSelectable())
					{
						// Recursively Generate Children
						for (int j = children_begin; j < new_size; j++)
						{
							// Iteratively Find the Root Complex Parent and Get Its Offset
							glm::vec2 new_vector = glm::vec2(0.0f, 0.0f);
							Object::Object* test_parent = instance->parent;
							while (test_parent != nullptr)
							{
								if (test_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
								{
									new_vector = *test_parent->pointerToPosition();
									break;
								}
								test_parent = test_parent->parent;
							}

							// Generate All Children of This Particular Instance
							instance->data_object->genChildrenRecursive(object_list, list_size, instance, new_vector, selector, false);
						}
					}
				}

				// Make Selectable Objects Unselectable
				for (Object::Object* instance : instances)
				{
					// Mark Data Object to be Selectable Again
					if (instance->data_object->isSelectable())
						selector->addUnselectable(instance->data_object);
					instance->data_object->disableSelecting();
				}
			}
		}
	}

	// Else, Make Sure to Mark as Unselectable
	else
		selectable = false;
}

bool DataClass::Data_Object::testIsParent(DataClass::Data_Object* parent)
{
	// If parent is a Complex Object, Test if Index is Located in Group Object Instances
	if (parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		return static_cast<Render::Objects::UnsavedComplex*>(parent->getGroup())->testForInstance(object_index);

	// Else, Test if Object Indices Match
	return object_index == parent->getObjectIndex();
}

void DataClass::Data_Object::disableSelecting()
{
	selectable = false;
}

void DataClass::Data_Object::enableSelection()
{
	// Enable for This Object
	selectable = true;

	// Enable For Children
	if (group_object != nullptr)
	{
		// Enable Selection for Children
		for (Data_Object* data_object : group_object->getChildren())
			data_object->enableSelection();
	}
}

void DataClass::Data_Object::enableSelectionNonRecursive()
{
	selectable = true;
}

bool DataClass::Data_Object::isSelectable()
{
	return selectable;
}

void DataClass::Data_Object::offsetPosition(glm::vec2& offset)
{
	// Most Objects Only Have 1 Position
	// The Exceptions are Lines and Triangles
	getPosition() += offset;
}

void DataClass::Data_Object::offsetPositionRecursive(glm::vec2& offset)
{
	// Offset Position of Object
	offsetPosition(offset);

	// Check Children Recursively
	offsetPositionRecursiveHelper(offset);
}

void DataClass::Data_Object::offsetPositionRecursiveHelper(glm::vec2& offset)
{
	// Recursively Offset Children
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
	{
		// Offset Unmodified Children
		for (Data_Object* child : group_object->getChildren())
			child->offsetPositionRecursive(offset);

		// Offset Children That Are Being Modified Also
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->recursiveSetModifiedOffset(offset);
	}
}

void DataClass::Data_Object::offsetOppositePosition(glm::vec2& offest)
{
	// Defualt: Most Objects Only Have 1 Position
}

glm::vec4& DataClass::Data_Object::returnLineColor(int8_t index)
{
	// The Colors Used to Visualize Parent-Child Relationships
	static glm::vec4 visualizer_colors[5] = {
		glm::vec4(0.0f, 0.0f, 0.8f, 1.0f), // Blue
		glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), // Red
		glm::vec4(0.0f, 0.8f, 0.0f, 1.0f), // Green
		glm::vec4(0.8f, 0.0f, 0.8f, 1.0f), // Pink
		glm::vec4(0.0f, 0.8f, 0.8f, 1.0f)  // Turquoise
	};

	// Color White for Groups
	static glm::vec4 visualizer_group = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// If Index is -1 (Group Object), Color Should be White
	if (index == -1)
		return visualizer_group;

	// Else, Return Color From 
	return visualizer_colors[index];
}

bool DataClass::Data_Object::hasReals()
{
	return object_pointers != nullptr;
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

	// Update Shape Position If Triangle
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

void DataClass::Data_Shape::offsetPosition(glm::vec2& offset)
{
	// Apply Normal Offset
	data.position += offset;

	// If Shape is a Triangle, Offset Triangle Vertices As Well
	if (object_identifier[2] == Shape::SHAPES::TRIANGLE)
	{
		Shape::Triangle& triangle = *static_cast<Shape::Triangle*>(shape);
		*triangle.pointerToSecondPosition() += offset;
		*triangle.pointerToThirdPosition() += offset;
	}
}

void DataClass::Data_Shape::offsetOppositePosition(glm::vec2& offset)
{
	// If Shape is a Triangle, Offset Triangle Vertices
	if (object_identifier[2] == Shape::SHAPES::TRIANGLE)
	{
		Shape::Triangle& triangle = *static_cast<Shape::Triangle*>(shape);
		*triangle.pointerToSecondPosition() += offset;
		*triangle.pointerToThirdPosition() += offset;
	}
}

void DataClass::Data_Shape::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is Allways Used at Index 3
	*position1 = &data.position;
	index1 = 3;

	// If Shape is a Triangle, Get Second and Third Position Pointers
	if (object_identifier[2] == Shape::SHAPES::TRIANGLE)
		static_cast<Shape::Triangle*>(shape)->setInfoPointers(index2, index3, position2, position3);

	// Else, Other Positions Are Not Important
	else
		position23Null(index2, index3, position2, position3);
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
	// Update Position of Data Object
	data.position += glm::vec2(deltaX, deltaY);

	// Update Shape Position If Triangle
	if (data_object->getObjectIdentifier()[2] == Shape::SHAPES::TRIANGLE)
	{
		// Terrain Triangle
		if (data_object->getObjectIdentifier()[0] == ObjectList::TERRAIN)
			static_cast<Shape::Triangle*>(static_cast<Terrain::TerrainBase*>(this)->returnShapePointer())->updateSelectedPosition(deltaX, deltaY);
		
		// Physics Triangle
		else if (data_object->getObjectIdentifier()[0] == ObjectList::PHYSICS && data_object->getObjectIdentifier()[1] == (uint8_t)Physics::PHYSICS_BASES::RIGID_BODY)
			static_cast<Shape::Triangle*>(static_cast<Physics::Rigid::RigidBody*>(this)->shape)->updateSelectedPosition(deltaX, deltaY);
	}

	// Update Model Matrix of Any Terrain Objects
	if (data_object->getObjectIdentifier()[0] == TERRAIN)
		static_cast<Terrain::TerrainBase*>(this)->updateModel();
}

