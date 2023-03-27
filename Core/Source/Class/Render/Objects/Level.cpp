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

void Render::Objects::Level::updateLevelPos(glm::vec2 position, glm::vec2& level)
{
	level.x = floor(position.x / 128);
	level.y = floor(position.y / 64);
}

int8_t Render::Objects::Level::index_from_level(glm::vec2 level_coords)
{
	if (abs(level_coords.x - level_position.x) > 1 || abs(level_coords.y - level_position.y) > 1) { return -1; }
	return ((uint8_t)4 + (uint8_t)floor(level_coords.x - level_position.x) - (uint8_t)((level_coords.y - level_position.y) * 3));
}

glm::vec2 Render::Objects::Level::level_from_index(int8_t index)
{
	// If Index is Outside Level Range, Retrun 0,0
	if (index > 8 || index < 0)
		return glm::vec2(0, 0);

	glm::vec2 coords;
	coords.x = level_position.x + (index % 3) - 1;
	coords.y = level_position.y + int8_t(index < 3) - int8_t(index > 5);
	return coords;
}

void Render::Objects::Level::testReload()
{
	// Get Level Coordinates of Camera
	glm::vec2 new_level;
	updateLevelPos(camera->Position, new_level);

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
			if (!sublevels[i]->initialized)
				sublevels[i]->readLevel(container.object_array, index, physics_list, entity_list);
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

void Render::Objects::Level::reloadLevels(glm::vec2& level_old, glm::vec2& level_new, bool reload_all)
{
	// Pointer to New Level
	SubLevel* new_level;

	// Reset Temp Index Holder
	temp_index_holder = 0;

	// Store Old Object Counts
	uint32_t total_object_count_old = container.total_object_count;

	// Variable for New Object Count
	uint32_t total_object_count_new = 0;

	// Test if Handler Should do a Complete Map Reset
	if ((abs(level_old.x - level_new.x) > 1 || abs(level_old.y - level_new.y) > 1) || (level_old == level_new) || reload_all)
	{
		// Deconstruct Level Objects
		for (int i = 0; i < 9; i++)
		{
			//sublevels[i]->~SubLevel();
			delete sublevels[i];
		}

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

				new_level = new SubLevel((int)level_new.x + level_x, (int)level_new.y + level_y);
				new_level->addHeader(total_object_count_new);
				sublevels[iterater] = new_level;
				iterater++;
			}
		}

		// Allocate Memory
		reallocateAll(initialized, total_object_count_new);
		initialized = true;

#ifdef SHOW_LEVEL_LOADING
		std::cout << "reloaded all\n";
#endif

		return;
	}

	// Load and Unload Selected Levels
	
	// Move to the Left
	if (level_old.x - level_new.x > 0)
	{
		// Deconstruct Old Levels
		for (int i = 2; i < 9; i += 3)
		{
			sublevels[i]->subtractHeader(container.total_object_count);
			delete sublevels[i];
		}

		// Shift Loaded Levels to the Right
		int next_level_location = -1;
		for (int i = 8; i > -1; i--)
		{
			// Load New Levels from the Left
			if (!(i % 3))
			{
				new_level = new SubLevel((int)level_new.x - 1, (int)level_new.y + next_level_location);
				new_level->addHeader(container.total_object_count);
				sublevels[i] = new_level;
				next_level_location++;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i - 1];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved west\n";
#endif

	}

	// Move to the Right
	else if (level_old.x - level_new.x < 0)
	{
		// Deconstruct Old Levels
		for (int i = 0; i < 9; i += 3)
		{
			sublevels[i]->subtractHeader(container.total_object_count);
			delete sublevels[i];
		}

		// Shift Loaded Levels to the Left
		int next_level_location = 1;
		for (int i = 0; i < 9; i++)
		{
			// Load New Levels from the Right
			if ((i % 3) == 2)
			{
				new_level = new SubLevel((int)level_new.x + 1, (int)level_new.y + next_level_location);
				new_level->addHeader(container.total_object_count);
				sublevels[i] = new_level;
				next_level_location--;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i + 1];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved east\n";
#endif

	}

	// Move to the North
	if (level_old.y - level_new.y < 0)
	{
		// Deconstruct Old Levels
		for (int i = 6; i < 9; i++)
		{
			sublevels[i]->subtractHeader(container.total_object_count);
			delete sublevels[i];
		}

		// Shift Loaded Levels to the South
		int next_level_location = 1;
		for (int i = 8; i > -1; i--)
		{
			// Load New Levels from the North
			if (i < 3)
			{
				new_level = new SubLevel((int)level_new.x + next_level_location, (int)level_new.y + 1);
				new_level->addHeader(container.total_object_count);
				sublevels[i] = new_level;
				next_level_location--;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i - 3];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved north\n";
#endif

	}

	// Move to the South
	else if (level_old.y - level_new.y > 0)
	{
		// Deconstruct Old Levels
		for (int i = 0; i < 3; i++)
		{
			sublevels[i]->subtractHeader(container.total_object_count);
			delete sublevels[i];
		}

		// Shift Loaded Levels to the North
		int next_level_location = -1;
		for (int i = 0; i < 9; i++)
		{
			// Load New Levels from the South
			if (i > 5)
			{
				new_level = new SubLevel((int)level_new.x + next_level_location, (int)level_new.y - 1);
				new_level->addHeader(container.total_object_count);
				sublevels[i] = new_level;
				next_level_location++;
			}

			// Shift Levels
			else
				sublevels[i] = sublevels[i + 3];
		}

#ifdef SHOW_LEVEL_LOADING
		std::cout << "moved south\n";
#endif

	}

#ifdef SHOW_LEVEL_LOADING
	std::cout << "\n";
#endif

	// Reallocate Memory of Pointers
	reallocatePostReload(total_object_count_old);
}

void Render::Objects::Level::segregateObjects()
{
	// IDEA: Segregation of Objects Will be Changed to Occour in a Heap Instead of Here All at Once.
	// This will reduce overall complexity. In addition, All Objects Will Remain in a Single List, But 
	// Seperated/Segregated by their Object Identifier. First Index in the Object Identifier will be Used
	// to Sort. If any tie, go to the Second then Third Identifier. Ties Between Unique Objects Have no
	// Importance. There will be a Data Structure That Contains the Start/End Indicies for Each Important
	// Object Group i.e. Terrain, Lights, Masks

	// EDIT: Sorting Will Instead be Done Through Quick Sort

	// Sort the Objects Through Quick Sort
	Algorithms::Sorting::quickIdentifierSort(container.object_array, container.total_object_count);

	// Need to Have Structure With Array Indexable Pointers and Array Lengths to Segregate Objects
	Object::Object** object_array_pointer = container.object_array;
	Object::Object*** current_array_pointer = reinterpret_cast<Object::Object***>(&container.floor_start);
	(*current_array_pointer) = object_array_pointer;
	uint16_t* current_array_size_pointer = &container.floor_size;
	uint8_t container_index = 0;

	// The Map for Container Index to Container Storage Type
	uint8_t storage_map[10] = { Object::FLOOR_COUNT, Object::LEFT_COUNT, Object::RIGHT_COUNT, Object::CEILING_COUNT, Object::TRIGGER_COUNT, Object::TERRAIN_COUNT, Object::DIRECTIONAL_COUNT, Object::POINT_COUNT, Object::SPOT_COUNT, Object::BEAM_COUNT };

	// Segregate Objects
	Object::Object* current_object = nullptr;
	Object::Object** object_array_end_pointer = object_array_pointer + container.total_object_count;
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
			if (container.object_array[i]->active)
			{
				//new_list[temp_index_holder.total_object_count] = objects[i];
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

	// Swap Arrays
	container.object_array = new_list;
}

void Render::Objects::Level::reallocateAll(bool del, uint32_t size)
{
	// Test if Memory Has Previously Been Allocated
	if (del)
	{
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
	Vertices::Buffer::clearObjectVAO(VAO, VBO, vertices);
	Vertices::Buffer::clearObjectDataBuffer(Global::InstanceBuffer, instances);

	// Bind Buffer Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

	// Remember to Set up Material and View Pos in Object Frag Shader
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

Render::Objects::Level::Level(std::string save_path, std::string core_path)
{
	// Generate Terrain Buffer Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Buffer
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

	// Initialize Change Controller
	change_controller->storeLevelPointer(this);

	// Construct Projection Matrices
	for (int i = 0; i < 6; i++)
	{
		projection[i] = Global::projection;
	}

	// Create Camera
	camera = new Camera::Camera(0, 0, 0);

	// Create Textures
	for (int i = 0; i < 6; i++)
	{
		//glGenTextures(1, terrain_textures[i]->texture.texture);
		//glGenTextures(1, terrain_textures[i]->texture.material);
		//glGenTextures(1, terrain_textures[i]->texture.mapping);
	}

	// Reset Level Objects
	container = { 0 };
	temp_index_holder = 0;

	// Initialize SubLevels
	level_position = glm::vec2(0, 0);
	for (int i = 0; i < 9; i++)
	{
		glm::vec2 coords = level_from_index(i);
		sublevels[i] = new SubLevel((int)coords.x, (int)coords.y);
		sublevels[i]->addHeader(container.total_object_count);
	}

	// Allocate Memory
	reallocateAll(false, container.total_object_count);
	initialized = true;

	// Read SubLevels
	uint16_t index1 = 0;
	uint16_t index2 = 0;
	uint16_t index3 = 0;
	for (int i = 0; i < 9; i++)
		sublevels[i]->readLevel(container.object_array, index1, physics_list, entity_list);

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
	glBindVertexArray(VAO);

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
	glBindVertexArray(VAO);
	//glUniformMatrix4fv(Global::matrixLocObject, 1, GL_FALSE, glm::value_ptr(matrix));
	glDrawArrays(GL_TRIANGLES, number_of_vertices[5], number_of_vertices[6]);
	glBindVertexArray(0);

	// Draw Visualizers for Groups
	for (int i = 0; i < container.total_object_count; i++)
		container.object_array[i]->drawGroupVisualizer();
}

#ifdef EDITOR

void Render::Objects::Level::drawVisualizers()
{
	// Bind Color Shader
	Global::colorShaderStatic.Use();

	// Draw Level Visualizers
	for (int i = 0; i < 9; i++)
		sublevels[i]->drawVisualizer();

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
}

void Render::Objects::Level::testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// If Mouse Moved, Reset Object Pass Over Flag
	if (Global::cursor_Move)
		resetObjectPassOver();

	// Set Highlighting to False
	selector.highlighting = false;
	object_info.active = false;

	// Test Selector On Formerground Lighting
	if (testSelectorLights(5, selector, object_info)) { return; }

	// Test Selector On Formerground Effects
	if (testSelectorEffects(5, selector, object_info)) { return; }

	// Test Selector On Formerground Terrain
	if (testSelectorTerrain(5, selector, object_info)) { return; }

	// Test Selector On Entities
	if (testSelectorEntity(selector, object_info)) { return; }

	// Test Selector On Physics Objects
	if (testSelectorPhysics(selector, object_info)) { return; }

	// Test Selector On Collision Masks
	if (testSelectorMasks(selector, object_info)) { return; }

	// Test Selector On Foreground Lighting
	if (testSelectorLights(4, selector, object_info)) { return; }

	// Test Selector On Foreground Terrain
	if (testSelectorTerrain(4, selector, object_info)) { return; }

	// Test Selector On Backgrond1 Lighting
	if (testSelectorLights(3, selector, object_info)) { return; }

	// Test Selector On Background1 Terrain
	if (testSelectorTerrain(3, selector, object_info)) { return; }

	// Test Selector On Background2 Lighting
	if (testSelectorLights(2, selector, object_info)) { return; }

	// Test Selector On Background2 Terrain
	if (testSelectorTerrain(2, selector, object_info)) { return; }

	// Test Selector On Background3 Lighting
	if (testSelectorLights(1, selector, object_info)) { return; }

	// Test Selector On Background3 Terrain
	if (testSelectorTerrain(1, selector, object_info)) { return; }

	// Test Selector On Backdrop Lighting
	if (testSelectorLights(0, selector, object_info)) { return; }

	// Test Selector On Backdrop Terrain
	if (testSelectorTerrain(0, selector, object_info)) { return; }

	// If We Ever Get To This Point, Reset the Pass Over Flag for All Objects
	resetObjectPassOver();
}

bool Render::Objects::Level::testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	for (int i = terrain_seperators[index]; i < terrain_seperators[index + 1]; i++)
	{
		uint8_t returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.terrain_start), container.terrain_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				constructTerrain();
			return true;
		}
	}

	return false;
}

bool Render::Objects::Level::testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
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
			return true;
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
			return true;
		}
	}

	// Spot Light
	for (int i = spot_seperators[index]; i < spot_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.point_size), container.spot_size, selector, i, object_info);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return true;
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
			return true;
		}
	}

	return false;
}

