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
#include "Source/Vertices/Visualizer/Visualizer.h"

// Collisions
#include "Source/Collisions/Mask Collisions/MaskCollisions.h"
#include "Source/Collisions/Physics Collisions/PhysicsCollisions.h"

#include "Class/Render/Struct/DataClasses.h"

#include "UnsavedGroup.h"
#include "UnsavedComplex.h"
#include "Class\Render\Editor\Debugger.h"

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
	uint8_t top_left_y = level_coords.y - level_position.y - level_coord_offset;

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
	int8_t index_y = floor((index - index_x) / level_diameter);

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
				sublevels[i].readLevel(container.object_array, index, physics_list, entity_list);
		}

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
		physics_list.erase();

		// Reset Entities
		entity_list.erase();

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

	// Need to Have Structure With Array Indexable Pointers and Array Lengths to Segregate Objects
	Object::Object** object_array_pointer = container.object_array;
	Object::Object*** current_array_pointer = reinterpret_cast<Object::Object***>(&container.floor_start);
	(*current_array_pointer) = object_array_pointer;
	uint16_t* current_array_size_pointer = &container.floor_size;
	uint8_t container_index = 0;

	// The Map for Container Index to Container Storage Type
	uint8_t storage_map[11] = { Object::FLOOR_COUNT, Object::LEFT_COUNT, Object::RIGHT_COUNT, Object::CEILING_COUNT, Object::TRIGGER_COUNT, Object::TERRAIN_COUNT, Object::DIRECTIONAL_COUNT, Object::POINT_COUNT, Object::SPOT_COUNT, Object::BEAM_COUNT, Object::GROUP_COUNT };

	// Segregate Objects
	Object::Object* current_object = nullptr;
	Object::Object** object_array_end_pointer = object_array_pointer + container.total_object_count;
	int non_temp_object_count = 0;
	while (object_array_pointer < object_array_end_pointer)
	{
		current_object = *object_array_pointer;
		while (current_object->storage_type != storage_map[container_index])
		{
			current_array_pointer++;
			(*current_array_pointer) = object_array_pointer;
			current_array_size_pointer++;
			container_index++;
		}
		(*current_array_size_pointer)++;
		object_array_pointer++;
	}

	// Store Current Object Count in Debugger
	debugger->storeObjectCount(container.total_object_count);
}

void Render::Objects::Level::reallocateTextures()
{
}

void Render::Objects::Level::reallocatePhysics()
{
	int instant = Constant::INSTANCE_SIZE;
	int instance_index = 1;
	int rigid_body_count = 1;

	// Determine the Number of Rigid Bodies
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
		if ((*physics_list.it).base == Object::Physics::PHYSICS_BASES::RIGID_BODY)
			rigid_body_count++;

	// Allocate Memory for the Physics Instance Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PhysicsInstanceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, Constant::INSTANCE_SIZE * rigid_body_count, NULL, GL_DYNAMIC_DRAW);
			
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
	{
		if ((*physics_list.it).base == Object::Physics::PHYSICS_BASES::RIGID_BODY)
		{
			static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it)->initializeVertices(instant, instance_index);
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
	// Test if Memory Has Previously Been Allocated
	if (del)
	{
		// Delete Objects in Container
		for (int i = 0; i < container.total_object_count; i++)
			delete container.object_array[i];

		// Delete Containers
		delete[] container.object_array;
		physics_list.erase();
		entity_list.erase();
	}

	// Null Initialize Container
	container = { 0 };

	// Set Size of Container
	container.total_object_count = size;

	// Allocate Memory
	container.object_array = new Object::Object*[size];
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

		terrain_seperators[i] = 0;

#endif

	}

	for (int i = 0; i < container.terrain_size; i++)
	{
		// Increment Instance Counts
		vertices += container.terrain_start[i]->number_of_vertices;
		instances++;
		
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
			terrain_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			terrain_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			terrain_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			terrain_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			terrain_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			terrain_seperators[6]++;
		}
		
#endif
	}
	vertices *= 5;

	// Allocate Memory
	Vertices::Buffer::clearObjectVAO(terrainVAO, terrainVBO, vertices);
	Vertices::Buffer::clearObjectDataBuffer(Global::InstanceBuffer, instances);

	// Bind Buffer Objects
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::InstanceBuffer);

	// Construct and Store Each Vertex and Instance
	// Store Offset and Instance in Object
	int offset = 0;
	int instant = Constant::INSTANCE_SIZE;
	int instance_index = 1;
	for (int i = 0; i < container.terrain_size; i++)
		container.terrain_start[i]->initializeTerrain(offset, instant, instance_index);

	// Unbind Buffer Object
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Update Model Matrix for Group Visualizers
	for (int i = 0; i < container.group_size; i++)
		container.group_start[i]->updateModelMatrix();
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
		directional_seperators[i] = 0;
		point_seperators[i] = 0;
		spot_seperators[i] = 0;
		beam_seperators[i] = 0;
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
			directional_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			directional_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			directional_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			directional_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			directional_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			directional_seperators[6]++;
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
			point_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			point_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			point_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			point_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			point_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			point_seperators[6]++;
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
			spot_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			spot_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			spot_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			spot_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			spot_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			spot_seperators[6]++;
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
			beam_seperators[1]++;
		case Object::Terrain::BACKGROUND_3:
			beam_seperators[2]++;
		case Object::Terrain::BACKGROUND_2:
			beam_seperators[3]++;
		case Object::Terrain::BACKGROUND_1:
			beam_seperators[4]++;
		case Object::Terrain::FOREGROUND:
			beam_seperators[5]++;
		case Object::Terrain::FORMERGROUND:
			beam_seperators[6]++;
		}
	}

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Set Flag to Reload Selected Lights
	Global::reload_lights = true;

	// Remember to Set up Material and View Pos in Object Frag Shader
}

