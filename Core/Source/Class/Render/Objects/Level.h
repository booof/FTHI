#pragma once
#ifndef LEVEL_H
#define LEVEL_H

//#define SHOW_LEVEL_LOADING

#include "Class\Render\Container\Container.h"

#include "TextureHandler.h"
#include "SubLevel.h"
#include "Macros.h"
#include "UnsavedBase.h"

// IDEA: Add Global Unsaved Level That is Always Loaded
// This is to Easily Incorporate the Player and Dynamic Entities Such as Bullets
// This Global Level is Only Accessable and Modifiable From the Editor Window

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

	// Level Container Subclass
	struct ObjectContainer_Level : public ObjectContainer
	{
		// Pointer to Start of Each Object Type in Container
		Object::Mask::Floor::FloorMask** floor_start = nullptr;
		Object::Mask::Left::LeftMask** left_wall_start = nullptr;
		Object::Mask::Right::RightMask** right_wall_start = nullptr;
		Object::Mask::Ceiling::CeilingMask** ceiling_start = nullptr;
		Object::Mask::Trigger::TriggerMask** trigger_start = nullptr;
		Object::Terrain::TerrainBase** terrain_start = nullptr;

		// Counts for Level Specific Objects
		uint16_t floor_size = 0;
		uint16_t left_wall_size = 0;
		uint16_t right_wall_size = 0;
		uint16_t ceiling_size = 0;
		uint16_t trigger_size = 0;
		uint16_t terrain_size = 0;

		// Seperators for Level Specific Objects
		short terrain_seperators[7];
		short directional_seperators[7];
		short point_seperators[7];
		short spot_seperators[7];
		short beam_seperators[7];

		// Physics Objects
		Struct::List<Object::Physics::PhysicsBase> physics_list;

		// Entities
		Struct::List<Object::Entity::EntityBase> entity_list;
	};

	// Container for All Objects Currently Being Rendered and Processed
	class Level : public Container
	{
		// Scene Data Object
		SceneData scene_data;

		// Path to the Object Data in Container
		std::string level_data_path = "";

		// Path to the Editor Data in Container
		std::string editor_level_data_path = "";

		// Array of Loaded SubLevels
		SubLevel* sublevels;

		// Current Level Position
		glm::i16vec2 level_position;

		// Number of Terrain Vertices
		GLuint number_of_vertices[7];

		// Projection Matrices
		glm::mat4 projection[6];

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Terrain Textures
		TextureHandler* terrain_textures[6];

		// Physics Texture
		TextureHandler* physics_textures;

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

		// The Object Container
		ObjectContainer_Level container;

		// Only to be Used When Reallocating Objects Array
		ObjectContainer& getContainer();

		// Test if SubLevels Should be Reloaded
		void testReload();

		// Reload Levels
		void reloadLevels(glm::i16vec2& level_old, glm::i16vec2& level_new, bool save);

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

	public:

		// Initialize New Level Container
		Level(std::string& level_path, float initial_x, float initial_y, bool force_coords);

		// Deconstructor to Avoid Memory Leaks
		~Level();

		// Returns the Type of Container Object
		CONTAINER_TYPES getContainerType();

		// Update Camera
		void updateCamera();

		// Update Level Container
		void updateContainer();

		// Draw Level Objects
		void drawContainer();

		// Draw Level Objects
		void loadObjects();

		// Reload the Terrain Objects of Container
		void constructTerrain();

		// Construct Terrain Using the Correct Algorithm
		void constructTerrainAlgorithm(int& instance, int& instance_index);

		// Reload the Lighting Objects of Container
		void loadLights();

		// Determine the Starting Pointers the Different Objects in Container Through Sorting
		void segregateObjects();

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

		// Add Object and Decendants Into Container
		void genObjectIntoContainer(DataClass::Data_Object* new_object, Object::Object* real_parent, uint16_t& index, int16_t delta_size);

		// Generate Objects Into This Container
		void buildObjectsGenerator(std::vector<DataClass::Data_Object*>& data_object_array, uint16_t& index, Object::Object* parent, glm::vec2 position_offset, uint16_t& active_index, UnsavedLevel& unsaved_level);

		// Test Selector Object
		uint8_t testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Remove Marked Object From List With Unkown Container Type
		void removeMarkedChildrenFromList(DataClass::Data_Object* parent);

		// Incorporate a List of Objects Into Loaded Level With Unkown Container Type
		void incorperatNewObjects(Object::Object** new_objects, int new_objects_size);

		// Set Objects to be Active in Corrisponding Sublevels
		void setActives(Object::Object** new_objects, int new_objects_size);

		// Reset the Object Pass Over Flag
		void resetObjectPassOver();

		// Draw the Level Border
		void drawLevelBorder();

		// Draw Static Level Objects if Visualization is Enabled
		void drawVisualizers();

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
		void storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, Object::Object* real_object);

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

		// Get Pointers to the Scene Information
		void getSceneInfo(SceneData** data, std::string** name);

		// Get the Level Size
		void getSublevelSize(glm::vec2& sizes);

		// Get the Level Data Path
		std::string getLevelDataPath();

		// Get the Editor Level Data Path
		std::string getEditorLevelDataPath();

#endif

	};
}

#endif
