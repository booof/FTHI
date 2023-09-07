#pragma once
#ifndef OBJECT_H
#define OBJECT_H

// Externals
#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Macros.h"
#endif

namespace Editor {
	class Selector;
	class ObjectInfo;
}

namespace DataClass {
	class Data_Object;
}

namespace Render::Objects {
	class UnsavedCollection;
}

namespace Object
{
	// Declaration for Object Class
	class Object;

	// Base Objects List
	enum ObjectList : unsigned char
	{
		MASK,
		TERRAIN,
		LIGHT,
		PHYSICS,
		ENTITY,
		EFFECT,
		GROUP
	};

	// Enumerated Index for Header Counts
	enum STORAGE_TYPES : uint8_t
	{
		NULL_TEMP = 0,
		FLOOR_COUNT,
		LEFT_COUNT,
		RIGHT_COUNT,
		CEILING_COUNT,
		TRIGGER_COUNT,
		TERRAIN_COUNT,
		DIRECTIONAL_COUNT,
		POINT_COUNT,
		SPOT_COUNT,
		BEAM_COUNT,
		PHYSICS_COUNT,
		ENTITY_COUNT,
		GROUP_COUNT
	};

	// Definition for the Garbage Collection Structure
	struct Active
	{
		// Determines if Object is Active
		bool active = true;

		// Determines if the Object is Alive
		bool alive = true;

		// Level Coords of Object
		glm::i16vec2 level_pos = glm::i16vec2(0, 0);

		// Pointer to the Object
		Object* object_ptr;
	};

	// Definition for Basic Object Initializer For Reading From File
	struct ObjectData
	{
		// Constant Position of Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Z-Pos of Object
		float zpos = -2.0f;

		// Colors of Object
		glm::vec4 colors = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Normals of Object
		glm::vec3 normals = glm::vec3(0.0f, 0.0f, 1.0f);

		// Name of Texture
		int texture_name = 0;

		// Name of Material
		int material_name = 0;

		// Script
		int script = 0;
	};

	// Definition for Object
	class Object
	{
	protected:

		// Initialize Script
		void initializeScript(int script);

	public:

		// I Hope This Works
		// Update: Why The Fuck Does This Work
		virtual ~Object();

#ifdef EDITOR

		// Pointer to the Data Class
		DataClass::Data_Object* data_object;

		// Editor Data
		std::string name = "";           // The User Given Name to the Object for Easy Identification
		bool clamp = false;              // If True, Vertices and Sides of the Object May be Moved to Fit In With Other Objects
		bool lock = false;               // If True, The Object is Ineligable for Selection and Modification
		uint32_t object_index = 0;       // The UUID For the Object
		bool skip_selection = false;     // If True, Object is Skipped for Selection Until Mouse is Moved

		// The Pointer to the Active Object
		Active* active_ptr = nullptr;

		// The Pointer to the Unsaved Group Object
		Render::Objects::UnsavedCollection* group_object = nullptr;

#endif

		// Which Level Array The Object Should be Placed Into
		uint8_t storage_type = 0;

		// Initializer Function for Scripts
		void (*init)(Object*) = nullptr;

		// Loop Function for Scripts
		void (*loop)(Object*) = nullptr;

		// Uninitializer Function for Scripts
		void (*del)(Object*) = nullptr;

		// The Pointer to the Parent Object
		Object* parent = nullptr;

		// The Pointer to Child Objects
		Object** children = nullptr;

		// The Number of Children of the Object
		int children_size = 0;

		// The External Data Used by the Object (Used for Individual Object Specific Data in Scripts)
		void* external_data = nullptr;

		// Update the Object
		virtual void updateObject() = 0;

		// Return Pointer to Position
		virtual glm::vec2* pointerToPosition() = 0;

#ifdef EDITOR

		// Select Object
		bool select(Editor::Selector& selector, Editor::ObjectInfo& object_info, bool add_children);

		// Test if Mouse Intersects Object
		virtual bool testMouseCollisions(float x, float y) = 0;

		// Return Position of Object
		virtual glm::vec2 returnPosition() = 0;

		// Update Selected Position of Object
		virtual void updateSelectedPosition(float deltaX, float deltaY) = 0;

		// Update Selected Position of Object From Complex Parent
		void updateSelectedComplexPosition(float deltaX, float deltaY);

		// Temp Debug Function
		void debug_funct();

		// Draw Group Visualizer
		void drawGroupVisualizer();

#endif

	};

	// Object Class Containing Object Data
	class SubObject : public Object
	{
	protected:

		// Object Data
		ObjectData data;

	public:

		// Number of Vertices in Object
		int number_of_vertices = 0;

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);
	};

	// Temp Object Class that Only Contains a Position for When Parents Are Deleted
	class TempObject : public Object
	{
		// Position of the Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Position Being Modified by the Selector
		glm::vec2* selected_position = nullptr;

		// Determines if this is the Original Object
		bool original = false;

	public:

		// Constructor
		TempObject(Object* object, glm::vec2* new_position_ptr, bool original_);

		// Deconstructor
		~TempObject();

		// Update the Object
		void updateObject() {}

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Return Pointer to Selected Position
		glm::vec2* pointerToSelectedPosition();

		// Tests if This is the Original Object
		bool isOriginal();
	};

	// List of Objects

	// Collision
	namespace Mask {
		class CollisionMask;
	}

	// Terrain
	namespace Terrain {
		class TerrainBase;
	}

	// Lighting
	namespace Light {
		class LightBase;
	}

	// Physics
	namespace Physics {
		class PhysicsBase;
	}

	// Entities
	namespace Entity {
		class EntityBase;
	}

	// Effects
	namespace Effects {
		class EffectBase;
	}
}

#endif