void Render::Objects::Level::getObjectIndicies(DataClass::Data_Object* parent, uint32_t** indicies, int& indicies_size)
{
	// Only Execute if Group is Not Nullptr Not Complex
	if (parent->getGroup() != nullptr)
	{
		// Get Vector of Children
		std::vector<DataClass::Data_Object*>& children = parent->getGroup()->getChildren();

		// Only Execute if There Are Children
		if (children.size() > 0)
		{
			// Dereference the Old Array
			uint32_t* old_array = *indicies;

			// Generate an Array to Hold the New Children
			uint32_t* children_indicies = new uint32_t[children.size()];

			// Copy Children Indicies Into Children Indicies Array
			for (int i = 0; i < children.size(); i++)
				children_indicies[i] = children[i]->getObjectIndex();

			// Sort Children Indicies (Selection Sort)
			int min_index = 0;
			for (int i = 0; i < children.size() - 1; i++)
			{
				min_index = i;
				for (int j = i; j < children.size(); j++)
				{
					if (children_indicies[j] < children_indicies[min_index])
						min_index = j;
				}
				uint32_t temp = children_indicies[min_index];
				children_indicies[min_index] = children_indicies[i];
				children_indicies[i] = temp;
			}

			// Determine the New Size of the Array
			int new_size = indicies_size + children.size();

			// Allocate Memory for the New Array
			uint32_t* new_array = new uint32_t[new_size];

			// Perform a Merge on the Old Array With New Children
			int left = 0;
			int right = 0;
			int count = 0;
			while (count < new_size)
			{
				if (left < indicies_size && right < children.size())
				{
					if (old_array[left] < children_indicies[right])
					{
						new_array[count] = old_array[left];
						left++;
					}

					else
					{
						new_array[count] = children_indicies[right];
						right++;
					}
				}

				else if (left < indicies_size)
				{
					new_array[count] = old_array[left];
					left++;
				}

				else
				{
					new_array[count] = children_indicies[right];
					right++;
				}

				count++;
			}

			// Delete Old and Temp Arrays
			if (indicies_size != 0)
				delete[] old_array;
			delete[] children_indicies;

			// Store Updated Array Values
			(*indicies) = new_array;
			indicies_size = new_size;

			// Recursively Add Children Indicies
			for (DataClass::Data_Object* child : children)
				getObjectIndicies(child, indicies, indicies_size);
		}
	}
}

bool Render::Objects::Level::searchObjectIndicies(uint32_t* indicies, int left, int right, uint32_t test_value)
{
	// If Code Reaches Here, Value Was Not Found
	if (left > right)
		return false;

	// Calculate Midpoint in Array
	int midpoint = (left + right) >> 1;

	// If Test Value is at Midpoint, Return True
	if (indicies[midpoint] == test_value)
		return true;

	// If Test Value is Less Than Midpoint Value, Test Lower Half
	if (test_value < indicies[midpoint])
		return searchObjectIndicies(indicies, left, midpoint - 1, test_value);

	// If Test Value is Greater Than Midpoint Value, Test Upper Half
	return searchObjectIndicies(indicies, midpoint + 1, right, test_value);
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
	scene_name.resize(scene_data.name_size);
	scene_data_file.read(&scene_name[0], scene_data.name_size);

#else

	scene_name = "test";
	scene_data.name_size = 5;
	scene_data.initial_camera_y = 40.0f;
	scene_data.initial_scale = 1.4f;

	// Open the Scene Data File for Reading
	std::ofstream scene_data_file = std::ofstream(level_path + "SceneData.dat");

	// Read the Scene Data
	scene_data_file.write((char*)&scene_data, sizeof(scene_data));

	// Read the Scene Name
	scene_data_file.write(scene_name.c_str(), scene_data.name_size);

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
	change_controller->storeLevelPointer(this);

	// Store Level Width for Debugger
	debugger->storeCurrentLevel(this);
	debugger->storeLevelPositions(scene_data.sublevel_width, scene_data.sublevel_height);
	camera->updateDebugPositions(true);

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
		sublevels[i].readLevel(container.object_array, index1, physics_list, entity_list);

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
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
	{
		//physics_list[i]->loop(physics_list[i]);
		(*physics_list.it).updateObject();
	}

	// Update Entity Objects
	for (entity_list.it = entity_list.beginStatic(); entity_list.it != entity_list.endStatic(); entity_list.it++)
	{
		(*entity_list.it).loop(&entity_list.it);
		(*entity_list.it).updateObject();
	}

	// Perform Physics-Mask Collision Detection
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
	{
		Source::Collisions::Mask::maskCollisionsPhysics(&physics_list.it, container.floor_size, container.left_wall_size, container.right_wall_size, container.ceiling_size,
			container.trigger_size, container.floor_start, container.left_wall_start, container.right_wall_start, container.ceiling_start, container.trigger_start);
	}

	// Perform Physics-Entity Collision Detection
	Source::Collisions::Physics::physicsCollisionDetection(physics_list);

	// Perform Entity-Mask Collision Detection
	for (entity_list.it = entity_list.beginStatic(); entity_list.it != entity_list.endStatic(); entity_list.it++)
	{
		Source::Collisions::Mask::maskCollisionsEntity(*entity_list.it, container.floor_size, container.left_wall_size, container.right_wall_size, container.ceiling_size,
			container.trigger_size, container.floor_start, container.left_wall_start, container.right_wall_start, container.ceiling_start, container.trigger_start);
	}

	// Perform Physics-Physics Collision Detection
	Source::Collisions::Physics::physicsEntityCollisionDetection(physics_list, entity_list);

	// Update Entity Interactions

	// Update Camera
}

