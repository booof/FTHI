#pragma once
#ifndef DYNAMICS_H
#define DYNAMICS_H

#ifndef DLL_HEADER
#include "EntityBase.h"
#endif

// These Entities Exist as Objects That Are Intended to be Instanced
// These Objects are Frequently Dynamically Allocated and Freed
// Each Instance is Stored in an Array in Class That is Dynamically Allocated on Initialization
// Each Instance Will Contain Its Own Instance Data That is Configured in the Script
// Collisions Between Physics and Entities are Configurable
// Updated and Handled in Update Function, But Instance Functionality is Determined in Script
// Ideas for Dynamics: Bullets, Grenades, Some Particles

// Another Idea: The Script Index Is Used to Determine the Blueprint of the Dynamic Entity
// There Will Only be One Instance of a Dynamic Object Loaded At a Time, But Many Stored in Different Sub Levels
// Compare the Script Index to See if Dynamic Object Needs to be Initialized

namespace Object::Entity
{
	// Dynamic Entity Data
	struct DynamicData
	{
		// The Max Number of Loaded Objects
		uint16_t max_number_of_instances;

		// If True, Object Will Test for Collisions
		bool enable_collisions;
	};

	// Dynamic Entity Class
	class Dynamics : public EntityBase
	{
		// Dynamic Data
		DynamicData dynamic;

	public:

		// Initialize Object
		Dynamics(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, DynamicData& dynamic_);

		// Update Object
		void updateObject();
	};
}

#endif
