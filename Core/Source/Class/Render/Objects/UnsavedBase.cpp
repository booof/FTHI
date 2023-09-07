#include "UnsavedBase.h"
#include "UnsavedGroup.h"
#include "Render/Struct/DataClasses.h"

Render::Objects::UnsavedBase::SlaveStack::SlaveStack()
{
	// Create Initial Allocation for Stack
	stack_array = new Changes * [array_size];

	// Increment Size By 1 for Implementation of Unmodified Data
	stack_size++;
	head++;
}

uint8_t Render::Objects::UnsavedBase::SlaveStack::appendInstance(Changes* instance)
{
	// Increment Stack Index
	stack_index++;
	stack_size++;

	// If Current Index is Equal to the Size of Array, Reallocate Array
	if (stack_index == array_size)
	{
		// Create New Array With Size 1.5 X Size of Original Array
		array_size = (int)(array_size * 1.5f);
		Changes** new_array = new Changes * [array_size];

		// Move Data From Old Array to New Array
		for (uint8_t i = 0; i < stack_index; i++)
			new_array[i] = std::move(stack_array[i]);

		// Delete Old Array
		delete[] stack_array;

		// Store Pointer to New Array in Old Array Location
		stack_array = new_array;
	}

	// Insert Instance Into Array
	stack_array[stack_index] = instance;

	// Return Index of New Array in Stack
	return stack_index;
}

Render::Objects::UnsavedBase::Changes* Render::Objects::UnsavedBase::SlaveStack::returnInstance()
{
	return stack_array[stack_index];
}

bool Render::Objects::UnsavedBase::SlaveStack::isEmpty()
{
	return (stack_size == 0 || stack_size == 1);
}

void Render::Objects::UnsavedBase::SlaveStack::deleteInstance(Changes* instance)
{
	// Delete Each Change and Respective Data Object in Change Array
	for (Change* change : instance->changes)
	{
		// Data Classes may Only be Deleted When Deleting an Add Operation. This is because 
		// A Data Class can Only Appear Twice, Once for Each Operation, and the Add Will
		// ALLWAYS Happen First. This is Because an Add Operation is Only Completed When an
		// Object is Created From the Editor Window, or a Copy is Made When Selecting an Object,
		// Which Triggers the Removal of the Old Object and the Adding of the New Object
		if (change->change_type == ADD)
			delete change->data;

		// Every Change is Unique, Safe to Delete at Any Point
		delete change;
	}

	// Delete Actual Object
	delete instance;
}

void Render::Objects::UnsavedBase::SlaveStack::deleteStack()
{
	// Iterate Through Array and Delete Instances
	for (int i = tail; i < head; i++)
	{
		// Delete Instance
		deleteInstance(stack_array[i]);
	}
}

void Render::Objects::UnsavedBase::SlaveStack::removeRecentInstance()
{
	deleteInstance(stack_array[stack_size - 1]);
	stack_size--;
}

void Render::Objects::UnsavedBase::SlaveStack::removeRecentInstanceWithoutDeletion()
{
	// Only Changes and Instances Should be Deleted. No DataObjects
	for (Change* change : stack_array[stack_size - 1]->changes)
		delete change;
	delete stack_array[stack_size - 1];
	stack_size--;
}

void Render::Objects::UnsavedBase::SlaveStack::moveForward()
{
	stack_index++;
}

bool Render::Objects::UnsavedBase::SlaveStack::moveBackward()
{
	stack_index--;
	return stack_index == 0;
}

bool Render::Objects::UnsavedBase::AddChildrenStack::addObject(DataClass::Data_Object* new_object, uint8_t number_of_children)
{
	// If Attempting to Overflow Array, Return False
	if (stack_size == array_size)
		return false;

	// Insert Object Into Stack
	stack_array[stack_size] = AddChildrenObject{ number_of_children, new_object };

	// Increment Stack Pointer
	stack_size++;

	return true;
}

bool Render::Objects::UnsavedBase::AddChildrenStack::addChild(DataClass::Data_Object* new_child_object)
{
	// If Stack is Empty, Don't Add Object and Return False
	if (stack_size == 0)
		return false;

	// Add Child to Top Object
	AddChildrenObject& top_object = stack_array[stack_size - 1];
	top_object.object->addChild(new_child_object);
	top_object.number_of_children_left--;
	new_child_object->setParent(top_object.object);
	new_child_object->setGroupLayer(top_object.object->getGroupLayer() + 1);

	// If Number of Children Left is 0, Pop Back
	if (top_object.number_of_children_left == 0)
		stack_size--;

	// Return True Since a Child was Successfully Added
	return true;
}

