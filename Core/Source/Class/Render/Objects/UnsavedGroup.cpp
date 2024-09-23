#include "UnsavedGroup.h"
#include "Render/Struct/DataClasses.h"
#include "ChangeController.h"

void Render::Objects::UnsavedGroup::setChildLayer(DataClass::Data_Object* data_object, int8_t new_layer)
{
	data_object->setGroupLayer(new_layer);
	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		data_group->recursiveSetGroupLayer(new_layer + 1);
}

void Render::Objects::UnsavedGroup::addWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset)
{
	// Add Object To Change List
	instance_with_changes.data_objects.push_back(data_object);

	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		parent_queue.enqueueParent(data_object, offset);

	// Set Group Layer
	setChildLayer(data_object, parent_pointer->getGroupLayer() + 1);
}

void Render::Objects::UnsavedGroup::removeWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset)
{
	// Remove Object From Change List
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (data_object == instance_with_changes.data_objects.at(i))
			instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
	}

	// Set Group Layer
	setChildLayer(data_object, 0);
}

void Render::Objects::UnsavedGroup::updatePostTraverse()
{
	// Plan: Update Parents of All Children With the Currently Stored Parent
	// Also, Update Group Layers for All Children

	// Update Parents of All Children
	updateParentofChildren();

	// Update Group Layers of Children
	recursiveSetGroupLayer(parent_pointer->getGroupLayer() + 1);
}

void Render::Objects::UnsavedGroup::removeChainListInstance()
{
	// No Need to Delete Instance Since DataObjects Don't Belong to This Object
	// Only Need to Decrement Stack Size
	slave_stack.removeRecentInstanceWithoutDeletion();
}

bool Render::Objects::UnsavedGroup::testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child)
{
	// Functionality is Diverted to Static Version
	return testValidSelectionStatic(parent, test_child);
}

bool Render::Objects::UnsavedGroup::testValidSelectionStatic(DataClass::Data_Object* parent, DataClass::Data_Object* test_child)
{
	// Only Standard Objects and Complex Root Objects Can be Selected
	// Hinge Connectors and Softbody Objects Are Not Allowed

	// If Attempting to Create a Circular Group, Stop That from Happening
	if (!preventCircularGroups(parent, test_child))
		return false;

	return true;
}

Render::Objects::UnsavedGroup::UnsavedGroup(uint8_t initial_size)
{
	// Allocate Memory for Children
	instance_with_changes.data_objects.reserve(initial_size);

	// Store Group in Change Controller
	change_controller->storeUnsavedGroup(this);
}

void Render::Objects::UnsavedGroup::recursiveSetGroupLayer(int8_t layer)
{
	// Set Next Layer Value
	int8_t next_layer = layer + 1;

	// Update All Children and Their Children
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		data_object->setGroupLayer(layer);
		if (data_object->getGroup() != nullptr)
			data_object->getGroup()->recursiveSetGroupLayer(next_layer);
	}
}

void Render::Objects::UnsavedGroup::recursiveSetModifiedOffset(glm::vec2& offset)
{
	// Don't Iterate If There Are No Changes
	if (current_change_list == nullptr)
		return;

	glm::vec2 offset2 = -offset;

	// Iterate Through All Children of the Modified Child and Provide Offset
	//for (Change* change : current_change_list->changes)
	//	change->data->offsetPositionRecursiveHelper(offset);
}

void Render::Objects::UnsavedGroup::updateParentofChildren()
{
	for (DataClass::Data_Object* child : instance_with_changes.data_objects)
		child->setParent(parent_pointer);
}

