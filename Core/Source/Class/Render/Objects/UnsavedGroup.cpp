#include "UnsavedGroup.h"
#include "Render/Struct/DataClasses.h"
#include "ChangeController.h"

void Render::Objects::UnsavedGroup::setChildLayer(DataClass::Data_Object* data_object, uint8_t new_layer)
{
	data_object->setGroupLayer(new_layer);
	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		data_group->recursiveSetGroupLayer(new_layer + 1);
}

void Render::Objects::UnsavedGroup::addWhileTraversing(DataClass::Data_Object* data_object, bool move_with_parent)
{
	// Add Object To Change List
	instance_with_changes.data_objects.push_back(data_object);

	// Determine if Object Should Move With Parent
	if (!move_with_parent)
		data_object->disableMoveWithParent();

	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		parent_queue.enqueueParent(data_object);

	// Set Group Layer
	setChildLayer(data_object, parent_pointer->getGroupLayer() + 1);
}

void Render::Objects::UnsavedGroup::removeWhileTraversing(DataClass::Data_Object* data_object)
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

	return true;
}

Render::Objects::UnsavedGroup::UnsavedGroup(uint8_t initial_size)
{
	// Allocate Memory for Children
	instance_with_changes.data_objects.reserve(initial_size);

	// Store Group in Change Controller
	change_controller->storeUnsavedGroup(this);
}

void Render::Objects::UnsavedGroup::recursiveSetGroupLayer(uint8_t layer)
{
	// Set Next Layer Value
	uint8_t next_layer = layer + 1;

	// Update All Children and Their Children
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		data_object->setGroupLayer(layer);
		if (data_object->getGroup() != nullptr)
			data_object->getGroup()->recursiveSetGroupLayer(next_layer);
	}
}

void Render::Objects::UnsavedGroup::updateParentofChildren()
{
	for (DataClass::Data_Object* child : instance_with_changes.data_objects)
		child->setParent(parent_pointer);
}

void Render::Objects::UnsavedGroup::setParent(DataClass::Data_Object* new_parent, bool move)
{
	// Move Objects If Enabled
	if (move && parent_pointer != nullptr)
	{
		glm::vec2 delta_pos = new_parent->getPosition() - parent_pointer->getPosition();
		for (DataClass::Data_Object* child : instance_with_changes.data_objects)
		{
			if (child->getMoveWithParent())
				child->updateSelectedPosition(delta_pos.x, delta_pos.y, true);
			else
				child->enableMoveWithParent();
		}
	}

	// Set the Parent Pointer to be the New Parent
	parent_pointer = new_parent;

	// Force an Update of Parent for Each Child
	updateParentofChildren();
}

void Render::Objects::UnsavedGroup::enqueueLevelParent(DataClass::Data_Object* data_object)
{
	parent_queue.enqueueParent(data_object);
}

void Render::Objects::UnsavedGroup::finalizeParentMovement()
{
	parent_queue.moveParents();
}

Render::Objects::UNSAVED_COLLECTIONS Render::Objects::UnsavedGroup::getCollectionType()
{
	return UNSAVED_COLLECTIONS::GROUP;
}

void Render::Objects::UnsavedGroup::ParentQueue::enqueueParent(DataClass::Data_Object* parent)
{
	// Test if Queue is Full. If So, Throw Error
	if (queue_amount == PARENTS_ARRAY_SIZE)
		throw "y";

	// Add Parent to Queue
	parents_array[queue_amount] = parent;

	// Increment Array Size
	queue_amount++;
}

void Render::Objects::UnsavedGroup::ParentQueue::moveParents()
{
	// For Each Parent, Perform Move Operation on Children
	for (int i = 0; i < queue_amount; i++)
	{
		// Get Parent
		DataClass::Data_Object* parent = parents_array[i];

		// Get Group Object
		if (parent->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::GROUP)
		{
			UnsavedGroup* parent_group = static_cast<UnsavedGroup*>(parent->getGroup());

			// Set New Parent for Group
			parent_group->setParent(parent, true);

			// Recursively Set Group Layer
			parent_group->recursiveSetGroupLayer(parent->getGroupLayer() + 1);
		}
	}

	// Set Queue Size to 0
	queue_amount = 0;
}

// Why is C++ Like This?
Render::Objects::UnsavedGroup::ParentQueue Render::Objects::UnsavedGroup::parent_queue;
