#include "UnsavedCollection.h"
#include "Render/Struct/DataClasses.h"
#include "ChangeController.h"

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
			change.move_with_parent = false;
	}
}

void Render::Objects::UnsavedCollection::makeOrphans()
{
	// Iterate Through All Children and Make them Orphans
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		data_object->setParent(nullptr);
		change_controller->handleSingleSelectorReturn(data_object, nullptr, false);
	}

	// Update Group Layers for All Children
	recursiveSetGroupLayer(0);
}