void Render::Objects::Level::drawContainer()
{
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
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
		(*physics_list.it).drawObject();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, Global::InstanceBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Draw Entities
	Global::texShaderStatic.Use();
	glUniform1i(Global::staticLocTexture, 0);
	for (entity_list.it = entity_list.beginStatic(); entity_list.it != entity_list.endStatic(); entity_list.it++)
		(&entity_list.it)->blitzEntity();
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

#ifdef EDITOR

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

	// Draw Visualizer for Group Objects
	for (int i = 0; i < container.group_size; i++)
		container.group_start[i]->drawObject();

	// Bind Texture Shader
	Global::texShaderStatic.Use();
	glUniform1i(Global::staticLocTexture, 0);
	glUniform1i(Global::directionLoc, 1);

	// Draw Directional Lights
	for (int i = 0; i < container.directional_size; i++)
		container.directional_start[i]->blitzObject();

	// Draw Point Lights
	for (int i = 0; i < container.point_size; i++)
		container.point_start[i]->blitzObject();

	// Draw Spot Lights
	for (int i = 0; i < container.spot_size; i++)
		container.spot_start[i]->blitzObject();

	// Draw Beam Lights
	for (int i = 0; i < container.beam_size; i++)
		container.beam_start[i]->blitzObject();

	// Draw Visualizers for Groups
	for (int i = 0; i < container.total_object_count; i++)
		container.object_array[i]->drawGroupVisualizer();
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
	if (result = testSelectorLights(5, selector, object_info)) { return result; }

	// Test Selector On Formerground Effects
	if (result = testSelectorEffects(5, selector, object_info)) { return result; }

	// Test Selector On Formerground Terrain
	if (result = testSelectorTerrain(5, selector, object_info)) { return result; }

	// Test Selector On Group Objects
	if (result = testSelectorGroup(selector, object_info)) { return result; }

	// Test Selector On Entities
	if (result = testSelectorEntity(selector, object_info)) { return result; }

	// Test Selector On Physics Objects
	if (result = testSelectorPhysics(selector, object_info)) { return result; }

	// Test Selector On Collision Masks
	if (result = testSelectorMasks(selector, object_info)) { return result; }

	// Test Selector On Foreground Lighting
	if (result = testSelectorLights(4, selector, object_info)) { return result; }

	// Test Selector On Foreground Terrain
	if (result = testSelectorTerrain(4, selector, object_info)) { return result; }

	// Test Selector On Backgrond1 Lighting
	if (result = testSelectorLights(3, selector, object_info)) { return result; }

	// Test Selector On Background1 Terrain
	if (result = testSelectorTerrain(3, selector, object_info)) { return result; }

	// Test Selector On Background2 Lighting
	if (result = testSelectorLights(2, selector, object_info)) { return result; }

	// Test Selector On Background2 Terrain
	if (result = testSelectorTerrain(2, selector, object_info)) { return result; }

	// Test Selector On Background3 Lighting
	if (result = testSelectorLights(1, selector, object_info)) { return result; }

	// Test Selector On Background3 Terrain
	if (result = testSelectorTerrain(1, selector, object_info)) { return result; }

	// Test Selector On Backdrop Lighting
	if (result = testSelectorLights(0, selector, object_info)) { return result; }

	// Test Selector On Backdrop Terrain
	if (result = testSelectorTerrain(0, selector, object_info)) { return result; }

	// If We Ever Get To This Point, Reset the Pass Over Flag for All Objects
	resetObjectPassOver();

	// Return 0 Since There was no Selection
	return 0;
}

uint8_t Render::Objects::Level::testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	for (int i = terrain_seperators[index]; i < terrain_seperators[index + 1]; i++)
	{
		uint8_t returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.terrain_start), container.terrain_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				constructTerrain();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Objects::Level::testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	uint8_t returned_value = 0;

	// Directional Light
	for (int i = directional_seperators[index]; i < directional_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.directional_start), container.directional_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}
	
	// Point Light
	for (int i = point_seperators[index]; i < point_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.point_start), container.point_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Spot Light
	for (int i = spot_seperators[index]; i < spot_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.spot_start), container.spot_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Beam Light
	for (int i = beam_seperators[index]; i < beam_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.beam_start), container.beam_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Objects::Level::testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	return 0;
}

