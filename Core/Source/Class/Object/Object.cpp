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
#include "Render/Objects/UnsavedLevel.h"

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

	Render::Objects::ChangeController* cc = change_controller;

	// Delete the Children Array
	if (children != nullptr && storage_type != STORAGE_TYPES::NULL_TEMP)
		delete[] children;

	// Remove Self from DataObject List
	if (data_object != nullptr)
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

void Object::Object::genTerrainRecursively(int& offset_static, int& offset_dynamic, int& instance, int& instance_index)
{
	// Current Implementation Does a Depth-First Search, Might Change to Breadth-First Search Later
	// BFS would Require Custum Queue Data Structure

	// Iterate Through Children to Generate Possible Terrain
	for (int i = 0; i < children_size; i++)
		children[i]->genTerrainRecursively(offset_static, offset_dynamic, instance, instance_index);
}

bool Object::Object::select(Editor::Selector& selector, Editor::ObjectInfo& object_info, bool add_children)
{
	// TODO: When Selecting, Set Complex Parent Data to Include Correct Position and Object Index
	// Also, Fix Some of the Code for Parents

	std::cout << "highlighting object: " << object_index << "    " << data_object << " " << this << "\n";

	// Determine the Offset for the Highlighter
	glm::vec2 highlight_offset = glm::vec2(0.0f, 0.0f);
	Object* current_parent = parent;
	while (current_parent != nullptr)
	{
		// If Parent is a Master Element, Add Offset
		if (current_parent->data_object->getObjectIdentifier()[0] == ELEMENT && current_parent->data_object->getObjectIdentifier()[1] == Render::GUI::MASTER)
			highlight_offset += static_cast<DataClass::Data_MasterElement*>(current_parent->data_object)->getScrollOffsets();

		// If Parent is Complex, Add Position To Offset
		else if ((current_parent->data_object->getGroup() != nullptr && current_parent->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
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
	/*
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX
		&& static_cast<DataClass::Data_ComplexParent*>(static_cast<Render::Objects::UnsavedComplex*>(group_object)->getComplexParent())->isActive())
	{
		object_info.clearAll();
		selector.highlighted_object = data_object;
		selector.activateHighlighter(highlight_offset, Editor::SelectedHighlight::INVALID_SELECTION);
		return false;
	}
	*/

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

glm::vec2 Object::Object::calculateComplexOffset(bool include_self)
{
	// The Calculated Offset
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);

	// If Should Include Self, And is a Complex Object, Add Position to Offset
	if (include_self && data_object->getGroup() != nullptr && data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		offset = data_object->getPosition();

	// If There is No Parent, Return Nothing
	if (parent == nullptr)
		return offset;

	// Get Offset of Parent
	offset += parent->calculateComplexOffset(false);

	// If Parent is a Complex Group Object, Add Position to Offset
	if (parent->data_object->getGroup() != nullptr && parent->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX) {

		// If Temp, Add Selected Position of Where the Data Object Currently Is
		if (parent->storage_type == STORAGE_TYPES::NULL_TEMP)
			offset += *static_cast<TempObject*>(parent)->pointerToSelectedPosition();

		// Else, Add Selected Position
		else
			offset += parent->data_object->getPosition();
	}

	// Return Offset
	return offset;
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

void Object::Object::deactivateDecendants()
{
	// Set All Decendants to be Inactive
	for (int i = 0; i < children_size; i++)
		children[i]->deactivateDecendants();

	// Set This Object to be Inactive
	active_ptr->active = false;
}

bool Object::Object::testSelectedComplexAncestor(bool passed_group)
{
	// Note: Current Implementation Returns True if Any Ancestor of First Group is Selected
	// Needs Further Testing

	// Determine if Complex
	bool is_complex = passed_group || group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX;

	// Test if Object is Selected
	if (is_complex && storage_type == STORAGE_TYPES::NULL_TEMP && !static_cast<TempObject*>(this)->isReturned())
		return true;

	// Test Next Object, If it Exists
	if (parent != nullptr)
		return parent->testSelectedComplexAncestor(is_complex);

	// Else, Return False Since No Complex Objects are Selected
	return false;
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
	active_ptr = object->active_ptr;
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

bool Object::TempObject::isReturned()
{
	return returned;
}

void Object::TempObject::deactivateDecendants()
{
	// Set This Object to be Inactive
	active_ptr->active = false;
	
	// If There is No Group Object, Do Nothing Else
	if (group_object == nullptr)
		return;

	// For Each Child Data Object, Find the Instance Whose Parent is This Object and Recurse
	for (DataClass::Data_Object* data_object : group_object->getChildren()) {
		for (Object* real_object : data_object->getObjects()) {
			if (real_object->parent == this)
				real_object->deactivateDecendants();
		}
	}
}

glm::vec2 Object::TempObject::calculateComplexOffset(bool include_self)
{
	// If Should Include Self, And is a Complex Object, Return Selected Position
	if (include_self && data_object->getGroup() != nullptr && data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX) {
		return *selected_position;
	}

	// If Not Complex, and Has Parent, Test Parent
	if (parent != nullptr && (data_object->getGroup() == nullptr || data_object->getGroup()->getCollectionType() != Render::Objects::UNSAVED_COLLECTIONS::COMPLEX))
	{
		// Get Offset Recursively From Parent
		glm::vec2 offset = parent->calculateComplexOffset(false);

		// If Parent is a Complex Group Object, Add Position to Offset
		if (parent->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX) {

			// If Temp, Add Selected Position of Where the Data Object Currently Is
			if (parent->storage_type == STORAGE_TYPES::NULL_TEMP)
				offset += *static_cast<TempObject*>(parent)->pointerToSelectedPosition();

			// Else, Add Selected Position
			else
				offset += parent->data_object->getPosition();
		}

		return offset;
	}

	// Temp Objects Already Returned Selected Position, Nothing Left to Add
	return glm::vec2(0.0f, 0.0f);
}

void Object::TempObject::replaceSelectedPositionPointer(glm::vec2* new_pointer)
{
	selected_position = new_pointer;
	returned = true;
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
		new_unsaved_group->setParent(this);
		group_object = new_unsaved_group;
	}

	// Store the Object in Group Object
	group_object->addChild(data_object);
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
	if (group_object != nullptr)
		drawGroupVisualizerHelper(new_offset2, new_offset, new_offset, new_offset);
}

void DataClass::Data_Object::drawParentConnection(glm::vec2& real_complex_offset, glm::vec2& complex_offset)
{
	if (parent != nullptr)
	{
		if (parent->level_editor_values.original_conditions != 0)
			Vertices::Visualizer::visualizeLine(*parent->getOriginalObject()->pointerToSelectedPosition() + real_complex_offset, getPosition() + real_complex_offset, 0.5f, returnLineColor(group_layer - 1));
		else
			Vertices::Visualizer::visualizeLine(parent->getPosition() + complex_offset, getPosition() + real_complex_offset, 0.5f, returnLineColor(group_layer - 1));
		Vertices::Visualizer::visualizePoint(getPosition() + real_complex_offset, 1.0f, returnLineColor(group_layer));
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

	/*

	// If Object Has a Group, Update Parent for Each Child to the Copy
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
	{
		// Set Parent for Objects In the Group
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->setParent(selected_copy);

		// For Any Group Objects Currently Selected, Also Update Their Parents
		selector.updateParentofSelected(selected_copy);
	}

	*/

	/*

	// If Object Has a Group, Update Parent for Each Child to the Copy
	if (group_object != nullptr && group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
	{
		// Set Parent for Objects In the Group
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->setParent(selected_copy);

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

	*/

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

void DataClass::Data_Object::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	// Perform Update on Position
	updateTraveresPositionHelper(deltaX, deltaY);

	// If Group is Not Null, Perform Position Update on Children
	if (group_object != nullptr)
	{
		for (DataClass::Data_Object* child : group_object->getChildren())
		{
			child->updateSelectedPosition(deltaX, deltaY, update_real);
		}
	}

	// Perform Position Update on All Object Pointers
	if (update_real)
	{
		for (std::vector<Object::Object*>::iterator it = object_pointers->begin(); it != object_pointers->end(); it++)
			(*it)->updateSelectedPosition(deltaX, deltaY);
	}
}

void DataClass::Data_Object::updateTraversePosition(float deltaX, float deltaY)
{
	// If Object is Was Also Modified, Do Nothing to Prevent Double Counting
	// NOTE: Using Pointer Since Last Byte Can be 0
	if (level_editor_values.original_conditions != nullptr)
	//if (level_editor_values.change_list_flags.is_being_traversed)
		return;

	// Perform Position Update
	updateTraveresPositionHelper(deltaX, deltaY);

	// If Group is Null, and is Or Complex, DO NOT Perform Position Update on Children
	if (group_object == nullptr || group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		return;
	
	// Move Children
	for (DataClass::Data_Object* child : group_object->getChildren())
		child->updateTraversePosition(deltaX, deltaY);
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
	//offsetPosition(offset);

	// Check Children Recursively
	//offsetPositionRecursiveHelper(offset);
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
	if (index < 0)
		return visualizer_group;

	// Else, Return Color From 
	return visualizer_colors[index];
}

bool DataClass::Data_Object::hasReals()
{
	return object_pointers != nullptr;
}

void DataClass::Data_Object::becomeOrphan(Object::Object* real_object)
{
	// If Object Already Has No Parent, Remove From Level
	if (parent == nullptr) {
		forceRemoveFromUnsaved(real_object);
		return;
	}

	// Create a Change in the Change Controller to Remove Orphan From Current Group
	parent->group_object->createChangePop(this, real_object);

	// Set Parent to Nothing
	parent = nullptr;
}

void DataClass::Data_Object::adoptOrphan(Data_Object* orphan, Object::Object* real_object_orphan, Object::Object* real_object_new_parent, Editor::Selector& selector)
{
	// For Testing, Delete Later
	Render::Objects::UnsavedLevel& test = *change_controller->getUnsavedLevel(0, -1, 0);

	// Test if "Orphan" is Parent. If it is, Make This Object an Orphan and Add it to Level
	if (parent != nullptr && parent->testMatchingObject(orphan)) {

		// For All Children, Remove Complex Offset
		if (group_object != nullptr) {
			glm::vec2 complex_offset = real_object_new_parent->calculateComplexOffset(false);
			for (Data_Object* object : group_object->getChildren())
				object->updateSelectedPosition(complex_offset.x, complex_offset.y, false);
		}

		// Force This Object to Become an Orphan
		becomeOrphan(real_object_new_parent);

		// Regenerate Real Objects of This Orphaned Object
		regernerateRealObjects(nullptr, true);

		return;
	}

	// Test if "Orphan" is a Child of This Object. If so, Make it an Orphan and Add it to Level
	DataClass::Data_Object* orphan_parent = orphan->getParent();
	//if (orphan_parent != nullptr && orphan_parent->testMatchingObject(this)) {
	if (group_object != nullptr && group_object->testObjectExists(orphan)) {

		// Force Child to be an Orphan
		orphan->becomeOrphan(real_object_orphan);

		// Make Copy of Data Object for Change
		DataClass::Data_Object* orphan_copy = orphan->makeCopy();

		// Remove Compex Offset of Copied Object
		glm::vec2 complex_offset = real_object_orphan->calculateComplexOffset(false);
		orphan_copy->updateSelectedPosition(complex_offset.x, complex_offset.y, false);

		// Revert Position of Original Object to Pre-Selected Position
		orphan->getPosition() = orphan->level_editor_values.original_conditions->original_position;

		// Add Object to Container
		orphan_copy->forceAddtoUnsaved(glm::vec2(0.0f, 0.0f));

		// Move Real Objects Into Copied Object
		orphan_copy->regernerateRealObjects(nullptr, false);

		return;
	}

	// Make Sure Orphan is Already an Orphan
	orphan->becomeOrphan(real_object_orphan);

	// Make Copy of Data Object for Change
	DataClass::Data_Object* orphan_copy = orphan->makeCopy();
	orphan_copy->object_pointers = orphan->object_pointers;

	// Remove Compex Offset of Copied Object
	glm::vec2 complex_offset = glm::vec2(0.0f, 0.0f);
	if (real_object_new_parent != nullptr)
		//complex_offset = real_object_orphan->calculateComplexOffset() - real_object_new_parent->calculateComplexOffset();
		complex_offset = real_object_orphan->calculateComplexOffset(false) - real_object_new_parent->calculateComplexOffset(true);
	else
		complex_offset = real_object_orphan->calculateComplexOffset(false);
	orphan_copy->updateSelectedPosition(complex_offset.x, complex_offset.y, false);

	// Make a Group Object if One Does Not Already Exist
	if (group_object == nullptr) {
		group_object = new Render::Objects::UnsavedGroup(1);
		static_cast<Render::Objects::UnsavedGroup*>(group_object)->setParent(this);
		if (object_pointers != nullptr) {
			for (Object::Object* object : *object_pointers)
				object->group_object = group_object;	
		}
	}

	// Create a Change in the Change Controller to Add Orphan to Group
	group_object->createChangeAppend(orphan_copy, glm::vec2(0.0f, 0.0f));

	// Regenerate Orphan and its Decendants as Members of New Parent
	orphan_copy->regernerateRealObjects(this, false);

	// Set Parent of Copied Object to This
	orphan_copy->setParent(this);

	// Update Group Layer of Orphan
	orphan_copy->setGroupLayer(group_layer + 1);
	if (orphan_copy->getGroup() != nullptr)
		orphan_copy->getGroup()->recursiveSetGroupLayer(group_layer + 2);

	// Update Children of Real Objects
	
	// Apply Any Offsets from Group Objects
	//orphan_copy->getPosition() -= real_object_new_parent->calculateComplexOffset();
}

void DataClass::Data_Object::forceAddtoUnsaved(glm::vec2 complex_offset)
{
	// Determine Level Coordinates of Object
	glm::i16vec2 level_coords = glm::i16vec2(0, 0);
	change_controller->getCurrentContainer()->updateLevelPos(getPosition(), level_coords);

	// Get the Unsaved Level
	Render::Objects::UnsavedLevel* level = change_controller->getUnsavedLevel(level_coords.x, level_coords.y, 0);

	// Append Object to Unsaved Level
	level->createChangeAppend(this, glm::vec2(0.0f, 0.0f));
}

void DataClass::Data_Object::forceRemoveFromUnsaved(Object::Object* real_object)
{
	// Determine Level Coordinates of Object
	glm::i16vec2 level_coords = glm::i16vec2(0, 0);
	change_controller->getCurrentContainer()->updateLevelPos(getPosition(), level_coords);

	// Get the Unsaved Level
	Render::Objects::UnsavedLevel* level = change_controller->getUnsavedLevel(level_coords.x, level_coords.y, 0);

	// Append Object to Unsaved Level
	level->createChangePop(this, real_object);
}

glm::vec2 DataClass::Data_Object::getGroupOffsets(Object::Object* real_object)
{
	// The Total Offset
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);

	// Iterate Through Each Parent. If Parent Provides an Offset, Add to Total Offset
	DataClass::Data_Object* current_parent = this;
	while (current_parent != nullptr)
	{
		if (current_parent->group_object != nullptr && current_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX) {

			// Add Offset of Group Object
			offset += current_parent->getEditingOffset();

			// Add Possible Offsets of Real Parent, If it Exists
			if (real_object->parent != nullptr)
				offset += real_object->parent->data_object->getGroupOffsets(real_object->parent);
		}

		// Check Next Parents
		current_parent = current_parent->getParent();
		real_object = real_object->parent;
	}

	return offset;
}

void DataClass::Data_Object::generateInitialConditions(glm::vec2 initial_complex_offset)
{
	// If No Initial Conditions Objects Exist, Create One and One for Each Decendant
	if (level_editor_values.original_conditions == nullptr)
	{
		// Generate Object
		level_editor_values.original_conditions = new OriginalConditions;

		// Store Initial Values
		level_editor_values.original_conditions->original_position = getPosition() + initial_complex_offset;

		// Generate Initial Conditions for All Decendants
		if (group_object != nullptr) {
			std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
			for (int i = 0; i < children.size(); i++)
				children.at(i)->generateInitialConditions(initial_complex_offset);
		}

		return;
	}

	// If Code Ever Reaches Here, A Parent Object Has Been Previously Selected and This Object is Now Selected
	// Nothing Should Happen
}

void DataClass::Data_Object::resetInitialConditions()
{
	// Only Delete Initial Conditions if Not Already Nullptr
	if (level_editor_values.original_conditions == nullptr)
		return;

	// Delete Original Conditions to Avoid Memory Leak
	//delete level_editor_values.original_conditions;

	// Set Original Conditions to Nullptr for Next Change
	level_editor_values.original_conditions = nullptr;

	// Do the Same For All Other Children That Also Have Initial Conditions
	if (group_object == nullptr)
		return;

	// Delete Original Conditions of Children
	std::vector<DataClass::Data_Object*>& children = group_object->getChildren();
	for (int i = 0; i < group_object->getNumberOfChildren(); i++)
		children.at(i)->resetInitialConditions();
}

void DataClass::Data_Object::testChangeAppend()
{
	// If Object Has an Append, Create a New Append
	if (level_editor_values.original_conditions != nullptr && level_editor_values.original_conditions->append_change != nullptr)
	{
		// Delete Original Append Instance
		removeMostRecentAppend();

		// Create New Append Change in Parent's Group
		parent->group_object->createChangeAppend(this, glm::vec2(0.0f, 0.0f));

		return;
	}

	// If There are No Children, or Group Object is Complex, Do Nothing
	if (group_object == nullptr || group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		return;

	// Else, Test All Other Children
	for (Data_Object* child : group_object->getChildren())
		child->testChangeAppend();
}

glm::vec2 DataClass::Data_Object::getEditingOffset()
{
	return getPosition();
}

glm::vec2 DataClass::Data_Object::calculateOffsetOverride()
{
	// The Calculated Offset Override for All Modified Parents
	glm::vec2 offset_override = glm::vec2(0.0f, 0.0f);

	// If Object has No Parent or Parent is a Complex Object, Return No Offset, Recursion is Finished
	if (parent == nullptr || parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		return glm::vec2(0.0f, 0.0f);

	// Calculate the Distance the Parent Changed, If Parent Was Changed
	if (parent->level_editor_values.original_conditions != nullptr)
		offset_override = parent->level_editor_values.original_conditions->original_position - parent->getLevelPosition();

	// Return the Offset Override for All Parents
	return offset_override + parent->calculateOffsetOverride();
}

void DataClass::Data_Object::regernerateRealObjects(DataClass::Data_Object* new_parent, bool is_selected)
{
	// Determine if any Temporary Objects Exist
	bool objects_are_temporary = false;
	if (object_pointers->size() > 0)
		objects_are_temporary = object_pointers->at(0)->storage_type == Object::STORAGE_TYPES::NULL_TEMP;

	// Calculate the Number of Data Objects that are Decendants, Plus This Object
	int16_t data_object_count = countDecendantsRecursively();
	if (!is_selected)
		data_object_count++;

	// Calculate the Old Number of Objects to Remove from Container
	int32_t old_real_count = object_pointers->size() * data_object_count;

	// Calculate the New Number of Objects to Add in Container
	int32_t new_real_count = data_object_count;
	if (new_parent != nullptr && new_parent->object_pointers != nullptr)
		new_real_count *= new_parent->object_pointers->size();

	// Calculate the Change in Objects in Container
	int32_t delta_objects = new_real_count - old_real_count;

	// Set Old Real Objects to be Innactive
	for (Object::Object* object : *object_pointers)
		object->deactivateDecendants();

	// Remove Actives From Container and Reallocate Memory to Fit New Real Objects
	uint16_t initial_index = change_controller->getCurrentContainer()->reallocateObjectsArray(delta_objects);

	// If This Object is Selected, Increment the Delta Size by 1 to Make Room for Temp Object in Active Array
	// Without This, There Will be an Index Out of Bounds Error (Bad)
	if (is_selected)
		data_object_count++;

	// Generate New Objects Into Container
	if (new_parent == nullptr)
		change_controller->getCurrentContainer()->genObjectIntoContainer(this, nullptr, initial_index, data_object_count);
	else if (new_parent->object_pointers != nullptr) {
		for (Object::Object* real_parent : *new_parent->object_pointers)
			change_controller->getCurrentContainer()->genObjectIntoContainer(this, real_parent, initial_index, data_object_count);
	}

	// Apply Changes
	change_controller->getCurrentContainer()->loadObjects();
}

int16_t DataClass::Data_Object::countDecendantsRecursively()
{
	// If Group Object is Nullptr, Return Nothing since This Object is a Dead End
	if (group_object == nullptr)
		return 0;

	// The Number of Decendants of This Object
	int16_t decendant_count = 0;

	// Count the Number of Decendants
	for (DataClass::Data_Object* child : group_object->getChildren())
		decendant_count += child->countDecendantsRecursively();

	// Return Number of Decendants Plus The Number of Children of This Object
	return decendant_count + group_object->getChildren().size();
}

glm::vec2 DataClass::Data_Object::getLevelPosition()
{
	return getPosition();
}

DataClass::LevelEditorValues& DataClass::Data_Object::getLevelEditorFlags()
{
	return level_editor_values;
}

Object::TempObject* DataClass::Data_Object::getOriginalObject()
{
	// If Object is New, Return Nullptr
	if (object_pointers == nullptr)
		return nullptr;

	for (Object::Object* object : *object_pointers) {
		
		// Prevent Real Objects From Being Selected
		if (object->storage_type != Object::NULL_TEMP)
			continue;

		// Convert to Temp Object
		Object::TempObject* temp_object = static_cast<Object::TempObject*>(object);

		// If This Object is the Original, Return it
		if (temp_object->isOriginal())
			return temp_object;
	}
}

bool DataClass::Data_Object::testMatchingObject(DataClass::Data_Object* test_object)
{
	// Prevent This Object From Being Nullptr
	if (this == nullptr)
		return false;

	return this == test_object;
}

void DataClass::Data_Object::removeMostRecentAppend()
{
	// If No Append Exists, Do Nothing
	if (level_editor_values.original_conditions->append_change == nullptr)
		return;

	// Delete Append from Vector
	std::vector<Render::Objects::UnsavedBase::Change*>& changes = *static_cast<std::vector<Render::Objects::UnsavedBase::Change*>*>(level_editor_values.original_conditions->append_change_vector);
	for (int i = 0; i < changes.size(); i++)
	{
		if (level_editor_values.original_conditions->append_change == changes.at(i)) {
			changes.erase(changes.begin() + i);
			break;
		}
	}

	// Remove DataObject From Instance With Changes
	static_cast<Render::Objects::UnsavedBase*>(level_editor_values.original_conditions->append_unsaved_object)->yeetObjectFromInstance(this);

	// Set Append Change to Nullptr, In Case This Function is Called Again
	level_editor_values.original_conditions->append_change = nullptr;
}

bool DataClass::Data_Object::testSelectedComplexAncestor()
{
	return level_editor_values.original_conditions->original_object->testSelectedComplexAncestor(false);
}

void DataClass::Data_Object::replaceSelectedPosition(glm::vec2* new_ptr)
{
	// Store Current Value for New Pointer
	*new_ptr = getPosition();

	// For Every Temp Object, Replace Selected Position Pointer
	for (Object::Object* object : *object_pointers) {
		static_cast<Object::TempObject*>(object)->replaceSelectedPositionPointer(new_ptr);
	}
}

void DataClass::Data_SubObject::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	// Update Primary Position of Object
	data.position.x += deltaX;
	data.position.y += deltaY;

	// Update Extra Shape Positions If Triangle
	if ((object_identifier[0] == Object::TERRAIN ||
		(object_identifier[0] == Object::PHYSICS && object_identifier[1] == (uint8_t)Object::Physics::PHYSICS_BASES::RIGID_BODY))
		&& object_identifier[2] == Shape::SHAPES::TRIANGLE)
		static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(this)->getShape())->updateSelectedPosition(deltaX, deltaY);
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

