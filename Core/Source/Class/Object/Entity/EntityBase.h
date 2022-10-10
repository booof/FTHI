#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#ifndef DLL_HEADER
#include "Object/Object.h"
#include "Class/Render/Struct/Texture.h"
#endif

// This is the Base Class for Entity Objects

// There are 4 Sub-Classes:
// Controllable:   Controlled by External Scripts, Ideal for Players
// NPCs:           Controlled by an AI script
// Interactables:  Collision Detection Similar to Trigger Masks, However, Collisions Affect the Interactable As Well
// Dynamics:       Entities That Are to be Instanced on a Mass Scale

// Entities Should be Allowed to Switch Between Entity Types Easily (Except for Dynamics)
// Example for Vehicles:
// Initially Interactable so Entities Can Use it (Bother Players and NPCs)
// If Controlled by NPC, Object is now an NPC and NPC is Stored as Memory Inside of Vehicle
// If Controlled by Player, Object is new a Controllable and Player is Stored as Memory Inside of Vehicle

// Another Switch Example : Grenades and Rockets (Dynamics) Create Explosions (Interactable)

namespace Object::Entity
{
	// List of Entities
	enum ENTITY_LIST : uint8_t
	{
		ENTITY_NPC,
		ENTITY_CONTROLLABLE,
		ENTITY_INTERACTABLE,
		ENTITY_DYNAMIC
	};

	// Entity Specific Data
	struct EntityData
	{
		// Half Size of Texture
		float half_width = 0.5f;
		float half_height = 0.5f;

		// Half Size of Collision Box
		float half_collision_width = 0.5f;
		float half_collision_height = 0.5f;
	};

	// The Current State of an Entity
	struct EntityState
	{
		// Position of Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// The Velocity of the Entity
		glm::vec2 velocity = glm::vec2(0.0f, 0.0f);

		// The Momentum of the Entity
		glm::vec2 momentum = glm::vec2(0.0f, 0.0f);

		// Direction the Entity is Facing
		uint8_t direction = 1;

		// Determines if the Entity is Grounded
		bool grounded = false;

		// Determines if the Entity Touches a Wall
		bool walled = false;
	};

	// The Stats of an Entity
	struct EntityStats
	{
		float Force = 10.0f;
	};

	// Named Node
	struct Named_Node
	{
		float value;
		int name;
		Named_Node(float Value, int Name) { value = Value; name = Name; }
		Named_Node() { value = 0.0f; name = 0; }
	};

	// The Base Entity Class
	class EntityBase : public SubObject
	{
		// Vertex Object
		GLuint VAO, VBO;

		// Texture
		Struct::SingleTexture texture;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

	protected:

		// Initialize Object
		EntityBase(uint32_t& uuid_, EntityData& entity_, ObjectData& data_);

		// General Update Function
		void updateEntity();

	public:

		// Entity Data
		EntityData entity;

		// Entity State
		EntityState state;

		// Entity Stats
		EntityStats stats;

		// UUID of Object
		uint32_t uuid = 0;

		// Vertices for Entity Collisions
		glm::vec2 vertices[4];
		Named_Node angle_offsets[4];
		short angle_to_vertex_left[4] = { 1, 2, 3, 0 };
		short angle_to_vertex_right[4] = { 2, 3, 0, 1 };

		// Blitz Object
		void blitzEntity();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

#ifdef EDITOR

		// Test Mouse Collisions
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

#endif

	};
}

#endif
