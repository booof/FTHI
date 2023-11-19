#pragma once
#ifndef LEVEL_H
#define LEVEL_H

//#define SHOW_LEVEL_LOADING

#include "Class/Render/Struct/LinkedList.h"
#include "Class/Render/Struct/List.h"

#include "TextureHandler.h"
#include "SubLevel.h"
#include "Macros.h"
#include "UnsavedBase.h"

// IDEA: Add Global Unsaved Level That is Always Loaded
// This is to Easily Incorporate the Player and Dynamic Entities Such as Bullets
// This Global Level is Only Accessable and Modifiable From the Editor Window

namespace DataClass
{
	class Data_Object;
}

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
	class TempObject;

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

	namespace Group
	{
		class GroupObject;
	}
}

namespace Render::Objects
{
	// Data for Scenes
	struct SceneData
	{
		// The File Identifier
		// [0] = Data Type: 0 = SceneList, 1 = SceneData, 2 = Level Data, 3 = Editor Data, 5 = Vars
		// [1] = Instance Type: 0 = Levels, 1 = GUIs. Similar Thing for Var Files
		// [2] = Identifier #1: Static Number
		// [3] = Identifier #2: Static Number
		char identifier[4] = { 1, 0, 42, 125 };

		// The Horizontal Size of a Sublevel
		float sublevel_width = 128.0f;

		// The Vertical Size of a Sublevel
		float sublevel_height = 64.0f;

		// The Number of Sublevels in a Given Direction
		// Does Not Include Sublevel (0,0)
		int sublevel_count_north = 10;
		int sublevel_count_east = 10;
		int sublevel_count_south = 10;
		int sublevel_count_west = 10;

		// The Size of the Level Name
		uint16_t name_size = 0;

		// Determines if the Sublevels Wrap
		bool wrap_sublevels = false;

		// Determines if the Camera is Stationary
		bool stationary = false;

		// Gravity
		float gravity_scale = 1.0f;

		// The Render Distance
		unsigned int render_distance = 2;

		// The Initial Camera Positions
		float initial_camera_x = 0.0f;
		float initial_camera_y = 0.0f;

