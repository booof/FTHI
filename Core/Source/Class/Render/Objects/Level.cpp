#include "Level.h"
#include "Globals.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"

#include "Class/Object/Object.h"
#include "Class/Render/Camera/Camera.h"

#include "Source/Vertices/Buffer/ObjectBuffers.h"
#include "Class/Object/Terrain/TerrainBase.h"
#include "Algorithms/Sorting/SortingAlgorithms.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"

// Masks
#include "Class/Object/Collision/Horizontal/FloorMask.h"
#include "Class/Object/Collision/Vertical/LeftMask.h"
#include "Class/Object/Collision/Vertical/RightMask.h"
#include "Class/Object/Collision/Horizontal/CeilingMask.h"

#include "Class/Render/Struct/LinkedList.h"
#include "Source/Algorithms/Common/Common.h"
#include "Class/Render/Shader/Shader.h"
#include "ChangeController.h"
#include "UnsavedLevel.h"

// Collisions
#include "Source/Collisions/Mask Collisions/MaskCollisions.h"
#include "Source/Collisions/Physics Collisions/PhysicsCollisions.h"

#include "Class/Render/Struct/DataClasses.h"

#include "UnsavedGroup.h"
#include "UnsavedComplex.h"
#include "Class\Render\Editor\Debugger.h"
#include "Source\Events\EventListeners.h"

void Render::Objects::Level::updateLevelPos(glm::vec2 position, glm::i16vec2& level)
{
	// Get Level Position Object is In
	level.x = floor(position.x / scene_data.sublevel_width);
	level.y = floor(position.y / scene_data.sublevel_height);

	// If Wrapping is Not Enabled, Return Position
	if (!scene_data.wrap_sublevels)
		return;

	// Test For Horizontal Wrapping
	if (level.x > scene_data.sublevel_count_east)
		level.x -= scene_data.sublevel_count_east + scene_data.sublevel_count_west + 1;
	else if (level.x < -scene_data.sublevel_count_west)
		level.x += scene_data.sublevel_count_east + scene_data.sublevel_count_west + 1;

	// Test For Vertical Wrapping
	if (level.y > scene_data.sublevel_count_north)
		level.y -= scene_data.sublevel_count_north + scene_data.sublevel_count_south + 1;
	else if (level.y < -scene_data.sublevel_count_south)
		level.y += scene_data.sublevel_count_north + scene_data.sublevel_count_south + 1;
}

glm::vec2 Render::Objects::Level::getObjectPos(glm::i16vec2 level_coords)
{
	return glm::vec2(level_coords.x * scene_data.sublevel_width, level_coords.y * scene_data.sublevel_height);
}

int8_t Render::Objects::Level::getIndexFromLevel(glm::i16vec2 level_coords)
{
	// Determine if Level Coord is Not Currently Loaded
	if (abs(level_coords.x - level_position.x) >= scene_data.render_distance || abs(level_coords.y - level_position.y) > scene_data.render_distance)
		return -1;

	// Get the Coords From the Top-Left Corner
	uint8_t top_left_x = level_coords.x - level_position.x - level_coord_offset;
	uint8_t top_left_y = level_coords.y - level_position.y + level_coord_offset;
	//uint8_t top_left_y = level_coords.y - level_position.y - level_coord_offset;

	// Get the Index From the Level Coord
	return (uint8_t)(top_left_y * level_diameter + top_left_x);
}

glm::i16vec2 Render::Objects::Level::getLevelFromIndex(int8_t index)
{
	// Update: The Values Will be Changed Based on Render Distance
	// Min Index Will Remain 0, Max Will be Radius^2 - 1
	// X-Pos Will Now be Modulused by the Diameter of Loaded Levels
	// Y-Pos Will be Changed Drastically

	// If Index is Outside Level Range, Retrun 0,0
	if (index > level_max_index || index < 0)
		return glm::i16vec2(0, 0);

	// The Coords That Will be Returned
	glm::i16vec2 coords = glm::vec2(0.0f, 0.0f);

	// Get the 2-D Index Positions Relative to the Top Left Corner 
	int8_t index_x = index % level_diameter;
	//int8_t index_y = floor((index - index_x) / level_diameter);
	int8_t index_y = scene_data.render_distance - floor((index - index_x) / level_diameter);

	// Get the Level Coords From the Indicies
	coords.x = level_position.x + index_x + level_coord_offset;
	coords.y = level_position.y + index_y + level_coord_offset;
	return coords;
}

glm::i16vec4 Render::Objects::Level::wrapLevelPos(int x, int y)
{
	// If Wrapping is Not Enabled, Return The Coords
	if (!scene_data.wrap_sublevels)
		return glm::i16vec4(x, y, 0, 0);

	// The Two Variables That Determine Which Axis is Wrapped, and in Which Direction
	// Sign Determines The Sign of the Value Added to the Objects
	short wrapped_horizontal = 0;
	short wrapped_vertical = 0;

	// Test For Horizontal Wrapping
	if (x > scene_data.sublevel_count_east) {
		x -= scene_data.sublevel_count_east + scene_data.sublevel_count_west + 1;
		wrapped_horizontal = 1;
	} else if (x < -scene_data.sublevel_count_west) {
		x += scene_data.sublevel_count_east + scene_data.sublevel_count_west + 1;
		wrapped_horizontal = -1;
	}

	// Test For Vertical Wrapping
	if (y > scene_data.sublevel_count_north) {
		y -= scene_data.sublevel_count_north + scene_data.sublevel_count_south + 1;
		wrapped_vertical = 1;
	} else if (y < -scene_data.sublevel_count_south) {
		y += scene_data.sublevel_count_north + scene_data.sublevel_count_south + 1;
		wrapped_vertical = -1;
	}

	// Return Wrapped Coords
	return glm::i16vec4(x, y, wrapped_horizontal, wrapped_vertical);
}

void Render::Objects::Level::wrapObjectPos(glm::vec2& pos)
{
	// If Wrapping is Not Enabled, Do Nothing
	if (!scene_data.wrap_sublevels)
		return;

	// Test if Object X is Beyond Level Width
	if (pos.x < level_size.x)
		pos.x += (level_size.y - level_size.x);
	else if (pos.x > level_size.y)
		pos.x -= (level_size.y - level_size.x);

	// Test if Object Y is Beyond Level Height
	if (pos.y < level_size.z)
		pos.y += (level_size.w - level_size.z);
	else if (pos.y > level_size.w)
		pos.y -= (level_size.w - level_size.z);
}