bool Render::Objects::Level::testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	return false;
}

bool Render::Objects::Level::testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Test Selector on Base Objects
	if (testSelectorOnList(physics_list, selector, object_info))
		return true;

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
						selector.object_index = 0;
						//selector.data_objects.push_back(object_pointer->data_object);
						selector.highlighted_object = object.data_springs[i];
						selector.temp_connection_pos_left = node_pos_1;
						selector.temp_connection_pos_right = node_pos_2;
						selector.activateHighlighter();
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

						return true;
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
					selector.object_index = 0;
					selector.highlighted_object = object.data_nodes[i];
					selector.activateHighlighter();
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

					return true;
				}
			}
		}
	}

	return false;
}

bool Render::Objects::Level::testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	return testSelectorOnList(entity_list, selector, object_info);
}

bool Render::Objects::Level::testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info)
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

template<class Type>
uint8_t Render::Objects::Level::testSelectorOnObject(Type*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info)
{
	// Get Reference of List and Object
	Type** temp_list = *object_list;
	Type& object = *(temp_list[index]);

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
		if (selector.object_index != object.object_index)
			temp_list[index]->select(selector, object_info);
		selector.highlighting = true;

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

				// Object Already Belongs to a Parent
				if (object.data_object->getParent() != nullptr)
				{
					// Remove Child from Current Group
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object);
					object.data_object->getParent()->getObjectIdentifier()[3]--;

					// If Attempting to Add To Its Current Parent, Remove Object From Being a Child
					if (object.data_object->getParent()->getObjectIndex() == selected_object->getObjectIndex())
						change_controller->handleSingleSelectorReturn(object.data_object->makeCopySelected(selector), true);

					// Else, Swap Parents
					else
						selector.addChildToOnlyOne(object.data_object);
				}

				// If Attempting to Add a Parent to its Child, Prevent That From Happening
				// For Now, This Will Cause a Deselection, Might Do Something Else in the Future
				else if (selected_parent != nullptr && selected_parent->getObjectIndex() == object.data_object->getObjectIndex())
				{
					// Remove Child from Current Group
					object.data_object->getObjectIdentifier()[3]--;

					// Set Parent of Selected Object to Nothing
					selected_object->setParent(nullptr);

					// Set Group Layer to 0
					selected_object->setGroupLayer(0);

					// Recursively Set Group Layer
					UnsavedGroup* data_group = selected_object->getGroup();
					if (data_group != nullptr)
						data_group->recursiveSetGroupLayer(1);
				}

				// Else, Add Object as a New Child
				else
				{
					// Add Child to Parent
					selector.addChildToOnlyOne(object.data_object);

					// Store Level of Origin
					storeLevelOfOrigin(selector, object.returnPosition());
				}

			}

			// Else, Add to Selector
			else
			{
				// Activate Selector
				selector.active = true;

				// Store Level of Origin if Originated From Level
				if (object.parent == nullptr)
					storeLevelOfOrigin(selector, object.returnPosition());

				// If Originated From Group, Remove from Group
				else
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object);

				// Remove Object From Global Objects List
				removeMarkedFromList(temp_list[index]);

				// Reset Object Info
				object_info.clearAll();

				// Make a Copy of the Data Class
				selector.unadded_data_objects.push_back(selector.highlighted_object->makeCopySelected(selector));
			}

			return 2;
		}

		return 1;
	}

	return 0;
}

