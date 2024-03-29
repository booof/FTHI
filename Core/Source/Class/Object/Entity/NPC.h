#pragma once
#ifndef NPC_H
#define NPC_H

#ifndef DLL_HEADER
#include "EntityBase.h"
#endif

// These Entities Represent the "Mobiles" Found Throught a World
// Each Entity is Assigned an "AI" Script Along With Traditional Object Scripts
// These Entities Are Mostly Dependend on their AI and Scripting Should be Dont to Handle Unique Interactions With Other Objects and Entities

namespace Object::Entity
{
	// NPC Class
	class NPC : public EntityBase
	{
		// Index of AI Script
		uint16_t ai_script;

		// AI Script Function
		void(*ai_funct)(NPC& npc);

	public:

		// Initialize Object
		NPC(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, uint16_t& ai, glm::vec2& offset);

		// Update Object
		void updateObject();
	};
}

#endif