void Render::Objects::Level::unwrapObjectPos(glm::vec2& pos)
{
	// If Wrapping is Not Enabled, Do Nothing
	if (!scene_data.wrap_sublevels)
		return;

	// Get The Level Coordinates Object is In
	glm::i16vec2 level_pos;
	updateLevelPos(pos, level_pos);

	// Test if X-Position Should be Unwrapped
	if (level_pos.x == scene_data.sublevel_count_east && camera->Position.x < 0)
		pos.x -= scene_size.x;
	else if (level_pos.x == -scene_data.sublevel_count_west && camera->Position.x > 0)
		pos.x += scene_size.x;

	// Test if X-Position Should be Unwrapped
	if (level_pos.y == scene_data.sublevel_count_north && camera->Position.y < 0)
		pos.y -= scene_size.y;
	else if (level_pos.y == -scene_data.sublevel_count_south && camera->Position.y > 0)
		pos.y += scene_size.y;
}

Render::ObjectContainer& Render::Objects::Level::getContainer()
{
	return container;
}

void Render::Objects::Level::testReload()
{
	// Get Level Coordinates of Camera
	glm::i16vec2 new_level;
	new_level.x = floor(camera->Position.x / scene_data.sublevel_width);
	new_level.y = floor(camera->Position.y / scene_data.sublevel_height);

	// Test if Reloading Of Level is Needed
	if (new_level != level_position)
	{
		// Reload Levels
		reloadLevels(level_position, new_level, false);

		// Store Coords of New Level
		level_position = new_level;

		// Read Levels
		uint16_t index = temp_index_holder;
		for (int i = 0; i < 9; i++)
		{
			if (!sublevels[i].initialized)
				sublevels[i].readLevel(index);
		}

		// Load the Objects
		loadObjects();
	}
}

