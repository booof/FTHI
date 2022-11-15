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
		NPC(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, uint16_t& ai);

		// Update Object
		void updateObject();

#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object 
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data);

#endif

	};
}

#endif