		// Initial Zoom Scale
		float initial_scale = 0.2f;
	};

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
		uint16_t group_size = 0;

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
		Object::Group::GroupObject** group_start = nullptr;
	};

	// Container for All Objects Currently Being Rendered and Processed
	class Level
	{
		// Determines if Initialized
		bool initialized = false;

		// Level Path
		std::string level_data_path = "";

		// Editor Path
		std::string editor_level_data_path = "";

		// Scene Data Object
		SceneData scene_data;

		// Scene Name
		std::string scene_name = "";

		// Array of Loaded SubLevels
		SubLevel* sublevels;

		// Current Level Position
		glm::i16vec2 level_position;

		// Vertex Array Objects for Terrain
		GLuint terrainVAO, terrainVBO;

		// Vertex Array Objects for Border
		GLuint borderVAO, borderVBO;

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

		// Array of Temporary Removed Objects
		std::vector<Object::TempObject*> temp_objects;

		// Array of Used Sublevel Arrays
		void** used_arrays;

		// The Number of Sublevels that Are Loaded Simultaniusly
		uint8_t level_count = 0;

		// The Diameter of Loaded Levels
		uint8_t level_diameter = 0;

		// The Maximum Index Possible in Loaded Levels
		uint8_t level_max_index = 0;

		// The Coordinate Offset for Level Index
		int8_t level_coord_offset = 0;

		// The Size of the Entire Scene, Width and Height
		glm::vec2 scene_size = glm::vec2(0.0f, 0.0f);

		// Dimensions for the Entire Level
		// MinX, MaxX, MinY, MaxY
		glm::vec4 level_size = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		// Test if SubLevels Should be Reloaded
		void testReload();

		// Reload Levels
		void reloadLevels(glm::i16vec2& level_old, glm::i16vec2& level_new, bool save);

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

		// Helper Function to Build a Sorted List of Object Indicies
		void getObjectIndicies(DataClass::Data_Object* parent, uint32_t** indicies, int& indicies_size);

		// Perform Binary Search on a List of Object Indicies
		bool searchObjectIndicies(uint32_t* indicies, int left, int right, uint32_t test_value);

	public:

		// Global Camera
		Camera::Camera* camera;

		// Initialize New Level Container
		Level(std::string& level_path, float initial_x, float initial_y, bool force_coords);

		// Deconstructor to Avoid Memory Leaks
		~Level();

		// Update Camera
		void updateCamera();

		// Update Level Container
		void updateContainer();

		// Draw Level Objects
		void drawContainer();

		// Update Level Location
		void updateLevelPos(glm::vec2 position, glm::i16vec2& level);

		// Get the Object Position From Level Post
		glm::vec2 getObjectPos(glm::i16vec2 level_coords);

		// Return Index of Corrisponding SubLevel from Level Coords
		int8_t getIndexFromLevel(glm::i16vec2 level_coords);

		// Return Coordinates of Sublevel from Corrisponding Sublevel Index
		glm::i16vec2 getLevelFromIndex(int8_t index);

		// Wrap the Coordinates of a Sublevel
		glm::i16vec4 wrapLevelPos(int x, int y);

		// Wrap the Coordinates of an Object in Level
		void wrapObjectPos(glm::vec2& pos);

		// Unwrrap the Coordinates of an Object in Level
		void unwrapObjectPos(glm::vec2& pos);

#ifdef EDITOR

		// Draw the Level Border
		void drawLevelBorder();

		// Draw Static Level Objects if Visualization is Enabled
		void drawVisualizers();

		// Test Selector Object
		uint8_t testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Terrain Objects
		uint8_t testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Lighting Objects
		uint8_t testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Effect Objects
		uint8_t testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Physics Objects
		uint8_t testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Entity Objects
		uint8_t testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Group Objects
		uint8_t testSelectorGroup(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on Collision Mask Objects
		uint8_t testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test Selector on an Individual Object
		uint8_t testSelectorOnObject(Object::Object*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info);

		// Test Selector on an Object List
		template <class Type> uint8_t testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Remove Object From Primary Object List
		void removeMarkedFromList(Object::Object* marked_object, glm::vec2* new_selected_position);

		// Remove Marked Children From List Without Causing a Reload
		void removeMarkedChildrenFromList(DataClass::Data_Object* marked_parent);

		// Reset the Object Pass Over Flag
		void resetObjectPassOver();

		// Perform a Quick Reset on Loaded Level Objects
		void resetLevel();

		// Return Object to Level
		void returnObject(Object::Object* new_object, float object_x, float object_y, uint8_t object_identifier[3]);

		// Reload All Objects
		void reloadAll(float new_x, float new_y);
		void reloadAll();

		// Incorporate a List of Objects Into Loaded Level
		void incorperatNewObjects(Object::Object** new_objects, int new_objects_size);

		// Clear All Temporary Objects Created During Selection
		void clearTemps();

		// Return Projection-View Matrix
		glm::mat4 returnProjectionViewMatrix(uint8_t layer);

		// Store Level of Origin
		void storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, MOVE_WITH_PARENT disable_move);

		// Return Size of Directional Light Buffer
		GLuint returnDirectionalBufferSize();

		// Return Size of Point Light Buffer
		GLuint returnPointBufferSize();

		// Return Size of Spot Light Buffer
		GLuint returnSpotBufferSize();

		// Return Size of Beam Light Buffer
		GLuint returnBeamBufferSize();

		// Returns the Sublevels of the Object
		SubLevel* getSublevels();

		// Get the Level Data Path
		std::string getLevelDataPath();

		// Get the Editor Level Data Path
		std::string getEditorLevelDataPath();

		// Get Pointers to the Scene Information
		void getSceneInfo(SceneData** data, std::string** name);

		// Get the Level Size
		void getSublevelSize(glm::vec2& sizes);

#endif

	};
}

#endif