void Render::Objects::Level::reloadLevels(glm::i16vec2& level_old, glm::i16vec2& level_new, bool reload_all)
{

#ifdef SHOW_LEVEL_LOADING
	std::cout << "Begin Reloading\n";
	std::cout << "Current Position: " << level_old.x << " " << level_old.y << "\n";
	std::cout << "New Position: " << level_new.x << " " << level_new.y << "\n\n";
#endif

	// Reset Temp Index Holder
	temp_index_holder = 0;

	// Store Old Object Counts
	uint32_t total_object_count_old = container.total_object_count;

	// Variable for New Object Count
	uint32_t total_object_count_new = 0;

	// Index to Insert Into Used Sublevels Array
	int used_index = 0;

	// Test if Handler Should do a Complete Map Reset
	if (((abs(level_old.x - level_new.x) > 1 || abs(level_old.y - level_new.y) > 1) || (level_old == level_new) || reload_all) && !camera->wrapped)
	{
		// Reset Physics
		container.physics_list.erase();

		// Reset Entities
		container.entity_list.erase();

		// Remove Active Object Pointer for Every Object to Prevent Active Arrays from Being Accessed After Being Freed
		for (int i = 0; i < container.total_object_count; i++)
			container.object_array[i]->active_ptr = nullptr;

		// Create New Objects
		int iterater = 0;
		for (int level_y = 1; level_y > -2; level_y--)
		{
			for (int level_x = -1; level_x < 2; level_x++)
			{
				SubLevel& new_level = sublevels[iterater];
				new_level.deleteSubLevel();
				new_level = SubLevel(level_data_path, wrapLevelPos((int)level_new.x + level_x, (int)level_new.y + level_y), scene_size);
				new_level.addHeader(total_object_count_new);
				iterater++;
			}
		}

		// Allocate Memory
		reallocateAll(initialized, total_object_count_new);
		initialized = true;

		// Reset the Sublevel Counts
		for (int i = 0; i < 9; i++)
			sublevels[i].resetCounts();

#ifdef SHOW_LEVEL_LOADING
		std::cout << "reloaded all\n";
#endif

		return;
	}

	// Load and Unload Selected Levels
	
	// Move to the Left
	if ((!camera->wrapped && level_old.x - level_new.x > 0) || (camera->wrapped && level_old.x < level_new.x))
	{
#ifdef SHOW_LEVEL_LOADING
		std::cout << "moving west\n\n";
#endif

		// If Wrapped, Move All Objects in the Positive Horizontal Direction
		if (camera->wrapped) {
			for (int i = 0; i < container.total_object_count; i++)
				container.object_array[i]->pointerToPosition()->x += scene_size.x;
		}

		// Deconstruct Old Levels
		for (int i = 2; i < 9; i += 3)
		{
			sublevels[i].subtractHeader(container.total_object_count);
			used_arrays[used_index++] = sublevels[i].deactivateObjects();
		}

		// Shift Loaded Levels to the Right
		int next_level_location = -1;
		for (int i = 8; i > -1; i--)
		{
#ifdef SHOW_LEVEL_LOADING
			std::cout << "Index: " << i << "\n";
#endif

			// Load New Levels from the Left
			if (!(i % 3))
			{
				SubLevel& new_level = sublevels[i];
				new_level = SubLevel(level_data_path, wrapLevelPos((int)level_new.x - 1, (int)level_new.y + next_level_location), scene_size);
				new_level.addHeader(container.total_object_count);
				next_level_location++;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i - 1];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved west\n\n";
#endif

	}

	// Move to the Right
	else if ((!camera->wrapped && level_old.x - level_new.x < 0) || (camera->wrapped && level_old.x > level_new.x))
	{
#ifdef SHOW_LEVEL_LOADING
		std::cout << "moving east\n";
#endif

		// If Wrapped, Move All Objects in the Negative Horizontal Direction
		if (camera->wrapped) {
			for (int i = 0; i < container.total_object_count; i++)
				container.object_array[i]->pointerToPosition()->x -= scene_size.x;
		}

		// Deconstruct Old Levels
		for (int i = 0; i < 9; i += 3)
		{
			sublevels[i].subtractHeader(container.total_object_count);
			used_arrays[used_index++] = sublevels[i].deactivateObjects();
		}

		// Shift Loaded Levels to the Left
		int next_level_location = 1;
		for (int i = 0; i < 9; i++)
		{
#ifdef SHOW_LEVEL_LOADING
			std::cout << "Index: " << i << "\n";
#endif

			// Load New Levels from the Right
			if ((i % 3) == 2)
			{
				SubLevel& new_level = sublevels[i];
				new_level = SubLevel(level_data_path, wrapLevelPos((int)level_new.x + 1, (int)level_new.y + next_level_location), scene_size);
				new_level.addHeader(container.total_object_count);
				next_level_location--;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i + 1];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved east\n\n";
#endif

	}

	// Move to the North
	if ((!camera->wrapped && level_old.y - level_new.y < 0) || (camera->wrapped && level_old.y > level_new.y))
	{
#ifdef SHOW_LEVEL_LOADING
		std::cout << "moving north\n";
#endif

		// If Wrapped, Move All Objects in the Negative Vertical Direction
		if (camera->wrapped) {
			for (int i = 0; i < container.total_object_count; i++)
				container.object_array[i]->pointerToPosition()->y -= scene_size.y;
		}

		// Deconstruct Old Levels
		for (int i = 6; i < 9; i++)
		{
			sublevels[i].subtractHeader(container.total_object_count);
			used_arrays[used_index++] = sublevels[i].deactivateObjects();
		}

		// Shift Loaded Levels to the South
		int next_level_location = 1;
		for (int i = 8; i > -1; i--)
		{
#ifdef SHOW_LEVEL_LOADING
			std::cout << "Index: " << i << "\n";
#endif

			// Load New Levels from the North
			if (i < 3)
			{
				SubLevel& new_level = sublevels[i];
				new_level = SubLevel(level_data_path, wrapLevelPos((int)level_new.x + next_level_location, (int)level_new.y + 1), scene_size);
				new_level.addHeader(container.total_object_count);
				next_level_location--;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i - 3];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved north\n\n";
#endif

	}

	// Move to the South
	else if ((!camera->wrapped && level_old.y - level_new.y > 0) || (camera->wrapped && level_old.y < level_new.y))
	{
#ifdef SHOW_LEVEL_LOADING
		std::cout << "moving south\n";
#endif

		// If Wrapped, Move All Objects in the Positive Vertical Direction
		if (camera->wrapped) {
			for (int i = 0; i < container.total_object_count; i++)
				container.object_array[i]->pointerToPosition()->y += scene_size.y;
		}

		// Deconstruct Old Levels
		for (int i = 0; i < 3; i++)
		{
			sublevels[i].subtractHeader(container.total_object_count);
			used_arrays[used_index++] = sublevels[i].deactivateObjects();
		}

		// Shift Loaded Levels to the North
		int next_level_location = -1;
		for (int i = 0; i < 9; i++)
		{
#ifdef SHOW_LEVEL_LOADING
			std::cout << "Index: " << i << "\n";
#endif

			// Load New Levels from the South
			if (i > 5)
			{
				SubLevel& new_level = sublevels[i];
				new_level = SubLevel(level_data_path, wrapLevelPos((int)level_new.x + next_level_location, (int)level_new.y - 1), scene_size);
				new_level.addHeader(container.total_object_count);
				next_level_location++;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i + 3];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved south\n\n";
#endif

	}

#ifdef SHOW_LEVEL_LOADING
	std::cout << "Finished Reloading\n";
#endif

#ifdef SHOW_LEVEL_LOADING
	std::cout << "\n";
#endif

	// Update the Active Pointers for Each Sub Object That Was Moved
	for (int i = 0; i < 9; i++)
		sublevels[i].reloadActivePointer();

	// Reallocate Memory of Pointers
	reallocatePostReload(total_object_count_old);

	// Delete the Now Useless Sublevels
	for (int i = 0; i < used_index; i++)
	{
		if (used_arrays[i] != 0)
			delete[] used_arrays[i];
	}

	// Reset the Sublevel Counts
	for (int i = 0; i < 9; i++)
		sublevels[i].resetCounts();
}

void Render::Objects::Level::reallocatePhysics()
{
	int instant = Constant::INSTANCE_SIZE;
	int instance_index = 1;
	int rigid_body_count = 1;

	// Determine the Number of Rigid Bodies
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
		if ((*container.physics_list.it).base == Object::Physics::PHYSICS_BASES::RIGID_BODY)
			rigid_body_count++;

	// Allocate Memory for the Physics Instance Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PhysicsInstanceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Constant::INSTANCE_SIZE * rigid_body_count, NULL, GL_DYNAMIC_DRAW);
			
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
	{
		if ((*container.physics_list.it).base == Object::Physics::PHYSICS_BASES::RIGID_BODY)
		{
			static_cast<Object::Physics::Rigid::RigidBody*>(&container.physics_list.it)->initializeVertices(instant, instance_index);
		}
	}

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Render::Objects::Level::reallocateEntities()
{
}

void Render::Objects::Level::reallocatePostReload(uint32_t old_object_count)
{
	// Retrieve the Total Object Count and Array
	uint16_t new_object_count = container.total_object_count;
	Object::Object** old_object_array = container.object_array;

	// Reset Container
	container = { 0 };

	// Store Values Back in Container
	container.total_object_count = new_object_count;
	container.object_array = old_object_array;

	// Reallocate Main Object List
	temp_index_holder = 0;
	Object::Object** new_list = new Object::Object*[container.total_object_count];
	if (container.total_object_count != 0)
	{
		// Move Active Objects Into Array
		for (uint16_t i = 0; i < old_object_count; i++)
		{
			// Copy Pointer of Active Object Into Array, If Object is Active
			if (container.object_array[i]->active_ptr->active)
			{
				new_list[temp_index_holder] = container.object_array[i];
				temp_index_holder++;
			}

			// Else, Delete Pointer to New Object Since It is No Longer Being Used
			else
				delete container.object_array[i];
		}

		// Delete Old Array
		delete container.object_array;
	}

	// Else, Delete All Objects in Array and Delete the Original Containter
	else if (old_object_count != 0)
	{
		for (uint16_t i = 0; i < old_object_count; i++)
			delete container.object_array[i];
		delete container.object_array;
	}

	// Swap Arrays
	container.object_array = new_list;
}

void Render::Objects::Level::reallocateAll(bool del, uint32_t size)
{
	Render::Objects::ChangeController* cc = change_controller;

	// Test if Memory Has Previously Been Allocated
	if (del)
	{
		// Delete Objects in Container
		for (int i = 0; i < container.total_object_count; i++)
			delete container.object_array[i];

		// Delete Containers
		delete[] container.object_array;
		container.physics_list.erase();
		container.entity_list.erase();
	}

	// Null Initialize Container
	container = { 0 };

	// Set Size of Container
	container.total_object_count = size;

	// Allocate Memory
	container.object_array = new Object::Object*[size];
}

template<class Type>
inline uint16_t Render::Objects::Level::reallocateHelper(Type*** list, int old_count, int new_count)
{
	uint16_t index = 0;

	// Allocate New Array
	Type** new_list = new Type*[new_count];

	if (old_count != 0)
	{
		// Move Active Objects Into New Array
		for (uint16_t i = 0; i < old_count; i++)
		{
			if ((*list)[i]->active)
			{
				//new_list[index] = std::move(*list[i]);
				new_list[index] = (*list)[i];
				index++;
			}
		}

		// Delete Old Array
		delete[] (*list);
	}

	// Store New Array
	*list = new_list;

	// Return Index for Future Allocation of New Objects
	return index;
}

Render::Objects::Level::Level(std::string& level_path, float initial_x, float initial_y, bool force_coords)
{
	// Reset Level Objects
	container = { 0 };
	temp_index_holder = 0;

	// Test Output
	std::cout << "Opening Scene at Path: " << level_path << "\n";

#define READ
#ifdef READ

	// Open the Scene Data File for Reading
	std::ifstream scene_data_file = std::ifstream(level_path + "SceneData.dat");

	// Read the Scene Data
	scene_data_file.read((char*)&scene_data, sizeof(scene_data));

	// Read the Scene Name
	container_name.resize(scene_data.name_size);
	scene_data_file.read(&container_name[0], scene_data.name_size);

#else

	container_name = "test";
	scene_data.name_size = 5;
	scene_data.initial_camera_y = 40.0f;
	scene_data.initial_scale = 1.4f;

	// Open the Scene Data File for Reading
	std::ofstream scene_data_file = std::ofstream(level_path + "SceneData.dat");

	// Read the Scene Data
	scene_data_file.write((char*)&scene_data, sizeof(scene_data));

	// Read the Scene Name
	scene_data_file.write(container_name.c_str(), scene_data.name_size);

#endif

	// Close the File
	scene_data_file.close();

	// Calculate the Sizes for the Entire Level in Object Coordinates
	level_size.x = -scene_data.sublevel_count_west * scene_data.sublevel_width;
	level_size.y = (scene_data.sublevel_count_east + 1) * scene_data.sublevel_width;
	level_size.z = -scene_data.sublevel_count_south * scene_data.sublevel_height;
	level_size.w = (scene_data.sublevel_count_north + 1) * scene_data.sublevel_height;

	// Calculate the Scene Size
	scene_size.x = level_size.y - level_size.x;
	scene_size.y = level_size.w - level_size.z;
	//scene_size.x = (scene_data.sublevel_count_west + 1 + scene_data.sublevel_count_east) * scene_data.sublevel_width;
	//scene_size.y = (scene_data.sublevel_count_north + 1 + scene_data.sublevel_count_south) * scene_data.sublevel_height;

	// Calculate the Sizes for the Sublevel Holder
	level_diameter = (scene_data.render_distance << 1) - 1;
	level_count = level_diameter * level_diameter;
	level_max_index = level_diameter * level_diameter - 1;
	level_coord_offset = 1 - scene_data.render_distance;

	// Generate the Level Data Path
	level_data_path = level_path + "LevelData\\";

	// Generate the Editor Level Data Path
	editor_level_data_path = level_path + "EditorLevelData\\";

	// Save the New Zoom Scale
	Global::zoom_scale = scene_data.initial_scale;
	Global::zoom = true;

	// Update Projection Matrix
	Global::projection = glm::ortho(-Global::halfScalarX * Global::zoom_scale, Global::halfScalarX * Global::zoom_scale, -50.0f * Global::zoom_scale, 50.0f * Global::zoom_scale, 0.1f, 100.0f);

	// Update Matrices Uniform Buffer
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Construct Projection Matrices
	for (int i = 0; i < 6; i++)
		projection[i] = Global::projection;

	// Create Camera
	if (force_coords)
		camera = new Camera::Camera(initial_x, initial_y, scene_data.stationary, scene_data.wrap_sublevels, level_size);
	else
		camera = new Camera::Camera(scene_data.initial_camera_x, scene_data.initial_camera_y, scene_data.stationary, scene_data.wrap_sublevels, level_size);

	// Generate Terrain Buffer Object
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);

	// Bind Buffer
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);

	// Enable Position Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Enable Instance Index
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Allocate Vertices for Level Border Positions (2 floats per sublevel)
	float* border_vertices = new float[2 * level_count];

	// Calculate the Positions for Each Sublevel
	int initial_value = (int)scene_data.render_distance - (int)level_diameter;
	int horizontal_counter = 0;
	for (int i = 0, x_val = initial_value, y_val = initial_value; i < level_count * 2; i += 2)
	{
		// Set Coordinates
		border_vertices[i] = scene_data.sublevel_width * (float)x_val;
		border_vertices[i + 1] = scene_data.sublevel_height * (float)y_val;
		horizontal_counter++;
		x_val++;

		// Test if Row is Finished
		if (horizontal_counter == level_diameter)
		{
			horizontal_counter = 0;
			x_val = initial_value;
			y_val++;
		}
	}

	// Generate Border Buffer Object
	glGenVertexArrays(1, &borderVAO);
	glGenBuffers(1, &borderVBO);

	// Bind Buffer
	glBindVertexArray(borderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, borderVBO);

	// Generate and Store Buffer Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2 * level_count, border_vertices, GL_STATIC_DRAW);

	// Enable Vec2 Position Pointers
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Delete Vertex Aray
	delete[] border_vertices;

	// Bind the Level Border Shader
	Global::borderShader.Use();

	// Store the Chunk Size and Render Distance in Shader
	glUniform1f(Global::level_border_level_width_loc, scene_data.sublevel_width);
	glUniform1f(Global::level_border_level_height_loc, scene_data.sublevel_height);
	glUniform1i(Global::level_border_render_distance_loc, scene_data.render_distance);

	// Initialize Change Controller
	change_controller->storeContainerPointer(this);

	// Store Level Width for Debugger
	debugger->storeCurrentLevel(this);
	debugger->storeLevelPositions(scene_data.sublevel_width, scene_data.sublevel_height);
	camera->updateDebugPositions(true);

	// Enable Zoom
	Global::zoom_scale = 0.2f;
	Global::force_dissable_zoom = false;
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);
	Source::Listeners::ScrollCallback(Global::window, 0.0f, 0.0f);

	// Get the Level Position from the Initial Coordinates
	updateLevelPos(glm::vec2(camera->Position.x, camera->Position.y), level_position);

	// Generate the Sublevel List
	sublevels = new SubLevel[level_count];
	used_arrays = new void*[level_diameter * 2 - 1];

	// Initialize SubLevels
	for (int i = 0; i < level_count; i++)
	{
		glm::vec2 coords = getLevelFromIndex(i);
		sublevels[i] = SubLevel(level_data_path, wrapLevelPos((int)coords.x, (int)coords.y), scene_size);
		sublevels[i].addHeader(container.total_object_count);
	}

	// Allocate Memory
	reallocateAll(false, container.total_object_count);
	initialized = true;

	// Read SubLevels
	uint16_t index1 = 0;
	uint16_t index2 = 0;
	uint16_t index3 = 0;
	for (int i = 0; i < level_count; i++)
		sublevels[i].readLevel(index1);

	loadObjects();
}

Render::Objects::Level::~Level()
{
	// Delete the Object List
	delete[] container.object_array;

	// Delete the Used Arrays Array
	delete[] used_arrays;

	// Delete the Terrain Buffer
	glDeleteVertexArrays(1, &terrainVAO);
	glDeleteBuffers(1, &terrainVBO);

	// Delete the Border Buffer
	glDeleteVertexArrays(1, &borderVAO);
	glDeleteBuffers(1, &borderVBO);

	// Delete the Camera
	delete camera;
}

Render::CONTAINER_TYPES Render::Objects::Level::getContainerType()
{
	return CONTAINER_TYPES::LEVEL;
}

void Render::Objects::Level::updateCamera()
{
	// Update Camera
	camera->updatePosition();

	// Test if New SubLevels Should be Loaded
	testReload();
}

void Render::Objects::Level::updateContainer()
{
	// Update General Objects in Level
	for (uint32_t i = 0; i < container.total_object_count; i++)
	{
		container.object_array[i]->loop(container.object_array[i]);
		container.object_array[i]->updateObject();
	}

	// Update Physics Objects
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
	{
		//physics_list[i]->loop(physics_list[i]);
		(*container.physics_list.it).updateObject();
	}

	// Update Entity Objects
	for (container.entity_list.it = container.entity_list.beginStatic(); container.entity_list.it != container.entity_list.endStatic(); container.entity_list.it++)
	{
		(*container.entity_list.it).loop(&container.entity_list.it);
		(*container.entity_list.it).updateObject();
	}

	// Perform Physics-Mask Collision Detection
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
	{
		Source::Collisions::Mask::maskCollisionsPhysics(&container.physics_list.it, container.floor_size, container.left_wall_size, container.right_wall_size, container.ceiling_size,
			container.trigger_size, container.floor_start, container.left_wall_start, container.right_wall_start, container.ceiling_start, container.trigger_start);
	}

	// Perform Physics-Entity Collision Detection
	Source::Collisions::Physics::physicsCollisionDetection(container.physics_list);

	// Perform Entity-Mask Collision Detection
	for (container.entity_list.it = container.entity_list.beginStatic(); container.entity_list.it != container.entity_list.endStatic(); container.entity_list.it++)
	{
		Source::Collisions::Mask::maskCollisionsEntity(*container.entity_list.it, container.floor_size, container.left_wall_size, container.right_wall_size, container.ceiling_size,
			container.trigger_size, container.floor_start, container.left_wall_start, container.right_wall_start, container.ceiling_start, container.trigger_start);
	}

	// Perform Physics-Physics Collision Detection
	Source::Collisions::Physics::physicsEntityCollisionDetection(container.physics_list, container.entity_list);

	// Update Entity Interactions

	// Update Camera
}

void Render::Objects::Level::drawContainer()
{
	// Test, Delete Later
	for (int i = 0; i < container.total_object_count; i++) {
		const int TEST_INDEX = 10;
		if (container.object_array[i]->data_object->getObjectIndex() == TEST_INDEX) {
			//std::cout << TEST_INDEX << ":  " << container.object_array[i]->data_object->getParent() << "   " << container.object_array[i]->parent << "\n";
			//std::cout << TEST_INDEX << ":  " << container.object_array[i]->data_object->getPosition().x << "   " << container.object_array[i]->data_object->getPosition().y << "\n";
		}
	}

	// If Framebuffer Resize, Remake Projection
	if (Global::framebufferResize || Global::zoom || true)
	{
		for (int i = 0; i < 6; i++)
			projection[i] = Global::projection;
	}

	// Bind Object Shader
	Global::objectShader.Use();
	glEnable(GL_DEPTH_TEST);

	// Store View Position
	glUniform4f(glGetUniformLocation(Global::objectShader.Program, "material.viewPos"), camera->Position.x, camera->Position.y, 0.0f, 0.0f);
	glUniform1i(glGetUniformLocation(Global::objectShader.Program, "material.shininess"), 1);
	//glUniformMatrix4fv(glGetUniformLocation(Global::objectShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(7.0f, 0.0f, 0.0f))));

	// Get Relative Mouse Coords
	Global::mouseRelativeX = (float)Global::mouseX + camera->Position.x;
	Global::mouseRelativeY = (float)Global::mouseY + camera->Position.y;

	// Temp Matrix
	glm::mat4 matrix;

	// Create View / Model Matrix
	glm::mat4 view_model = camera->view * model;

	// Enable Vertex Object
	glBindVertexArray(terrainVAO);

	// Draw Terrain
	for (int i = 0; i < 5; i++)
	{
		// Bind Specific Terrain Texture
		//glBindTexture(GL_TEXTURE0, *terrain_textures[i]->texture.texture);
		//glBindTexture(GL_TEXTURE1, *terrain_textures[i]->texture.material);
		//glBindTexture(GL_TEXTURE2, *terrain_textures[i]->texture.mapping);

		// Bind Projection Matrix
		//matrix = glm::translate(glm::mat4(1.0f), glm::vec3(10 * sin(theta), 0.0f, 0.0f));
		//glUniformMatrix4fv(Global::matrixLocObject, 1, GL_FALSE, glm::value_ptr(matrix));

		// Draw Vertices
		glDrawArrays(GL_TRIANGLES, number_of_vertices[i], number_of_vertices[i + 1]);
	}

	// Unbind Vertex Object
	glBindVertexArray(0);

	// Create View / Projection Matrix
	glm::mat4 view_project = projection[5] * camera->view;

	// Draw Physics
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PhysicsInstanceBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Global::PhysicsInstanceBuffer);
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
		(*container.physics_list.it).drawObject();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Global::InstanceBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Draw Entities
	Global::texShaderStatic.Use();
	glUniform1i(Global::staticLocTexture, 0);
	for (container.entity_list.it = container.entity_list.beginStatic(); container.entity_list.it != container.entity_list.endStatic(); container.entity_list.it++)
		(&container.entity_list.it)->blitzEntity();
	Global::objectShader.Use();
	
	// Draw Formerground
	//glBindTexture(GL_TEXTURE0, *terrain_textures[5]->texture.texture);
	//glBindTexture(GL_TEXTURE1, *terrain_textures[5]->texture.material);
	//glBindTexture(GL_TEXTURE2, *terrain_textures[5]->texture.mapping);
	matrix = view_project * model;
	glBindVertexArray(terrainVAO);
	//glUniformMatrix4fv(Global::matrixLocObject, 1, GL_FALSE, glm::value_ptr(matrix));
	glDrawArrays(GL_TRIANGLES, number_of_vertices[5], number_of_vertices[6]);
	glBindVertexArray(0);
}

void Render::Objects::Level::loadObjects()
{
	// Segregate Some Objects Into Seperate Arrays
	segregateObjects();

	// Load Textures
	reallocateTextures();

	// Build Terrain Vertices
	constructTerrain();

	// Store Lights in Shader
	loadLights();

	// Update Physics for Rendering
	reallocatePhysics();
}

void Render::Objects::Level::constructTerrain()
{
	// Sort Terrain Objects By Z-Position
	Algorithms::Sorting::quickZSort(container.terrain_start, container.terrain_size);

	// Calculate Total Number of Vertices and Color Instances
	int vertices = 0;
	int instances = 1;

	// Reset Number of Vertices
	for (int i = 0; i < 7; i++)
	{
		number_of_vertices[i] = 0;

#ifdef EDITOR

		container.terrain_seperators[i] = 0;

#endif

	}

	for (int i = 0; i < container.terrain_size; i++)
	{
		// Increment Instance Counts
		vertices += container.terrain_start[i]->number_of_vertices;
		instances++;

		// Reset the Terrain Vertices
		container.terrain_start[i]->resetVertices();

		// Increment Terrain Layer Segregators
		switch (container.terrain_start[i]->layer)
		{
		case Object::Terrain::BACKDROP:
			number_of_vertices[1] += container.terrain_start[i]->number_of_vertices;
		case Object::Terrain::BACKGROUND_3:
			number_of_vertices[2] += container.terrain_start[i]->number_of_vertices;
		case Object::Terrain::BACKGROUND_2:
			number_of_vertices[3] += container.terrain_start[i]->number_of_vertices;
		case Object::Terrain::BACKGROUND_1:
			number_of_vertices[4] += container.terrain_start[i]->number_of_vertices;
		case Object::Terrain::FOREGROUND:
			number_of_vertices[5] += container.terrain_start[i]->number_of_vertices;
		case Object::Terrain::FORMERGROUND:
			number_of_vertices[6] += container.terrain_start[i]->number_of_vertices;
		}

#ifdef EDITOR

		switch (container.terrain_start[i]->layer)
		{
		case Object::Terrain::BACKDROP:
			container.terrain_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			container.terrain_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			container.terrain_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			container.terrain_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			container.terrain_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			container.terrain_seperators[6]++;
		}

#endif
	}

	// Construct the Terrain Objects
	constructTerrainHelper(vertices * 5, container.terrain_size, container.group_size, container.terrain_start, container.group_start);
}

void Render::Objects::Level::constructTerrainAlgorithm(int& instance, int& instance_index)
{
	int offset = 0;
	for (int i = 0; i < container.terrain_size; i++)
		container.terrain_start[i]->initializeTerrain(offset, instance, instance_index);
}

void Render::Objects::Level::loadLights()
{
	// Allocate Memory for Lights
	Vertices::Buffer::clearLightBuffer(Global::DirectionalBuffer, Global::PointBuffer, Global::SpotBuffer, Global::BeamBuffer, container.directional_size, container.point_size, container.spot_size, container.beam_size);

	// Sort Lights Based on Layer
	Algorithms::Sorting::quickZSort(container.directional_start, container.directional_size);
	Algorithms::Sorting::quickZSort(container.point_start, container.point_size);
	Algorithms::Sorting::quickZSort(container.spot_start, container.spot_size);
	Algorithms::Sorting::quickZSort(container.beam_start, container.beam_size);

	// Reset Number of Vertices
	for (int i = 0; i < 7; i++)
	{
		container.directional_seperators[i] = 0;
		container.point_seperators[i] = 0;
		container.spot_seperators[i] = 0;
		container.beam_seperators[i] = 0;
	}

	// Generate Directional Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::DirectionalBuffer);
	for (int i = 0, j = 16; i < container.directional_size; i++, j += 96)
	{
		// Bind Light
		container.directional_start[i]->buffer_offset = j;
		container.directional_start[i]->loadLight();

		// Determine Segregators for Light
		switch (container.directional_start[i]->returnLayer())
		{
		case Object::Terrain::BACKDROP:
			container.directional_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			container.directional_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			container.directional_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			container.directional_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			container.directional_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			container.directional_seperators[6]++;
		}
	}

	// Generate Point Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);
	for (int i = 0, j = 16; i < container.point_size; i++, j += 80)
	{
		// Bind Light
		container.point_start[i]->buffer_offset = j;
		container.point_start[i]->loadLight();

		// Determine Segregators for Light
		switch (container.point_start[i]->returnLayer())
		{
		case Object::Terrain::BACKDROP:
			container.point_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			container.point_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			container.point_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			container.point_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			container.point_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			container.point_seperators[6]++;
		}
	}

	// Generate Spot Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::SpotBuffer);
	for (int i = 0, j = 16; i < container.spot_size; i++, j += 96)
	{
		// Bind Light
		container.spot_start[i]->buffer_offset = j;
		container.spot_start[i]->loadLight();

		// Determine Segregators for Light
		switch (container.spot_start[i]->returnLayer())
		{
		case Object::Terrain::BACKDROP:
			container.spot_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			container.spot_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			container.spot_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			container.spot_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			container.spot_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			container.spot_seperators[6]++;
		}
	}

	// Generate Beam Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);
	for (int i = 0, j = 16; i < container.beam_size; i++, j += 96)
	{
		// Bind Light
		container.beam_start[i]->buffer_offset = j;
		container.beam_start[i]->loadLight();

		// Determine Segregators for Light
		switch (container.beam_start[i]->returnLayer())
		{
		case Object::Terrain::BACKDROP:
			container.beam_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			container.beam_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			container.beam_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			container.beam_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			container.beam_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			container.beam_seperators[6]++;
		}
	}

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Set Flag to Reload Selected Lights
	Global::reload_lights = true;

	// Remember to Set up Material and View Pos in Object Frag Shader
}

void Render::Objects::Level::segregateObjects()
{
	// IDEA: Segregation of Objects Will be Changed to Occour in a Heap Instead of Here All at Once.
	// This will reduce overall complexity. In addition, All Objects Will Remain in a Single List, But 
	// Seperated/Segregated by their Object Identifier. First Index in the Object Identifier will be Used
	// to Sort. If any tie, go to the Second then Third Identifier. Ties Between Unique Objects Have no
	// Importance. There will be a Data Structure That Contains the Start/End Indicies for Each Important
	// Object Group i.e. Terrain, Lights, Masks

	// Update Unsaved Level Model Matrices
	if (Global::editing)
	{
		for (int i = 0; i < level_count; i++)
			sublevels[i].updateModelMatrix();
	}

	// Sort the Objects Through Quick Sort
	Algorithms::Sorting::quickIdentifierSort(container.object_array, container.total_object_count);

	// The Map for Pointers in Object Container in Relation to Storage Map
	//                                          NULL,    FLOOR,                  LEFT,                       RIGHT,                       CEILING,                  TRIGGER,                  TERRAIN,                  DIRECTIONAL,                  POINT,                  SPOT,                  BEAM,                  GROUP,                  ELEMENT
	uint16_t* count_map[STORAGE_TYPE_COUNT] = { nullptr, &container.floor_size,  &container.left_wall_size,  &container.right_wall_size,  &container.ceiling_size,  &container.trigger_size,  &container.terrain_size,  &container.directional_size,  &container.point_size,  &container.spot_size,  &container.beam_size,  &container.group_size,  nullptr };
	void* pointer_map[STORAGE_TYPE_COUNT]   = { nullptr, &container.floor_start, &container.left_wall_start, &container.right_wall_start, &container.ceiling_start, &container.trigger_start, &container.terrain_start, &container.directional_start, &container.point_start, &container.spot_start, &container.beam_start, &container.group_start, nullptr };

	// Perform the Segregation
	segregateHelper(container, count_map, pointer_map);
}

#ifdef EDITOR

void Render::Objects::Level::genObjectIntoContainer(DataClass::Data_Object* new_object, Object::Object* real_parent, uint16_t& index, int16_t delta_size)
{
	// Get the Current Complex Offset of the New Object From Parent
	glm::vec2 complex_offset = glm::vec2(0.0f, 0.0f);
	if (real_parent != nullptr) {
		complex_offset = real_parent->calculateComplexOffset(false);
		if (real_parent != nullptr && real_parent->group_object->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
			complex_offset += real_parent->data_object->getPosition();
	}

	// Also get the Position of the Root Parent, or The Current Position of Data Object, If There is No Parent
	glm::vec2 root_position = new_object->getPosition();
	Object::Object* root_parent = real_parent;
	while (root_parent != nullptr) {
		root_position = *root_parent->pointerToPosition();
		root_parent = root_parent->parent;
	}

	// Determine the Unsaved Level of the Root Parent to Store Active Objects in
	glm::i16vec2 level_pos = glm::i16vec2(0, 0);
	updateLevelPos(root_position, level_pos);
	UnsavedLevel* unsaved_level = change_controller->getUnsavedLevel(level_pos.x, level_pos.y, 0);

	// TODO: Change Delta Size so Any Inactive Objects Are Removed At the Sime Time the Array is Reallocated

	// Reallocate Actives Array in Unsaved Level
	uint16_t active_index = unsaved_level->reallocateActivesList(delta_size);

	// Reallocate Parent's Children Array
	if (real_parent != nullptr)
		real_parent->children = new Object::Object*[1];

	// Generate Objects Into Container
	std::vector<DataClass::Data_Object*> temp_vector = { new_object };
	buildObjectsGenerator(temp_vector, index, real_parent, complex_offset, active_index, *unsaved_level);
}

void Render::Objects::Level::buildObjectsGenerator(std::vector<DataClass::Data_Object*>& data_object_array, uint16_t& index, Object::Object* parent, glm::vec2 position_offset, uint16_t& active_index, UnsavedLevel& unsaved_level)
{
	for (DataClass::Data_Object* data_object : data_object_array)
	{
		// TODO: If Object is Currently Selected, Generate a Temp Object, Not a Real Object

		// Generate Object and Attach Data Object
		Object::Object*  new_object = data_object->generateObject(position_offset);
		new_object->parent = parent;

		// If Parent != Nullptr, Add to Parent's Children Array
		if (parent != nullptr)
		{
			parent->children[parent->children_size] = new_object;
			parent->children_size++;
		}

		// Normally, Add Object Into Container
		if (data_object->getLevelEditorFlags().original_conditions == nullptr || !(
			(data_object->getLevelEditorFlags().original_conditions->append_change == nullptr) ^ 
			(data_object->getLevelEditorFlags().original_conditions->pop_change == nullptr)))
		{
			// Determine How to Store the Object
			switch (data_object->getObjectIdentifier()[0])
			{

				// Generate a Physics Objects
			case (Object::PHYSICS):
			{
				container.physics_list.appendStatic((Object::Physics::PhysicsBase*)new_object);
				break;
			}

			// Generate an Entity
			case (Object::ENTITY):
			{
				container.entity_list.appendStatic((Object::Entity::EntityBase*)new_object);
				break;
			}

			// Generate Normal, Stationary Object
			default:
			{
				container.object_array[index] = new_object;
				index++;
				unsaved_level.addToActivesList(new_object, active_index);
			}

			}
		}
		
		// If Object is Being Edited, Make a Temp Object
		else
		{
			unsaved_level.addToActivesList(new_object, active_index);
			Object::Object* temp_object = new Object::TempObject(new_object, &data_object->getPosition(), false);
			temp_objects.push_back(static_cast<Object::TempObject*>(temp_object));
			delete new_object;
			new_object = temp_object;
		}

		// Generate Children, if Applicable
		UnsavedCollection* group = data_object->getGroup();
		if (group != nullptr)
		{
			// Generate the Children Array in Object
			new_object->children = new Object::Object * [group->getChildren().size()];

			// Get the Potential Offset of the Object
			glm::vec2 new_offset = position_offset;
			if (group->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
				new_offset = new_object->returnPosition();

			// Recursively Generate Children
			buildObjectsGenerator(group->getChildren(), index, new_object, new_offset, active_index, unsaved_level);
		}
	}
}

uint8_t Render::Objects::Level::testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// If Mouse Moved, Reset Object Pass Over Flag
	if (Global::cursor_Move)
		resetObjectPassOver();

	// Set Highlighting to False
	selector.highlighting = false;
	object_info.active = false;

	// The Result of Selection
	uint8_t result = 0;

	// Test Selector On Formerground Lighting
	if (result = testSelectorLights(5, selector, object_info, container)) { return result; }

	// Test Selector On Formerground Effects
	if (result = testSelectorEffects(5, selector, object_info, container)) { return result; }

	// Test Selector On Formerground Terrain
	if (result = testSelectorTerrain(5, selector, object_info, container)) { return result; }

	// Test Selector On Group Objects
	if (result = testSelectorGroup(selector, object_info, container)) { return result; }

	// Test Selector On Entities
	if (result = testSelectorEntity(selector, object_info, container)) { return result; }

	// Test Selector On Physics Objects
	if (result = testSelectorPhysics(selector, object_info, container)) { return result; }

	// Test Selector On Collision Masks
	if (result = testSelectorMasks(selector, object_info, container)) { return result; }

	// Test Selector On Foreground Lighting
	if (result = testSelectorLights(4, selector, object_info, container)) { return result; }

	// Test Selector On Foreground Terrain
	if (result = testSelectorTerrain(4, selector, object_info, container)) { return result; }

	// Test Selector On Backgrond1 Lighting
	if (result = testSelectorLights(3, selector, object_info, container)) { return result; }

	// Test Selector On Background1 Terrain
	if (result = testSelectorTerrain(3, selector, object_info, container)) { return result; }

	// Test Selector On Background2 Lighting
	if (result = testSelectorLights(2, selector, object_info, container)) { return result; }

	// Test Selector On Background2 Terrain
	if (result = testSelectorTerrain(2, selector, object_info, container)) { return result; }

	// Test Selector On Background3 Lighting
	if (result = testSelectorLights(1, selector, object_info, container)) { return result; }

	// Test Selector On Background3 Terrain
	if (result = testSelectorTerrain(1, selector, object_info, container)) { return result; }

	// Test Selector On Backdrop Lighting
	if (result = testSelectorLights(0, selector, object_info, container)) { return result; }

	// Test Selector On Backdrop Terrain
	if (result = testSelectorTerrain(0, selector, object_info, container)) { return result; }

	// If We Ever Get To This Point, Reset the Pass Over Flag for All Objects
	resetObjectPassOver();

	// Return 0 Since There was no Selection
	return 0;
}

void Render::Objects::Level::removeMarkedChildrenFromList(DataClass::Data_Object* parent)
{
	removeMarkedChildrenFromListHelper(parent, container);
}

void Render::Objects::Level::resetObjectPassOver()
{
	// General Objects
	for (uint32_t i = 0; i < container.total_object_count; i++)
		container.object_array[i]->skip_selection = false;

	// Entities
	for (container.entity_list.it = container.entity_list.begin(); container.entity_list.it != container.entity_list.end(); container.entity_list.it++)
		(*container.entity_list.it).skip_selection = false;

	// Physics
	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it = container.physics_list.begin(); it != container.physics_list.end(); it++)
	{
		(*it).skip_selection = false;

		// SpringMass
		if ((*it).type == Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS)
		{
			Object::Physics::Soft::SpringMass& spring_mass = *static_cast<Object::Physics::Soft::SpringMass*>(&*it);
			for (int i = 0; i < spring_mass.node_count; i++)
				spring_mass.skip_nodes[i] = false;
			for (int i = 0; i < spring_mass.spring_count; i++)
				spring_mass.skip_springs[i] = false;
		}

		// Hinge
	}
}

void Render::Objects::Level::drawLevelBorder()
{
	// If Level Border Visualization is Disabled, Do Nothing
	if (!Global::level_border)
		return;

	// Bind Level Boarder Shader
	Global::borderShader.Use();

	// Bind Empty Vertex Object
	glBindVertexArray(borderVAO);

	// Store Camera Position
	glUniform2f(Global::level_border_camera_pos_loc, camera->Position.x, camera->Position.y);

	// Draw Borders
	glDrawArrays(GL_POINTS, 0, level_count);

	// Unbind Empty Vertex Object
	glBindVertexArray(0);
}

void Render::Objects::Level::drawVisualizers()
{
	// Bind Color Shader
	Global::colorShaderStatic.Use();

	// Draw Level Visualizers
	for (int i = 0; i < 9; i++)
		sublevels[i].drawVisualizer();

	// Draw Complex Object Visualizers
	change_controller->drawVisualizers();

	// Draw Floor Masks
	for (int i = 0; i < container.floor_size; i++)
		container.floor_start[i]->blitzLine();

	// Draw Left Masks
	for (int i = 0; i < container.left_wall_size; i++)
		container.left_wall_start[i]->blitzLine();

	// Draw Right Masks
	for (int i = 0; i < container.right_wall_size; i++)
		container.right_wall_start[i]->blitzLine();

	// Draw Ceiling Masks
	for (int i = 0; i < container.ceiling_size; i++)
		container.ceiling_start[i]->blitzLine();

	// Draw Trigger Masks
	for (int i = 0; i < container.trigger_size; i++)
		container.trigger_start[i]->blitzLine();

	// Draw Everything Else
	drawCommonVisualizers(container);
}

void Render::Objects::Level::resetLevel()
{
	// Reload All Sub Levels
	reloadLevels(level_position, level_position, false);
}

void Render::Objects::Level::returnObject(Object::Object* new_object, float object_x, float object_y, uint8_t object_identifier[3])
{

}

void Render::Objects::Level::reloadAll(float new_x, float new_y)
{
	// Store Camera Poition
	//camera->setPosition(glm::vec3(new_x, new_y, 0.0f));

	reloadAll();
}

void Render::Objects::Level::reloadAll()
{
	// Get Level Location of Camera
	glm::i16vec2 new_level;
	updateLevelPos(camera->Position, new_level);

	// Reload Levels
	reloadLevels(level_position, new_level, true);

	// Read Levels
	uint16_t index1 = 0;
	uint16_t index2 = 0;
	uint16_t index3 = 0;
	for (int i = 0; i < 9; i++)
		sublevels[i].readLevel(index1);

	// Load the Objects
	loadObjects();

	//std::cout << "Reloaded Actives: \n";
	//for (int i = 0; i < 9; i++)
	//	std::cout << "   " << sublevels[i].level_x << " " << sublevels[i].level_y << "   " << sublevels[i].number_of_loaded_objects << "\n";
}

void Render::Objects::Level::incorperatNewObjects(Object::Object** new_objects, int new_objects_size)
{
	incorperatNewObjectsHelper(new_objects, new_objects_size, container);
}

void Render::Objects::Level::setActives(Object::Object** new_objects, int new_objects_size)
{
	// Generate the New Active Object Array
	Object::Active* new_actives = new Object::Active[new_objects_size];

	// Determine the Level Coords for Each of the New Objects
	glm::i16vec2 level_coords = glm::i16vec2(0.0f, 0.0f);
	Object::Object* root_parent = nullptr;
	for (int i = 0; i < new_objects_size; i++)
	{
		// Get the Level Coords of the Object Root Parent Object
		root_parent = new_objects[i];
		while (root_parent->parent != nullptr)
			root_parent = root_parent->parent;
		updateLevelPos(*root_parent->pointerToPosition(), level_coords);

		// Generate the New Active Object
		new_actives[i] = Object::Active(true, true, level_coords, new_objects[i]);

		// Tell Respective Sublevel That There is a New Object
		sublevels[getIndexFromLevel(level_coords)].new_active_objects++;
	}

	// Update the Sublevel Active Pointers
	for (int i = 0; i < 9; i++) {
		sublevels[i].includeNewActives(new_actives, new_objects_size, this);
	}

	// Delete the Temp Active Array
	delete[] new_actives;
}

glm::mat4 Render::Objects::Level::returnProjectionViewMatrix(uint8_t layer)
{
	projection[layer] = Global::projection;
	return projection[layer] * camera->view;
}

void Render::Objects::Level::storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, Object::Object* real_object)
{
	// Get Level Coords of Object
	glm::i16vec2 coords;
	updateLevelPos(position, coords);

	// Store Pointer of Unsaved Level in Selector
	Render::Objects::UnsavedLevel* test = change_controller->getUnsavedLevel((int)coords.x, (int)coords.y, 0);
	selector.level_of_origin = test;

	// Remove Object from Unsaved Level
	selector.level_of_origin->createChangePop(selector.highlighted_object, real_object);

	// Set Originated From Level Flag to True
	selector.originated_from_level = true;
}

GLuint Render::Objects::Level::returnDirectionalBufferSize()
{
	return 96 * container.directional_size + 16;
}

GLuint Render::Objects::Level::returnPointBufferSize()
{
	return 80 * container.point_size + 16;
}

GLuint Render::Objects::Level::returnSpotBufferSize()
{
	return 96 * container.spot_size + 16;
}

GLuint Render::Objects::Level::returnBeamBufferSize()
{
	return 96 * container.beam_size + 16;
}

Render::Objects::SubLevel* Render::Objects::Level::getSublevels()
{
	return sublevels;
}

void Render::Objects::Level::getSceneInfo(SceneData** data, std::string** name)
{
	*data = &scene_data;
	*name = &container_name;
}

void Render::Objects::Level::getSublevelSize(glm::vec2& sizes)
{
	sizes.x = scene_data.sublevel_width;
	sizes.y = scene_data.sublevel_height;
}

std::string Render::Objects::Level::getLevelDataPath()
{
	return level_data_path;
}

std::string Render::Objects::Level::getEditorLevelDataPath()
{
	return editor_level_data_path;
}

#endif
