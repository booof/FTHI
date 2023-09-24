#include "ChangeController.h"
#include "UnsavedLevel.h"
#include "UnsavedGroup.h"
#include "UnsavedComplex.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Camera/Camera.h"
#include "Class/Render/Struct/DataClasses.h"
#include "Level.h"

void Render::Objects::ChangeController::updateLevelPos(glm::vec2 position, glm::vec2& level)
{
	level.x = floor(position.x / 128);
	level.y = floor(position.y / 64);
}

bool Render::Objects::ChangeController::testIfSaved(SavedIdentifier test_identifier)
{
	// Iterate Through Saved Array
	for (std::vector<SavedIdentifier>::iterator it = saved_levels.begin(); it != saved_levels.end(); it++)
	{
		if (*it == test_identifier)
			return true;
	}

	return false;
}

void Render::Objects::ChangeController::addToUnsaved(DataClass::Data_Object* data_object)
{
	// Set Parent in Object's Data Group
	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr && data_group->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
		static_cast<Render::Objects::UnsavedGroup*>(data_group)->setParent(data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);

	// Object Has a Parent
	DataClass::Data_Object* parent = data_object->getParent();
	if (parent != nullptr)
	{
		// If Complex, Create Offset

		// Get the Root Parent of Objects
		DataClass::Data_Object* root_parent = parent;
		while (root_parent->getParent() != nullptr)
			root_parent = root_parent->getParent();

		// If Parent Group is a Complex Group, Deactivate Root
		if (root_parent->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
		{
			// First, Offset Object
			glm::vec2 offset = -static_cast<DataClass::Data_ComplexParent*>(root_parent)->getPositionOffset();
			data_object->offsetPosition(offset);
			//data_object->getPosition() -= static_cast<DataClass::Data_ComplexParent*>(root_parent)->getPositionOffset();

			// Deactivate Object
			static_cast<DataClass::Data_ComplexParent*>(root_parent)->setInactive();
		}

		// Create a Change in the Group Object to Add the Object
		parent->getGroup()->createChangeAppend(data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);

		// Disable Move With Parent in the Event A Parent Was Also Changed
		parent->getGroup()->disableMoveWithParent(data_object);
	}

	// Object is in the Level
	else
	{
		// Get Position of Object in Terms of Level
		glm::vec2 object_level_position;
		updateLevelPos(data_object->getPosition(), object_level_position);

		// Get Unsaved Level of Where Object is Now
		UnsavedLevel* temp_unsaved_level = getUnsavedLevel((int)object_level_position.x, (int)object_level_position.y, 0);
		temp_unsaved_level->createChangeAppend(data_object, MOVE_WITH_PARENT::MOVE_ENABLED);
	}
}

void Render::Objects::ChangeController::initializeChangeController()
{
	// Initialize Master Stack
	master_stack = new MasterStack();

	// Store Unsaved Level Pointer
	master_stack->storePointerToUnsavedLevels(&unsaved_levels);

	// Initialize First Master Stack Instance
	master_stack->createInitialInstance();
}

Render::Objects::ChangeController* Render::Objects::ChangeController::get()
{
	return &instance;
}

void Render::Objects::ChangeController::storeLevelPointer(Level* level_)
{
	// Store the Level Pointer
	level = level_;

	// Reset the Change List
	reset(false);
}

Render::Objects::UnsavedLevel* Render::Objects::ChangeController::getUnsavedLevel(int16_t x, int16_t y, int8_t z)
{
	// Iterate Through Unsaved Levels to Find Matching Coords
	for (std::vector<UnsavedLevel*>::iterator it = unsaved_levels.begin(); it != unsaved_levels.end(); it++)
	{
		Render::Objects::UnsavedLevel* unsaved_level = *it;
		if (unsaved_level->level_x == x && unsaved_level->level_y == y && unsaved_level->level_version == z)
			return *it;
	}

	// Else, Create New Unsaved Level
	return generateUnsavedLevel(x, y, z);
}

Render::Objects::UnsavedLevel* Render::Objects::ChangeController::getUnsavedLevelObject(DataClass::Data_Object* object)
{
	// Convert Position into Level Coords
	glm::i16vec2 level_pos;
	level->updateLevelPos(object->getPosition(), level_pos);

	// Retrive the Level
	return getUnsavedLevel(level_pos.x, level_pos.y, 0);
}

Render::Objects::UnsavedLevel* Render::Objects::ChangeController::generateUnsavedLevel(int16_t x, int16_t y, int8_t z)
{
	static SavedIdentifier identifier;

	// Generate Object
	UnsavedLevel* new_unsaved_level = new UnsavedLevel();

	// Generate Unmodified Data
	new_unsaved_level->constructUnmodifiedData(x, y, z, level->getLevelDataPath(), level->getEditorLevelDataPath());

	// Test if Unsaved Level is Saved
	identifier.level_x = x;
	identifier.level_y = y;
	identifier.level_version = z;
	new_unsaved_level->saved = testIfSaved(identifier);

	// Store Object in Unsaved Levels Vector
	new_unsaved_level->unsaved_object_index = (uint8_t)unsaved_levels.size();
	unsaved_levels.push_back(new_unsaved_level);

	// Return New Unsaved Level
	return new_unsaved_level;
}

Render::Objects::UnsavedComplex* Render::Objects::ChangeController::getUnsavedComplex(std::string& file_path)
{
	// Iterate Though All Loaded Complex Objects and Compare File Names
	for (std::vector<UnsavedComplex*>::iterator it = unsaved_complex.begin(); it != unsaved_complex.end(); it++)
	{
		Render::Objects::UnsavedComplex* unsaved_complex = *it;
		if (unsaved_complex->getFilePath() == file_path)
			return unsaved_complex;
	}

	// Object Not Found, Create a New Unsaved Group

	// Generate New Object
	Render::Objects::UnsavedComplex* new_unsaved_complex = new Render::Objects::UnsavedComplex();

	// Read Complex Object Files
	new_unsaved_complex->constructUnmodifiedData(file_path);

	// Store Object in Unsaved Complex Vector
	new_unsaved_complex->unsaved_object_index = (uint8_t)unsaved_complex.size();
	unsaved_complex.push_back(new_unsaved_complex);

	// Return New Unsaved Complex Object
	return new_unsaved_complex;
}

void Render::Objects::ChangeController::incrementRemovedCount(int16_t x, int16_t y, int8_t z)
{
	// Get the Index of the Sublevel
	int8_t index = level->index_from_level(glm::i16vec2(x, y));
	if (index == -1)
		return;

	// Increment the Removed Count
	level->getSublevels()[index]->removed_count++;
}

void Render::Objects::ChangeController::storeUnsavedGroup(UnsavedGroup* new_group)
{
	unsaved_groups.push_back(new_group);
}

void Render::Objects::ChangeController::transferObject(DataClass::Data_Object* data_object, int16_t x, int16_t y, int8_t z)
{
	// Get Correct Unsaved Level
	UnsavedLevel* unsaved_level = getUnsavedLevel(x, y, z);

	// Place Object in Unsaved Level
	unsaved_level->transferObject(data_object);
}

void Render::Objects::ChangeController::handleSelectorReturn(Editor::Selector* selector)
{
	// Note: For Now, This Operation will Result in a Finalization of Changes. Will Change Later

	// If This Return Occoured Further Back in the Undo Chain, Remove all Future Undos from the Stack
	master_stack->deleteFromIndexToHead();

	// Create New Instance in Master Stack
	master_stack->appendInstance();

	// Get Pointer to Current Instance
	ChainMember* current_instance = master_stack->returnCurrentInstance();

	// Store Camera Position in Current Instance
	current_instance->camera_pos = glm::vec2(level->camera->Position.x, level->camera->Position.y);

	// Add All Data Objects
	for (DataClass::Data_Object* data_object : selector->data_objects)
		addToUnsaved(data_object);

	// Finalize Changes in Unsaved Levels
	for (int i = 0; i < unsaved_levels.size(); i++)
	{
		UnsavedLevel& level = *unsaved_levels.at(i);
		if (level.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_levels.at(i));
	}

	// Finalize Changes in Unsaved Groups
	for (int i = 0; i < unsaved_groups.size(); i++)
	{
		UnsavedGroup& group = *unsaved_groups.at(i);
		if (group.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_groups.at(i));
	}

	// Finalize Changes in Unsaved Complex
	for (int i = 0; i < unsaved_complex.size(); i++)
	{
		UnsavedComplex& group = *unsaved_complex.at(i);
		if (group.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_complex.at(i));
	}

	// Clear Temps
	level->clearTemps();

	// Perform a Parent Reload on All Complex Objects
	for (UnsavedComplex* group : unsaved_complex)
		static_cast<DataClass::Data_ComplexParent*>(group->getComplexParent())->setInactive();

	// Reload Objects
	level->reloadAll();

	// Note: Should Probably Find a Way to Incorporate Objects Similar to Single Selector Return
	// To Only Update Objects That Are Added. The Only Problem is Finding a Way to Deal With the
	// Temp Objects, Both Deleting Them and Removing Parents Set as Them
}

void Render::Objects::ChangeController::handleSingleSelectorReturn(DataClass::Data_Object* data_object, DataClass::Data_Object* original_object, Editor::Selector* selector, bool reload_all, bool keep_parent)
{
	// Get the Root Parent of Objects
	DataClass::Data_Object* root_parent = data_object;
	while (root_parent->getParent() != nullptr)
	{
		// Get the Next Parent to Test
		root_parent = root_parent->getParent();

		// If From a Root Parent, Remove Offset
		if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		{
			// Remove Offset From Objects and Children
			glm::vec2 offset = static_cast<DataClass::Data_ComplexParent*>(root_parent)->getPositionOffset();
			glm::vec2* offset_ptr = static_cast<Render::Objects::UnsavedComplex*>(root_parent->getGroup())->getSelectedPosition();
			if (offset_ptr != nullptr)
				offset = *offset_ptr;
			data_object->updateSelectedPosition(offset.x, offset.y, false);

			// Remove Children From Level
			level->removeMarkedChildrenFromList(data_object);
			break;
		}
	}

	// Get Position of Object in Terms of Level
	glm::vec2 object_level_position;
	updateLevelPos(data_object->getPosition(), object_level_position);

	// Get Unsaved Level of Where Object is Now
	UnsavedLevel* temp_unsaved_level = getUnsavedLevel((int)object_level_position.x, (int)object_level_position.y, 0);
	temp_unsaved_level->createChangeAppend(data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);

	// If Should Not Keep Parent, Remove Parent and Reset Group Object
	if (!keep_parent)
	{
		// Reset Group Variables
		data_object->setParent(nullptr);
		data_object->setGroupLayer(0);

		// Recursively set Group Layer
		UnsavedCollection* data_group = data_object->getGroup();
		if (data_group != nullptr && data_group->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
		{
			static_cast<Render::Objects::UnsavedGroup*>(data_group)->setParent(data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);
			data_group->recursiveSetGroupLayer(data_object->getGroupLayer() + 1);
		}
	}

	// Add Object and Children to Level, If From a Complex Object
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);
	if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
	{
		Object::Object** object_list = new Object::Object*[1];
		object_list[0] = data_object->generateObject(offset);
		object_list[0]->parent = nullptr;
		int list_size = 1;
		data_object->enableSelectionNonRecursive();
		data_object->genChildrenRecursive(&object_list, list_size, object_list[0], offset, selector, true);
		selector->addUnselectableRecursive(data_object);
		level->incorperatNewObjects(object_list, list_size);
	}

	// Add Single Object Into Level
	else
	{
		// Generate the Object
		Object::Object* new_object = data_object->generateObject(offset);

		level->incorperatNewObjects(&new_object, 1);
	}
}

void Render::Objects::ChangeController::handleSelectorRealReturn(DataClass::Data_Object* data_object, Editor::Selector* selector)
{
	// Get the Root Parent of Objects
	DataClass::Data_Object* root_parent = data_object;
	glm::vec2 offset = glm::vec2(0.0f, 0.0f);
	while (root_parent->getParent() != nullptr)
	{
		// Get the Next Parent to Test
		root_parent = root_parent->getParent();

		// If From a Root Parent, Remove Offset
		if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		{
			// Remove Offset From Objects and Children
			offset = static_cast<DataClass::Data_ComplexParent*>(root_parent)->getPositionOffset();
			glm::vec2* offset_ptr = static_cast<Render::Objects::UnsavedComplex*>(root_parent->getGroup())->getSelectedPosition();
			if (offset_ptr != nullptr)
				offset = *offset_ptr;

			// Remove Children From Level
			level->removeMarkedChildrenFromList(data_object);
			break;
		}
	}

	// Add Object to Unsaved
	addToUnsaved(data_object);

	// Add Object and Children to Level, If From a Complex Object
	if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
	{
		// Determine the Number of Instances to Create
		int instance_size = data_object->getParent()->getObjects().size();
		int list_size = instance_size;
		Object::Object** object_list = new Object::Object*[instance_size];
		Object::Object* root_real_parent = nullptr;
		glm::vec2 real_offset = glm::vec2(0.0f, 0.0f);

		// Dissable Selection for Data Object
		data_object->enableSelectionNonRecursive();
		selector->addUnselectableRecursive(data_object);

		// Generate Children and Set Parent for Each Instance
		for (int i = 0; i < instance_size; i++)
		{
			// Determine the Offset of This Individual Object
			real_offset = glm::vec2(0.0f, 0.0f);
			root_real_parent = data_object->getParent()->getObjects().at(i);
			while (root_real_parent != nullptr)
			{
				if (root_real_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				{
					real_offset = *root_real_parent->pointerToPosition();
					break;
				}
				root_real_parent = root_real_parent->parent;
			}

			// Generate the Object and Set the Parent
			object_list[i] = data_object->generateObject(real_offset);
			object_list[i]->parent = data_object->getParent()->getObjects().at(i);

			// Generate the Children
			data_object->genChildrenRecursive(&object_list, list_size, object_list[i], real_offset, selector, true);
		}

		// Add New Objects into the Level
		level->incorperatNewObjects(object_list, list_size);
	}

	// Add Single Object Into Level
	else
	{
		// Generate the Object
		Object::Object* new_object = data_object->generateObject(offset);

		level->incorperatNewObjects(&new_object, 1);
	}
}

void Render::Objects::ChangeController::handleSelectorDelete(Editor::Selector* selector)
{
	// This Operation will ALLWAYS Result in a Finalization of Changes

	// If This Return Occoured Further Back in the Undo Chain, Remove all Future Undos from the Stack
	master_stack->deleteFromIndexToHead();

	// Create New Instance in Master Stack
	master_stack->appendInstance();

	// Get Pointer to Current Instance
	ChainMember* current_instance = master_stack->returnCurrentInstance();

	// Store Camera Position in Current Instance
	current_instance->camera_pos = glm::vec2(level->camera->Position.x, level->camera->Position.y);

	// Remove Parent from All Groups of Selected Objects
	std::vector<DataClass::Data_Object*> orphan_list;
	for (DataClass::Data_Object* data_object : selector->data_objects)
	{
		if (data_object->getGroup() != nullptr)
			data_object->getGroup()->makeOrphans(selector, orphan_list);
	}

	// Finalize Orphans After Processing
	for (DataClass::Data_Object* orphan : orphan_list)
		orphan->setParent(nullptr);

	// As Deletion Change Has Already Been Made when Selecting, Simply Finalize the Changes

	// Finalize Changes in Unsaved Levels
	for (int i = 0; i < unsaved_levels.size(); i++)
	{
		UnsavedLevel& level = *unsaved_levels.at(i);
		if (level.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_levels.at(i));
	}

	// Finalize Changes in Unsaved Groups
	for (int i = 0; i < unsaved_groups.size(); i++)
	{
		UnsavedGroup& group = *unsaved_groups.at(i);
		if (group.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_groups.at(i));
	}

	// Finalize Changes in Unsaved Complex
	for (int i = 0; i < unsaved_complex.size(); i++)
	{
		UnsavedComplex& group = *unsaved_complex.at(i);
		if (group.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_complex.at(i));
	}

	// Clear Temps
	level->clearTemps();

	// Perform a Parent Reload on All Complex Objects
	for (UnsavedComplex* group : unsaved_complex)
		static_cast<DataClass::Data_ComplexParent*>(group->getComplexParent())->setInactive();

	// Reload Objects
	level->reloadAll();
}

void Render::Objects::ChangeController::handleSelectorCancelation(Editor::Selector* selector)
{
	// For All Data Objects, If From a Complex Object, Add Offset Back In
	for (DataClass::Data_Object* data_object : selector->data_objects)
	{
		// Get the Root Parent of Objects
		DataClass::Data_Object* root_parent = data_object;
		while (root_parent->getParent() != nullptr)
		{
			// Get the Next Parent to Test
			root_parent = root_parent->getParent();

			// If From a Root Parent, Remove Offset
			if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			{
				// Remove Offset From Objects and Children
				glm::vec2 offset = -static_cast<DataClass::Data_ComplexParent*>(root_parent)->getPositionOffset();
				glm::vec2* offset_ptr = static_cast<Render::Objects::UnsavedComplex*>(root_parent->getGroup())->getSelectedPosition();
				if (offset_ptr != nullptr)
					offset = -*offset_ptr;
				data_object->offsetPosition(offset);
				break;
			}
		}
	}

	// Undo the Current Changes in Levels
	for (UnsavedLevel* unsaved_level : unsaved_levels)
		unsaved_level->resetChangeList();

	// Undo the Current Changes in Groups
	for (UnsavedGroup* unsaved_group : unsaved_groups)
		unsaved_group->resetChangeList();

	// Undo the Current Changes in Complex
	for (UnsavedComplex* unsaved_complex_ : unsaved_complex)
		unsaved_complex_->resetChangeList();

	// Move Children of Group Objects
	Objects::UnsavedGroup::finalizeParentMovement();

	// Delete All Newly Selected Objects
	selector->deleteSelectedObjects();

	// Clear Temps
	level->clearTemps();

	// Perform a Parent Reload on All Complex Objects
	for (UnsavedComplex* group : unsaved_complex)
		static_cast<DataClass::Data_ComplexParent*>(group->getComplexParent())->setInactive();

	// Reload Objects
	level->reloadAll();
}

void Render::Objects::ChangeController::undo()
{
	// Traverse Backwards Trhough Stack
	if (master_stack->traverseBackwards())
		reloadObjects();
}

void Render::Objects::ChangeController::redo()
{
	// Traverse Forwards Through Stack
	if (master_stack->traverseForwards())
		reloadObjects();
}

void Render::Objects::ChangeController::reloadObjects()
{
	// Get Current Instance
	ChainMember* current_instance = master_stack->returnCurrentInstance();

	// Switch Instance in Each Unsaved Level
	for (int i = 0; i < unsaved_levels.size(); i++)
	{
		//unsaved_levels[i]->switchInstance(current_instance->stack_indicies[i]);
	}

	// Reload Objects in Level
	level->reloadAll(current_instance->camera_pos.x, current_instance->camera_pos.y);
}

void Render::Objects::ChangeController::revertAllChanges()
{
}

void Render::Objects::ChangeController::save()
{
	static SavedIdentifier identifier;
	bool saved;

	// Update Saved Levels Vector and Save Every Unsaved Level
	for (std::vector<UnsavedLevel*>::iterator it = unsaved_levels.begin(); it != unsaved_levels.end(); it++)
	{
		// Get Identifier
		identifier.level_x = (*it)->level_x;
		identifier.level_y = (*it)->level_y;
		identifier.level_version = (*it)->level_version;

		// Save Level
		saved = false;
		(*it)->write(saved);

		// Update Saved Levels Vector
		if (saved)
		{
			if (!testIfSaved(identifier))
				saved_levels.push_back(identifier);
		}
	}

	// Update Saved Complex Vector and Save Every Complex Object
	for (std::vector<UnsavedComplex*>::iterator it = unsaved_complex.begin(); it != unsaved_complex.end(); it++)
	{
		// Save Object
		saved = false;
		(*it)->write(saved);
	}

	// Reset Vectors
	unsaved_levels.clear();
	unsaved_groups.clear();
	unsaved_complex.clear();

	// Reset Master Stack
	master_stack->reset();

	// Reload Level
	level->reloadAll();
}

bool Render::Objects::ChangeController::returnIfUnsaved()
{
	return master_stack->retrunArraySize() > 1;
}

void Render::Objects::ChangeController::reset(bool reload)
{
	// Reset Unsaved Vectors
	unsaved_levels.clear();
	unsaved_complex.clear();
	unsaved_groups.clear();

	// Reset Saved Vector
	saved_levels.clear();

	// Reset Master Stack
	master_stack->reset();

	// Reload Level
	if (reload)
		level->reloadAll();
}

void Render::Objects::ChangeController::drawVisualizers()
{
	// Draw Visualizers for Complex Objects
	for (std::vector<UnsavedComplex*>::iterator it = unsaved_complex.begin(); it != unsaved_complex.end(); it++)
		(*it)->drawVisualizer();
}

void Render::Objects::ChangeController::MasterStack::deleteInstance(uint8_t index)
{
	// Get Instance
	ChainMember& current_instance = stack_array[index];

	// For Each Unsaved Level in this Instance, Move the Slave Stack Back
	// By 1 and Delete Any Changes in that Slave Stack Instance
	for (UnsavedBase* unsaved_level : current_instance.stack_indicies)
		unsaved_level->removeChainListInstance();

	// Should be Done in Reverse Order to Delete Grouping Objects First
	//for (int i = current_instance.stack_indicies.size() - 1; i >= 0; i--)
	//	current_instance.stack_indicies[i]->removeChainListInstance();

	// Reset Size of Vector
	current_instance.stack_indicies.clear();
	current_instance.stack_indicies.reserve(0);
}

Render::Objects::ChangeController::MasterStack::MasterStack()
{
	// Allocate Memory for Array
	stack_array = new ChainMember[100];
}

void Render::Objects::ChangeController::MasterStack::storePointerToUnsavedLevels(std::vector<UnsavedLevel*>* pointer)
{
	unsaved_level_pointer = pointer;
}

bool Render::Objects::ChangeController::MasterStack::traverseForwards()
{
	// If At Head of Stack, There is No Where to Traverse To
	if (stack_index == head)
		return false;

	// If At End of Array, Circle Back to Beginning of Array
	if (stack_index == max_master_stack_size - 1)
		stack_index = 0;

	// Else, Increment Index
	else
		stack_index++;

	// Make Changes in the Current Stack Index
	ChainMember& chain_member = stack_array[stack_index];
	for (UnsavedBase* unsaved_level : chain_member.stack_indicies)
		unsaved_level->traverseChangeList(false);

	// Move Children of Group Objects
	Objects::UnsavedGroup::finalizeParentMovement();

	return true;
}

bool Render::Objects::ChangeController::MasterStack::traverseBackwards()
{
	// If At Tail of Stack, There is No Where to Traverse To
	if (stack_index == tail)
		return false;

	// Make Inverse Changes in Current Stack Index
	ChainMember& chain_member = stack_array[stack_index];

	//for (UnsavedBase* unsaved_level : chain_member.stack_indicies)

	for (UnsavedBase* unsaved_level : chain_member.stack_indicies)
		unsaved_level->traverseChangeList(true);

	// Move Children of Group Objects
	Objects::UnsavedGroup::finalizeParentMovement();

	// If At Beginning of Array, Circle Back to End of Array
	if (stack_index == 0)
		stack_index = max_master_stack_size - 1;
	
	// Else, Decremenmt Index
	else
		stack_index--;

	return true;
}

void Render::Objects::ChangeController::MasterStack::appendInstance()
{
	// Get Pointer to Head Instance
	ChainMember& current_instance = stack_array[head];

	// If Head is Equal to Array Size - 1, Set Head to Beginning of Array
	if (head == max_master_stack_size - 1)
		head = 0;

	// Else, Increment Head
	else
		head++;

	// If Head is At Tail, Delete Instance at Tail
	if (head == tail)
	{
		// Delete Tail Instance
		deleteInstance(tail);

		// If Tail is Equal to Array Size - 1, Set Tail to Beginning of Array
		if (tail = max_master_stack_size - 1)
			tail = 0;

		// Else, Increment Tail
		tail++;
	}

	// Else, Increment Stack Size
	else
		stack_size++;

	// Get Pointer to New Head Instance
	ChainMember& new_instance = stack_array[head];

	// Store Camera Coords as Origin
	new_instance.camera_pos = glm::vec2(0.0f, 0.0f);

	// Set Stack Index to Head
	stack_index = head;
}

Render::Objects::ChangeController::ChainMember* Render::Objects::ChangeController::MasterStack::returnCurrentInstance()
{
	return &stack_array[stack_index];
}

void Render::Objects::ChangeController::MasterStack::deleteFromIndexToHead()
{
	// If Stack Index is Equal to Head, Do Nothing
	if (stack_index == head)
		return;

	// Stack Index is Less Than Head
	if (stack_index < head)
	{
		// Delete Instances Between Stack Index and Head	
		for (int i = head; i > stack_index; i--)
			 deleteInstance(i);
	}

	// Stack Index is Greater Than Head
	else
	{
		// Delete Instances Between Stack Index and End of Array
		//for (int i = stack_index + 1; i < max_master_stack_size; i++)
		for (int i = max_master_stack_size - 1; i > stack_index; i--)
			deleteInstance(i);

		// Delete Instances Between Beginning of Array and Head
		//for (int i = 0; i <= head; i++)
		for (int i = head; i >= 0; i--)
			deleteInstance(i);
	}

	// Set Head Equal to Index
	head = stack_index;
}

void Render::Objects::ChangeController::MasterStack::createInitialInstance()
{
	// Set Initial Camera to Origin
	stack_array[0].camera_pos = glm::vec2(0.0f, 0.0f);

	// Increment Stack Size
	stack_size++;
}

void Render::Objects::ChangeController::MasterStack::reset()
{
	// Delete Chain Members
	delete[] stack_array;

	// Reset Variables
	stack_size = 0;
	tail = 0;
	head = 0;
	stack_index = 0;

	// Allocate Memory for New Array
	stack_array = new ChainMember[100];

	// Initialize First Master Stack Instance
	createInitialInstance();
}

int Render::Objects::ChangeController::MasterStack::retrunArraySize()
{
	return stack_size;
}

bool Render::Objects::operator==(const SavedIdentifier& first, const SavedIdentifier& second)
{
	return (first.level_x == second.level_x && first.level_y == second.level_y && first.level_version == second.level_version);
}

Render::Objects::ChangeController Render::Objects::ChangeController::instance;
