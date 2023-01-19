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

namespace Object
{
	// Base Objects List
	enum ObjectList : unsigned char
	{
		MASK,
		TERRAIN,
		LIGHT,
		PHYSICS,
		ENTITY,
		EFFECT
	};

	// Enumerated Index for Header Counts
	enum STORAGE_TYPES : uint8_t
	{
		FLOOR_COUNT = 1,
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
		ENTITY_COUNT
	};

	// Definition for Basic Object Initializer For Reading From File
	struct ObjectData
	{
		// Constant Position of Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Z-Pos of Object
		float zpos = -1.0f;

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

#ifdef EDITOR

	public:

		// Pointer to the Data Class
		DataClass::Data_Object* data_object;

		// Editor Data
		std::string name = "";           // The User Given Name to the Object for Easy Identification
		bool clamp = false;              // If True, Vertices and Sides of the Object May be Moved to Fit In With Other Objects
		bool lock = false;               // If True, The Object is Ineligable for Selection and Modification
		uint32_t object_index = 0;       // The UUID For the Object
		bool skip_selection = false;     // If True, Object is Skipped for Selection Until Mouse is Moved

#endif

	public:

		// Determines if Object is Active
		bool active = true;

		// Which Level Array The Object Should be Placed Into
		uint8_t storage_type = 0;

		// Initializer Function for Scripts
		void (*init)(Object*) = nullptr;

		// Loop Function for Scripts
		void (*loop)(Object*) = nullptr;

		// Uninitializer Function for Scripts
		//virtual void del() = 0;
		void (*del)(Object*) = nullptr;

		// The External Data Used by the Object (Used for Individual Object Specific Data in Scripts)
		void* external_data = nullptr;

		// Update the Object
		virtual void updateObject() = 0;

		// Return Pointer to Position
		virtual glm::vec2* pointerToPosition() = 0;

#ifdef EDITOR

		// Write Object to File
		virtual void write(std::ofstream& object_file, std::ofstream& editor_file) = 0;

		// Select Object
		virtual void select(Editor::Selector& selector, Editor::ObjectInfo& object_info) = 0;

		void select2(Editor::Selector& selector);

		// Test if Mouse Intersects Object
		virtual bool testMouseCollisions(float x, float y) = 0;

		// Return Position of Object
		virtual glm::vec2 returnPosition() = 0;

		// If True, Object is Marked for Deletion
		bool marked = false;

		// Temp Debug Function
		void debug_funct();

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