template<class Type>
bool Render::Objects::Level::testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info)
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
			if (selector.object_index != object.object_index)
				object.select(selector, object_info);
			selector.highlighting = true;

			// If Left Click, Select Object
			if (Global::LeftClick)
			{
				// Activate Selector
				selector.active = true;

				// Store Level of Origin
				storeLevelOfOrigin(selector, object.returnPosition());

				// Remove Object From List
				object_list.removeObject(object_list.it);

				// Reset Object Info
				object_info.clearAll();

				// Make a Copy of the Data Class
				selector.unadded_data_objects.push_back(selector.highlighted_object->makeCopySelected(selector));

				return true;
			}

			return true;
		}
	}

	return false;
}

void Render::Objects::Level::removeMarkedFromList(Object::Object* marked_object)
{
	// Retrieve the Total Object Count and Array
	uint16_t new_object_count = container.total_object_count - 1;
	Object::Object** old_object_array = container.object_array;

	// Reset Container
	container = { 0 };

	// Store Values Back in Container
	container.total_object_count = new_object_count;
	container.object_array = old_object_array;

	// Create New Array of Objects
	Object::Object** new_list = new Object::Object*[container.total_object_count];

	// Coppy Objects From Old List to New List, Skipping Marked Object
	short new_list_index = 0;
	for (uint32_t i = 0; i < container.total_object_count + 1; i++)
	{
		// Test if Object is Marked
		//if (container.object_array[i]->marked)
		if (container.object_array[i] == marked_object)
		{
			// Delete Object
			delete container.object_array[i];
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
	glm::vec2 new_level;
	updateLevelPos(camera->Position, new_level);

	// Reload Levels
	reloadLevels(level_position, new_level, true);

	// Read Levels
	uint16_t index1 = 0;
	uint16_t index2 = 0;
	uint16_t index3 = 0;
	for (int i = 0; i < 9; i++)
		sublevels[i]->readLevel(container.object_array, index1, physics_list, entity_list);

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

glm::mat4 Render::Objects::Level::returnProjectionViewMatrix(uint8_t layer)
{
	projection[layer] = Global::projection;
	return projection[layer] * camera->view;
}

void Render::Objects::Level::storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position)
{
	// Get Level Coords of Object
	glm::vec2 coords;
	updateLevelPos(position, coords);

	// Store Pointer of Unsaved Level in Selector
	selector.level_of_origin = change_controller->getUnsavedLevel((int)coords.x, (int)coords.y, 0);

	// Remove Object from Unsaved Level
	selector.level_of_origin->createChangePop(selector.highlighted_object);

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

#endif