uint8_t Render::Objects::Level::testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Test Selector on Base Objects
	if (testSelectorOnList(physics_list, selector, object_info))
		return 2;

	// Test Selector on Hinge Objects
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
	{
		// Get Pointer to Object
		Object::Physics::PhysicsBase* object_pointer = &*(physics_list.it);

		// Test if Object is a SpringMass
		if (object_pointer->type == Object::Physics::PHYSICS_TYPES::TYPE_HINGE)
		{

		}
	}

	// Test Selector on SpringMass Objects
	for (physics_list.it = physics_list.beginStatic(); physics_list.it != physics_list.endStatic(); physics_list.it++)
	{
		// Get Pointer to Object
		Object::Physics::PhysicsBase* object_pointer = &*(physics_list.it);

		// Test if Object is a SpringMass
		if (object_pointer->type == Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS)
		{
			// Get Reference to Object
			Object::Physics::Soft::SpringMass& object = *static_cast<Object::Physics::Soft::SpringMass*>(&*(physics_list.it));
			selector.add_child_object = Editor::CHILD_OBJECT_TYPES::NONE;

			// Test if Object is Locked or Marked to Pass Over
			if (object.lock || object.skip_selection)
			{
				Global::Selected_Cursor = Global::CURSORS::LOCK;
				continue;
			}

			// Iterate Through Springs
			for (int i = 0; i < object.spring_count; i++)
			{
				// Test if Spring is to be Skipped
				if (object.skip_springs[i])
					continue;

				// Calculate the Angle Between Nodes
				glm::vec2& node_pos_1 = object.nodes[object.springs[i].Node1].Position;
				glm::vec2& node_pos_2 = object.nodes[object.springs[i].Node2].Position;
				float node_angle = Algorithms::Math::angle_from_vector(node_pos_1 - node_pos_2);

				// Get Angle Between Mouse and Node1
				float mouse_angle = Algorithms::Math::angle_from_vector(node_pos_1 - glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));

				// Get Upper and Lower Bounds of Node Angle to Compare to
				float upper_bound = node_angle + 0.01f;
				float lower_bound = node_angle - 0.01f;

				// Variable That Determines if Angles Match
				bool matching_angles = false;

				// Lower Bound is Negative
				if (lower_bound < 0.0f)
				{
					// Mouse is in Quadrant IV
					if (mouse_angle > 3.14159f)
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle - 6.2832f > lower_bound)
							matching_angles = true;
					}

					// Mouse is in Quadrant I
					else
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle > lower_bound)
							matching_angles = true;
					}
				}

				// Upper Bound is Greater Than 2*pi
				else if (upper_bound > 6.2832f)
				{
					// Mouse is in Quadrant IV
					if (mouse_angle > 3.14159f)
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle > lower_bound)
							matching_angles = true;
					}

					// Mouse is in Quadrant I
					else
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound - 6.2832f && mouse_angle > lower_bound)
							matching_angles = true;
					}
				}

				// Both Bounds are in Respectable Values
				else
				{
					// Test if Mouse Angle is Within Node Angle
					if (mouse_angle < node_angle + 0.01f && mouse_angle > node_angle - 0.01f)
						matching_angles = true;
				}

				// If Angles Match, Move On to Test Distances
				if (matching_angles)
				{
					// Calculate Distances
					float max_distance = glm::distance(node_pos_1, node_pos_2);
					float mouse_distance = glm::distance(node_pos_1, glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));

					// If Mouse Distance is Less Than Max Distance, There is a Collision
					if (mouse_distance < max_distance)
					{
						// Test if User is Going to Skip Spring
						if (Global::Keys[GLFW_KEY_TAB])
						{
							Global::Keys[GLFW_KEY_TAB] = false;
							object.skip_springs[i] = true;
							continue;
						}

						// Enable Highlighter
						selector.highlighting = true;
						object_info.active = true;

						// Set Cursor to Hand
						Global::Selected_Cursor = Global::CURSORS::HAND;

						// Select Spring
						selector.add_child_object = Editor::CHILD_OBJECT_TYPES::SPRINGMASS_SPRING;
						//selector.spring_data.Node1 = object.nodes[object.springs[i].Node1].Name;
						//selector.spring_data.Node2 = object.nodes[object.springs[i].Node2].Name;
						//selector.object_index = 0;
						selector.moused_object = nullptr;
						//selector.data_objects.push_back(object_pointer->data_object);
						selector.highlighted_object = object.data_springs[i];
						selector.temp_connection_pos_left = node_pos_1;
						selector.temp_connection_pos_right = node_pos_2;
						selector.activateHighlighter(glm::vec2(0.0f, 0.0f), Editor::SelectedHighlight::SELECTABLE);
						selector.highlighting = true;
						object_info.clearAll();
						object_info.setObjectType("SpringMass Spring", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
						object_info.addDoubleValue("Nodes: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "L: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " R: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &object.nodes[object.springs[i].Node1].Name, &object.nodes[object.springs[i].Node2].Name, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), true);
						object_info.addSingleValue("Rest Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].RestLength, glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), false);
						object_info.addSingleValue("Max Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].MaxLength, glm::vec4(0.0f, 0.9f, 0.0f, 1.0f), false);
						object_info.addSingleValue("Spring Constant: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].Stiffness, glm::vec4(0.0f, 0.0f, 0.9f, 1.0f), false);
						object_info.addSingleValue("Dampening: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].Dampening, glm::vec4(0.9f, 0.0f, 0.9f, 1.0f), false);

						// IDEA: Draw Red Circle at Left Node and Blue Circle at Right Node When Selecting
						Vertices::Visualizer::visualizePoint(object.nodes[object.springs[i].Node1].Position, 0.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
						Vertices::Visualizer::visualizePoint(object.nodes[object.springs[i].Node2].Position, 0.5f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

						// If Left Click, Select Object
						if (Global::LeftClick)
						{
							Global::LeftClick = false;

							// Select Spring
							selector.unadded_data_objects.push_back(object.data_springs[i]);

							// Copy File Data Into Stream
							std::stringstream file_stream;
							std::ifstream in_file;
							in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);
							file_stream << in_file.rdbuf();
							in_file.close();

							// Open File for Writing
							std::ofstream out_file;
							out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);

							// Copy the Number of Nodes
							char temp_byte;
							file_stream.read(&temp_byte, 1);
							out_file.put(temp_byte);

							// Copy the Number of Springs Decremented by 1
							file_stream.read(&temp_byte, 1);
							temp_byte--;
							out_file.put(temp_byte);

							// Copy Data Until Match is Found. Skip Match, then Continue Copying
							Object::Physics::Soft::NodeData temp_node_data;
							Object::Physics::Soft::Spring temp_spring_data;
							int spring_count = 0;
							while (!file_stream.eof())
							{
								// Read Identifier
								file_stream.read(&temp_byte, 1);

								if (file_stream.eof())
									break;

								// Node
								if (temp_byte == 0)
								{
									out_file.put(temp_byte);
									file_stream.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
									out_file.write((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								}

								// Spring
								else
								{
									file_stream.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
									if (i != spring_count)
									{
										out_file.put(temp_byte);
										out_file.write((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
									}
									spring_count++;
								}
							}

							// Close File
							out_file.close();

							object.read();

							// Reload All SpringMass Objects With Matching File

							// Enable Selector for Editing
							selector.active = true;
							selector.editing = false;
							//selector.data_objects.push_back(object.data_object);
							//selector.unadded_data_objects.push_back(selector.highlighted_object);
							//selector.readSpringMassFile();

							//return true;
						}

						return 2;
					}
				}
			}

			// Iterate Through Nodes
			for (int i = 0; i < object.node_count; i++)
			{
				// Test if Node is to be Skipped
				if (object.skip_nodes[i])
					continue;

				// Test if Mouse Intersects Node
				if (glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), object.nodes[i].Position) < object.nodes[i].Radius)
				{
					// Test if User is Going to Skip Node
					if (Global::Keys[GLFW_KEY_TAB])
					{
						Global::Keys[GLFW_KEY_TAB] = false;
						object.skip_nodes[i] = true;
						continue;
					}

					// Enable Highlighter
					selector.highlighting = true;
					object_info.active = true;

					// Set Cursor to Hand
					Global::Selected_Cursor = Global::CURSORS::HAND;

					// Select Node
					selector.add_child_object = Editor::CHILD_OBJECT_TYPES::SPRINGMASS_NODE;
					//selector.node_data.position = object.nodes[i].Position;
					//selector.node_data.mass = object.nodes[i].Mass;
					//selector.node_data.health = object.nodes[i].Health;
					//selector.node_data.material = object.nodes[i].material;
					//selector.node_data.radius = object.nodes[i].Radius;
					//selector.node_data.name = object.nodes[i].Name;
					//selector.object_index = 0;
					selector.moused_object = nullptr;
					selector.highlighted_object = object.data_nodes[i];
					selector.activateHighlighter(glm::vec2(0.0f, 0.0f), Editor::SelectedHighlight::SELECTABLE);
					selector.highlighting = true;
					object_info.clearAll();
					object_info.setObjectType("SpringMass Node", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
					object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.nodes[i].Name, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
					object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &object.nodes[i].Position.x, &object.nodes[i].Position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

					// If Left Click, Select Object
					if (Global::LeftClick)
					{
						Global::LeftClick = false;

						// Select Node
						selector.unadded_data_objects.push_back(object.data_nodes[i]);

						//return true;

						// Copy File Data Into Stream
						std::stringstream file_stream;
						std::ifstream in_file;
						in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);
						file_stream << in_file.rdbuf();
						in_file.close();

						// Open File for Writing
						std::ofstream out_file;
						out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);

						// Copy the Number of Nodes Decremented by 1
						char temp_byte;
						file_stream.read(&temp_byte, 1);
						temp_byte--;
						out_file.put(temp_byte);

						// Copy the Number of Springs 
						file_stream.read(&temp_byte, 1);
						out_file.put(temp_byte);

						// Copy Data Until Match is Found. Skip Match, then Continue Copying
						Object::Physics::Soft::NodeData temp_node_data;
						Object::Physics::Soft::Spring temp_spring_data;
						while (!file_stream.eof())
						{
							// Read Identifier
							file_stream.read(&temp_byte, 1);

							if (file_stream.eof())
								break;

							// Node
							if (temp_byte == 0)
							{
								file_stream.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								if (temp_node_data.name != object.nodes[i].Name)
								{
									out_file.put(temp_byte);
									out_file.write((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								}
							}

							// Spring
							else
							{
								out_file.put(temp_byte);
								file_stream.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
								out_file.write((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
							}
						}

						// Close File
						out_file.close();

						// ReRead Data in SpringMass File
						object.read();

						// Perform Secondary Selection to Link Springs
						object.select3(selector);

						// Enable Selector for Editing
						selector.active = true;
						selector.editing = false;
						//selector.data_objects.push_back(object.data_object);
						//selector.unadded_data_objects.push_back(selector.highlighted_object);
					}

					return 2;
				}
			}
		}
	}

	return 0;
}

uint8_t Render::Objects::Level::testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	return testSelectorOnList(entity_list, selector, object_info);
}

uint8_t Render::Objects::Level::testSelectorGroup(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	for (int i = 0; i < container.group_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.group_start), container.group_size, selector, i, object_info))
			return true;
	}

	return false;
}

uint8_t Render::Objects::Level::testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Test Floor Masks
	for (int i = 0; i < container.floor_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.floor_start), container.floor_size, selector, i, object_info))
		{
			return true;
		}
	}

	// Test Left Wall Masks
	for (int i = 0; i < container.left_wall_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.left_wall_start), container.left_wall_size, selector, i, object_info))
		{
			return true;
		}
	}

	// Test Right Wall Masks
	for (int i = 0; i < container.right_wall_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.right_wall_start), container.right_wall_size, selector, i, object_info))
		{
			return true;
		}
	}

	// Test Ceiling Masks
	for (int i = 0; i < container.ceiling_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.ceiling_start), container.ceiling_size, selector, i, object_info))
		{
			return true;
		}
	}

	// Test Trigger Masks
	for (int i = 0; i < container.trigger_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.trigger_start), container.trigger_size, selector, i, object_info))
		{
			return true;
		}
	}

	return false;
}

