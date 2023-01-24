#pragma once
#ifndef LEVEL_H
#define LEVEL_H

//#define SHOW_LEVEL_LOADING

#include "Class/Render/Struct/LinkedList.h"
#include "Class/Render/Struct/List.h"

#include "TextureHandler.h"
#include "SubLevel.h"
#include "Macros.h"

// IDEA: Add Global Unsaved Level That is Always Loaded
// This is to Easily Incorporate the Player and Dynamic Entities Such as Bullets
// This Global Level is Only Accessable and Modifiable From the Editor Window

namespace Editor 
{
	class Selector;
	class ObjectInfo;
}

namespace Render::Objects
{
	class UnsavedLevel;
	class ChangeController;
}

namespace Render::Camera
{
	class Camera;
}

namespace Object
{
	class Object;

	namespace Mask
	{
		namespace Floor
		{
			class FloorMask;
		}

		namespace Left
		{
			class LeftMask;
		}

		namespace Right
		{
			class RightMask;
		}

		namespace Ceiling
		{
			class CeilingMask;
		}

		namespace Trigger
		{
			class TriggerMask;
		}
	}

	namespace Terrain
	{
		class TerrainBase;
	}

	namespace Light
	{
		namespace Directional
		{
			class Directional;
		}

		namespace Point
		{
			class Point;
		}

		namespace Spot
		{
			class Spot;
		}

		namespace Beam
		{
			class Beam;
		}
	}

	namespace Physics
	{
		class PhysicsBase;
	}

	namespace Entity
	{
		class EntityBase;
	}
}

namespace Render::Objects
{
	// Container for Currently Loaded Objects
	struct ObjectContainer
	{
		uint32_t total_object_count = 0; // Total Number of Objects Currently Loaded
		uint16_t floor_size = 0;
		uint16_t left_wall_size = 0;
		uint16_t right_wall_size = 0;
		uint16_t ceiling_size = 0;
		uint16_t trigger_size = 0;
		uint16_t terrain_size = 0;
		uint16_t directional_size = 0;
		uint16_t point_size = 0;
		uint16_t spot_size = 0;
		uint16_t beam_size = 0;

		Object::Object** object_array = nullptr; // The Array of All Objects Together
		Object::Mask::Floor::FloorMask** floor_start = nullptr;
		Object::Mask::Left::LeftMask** left_wall_start = nullptr;
		Object::Mask::Right::RightMask** right_wall_start = nullptr;
		Object::Mask::Ceiling::CeilingMask** ceiling_start = nullptr;
		Object::Mask::Trigger::TriggerMask** trigger_start = nullptr;
		Object::Terrain::TerrainBase** terrain_start = nullptr;
		Object::Light::Directional::Directional** directional_start = nullptr;
		Object::Light::Point::Point** point_start = nullptr;
		Object::Light::Spot::Spot** spot_start = nullptr;
		Object::Light::Beam::Beam** beam_start = nullptr;
	};

	// Container for All Objects Currently Being Rendered and Processed
	class Level
	{
		// Determines if Initialized
		bool initialized = false;

		// Array of Loaded SubLevels
		SubLevel* sublevels[9];

		// Current Level Position
		glm::vec2 level_position;

		// Vertex Array Objects
		GLuint VAO, VBO;

		// Number of Terrain Vertices
		GLuint number_of_vertices[7];

		// Projection Matrices
		glm::mat4 projection[6];

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Terrain Textures
		TextureHandler* terrain_textures[6];

		// Terrain Seperators
		short terrain_seperators[7];

		// Physics Texture
		TextureHandler* physics_textures;

		// The Container for All Level Objects
		ObjectContainer container;

		// Physics Objects
		Struct::List<Object::Physics::PhysicsBase> physics_list;

		// Entities
		Struct::List<Object::Entity::EntityBase> entity_list;

		// Light Seperators
		short directional_seperators[7];
		short point_seperators[7];
		short spot_seperators[7];
		short beam_seperators[7];

		// Temporary Index Holder for Adding New Objects
		uint32_t temp_index_holder = 0;

		// Test if SubLevels Should be Reloaded
		void testReload();

		// Reload Levels
		void reloadLevels(glm::vec2& level_old, glm::vec2& level_new, bool save);

		// Segregate Objects from Main List Into Smaller Lists
		void segregateObjects();

		// Reallocate Memory For Textures
		void reallocateTextures();

		// Reallocate Memory For Physics Objects
		void reallocatePhysics();

		// Reallocate Memory For Entities
		void reallocateEntities();

		// Reallocate Memory After Level Reload
		void reallocatePostReload(uint32_t old_total_object_count);

		// Helper Reallocate Function
		template <class Type> uint16_t reallocateHelper(Type*** list, int old_count, int new_count);

		// Reallocate All Memory
		void reallocateAll(bool del, uint32_t size);

		// Construct Terrain 
		void constructTerrain();

		// Load Lights Into Shader
		void loadLights();

	public:

		// Global Camera
		Camera::Camera* camera;

		// Initialize New Level Container
		Level(std::string save_path, std::string core_path);

		// Update Camera
		void updateCamera();

		// Update Level Container
		void updateContainer();

		// Draw Level Objects
		void drawContainer();

		// Update Level Location
		void updateLevelPos(glm::vec2 position, glm::vec2& level);

		// Return Index of Corrisponding SubLevel from Level Coords
		int8_t index_from_level(glm::vec2 level_coords);

		// Return Coordinates of Sublevel from Corrisponding Sublevel Index
		glm::vec2 level_from_index(int8_t index);

#ifdef EDITOR

		// Draw Static Level Objects if Visualization is Enabled
		void drawVisualizers();

		// Test Selector Object
		void testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Terrain Objects
		bool testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Lighting Objects
		bool testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Effect Objects
		bool testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Physics Objects
		bool testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Entity Objects
		bool testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Collision Mask Objects
		bool testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on an Individual Object
		template <class Type> uint8_t testSelectorOnObject(Type*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info);

		// Test Selector on an Object List
		template <class Type> bool testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Remove Object From Primary Object List
		void removeMarkedFromList(Object::Object* marked_object);

		// Reset the Object Pass Over Flag
		void resetObjectPassOver();

		// Perform a Quick Reset on Loaded Level Objects
		void resetLevel();

		// Return Object to Level
		void returnObject(Object::Object* new_object, float object_x, float object_y, uint8_t object_identifier[3]);

		// Reload All Objects
		void reloadAll(float new_x, float new_y);
		void reloadAll();

		// Return Projection-View Matrix
		glm::mat4 returnProjectionViewMatrix(uint8_t layer);

		// Store Level of Origin
		void storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position);

		// Return Size of Directional Light Buffer
		GLuint returnDirectionalBufferSize();

		// Return Size of Point Light Buffer
		GLuint returnPointBufferSize();

		// Return Size of Spot Light Buffer
		GLuint returnSpotBufferSize();

		// Return Size of Beam Light Buffer
		GLuint returnBeamBufferSize();


#endif

	};
}

#endif