void Render::Objects::UnsavedGroup::setParent(DataClass::Data_Object* new_parent)
{
	// Apply the Offset Override for the New Parent
	//new_parent->getPosition() -= offset_override;

	// If Already Has a Parent, Move Children
	if (parent_pointer != nullptr)
	{
		// Determine How Much to Move Children
		glm::vec2 delta_pos = new_parent->getPosition() - parent_pointer->getPosition();

		//std::cout << "parent delta pos: " << delta_pos.x << " " << delta_pos.y << "   override: " << offset_override.x << " " << offset_override.y << "   parent offset: " << new_parent->getPosition().x - parent_pointer->getPosition().x << " " << new_parent->getPosition().y - parent_pointer->getPosition().y <<  "\n";

		// Move Objects
		//for (DataClass::Data_Object* child : instance_with_changes.data_objects)
		//	child->updateSelectedPosition(delta_pos.x, delta_pos.y, true);
		for (DataClass::Data_Object* child : instance_with_changes.data_objects)
			child->updateTraversePosition(delta_pos.x, delta_pos.y);
	}

	// Set the Parent Pointer to be the New Parent
	parent_pointer = new_parent;

	// Force an Update of Parent for Each Child
	updateParentofChildren();
}

void Render::Objects::UnsavedGroup::setParentTraverseChange(DataClass::Data_Object* new_parent, glm::vec2 offset)
{
	std::cout << "setting parent: " << new_parent << "   at offset: " << offset.x << " " << offset.y << "\n";

	// Apply Offset for Each Child
	for (DataClass::Data_Object* child : instance_with_changes.data_objects)
		child->updateTraversePosition(offset.x, offset.y);

	// Set Parent for Children
	setParentTraverseChangeNoMove(new_parent);
}

void Render::Objects::UnsavedGroup::setParentTraverseChangeNoMove(DataClass::Data_Object* new_parent)
{
	// Set the Parent Pointer to be the New Parent
	parent_pointer = new_parent;

	// Force an Update of Parent for Each Child
	updateParentofChildren();
}

DataClass::Data_Object* Render::Objects::UnsavedGroup::getParent()
{
	return parent_pointer;
}

void Render::Objects::UnsavedGroup::enqueueLevelParent(DataClass::Data_Object* data_object, glm::vec2 offset_override)
{
	parent_queue.enqueueParent(data_object, offset_override);
}

void Render::Objects::UnsavedGroup::finalizeParentMovement()
{
	parent_queue.moveParents();
}

Render::Objects::UNSAVED_COLLECTIONS Render::Objects::UnsavedGroup::getCollectionType()
{
	return UNSAVED_COLLECTIONS::GROUP;
}

void Render::Objects::UnsavedGroup::ParentQueue::enqueueParent(DataClass::Data_Object* parent, glm::vec2 offset_override)
{
	// Test if Queue is Full. If So, Throw Error
	if (queue_amount == PARENTS_ARRAY_SIZE)
		throw "y";

	// Add Parent to Queue
	parents_array[queue_amount] = QueuedParent(offset_override, parent);

	// Increment Array Size
	queue_amount++;
}

void Render::Objects::UnsavedGroup::ParentQueue::moveParents()
{
	// TODO: Rebase This Static Class Into Unsaved Collection as It Uses Both Groups and Complex Objects

	// For Each Parent, Perform Move Operation on Children
	for (int i = 0; i < queue_amount; i++)
	{
		// Get Parent and its Group
		QueuedParent& current_parent_holder = parents_array[i];
		DataClass::Data_Object* parent = current_parent_holder.parent;
		UnsavedCollection* parent_group = static_cast<UnsavedGroup*>(parent->getGroup());

		// If Parent is a Normal Group Object, Set Parent of Children and Apply Position Offsets
		if (parent->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::GROUP)
		{
			// Set New Parent for Group
			static_cast<UnsavedGroup*>(parent_group)->setParentTraverseChange(parent, current_parent_holder.offset);

		}

		// If Parent is a Complex Group Object, Set Parent Without Applying Position Offsets
		else
		{
			// Set New Parent for Complex Group
			parent_group->setParentTraverseChangeNoMove(parent);
		}

		// Recalculate the Group Layer for all Decendants
		parent_group->recursiveSetGroupLayer(parent->getGroupLayer() + 1);

	}

	// Set Queue Size to 0
	queue_amount = 0;
}

// Why is C++ Like This?
Render::Objects::UnsavedGroup::ParentQueue Render::Objects::UnsavedGroup::parent_queue;