uint8_t Render::Objects::Level::testSelectorOnObject(Object::Object*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info)
{
	// Get Reference of List and Object
	Object::Object** temp_list = *object_list;
	Object::Object& object = *(temp_list[index]);

	// Test if Object is Locked or Marked to Pass Over
	if (object.lock || object.skip_selection)
	{
		Global::Selected_Cursor = Global::CURSORS::LOCK;
		return 0;
	}

	// Test if Mouse Intersects Object
	if (object.testMouseCollisions(Global::mouseRelativeX, Global::mouseRelativeY))
	{
		// If Tab is Pressed, Mark Object for Pass Over
		if (Global::Keys[GLFW_KEY_TAB])
		{
			Global::Keys[GLFW_KEY_TAB] = false;
			object.skip_selection = true;
			return 0;
		}

		// Enable Highlighter
		selector.highlighting = true;
		object_info.active = true;

		// If Object is Not Currently Selected, Set Highlighter Visualizer
		if (selector.moused_object != &object)
		{
			selector.moused_object = &object;

			// If Select Was Unsuccessful, Test Next Object
			if (!temp_list[index]->select(selector, object_info, Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT]))
			{
				object_info.active = false;
				Global::Selected_Cursor = Global::CURSORS::FORBIDEN;
				return 1;
			}
		}

		// Set Cursor to Hand
		Global::Selected_Cursor = Global::CURSORS::HAND;

		// If Left Click, Select Object
		if (Global::LeftClick)
		{
			// Disable Left Click
			Global::LeftClick = false;

			// If Attempting to Add a Child Object, Attempt to Add Object as a Child
			if (selector.selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT]))
			{
				// Get Selected Object
				DataClass::Data_Object* selected_object = selector.getOnlyOne();

				// Get Parent of Selector
				DataClass::Data_Object* selected_parent = selected_object->getParent();

				// If Attempting to Add a Parent to its Child, Prevent That From Happening
				// For Now, This Will Cause a Deselection, Might Do Something Else in the Future
				// For This object.data_object is calling the function, Test is the selected_parent->getObjectIndex();
				if (selected_parent != nullptr && object.data_object->testIsParent(selected_parent))
				{
					// If Selected Object Cannot be Returned to Level, Don't Deselect
					if (!change_controller->getUnsavedLevelObject(selected_object)->testValidSelection(object.data_object, selected_object))
						return 1;

					// Remove Child from Current Group
					object.data_object->getObjectIdentifier()[3]--;

					// Set Parent of Selected Object to Nothing
					selected_object->setParent(nullptr);

					// Clear Parent of Selected Object in Selector
					selector.clearOnlyOneComplexParent();

					// Set Group Layer to 0
					selected_object->setGroupLayer(0);

					// Recursively Set Group Layer
					UnsavedCollection* data_group = selected_object->getGroup();
					if (data_group != nullptr)
						data_group->recursiveSetGroupLayer(1);
				}

				// Object Already Belongs to a Parent
				else if (object.data_object->getParent() != nullptr)
				{
					// Determine if the Specified Operation is Valid
					bool adding_to_current_parent = selected_object->testIsParent(object.data_object->getParent());
					if (adding_to_current_parent) {
						if (!change_controller->getUnsavedLevelObject(object.data_object)->testValidSelection(selected_object, object.data_object))
							return 1; }
					else {
						// Note: If Object Does Not Have a Group, It is a Standard Group Object
						if (selected_object->getGroup() == nullptr) {
							if (!Render::Objects::UnsavedGroup::testValidSelectionStatic(selected_object, object.data_object))
								return 1; }
						else if (!selected_object->getGroup()->testValidSelection(selected_object, object.data_object))
							return 1;
					}
						
					// Remove Child from Current Group
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);
					object.data_object->getParent()->getObjectIdentifier()[3]--;

					// If Attempting to Add To Its Current Parent, Remove Object From Being a Child
					// For This, Selected Object DataObject is Calling the Function, Test is the object.data_object->getParent()->getObjectIndex();
					if (adding_to_current_parent)
						change_controller->handleSingleSelectorReturn(object.data_object->makeCopySelected(selector), object.data_object, &selector,  true, false);

					// Else, Swap Parents
					else
						selector.addChildToOnlyOne(object.data_object->makeCopySelected(selector), object);
				}

				// Else, Add Object as a New Child
				else
				{
					// Test if Object can be Placed in Group
					// Note: If Object Does Not Have a Group, It is a Standard Group Object
					if (selected_object->getGroup() == nullptr) {
						if (!Render::Objects::UnsavedGroup::testValidSelectionStatic(selected_object, object.data_object))
							return 1; }
					else if (!selected_object->getGroup()->testValidSelection(selected_object, object.data_object))
						return 1;

					// Remove Child From Level
					storeLevelOfOrigin(selector, object.returnPosition(), MOVE_WITH_PARENT::MOVE_DISSABLED);

					// Add Child to Parent
					selector.addChildToOnlyOne(object.data_object->makeCopySelected(selector), object);
				}
			}

			// Else, Add to Selector
			else
			{
				// Activate Selector
				selector.active = true;

				// Possible Offset Created if From Complex Object
				glm::vec2 complex_offset = glm::vec2(0.0f, 0.0f);

				// If Object is a Complex Object, Store Values for Complex Parent
				// Note: Need to Find a Way to Do This for Children of Objects To Be Selected
				// This Is Because Complex Objects Can Now Become Children, And This Function Will be Skipped
				if (object.group_object != nullptr && object.data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				{
					// Get the Root Parent Object of Object's Group
					DataClass::Data_Object* complex_version = static_cast<Render::Objects::UnsavedComplex*>(object.data_object->getGroup())->getComplexParent();

					// If Inactive, Set Position Offset
					if (!static_cast<DataClass::Data_ComplexParent*>(complex_version)->isActive())
					{
						// Store Root Parent in Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->storeRootParent(&object);

						// Store Offset in Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->setPositionOffset(object.returnPosition());

						// Activate Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->setActive();
					}

					// If Not Active, DO NOT SELECT
					else
						return 1;
				}

				// Store Level of Origin if Originated From Level
				if (object.parent == nullptr)
					storeLevelOfOrigin(selector, object.returnPosition(), MOVE_WITH_PARENT::MOVE_ENABLED);

				// If Originated From Group, Remove from Group
				else if (object.data_object->getParent() != nullptr)
				{
					// Look Through List of Objects to Determine If Object is From Complex Objects
					Object::Object* current_parent_object = object.parent;
					bool offseting = true;
					while (current_parent_object != nullptr)
					{
						// Test if Root Parent of Group is A Complex Object
						if (current_parent_object->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
						{
							// Get the Parent Object That Matches the DataClass Parent
							DataClass::Data_Object* current_parent = static_cast<Render::Objects::UnsavedComplex*>(current_parent_object->group_object)->getComplexParent();

							// If Root Parent Object is a Temp Object, DO NOT SELECT
							if (current_parent_object->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
								return 1;

							// If Inactive, Set Position Offset
							if (!static_cast<DataClass::Data_ComplexParent*>(current_parent)->isActive())
							{
								// Store Root Parent in Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->storeRootParent(current_parent_object);

								// Store Offset in Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->setPositionOffset(current_parent_object->returnPosition());

								// Activate Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->setActive();
							}

							// Remove Group Object Offset
							if (offseting)
								complex_offset = static_cast<DataClass::Data_ComplexParent*>(current_parent)->getPositionOffset();
							offseting = false;
						}

						// Check Out the Next Parent
						current_parent_object = current_parent_object->parent;
					}

					// For All Group Objects, Simply Pop From Current Object
					//object.data_object->getParent()->getGroup()->createChangePop(object.data_object, MOVE_WITH_PARENT::MOVE_ENABLED);
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object, MOVE_WITH_PARENT::MOVE_DISSABLED);
				}

				// Make a Copy of the Data Class
				DataClass::Data_Object* dataclass_copy = selector.highlighted_object->makeCopySelected(selector);

				// Remove Object From Global Objects List
				removeMarkedFromList(temp_list[index], &dataclass_copy->getPosition());

				// Reset Object Info
				object_info.clearAll();

				// Apply Possible Complex Offset
				dataclass_copy->offsetPosition(complex_offset);

				// Select the Object
				selector.unadded_data_objects.push_back(dataclass_copy);
			}

			return 2;
		}

		return 1;
	}

	return 0;
}

