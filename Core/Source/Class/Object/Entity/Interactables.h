#ifndef INTERACTABLES_H
#define INTERACTABLES_H

#ifndef DLL_HEADER
#include "EntityBase.h"
#endif

// These Entities Are the Only Entities That Test for Collisions
// Collisions are Tested Between Entities and/or Physics
// Collisions Can be Generic Between All Objects or Only for Specificly Identified Object
// These Collisions Should Behave Similar to That of a Trigger Mask
// These Interactions Can Cause Changes to the Object, the Object it Collided With, Or Cause a Function to Execute
// Like Triggers, They Have Their Own Invoke Function
// Ideal for Creating Buttons and Switches
// Also Can be Used to Create Explosions

namespace Object::Entity
{
	// Interactable Data
	struct InteractableData
	{
		// Index of Function
		uint16_t function_index = 0;
	};

	// Interactable Entity
	class Interactables : public EntityBase
	{
		// Interactable Data
		InteractableData interactable;

		// The Function That Invokes on Collision
		void(*invoke_function)(SubObject* object, Interactables* self);

	public:

		// Initialize Object
		Interactables(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, InteractableData& interactable_);

		// Update Object
		void updateObject();

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object 
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

#endif

	};
}

#endif
