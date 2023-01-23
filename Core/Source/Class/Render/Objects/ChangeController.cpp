#include "ChangeController.h"
#include "UnsavedLevel.h"
#include "Class/Render/Editor/Selector.h"
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
	level = level_;
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

Render::Objects::UnsavedLevel* Render::Objects::ChangeController::generateUnsavedLevel(int16_t x, int16_t y, int8_t z)
{
	static SavedIdentifier identifier;

	// Generate Object
	UnsavedLevel* new_unsaved_level = new UnsavedLevel();

	// Generate Unmodified Data
	new_unsaved_level->constructUnmodifiedData(x, y, z);

	// Test if Unsaved Level is Saved
	identifier.level_x = x;
	identifier.level_y = y;
	identifier.level_version = z;
	new_unsaved_level->saved = testIfSaved(identifier);

	// Store Object in Unsaved Levels Vector
	new_unsaved_level->unsaved_level_index = (uint8_t)unsaved_levels.size();
	unsaved_levels.push_back(new_unsaved_level);

	// Return New Unsaved Level
	return new_unsaved_level;
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
	current_instance->camera_pos = selector->getObjectPosition();

	// Get Position of Object in Terms of Level
	glm::vec2 object_level_position;
	updateLevelPos(selector->getObjectPosition(), object_level_position);

	// Get Unsaved Level of Where Object is Now
	UnsavedLevel* temp_unsaved_level2 = getUnsavedLevel((int)object_level_position.x, (int)object_level_position.y, 0);
	temp_unsaved_level2->createChangeAppend(selector);

	// Finalize Changes
	for (int i = 0; i < unsaved_levels.size(); i++)
	{
		UnsavedLevel& level = *unsaved_levels.at(i);
		if (level.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_levels.at(i));
	}

	// Reload Objects
	level->reloadAll();
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
	current_instance->camera_pos = selector->getObjectPosition();

	// As Deletion Change Has Already Been Made when Selecting, Simply Finalize the Changes

	// Get Position of Object in Terms of Level
	glm::vec2 object_level_position;
	updateLevelPos(selector->getObjectPosition(), object_level_position);

	// Finalize Changes
	for (int i = 0; i < unsaved_levels.size(); i++)
	{
		UnsavedLevel& level = *unsaved_levels.at(i);
		if (level.finalizeChangeList())
			current_instance->stack_indicies.push_back(unsaved_levels.at(i));
	}

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

	// Reset Vector
	unsaved_levels.clear();

	// Reset Master Stack
	master_stack->reset();

	// Reload Level
	level->reloadAll();
}

bool Render::Objects::ChangeController::returnIfUnsaved()
{
	return master_stack->retrunArraySize() > 1;
}

void Render::Objects::ChangeController::reset()
{
	// Reset Vector
	unsaved_levels.clear();

	// Reset Saved Vector
	saved_levels.clear();

	// Reset Master Stack
	master_stack->reset();

	// Reload Level
	level->reloadAll();
}

void Render::Objects::ChangeController::MasterStack::deleteInstance(uint8_t index)
{
	// Get Instance
	ChainMember& current_instance = stack_array[index];

	// For Each Unsaved Level in this Instance, Move the Slave Stack Back
	// By 1 and Delete Any Changes in that Slave Stack Instance
	for (UnsavedLevel* unsaved_level : current_instance.stack_indicies)
		unsaved_level->removeChainListInstance();

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
	for (UnsavedLevel* unsaved_level : chain_member.stack_indicies)
		unsaved_level->traverseChangeList(false);

	return true;
}

bool Render::Objects::ChangeController::MasterStack::traverseBackwards()
{
	// If At Tail of Stack, There is No Where to Traverse To
	if (stack_index == tail)
		return false;

	// Make Inverse Changes in Current Stack Index
	ChainMember& chain_member = stack_array[stack_index];
	for (UnsavedLevel* unsaved_level : chain_member.stack_indicies)
		unsaved_level->traverseChangeList(true);

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