template<class Type>
uint8_t Render::Objects::Level::testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Iterate Through Object List
	for (object_list.it = object_list.beginStatic(); object_list.it != object_list.endStatic(); object_list.it++)
	{
		// Get Reference and Object
		Type& object = *(object_list.it);

		// Test if Object is Locked or Marked to Pass Over
		if (object.lock || object.skip_selection)
		{
			Global::Selected_Cursor = Global::CURSORS::LOCK;
			return 0;
		}

		// Test if Mouse Intersects Object
		if (object.testMouseCollisions(Global::mouseRelativeX, Global::mouseRelativeY))
		{
			// If Tab is Pressed, Mark Object for Pass Over
			if (Global::Keys[GLFW_KEY_TAB])
			{
				Global::Keys[GLFW_KEY_TAB] = false;
				object.skip_selection = true;
				return 0;
			}

			// Enable Highlighter
			selector.highlighting = true;
			object_info.active = true;

			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If Object is Not Currently Selected, Set Highlighter Visualizer
			if (selector.moused_object != &object)
				object.select(selector, object_info, false);
			selector.highlighting = true;

			// If Left Click, Select Object
			if (Global::LeftClick)
			{
				// Activate Selector
				selector.active = true;

				// Store Level of Origin
				storeLevelOfOrigin(selector, object.returnPosition(), MOVE_WITH_PARENT::MOVE_ENABLED);

				// Remove Object From List
				object_list.removeObject(object_list.it);

				// Reset Object Info
				object_info.clearAll();

				// Make a Copy of the Data Class
				selector.unadded_data_objects.push_back(selector.highlighted_object->makeCopySelected(selector));

				return 2;
			}

			return 1;
		}
	}

	return 0;
}