DataClass::Data_Object* Render::Objects::UnsavedBase::lambdaDataObject(uint8_t object_identifier[4])
{

#define ENABLE_LAG2

#ifdef ENABLE_LAG2

	auto readFloors = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::HORIZONTAL_LINE): return new DataClass::Data_FloorMaskHorizontalLine(object_identifier[3]);
		case (Object::Mask::HORIZONTAL_SLANT): return new DataClass::Data_FloorMaskSlant(object_identifier[3]);
		case (Object::Mask::HORIZONTAL_SLOPE): return new DataClass::Data_FloorMaskSlope(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readLeftWalls = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::VERTICAL_LINE): return new DataClass::Data_LeftMaskVerticalLine(object_identifier[3]);
		case (Object::Mask::VERTICAL_CURVE): return new DataClass::Data_LeftMaskCurve(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readRightWalls = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::VERTICAL_LINE): return new DataClass::Data_RightMaskVerticalLine(object_identifier[3]);
		case (Object::Mask::VERTICAL_CURVE): return new DataClass::Data_RightMaskCurve(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readCeilings = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::HORIZONTAL_LINE): return new DataClass::Data_CeilingMaskHorizontalLine(object_identifier[3]);
		case (Object::Mask::HORIZONTAL_SLANT): return new DataClass::Data_CeilingMaskSlant(object_identifier[3]);
		case (Object::Mask::HORIZONTAL_SLOPE): return new DataClass::Data_CeilingMaskSlope(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readTriggers = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_TriggerMask(object_identifier[3]);
	};

	auto readMasks = [&object_identifier, &readFloors, &readLeftWalls, &readRightWalls, &readCeilings, &readTriggers]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object* ()> masks[5] = { readFloors, readLeftWalls, readRightWalls, readCeilings, readTriggers };
		return masks[object_identifier[1]]();
	};

	auto readTerrain = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_Terrain(object_identifier[1], object_identifier[2], object_identifier[3]);
	};

	auto readLights = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[1])
		{
		case (Object::Light::DIRECTIONAL): return new DataClass::Data_Directional(object_identifier[3]);
		case (Object::Light::POINT): return new DataClass::Data_Point(object_identifier[3]);
		case (Object::Light::SPOT): return new DataClass::Data_Spot(object_identifier[3]);
		case (Object::Light::BEAM): return new DataClass::Data_Beam(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readRigid = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_RigidBody(object_identifier[2], object_identifier[3]);
	};

	auto readSoft = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS): return new DataClass::Data_SpringMass(object_identifier[3]);
		case ((int)Object::Physics::SOFT_BODY_TYPES::WIRE): return new DataClass::Data_Wire(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readHinge = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Physics::HINGES::ANCHOR): return new DataClass::Data_Anchor(object_identifier[3]);
		case ((int)Object::Physics::HINGES::HINGE): return new DataClass::Data_Hinge(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readPhysics = [&object_identifier, &readRigid, &readSoft, &readHinge]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object* ()> physics[3] = { readRigid, readSoft, readHinge };
		return physics[object_identifier[1]]();
	};

	auto readEntities = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[1])
		{
		case ((int)Object::Entity::ENTITY_NPC): return new DataClass::Data_NPC(object_identifier[3]);
		case ((int)Object::Entity::ENTITY_CONTROLLABLE): return new DataClass::Data_Controllable(object_identifier[3]);
		case ((int)Object::Entity::ENTITY_INTERACTABLE): return new DataClass::Data_Interactable(object_identifier[3]);
		case ((int)Object::Entity::ENTITY_DYNAMIC): return new DataClass::Data_Dynamic(object_identifier[3]);
		default: return nullptr;
		}
	};

	auto readEffects = []()->DataClass::Data_Object* {
		return nullptr;
	};

	auto readGroups = []()->DataClass::Data_Object* {
		return new DataClass::Data_GroupObject();
	};

	std::function<DataClass::Data_Object* ()> objects[7] = { readMasks, readTerrain, readLights, readPhysics, readEntities, readEffects, readGroups };
	return objects[object_identifier[0]]();

#else

	return nullptr;

#endif

}

void Render::Objects::UnsavedBase::generateChangeList()
{
	if (!making_changes)
		current_change_list = new Changes;
	current_change_list->change_count++;
	making_changes = true;
}

