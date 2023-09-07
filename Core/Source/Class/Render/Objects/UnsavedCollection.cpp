#include "UnsavedCollection.h"
#include "Render/Struct/DataClasses.h"
#include "ChangeController.h"
#include "Render/Editor/Notification.h"
#include "UnsavedLevel.h"
#include "UnsavedComplex.h"
#include "UnsavedGroup.h"

uint8_t Render::Objects::UnsavedCollection::getNumberOfChildren()
{
	return instance_with_changes.data_objects.size();
}

std::vector<DataClass::Data_Object*>& Render::Objects::UnsavedCollection::getChildren()
{
	return instance_with_changes.data_objects;
}

void Render::Objects::UnsavedCollection::addChild(DataClass::Data_Object* new_child)
{
	instance_with_changes.data_objects.push_back(new_child);
	instance_with_changes.number_of_loaded_objects++;
}

void Render::Objects::UnsavedCollection::disableMoveWithParent(DataClass::Data_Object * data_object)
{
	for (int i = 0; i < current_change_list->change_count; i++)
	{
		// Get the Current Change
		Change& change = *current_change_list->changes[i];

		// If Data Objects Match, Disable Move With Parent in Change
		if (change.data->getObjectIndex() == data_object->getObjectIndex())
			change.move_with_parent = MOVE_DISSABLED;
	}
}

void Render::Objects::UnsavedCollection::makeOrphans(Editor::Selector* selector, std::vector<DataClass::Data_Object*>& orphan_list)
{
	// If Complex, Provide Offset
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);
	std::vector<Object::Object*>* instance_vector = nullptr;

	// The Object Itself is the Complex Object
	if (getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
	{
		// Offset is the Stored Position of the Complex Parent Singleton
		offset = static_cast<UnsavedComplex*>(this)->getComplexParent()->getPosition();

		// Get the Distance the Complex Object Moved and Subtract It From the Offset
		instance_vector = &static_cast<UnsavedComplex*>(this)->getInstances();
		for (Object::Object* temp_object : *instance_vector)
		{
			if (static_cast<Object::TempObject*>(temp_object)->isOriginal())
			{
				offset -= *temp_object->pointerToPosition() - *static_cast<Object::TempObject*>(temp_object)->pointerToSelectedPosition();
				break;
			}
		}
	}

	// The Object is Part of a Complex Object
	else
	{
		// Offset is the Position of the First Complex Parent
		DataClass::Data_Object* root_parent = static_cast<UnsavedGroup*>(this)->getParent();
		instance_vector = &root_parent->getObjects();
		while (root_parent != nullptr)
		{
			if (root_parent->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
			{
				offset = root_parent->getPosition();
				break;
			}
			root_parent = root_parent->getParent();
		}
	}

	// Update Group Layers for All Children
	recursiveSetGroupLayer(0);

	// Make a Temporary Copy of the Children Data Objects
	int child_data_object_count = instance_with_changes.data_objects.size();
	DataClass::Data_Object** temp_data_object_holder = new DataClass::Data_Object*[child_data_object_count];
	for (int i = 0; i < child_data_object_count; i++)
		temp_data_object_holder[i] = instance_with_changes.data_objects.at(i);

	// Iterate Through All Children and Make them Orphans
	for (int i = 0; i < child_data_object_count; i++)
	{
		// Get the Currently Iterated Child
		DataClass::Data_Object* data_object = temp_data_object_holder[i];

		// Get the Unsaved Level the Object is Going to
		UnsavedLevel* level = change_controller->getUnsavedLevelObject(data_object);

		// Make a Copy of the Data Object
		DataClass::Data_Object* data_copy = data_object->makeCopySelected(*selector);
		
		// Add Data Copy to the Orphan List to be Orphaned After All Orphans Have Been Processed
		orphan_list.push_back(data_copy);

		// Apply Offset to Object and All Children
		data_copy->offsetPositionRecursive(offset);

		// Apply Secondary Offset of Original Child
		data_object->offsetOppositePosition(offset);

		// Remove Object From This Group
		createChangePop(data_object, Render::Objects::MOVE_WITH_PARENT::MOVE_SECONDARY_ONLY);

		// Add Object Into Level
		level->createChangeAppend(data_copy, MOVE_WITH_PARENT::MOVE_DISSABLED);
	}
}

bool Render::Objects::UnsavedCollection::preventCircularGroups(DataClass::Data_Object* parent, DataClass::Data_Object* test_child)
{
	// Get the Child's Object Index
	uint32_t child_index = test_child->getObjectIndex();

	// Test for All Possible Temp Objects Currently Loaded
	Object::Object* real_object = nullptr;
	for (Object::Object* test_object : parent->getObjects())
	{
		// Iterate Until Root Parent is Found or Test Child is Found
		Object::Object* current_parent = test_object->parent;
		while (current_parent != nullptr)
		{
			// If Child is Found, Throw Error Message
			if (current_parent->object_index == child_index)
			{
				// Send the Error Message
				std::string message = "ERROR: INVALID OPERATION\n\nAttempting to Create a Circular Group by\nAdding a Child to an Ancestor.\n\nCircular Groups are Not Allowed. Please\nSelect Another Object to Group.";
				notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);

				// Return False to Indicate Selection is Invalid
				return false;
			}

			// Test Next Parent
			current_parent = current_parent->parent;
		}
	}

	// Child Was Not Found
	return true;
}