void Render::Objects::Level::removeMarkedFromList(Object::Object* marked_object, glm::vec2* new_selected_position)
{
	// Note: This Function is Guarenteed to Remove 1 Object From the List, However,
	// For the Case Where the Object is From a Complex Object, More Than 1 Objects
	// With the Same Index May be Removed. The Final Array Size Will be Equal to the
	// Old Object Count - 1, However, The Container Total Object Count May be Decremented
	// Multiple Times for Any Repeated Indicies. The Excess and Unused Elements in the New
	// Array Will Not be Used and Will be Ignored Until Next Build of the Array

	// First, Reset DataClass Objects Array
	marked_object->data_object->clearObjects();

	// Retrieve the Object Index of the Marked Object
	uint32_t marked_index = marked_object->object_index;

	// Retrieve the Total Object Count and Array
	uint16_t old_object_count = container.total_object_count;
	Object::Object** old_object_array = container.object_array;

	// Reset Container
	container = { 0 };

	// Store Values Back in Container
	container.total_object_count = old_object_count;
	container.object_array = old_object_array;

	// Create New Array of Objects
	Object::Object** new_list = new Object::Object*[old_object_count - 1];

	// Coppy Objects From Old List to New List, Skipping Marked Object(s)
	short new_list_index = 0;
	for (uint32_t i = 0; i < old_object_count; i++)
	{
		// Test if Object is Marked
		if (container.object_array[i]->object_index == marked_index)
		{
			// Get the Object
			Object::Object* object = container.object_array[i];

			// Generate the Temp Object
			Object::TempObject* temp_object = new Object::TempObject(object, new_selected_position, object == marked_object);
			temp_objects.push_back(temp_object);

			// Store Temp Object in Place of Parent, If Object Has a Group
			if (object->group_object != nullptr)
			{
				// If Object is Complex, Add as an Instance to Complex Object
				if (object->group_object->getCollectionType() == Objects::UNSAVED_COLLECTIONS::COMPLEX)
					static_cast<Objects::UnsavedComplex*>(object->group_object)->addComplexInstance(temp_object);

				// Store Temp Object as Parent
				Object::Object** children = object->children;
				for (int i = 0; i < object->children_size; i++)
					children[i]->parent = temp_object;
			}

			// If Object Has a Parent, Replace the Original Child Instance With the Temp Object
			if (object->parent != nullptr)
			{
				Object::Object* test_child_object = nullptr;
				for (int i = 0; i < object->parent->children_size; i++)
				{
					test_child_object = object->parent->children[i];
					if (test_child_object == object)
					{
						object->parent->children[i] = temp_object;
						break;
					}
				}
			}

			// Delete Object
			delete object;

			// Also, Decrement Total Object Count
			container.total_object_count--;

			continue;
		}

		// Else, Copy Object
		new_list[new_list_index] = container.object_array[i];
		new_list_index++;
	}

	// Delete Old List
	delete[] container.object_array;

	// Swap Lists
	container.object_array = new_list;

	// Segregate Objects
	segregateObjects();
}