void Render::Objects::UnsavedBase::changeToModified()
{
	float colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedBase::changeToUnmodified()
{
	float colors[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedBase::changeToSaved()
{
	float colors[4] = { 0.0f, 0.9f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedBase::changeColors(float* color)
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Change Colors
	for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), color);
	}

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::Objects::UnsavedBase::moveForwardsThroughChanges(Changes* changes)
{
	for (Change* change : changes->changes)
	{
		// If this is an Add Change, Add Change to Current Data Object List
		if (change->change_type == ADD)
			addWhileTraversing(change->data, change->move_with_parent);

		// If this is a Remove Change Remove Change from Current Data Object List
		else
			removeWhileTraversing(change->data);
	}
}

void Render::Objects::UnsavedBase::moveBackwardsThroughChanges(Changes* changes)
{
	for (Change* change : changes->changes)
	{
		// If this is a Remove Change, Add Change to Current Data Object List
		if (change->change_type == REMOVE)
		{
			addWhileTraversing(change->data, change->move_with_parent);
			if (change->move_with_parent == MOVE_WITH_PARENT::MOVE_SECONDARY_ONLY_NO_OFFSET)
				change->move_with_parent = MOVE_WITH_PARENT::MOVE_SECONDARY_ONLY;
		}

		// If this is an Add Change Remove Change from Current Data Object List
		else
			removeWhileTraversing(change->data);
	}
}

void Render::Objects::UnsavedBase::createChangeAppend(DataClass::Data_Object* data_object, MOVE_WITH_PARENT disable_move)
{
	// Generate Change List if Not Generated Already
	generateChangeList();

	// Generate the New Change
	Change* change = new Change;
	change->change_type = CHANGE_TYPES::ADD;
	change->move_with_parent = disable_move;

	// Store Data Object in Change
	change->data = data_object;

	// Execute Change
	instance_with_changes.data_objects.push_back(data_object);
	instance_with_changes.number_of_loaded_objects++;

	// Store Change in Change List
	current_change_list->changes.push_back(change);

	// Level Has Been Modified
	selected_unmodified = false;
	changeToModified();
}

void Render::Objects::UnsavedBase::createChangePop(DataClass::Data_Object* data_object_to_remove, MOVE_WITH_PARENT disable_move)
{
	// Generate Change List if Not Generated Already
	generateChangeList();

	// Generate the New Change
	Change* change = new Change;
	change->change_type = CHANGE_TYPES::REMOVE;
	change->move_with_parent = disable_move;

	// Find Data Object in the List of Data Objects, Remove it, and Store it in Change
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (instance_with_changes.data_objects.at(i) == data_object_to_remove)
		{
			change->data = instance_with_changes.data_objects.at(i);
			instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
			break;
		}
	}

	// Execute Change
	instance_with_changes.number_of_loaded_objects--;

	// Store Change in Change List
	current_change_list->changes.push_back(change);

	// Clear Objects List in Data Object
	data_object_to_remove->clearObjects();

	// Level Has Been Modified
	selected_unmodified = false;
	changeToModified();
}

void Render::Objects::UnsavedBase::resetChangeList()
{
	// Only Execute if Changes Have Been Made to Unsaved Level
	if (making_changes)
	{
		// Undo All Changes in the Current Change List
		moveBackwardsThroughChanges(current_change_list);

		// Delete the Current Change List
		delete current_change_list;

		// Indicate Changes Have Been Reset
		making_changes = false;
	}
}

void Render::Objects::UnsavedBase::traverseChangeList(bool backward)
{
	// Move Backwards Through Change List
	if (backward)
	{
		moveBackwardsThroughChanges(slave_stack.returnInstance());
		if (slave_stack.moveBackward())
			changeToUnmodified();
	}

	// Move Forwards Through Change List
	else
	{
		slave_stack.moveForward();
		moveForwardsThroughChanges(slave_stack.returnInstance());
		changeToModified();
	}

	// Perform Object-Specific Functions After Making Changes
	updatePostTraverse();
}

bool Render::Objects::UnsavedBase::finalizeChangeList()
{
	if (making_changes)
	{
		// Disabling this Flag Lets the Engine Know That New
		// Changes Should be Made
		making_changes = false;

		// Add Code to Store Change List Somewhere
		slave_stack.appendInstance(current_change_list);

		// Return True to Indicate Unsaved Level Has Been Changed
		// Causes Unsaved Level to be Added to Master Stack
		return true;
	}

	return false;
}

std::vector<Render::Objects::UnsavedBase::Change*>* Render::Objects::UnsavedBase::getChanges()
{
	if (current_change_list == nullptr)
		return nullptr;
	return &current_change_list->changes;
}