void Render::Objects::Level::removeMarkedChildrenFromList(DataClass::Data_Object* marked_parent)
{
	// Note: Array Will NOT be Reduced, Since it WILL be Reduced After Parent is Returned
	
	// Get the Array of Indicies to Remove
	uint32_t* indices = new uint32_t[1];
	indices[0] = marked_parent->getObjectIndex();
	int indices_size = 1;
	getObjectIndicies(marked_parent, &indices, indices_size);

	// If There Were No Children to Remove, No Need to Iterate
	if (indices_size == 0)
		return;

	// Remove Any Indices Found in the Array
	int old_object_count = container.total_object_count;
	int placement_index = 0;
	for (int i = 0; i < old_object_count; i++)
	{
		// Match Was Found
		if (searchObjectIndicies(indices, 0, indices_size, container.object_array[i]->object_index))
		{
			delete container.object_array[i];
			container.total_object_count--;
		}

		// No Match Was Found
		else
		{
			container.object_array[placement_index] = container.object_array[i];
			placement_index++;
		}
	}
}

void Render::Objects::Level::resetObjectPassOver()
{
	// General Objects
	for (uint32_t i = 0; i < container.total_object_count; i++)
		container.object_array[i]->skip_selection = false;

	// Entities
	for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
		(*entity_list.it).skip_selection = false;

	// Physics
	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it = physics_list.begin(); it != physics_list.end(); it++)
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
		sublevels[i].readLevel(container.object_array, index1, physics_list, entity_list);

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

void Render::Objects::Level::incorperatNewObjects(Object::Object** new_objects, int new_objects_size)
{
	// Generate a New Object Container Array That Can Fit New Objects
	int new_container_size = container.total_object_count + new_objects_size;
	Object::Object** new_object_array = new Object::Object*[new_container_size];

	// Copy Current Objects Into New Container Array
	for (int i = 0; i < container.total_object_count; i++)
		new_object_array[i] = container.object_array[i];

	// Copy New Objects Into New Container Array
	for (int i = container.total_object_count, j = 0; i < new_container_size; i++, j++)
		new_object_array[i] = new_objects[j];

	// Delete the Old Object Array
	delete[] container.object_array;

	// Reset the Container
	container = { 0 };

	// Store New Container Array and Size in Container Object
	container.total_object_count = new_container_size;
	container.object_array = new_object_array;

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
	for (int i = 0; i < 9; i++)
		sublevels[i].includeNewActives(new_actives, new_objects_size, this);

	// Delete the Temp Active Array
	delete[] new_actives;

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

void Render::Objects::Level::clearTemps()
{
	// Delete ALL Temp Objects
	for (Object::Object* temp : temp_objects)
		delete temp;
		//delete static_cast<Object::TempObject*>(temp);

	// Clear Temp Object Vector
	temp_objects.clear();
}

glm::mat4 Render::Objects::Level::returnProjectionViewMatrix(uint8_t layer)
{
	projection[layer] = Global::projection;
	return projection[layer] * camera->view;
}

void Render::Objects::Level::storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, MOVE_WITH_PARENT disable_move)
{
	// Get Level Coords of Object
	glm::i16vec2 coords;
	updateLevelPos(position, coords);

	// Store Pointer of Unsaved Level in Selector
	selector.level_of_origin = change_controller->getUnsavedLevel((int)coords.x, (int)coords.y, 0);

	// Remove Object from Unsaved Level
	selector.level_of_origin->createChangePop(selector.highlighted_object, disable_move);

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

std::string Render::Objects::Level::getLevelDataPath()
{
	return level_data_path;
}

std::string Render::Objects::Level::getEditorLevelDataPath()
{
	return editor_level_data_path;
}

void Render::Objects::Level::getSceneInfo(SceneData** data, std::string** name)
{
	*data = &scene_data;
	*name = &scene_name;
}

void Render::Objects::Level::getSublevelSize(glm::vec2& sizes)
{
	sizes.x = scene_data.sublevel_width;
	sizes.y = scene_data.sublevel_height;
}

#endif
