#include "UnsavedLevel.h"
#include "Source/Algorithms/Common/Common.h"
#include "Class/Render/Editor/Selector.h"
#include "Globals.h"

// Verbose Objects
#include "Class/Render/Shape/Shape.h"
#include "Class/Render/Shape/Rectangle.h"
#include "Class/Render/Shape/Trapezoid.h"
#include "Class/Render/Shape/Triangle.h"
#include "Class/Render/Shape/Circle.h"
#include "Class/Render/Shape/Polygon.h"
#include "Class/Object/Collision/Horizontal/Line/FloorMaskLine.h"
#include "Class/Object/Collision/Horizontal/Slant/FloorMaskSlant.h"
#include "Class/Object/Collision/Horizontal/Slope/FloorMaskSlope.h"
#include "Class/Object/Collision/Vertical/Line/LeftMaskLine.h"
#include "Class/Object/Collision/Vertical/Curve/LeftMaskCurve.h"
#include "Class/Object/Collision/Vertical/Line/RightMaskLine.h"
#include "Class/Object/Collision/Vertical/Curve/RightMaskCurve.h"
#include "Class/Object/Collision/Horizontal/Line/CeilingMaskLine.h"
#include "Class/Object/Collision/Horizontal/Slant/CeilingMaskSlant.h"
#include "Class/Object/Collision/Horizontal/Slope/CeilingMaskSlope.h"
#include "Class/Object/Collision/Trigger/TriggerMask.h"
#include "Class/Object/Terrain/Foreground.h"
#include "Class/Object/Terrain/Formerground.h"
#include "Class/Object/Terrain/Background.h"
#include "Class/Object/Terrain/Backdrop.h"

DataClass::Data_Object* Render::Objects::UnsavedLevel::lambdaDataObject(uint8_t object_identifier[3])
{  

//#define ENABLE_LAG2

#ifdef ENABLE_LAG2

	auto readFloors = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::HORIZONTAL_LINE): return new DataClass::Data_FloorMaskHorizontalLine();
		case (Object::Mask::HORIZONTAL_SLANT): return new DataClass::Data_FloorMaskSlant();
		case (Object::Mask::HORIZONTAL_SLOPE): return new DataClass::Data_FloorMaskSlope();
		}
	};

	auto readLeftWalls = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::VERTICAL_LINE): return new DataClass::Data_LeftMaskVerticalLine();
		case (Object::Mask::VERTICAL_CURVE): return new DataClass::Data_LeftMaskCurve();
		}
	};

	auto readRightWalls = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::VERTICAL_LINE): return new DataClass::Data_RightMaskVerticalLine();
		case (Object::Mask::VERTICAL_CURVE): return new DataClass::Data_RightMaskCurve();
		}
	};

	auto readCeilings = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case (Object::Mask::HORIZONTAL_LINE): return new DataClass::Data_CeilingMaskHorizontalLine();
		case (Object::Mask::HORIZONTAL_SLANT): return new DataClass::Data_CeilingMaskSlant();
		case (Object::Mask::HORIZONTAL_SLOPE): return new DataClass::Data_CeilingMaskSlope();
		}
	};

	auto readTriggers = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_TriggerMask();
	};

	auto readMasks = [&object_identifier, &readFloors, &readLeftWalls, &readRightWalls, &readCeilings, &readTriggers]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object* ()> masks[5] = { readFloors, readLeftWalls, readRightWalls, readCeilings, readTriggers };
		return masks[object_identifier[1]]();
	};

	auto readTerrain = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_Terrain(object_identifier[1], object_identifier[2]);
	};

	auto readLights = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[1])
		{
		case (Object::Light::DIRECTIONAL): return new DataClass::Data_Directional();
		case (Object::Light::POINT): return new DataClass::Data_Point();
		case (Object::Light::SPOT): return new DataClass::Data_Spot();
		case (Object::Light::BEAM): return new DataClass::Data_Beam();
		}
	};

	auto readRigid = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_RigidBody(object_identifier[2]);
	};

	auto readSoft = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS): return new DataClass::Data_SpringMass();
		case ((int)Object::Physics::SOFT_BODY_TYPES::WIRE): return new DataClass::Data_Wire();
		}
	};

	auto readHinge = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Physics::HINGES::ANCHOR): return new DataClass::Data_Anchor();
		case ((int)Object::Physics::HINGES::HINGE): return new DataClass::Data_Hinge();
		}
	};

	auto readPhysics = [&object_identifier, &readRigid, &readSoft, &readHinge]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object* ()> physics[3] = { readRigid, readSoft, readHinge };
		return physics[object_identifier[1]];
	};

	auto readEntities = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Entity::ENTITY_NPC): return new DataClass::Data_NPC();
		case ((int)Object::Entity::ENTITY_CONTROLLABLE): return new DataClass::Data_Controllable();
		case ((int)Object::Entity::ENTITY_INTERACTABLE): return new DataClass::Data_Interactable();
		case ((int)Object::Entity::ENTITY_DYNAMIC): return new DataClass::Data_Dynamic();
		}
	};

	std::function<void()> objects[5] = { readMasks, readTerrain, readLights, readPhysics, readEntities };
	return objects(object_identifier[0]);

#else

	return nullptr;

#endif

}

void Render::Objects::UnsavedLevel::constructUnmodifiedDataHelper(LevelInstance& instance)
{
	// Temporary Holder for Object Identifier
	uint8_t object_identifier[3] = { 0 };

	// Size of Object File
	uint32_t object_size = 0;

	// Set Unmodified Flag
	instance.unmodified = true;

	// If Editor File is Empty, Load Empty Headers
	std::filesystem::path temp = editor_path;
	std::error_code ec;
	if ((int)std::filesystem::file_size(temp, ec) == 0)
	{
		return;
	}
	if (ec)
	{
		return;
	}

	// Open Object Data File
	std::ifstream object_file;
	object_file.open(object_path, std::ios::binary);

	// Open Editor Data File
	std::ifstream editor_file;
	editor_file.open(editor_path, std::ios::binary);

	// Get Size of Object File
	object_file.seekg(0, std::ios::end);
	object_size = (uint32_t)object_file.tellg();
	object_file.seekg(0, std::ios::beg);
	editor_file.seekg(0, std::ios::beg);

	// Read Number of Objects in Object File

	// Iterate Through Object File Until All is Read
	//while (!object_file.eof())
	Global::object_index_counter++;
	while (object_file.tellg() < object_size)
	{
		object_file.read((char*)&object_identifier, 3);
		DataClass::Data_Object* object = lambdaDataObject(object_identifier);
		object->readObject(object_file, editor_file);
		instance.data_objects.push_back(object);
		Global::object_index_counter++;
	}
	Global::object_index_counter++;

	// Close Files
	object_file.close();
	editor_file.close();
}

void Render::Objects::UnsavedLevel::buildObjectsHelper(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, LevelInstance& instance)
{
	// Allocate Memory for Active Array
	*active_objects = new Object::Object*[instance.header.number_of_loaded_objects];
	Object::Object** active_array = *active_objects;
	uint16_t active_index = 0;

	std::cout << "lulw\n";
	std::cout << instance.data_objects.size() << " " << level_x << " " << level_y << "\n";
	std::cout << "num objects: " << instance.header.number_of_loaded_objects << " \n";

	// Generate Objects
	for (DataClass::Data_Object* data_object : instance.data_objects)
	{
		switch (data_object->getObjectIdentifier()[0])
		{

		// Generate a Physics Objects
		case (Object::PHYSICS):
		{
			physics.appendStatic((Object::Physics::PhysicsBase*)data_object->generateObject());
		}

		// Generate an Entity
		case (Object::ENTITY):
		{
			entities.appendStatic((Object::Entity::EntityBase*)data_object->generateObject());
		}

		// Generate Normal, Stationary Object
		default:
		{
			std::cout << "creating normal object\n";
			objects[index] = data_object->generateObject();
			index++;
		}

		}
	}
}

void Render::Objects::UnsavedLevel::writeInstance(LevelInstance& instance)
{
	// Array of Objects
	Object::Object** objects = new Object::Object*[instance.header.number_of_loaded_objects];
	uint16_t temp_var = 0;

	// Array of Physics
	Struct::List<Object::Physics::PhysicsBase> physics = Struct::List<Object::Physics::PhysicsBase>();

	// Array of Entities
	Struct::List<Object::Entity::EntityBase> entities = Struct::List<Object::Entity::EntityBase>();

	// Build Objects
	buildObjects(objects, temp_var, physics, entities);

	// Files
	std::ofstream object_file;
	std::ofstream editor_file;

	// Open Files
	object_file.open(object_path, std::ios::binary);
	editor_file.open(editor_path, std::ios::binary);

	// Write Headers Number of Objects

	// Write Data to Files
	for (int i = 0; i < temp_var; i++)
	{
		objects[i]->write(object_file, editor_file);
		delete objects[i];
	}

	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it = physics.beginStatic(); it != physics.endStatic(); it++)
		(*it).write(object_file, editor_file);

	for (Struct::List<Object::Entity::EntityBase>::Iterator it = entities.beginStatic(); it != entities.endStatic(); it++)
		(*it).write(object_file, editor_file);

	// Delete Arrays
	delete[] objects;

	// Close Files
	object_file.close();
	editor_file.close();
}

Render::Objects::UnsavedLevel::LevelInstance* Render::Objects::UnsavedLevel::makeCopyOfInstance(LevelInstance& instance)
{
	// Iterator
	uint16_t iterator = 0;

	// Create a New Instance
	LevelInstance* new_instance_ = new LevelInstance;
	LevelInstance& new_instance = *new_instance_;

	// Set Number of Stack Instances to 1
	new_instance.master_stack_instances = 1;

	// Copy Data Into Arrays
	new_instance.data_objects.resize(instance.data_objects.size());
	for (int i = 0; i < instance.data_objects.size(); i++)
		new_instance.data_objects[i] = instance.data_objects[i];

	// Return Pointer to Instance
	return new_instance_;
}

Shape::Shape* Render::Objects::UnsavedLevel::getShapePointer(Editor::Selector* selector)
{
	Shape::Shape* old_shape = static_cast<DataClass::Data_Shape*>(selector->data_object)->getShape();
	switch (selector->data_object->getObjectIdentifier()[2])
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Rectangle(*static_cast<Shape::Rectangle*>(old_shape)));
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		return static_cast<Shape::Shape*>(new Shape::Trapezoid(*static_cast<Shape::Trapezoid*>(old_shape)));
	}

	// Triangle Data
	case Shape::TRIANGLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Triangle(*static_cast<Shape::Triangle*>(old_shape)));
	}

	// Circle Data
	case Shape::CIRCLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Circle(*static_cast<Shape::Circle*>(old_shape)));
	}

	// Polygon Data
	case Shape::POLYGON:
	{
		return static_cast<Shape::Shape*>(new Shape::Polygon(*static_cast<Shape::Polygon*>(old_shape)));
	}

	}

	return nullptr;
}

Render::Objects::UnsavedLevel::UnsavedLevel()
{
	// Constant Vertex Positions
	const float vertices[] = {
		0.1f,   0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
	};

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(128.0f * level_x, 64.0f * level_y, 0.0f));

	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Store Vertex Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Render::Objects::UnsavedLevel::~UnsavedLevel()
{
	// Delete the Stack
	slave_stack.deleteStack();

	// Delete the Unmodified Data
	slave_stack.deleteInstance(unmodified_data);
}

void Render::Objects::UnsavedLevel::constructUnmodifiedData(int16_t x, int16_t y, uint8_t z)
{
	// Store Coordinates of Level
	level_x = x;
	level_y = y;
	level_version = z;

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(128.0f * level_x, 64.0f * level_y, 0.0f));

	// Generate File Name
	std::string file_name = "";
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(x)));
	file_name.append(",");
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(y)));

	// Get Paths of Level
	object_path = Global::level_data_path + file_name;
	editor_path = Global::editor_level_data_path + file_name;

	// Create Memory for Unmodified Data
	unmodified_data = new LevelInstance();

	// Increment Stack Instance Count
	unmodified_data->master_stack_instances++;

	// Store Memory in First Index in Stack Array
	slave_stack.storeUnmodified(unmodified_data);

	// Read Unmodified Data
	constructUnmodifiedDataHelper(*unmodified_data);
}

void Render::Objects::UnsavedLevel::buildObjects(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities)
{
	// Get Instance From Stack
	//LevelInstance* instance = slave_stack.returnInstance();

	// Construct Objects
	buildObjectsHelper(objects, index, physics, entities, instance_with_changes);
}

void Render::Objects::UnsavedLevel::changeToModified()
{
	float colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::changeToUnmodified()
{
	float colors[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::changeToSaved()
{
	float colors[4] = { 0.0f, 0.9f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::switchInstance(uint8_t selected_instance)
{
	// Set Stack Index
	slave_stack.switchInstance(selected_instance);

	// Selected Unmodified is Equal to Unmodified Bool in Instance
	selected_unmodified = false;
	if (slave_stack.returnInstance()->unmodified)
	{
		selected_unmodified = true;
		changeToUnmodified();
	}
}

void Render::Objects::UnsavedLevel::changeColors(float* color)
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Change Colors
	for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), color);
	}

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::Objects::UnsavedLevel::incrementStackApperance(uint8_t index)
{
	slave_stack.incrementStackInstances(index);
}

void Render::Objects::UnsavedLevel::decrementStackApperance(uint8_t index)
{
	slave_stack.decrementStackInstances(index);
}

Render::Objects::LevelHeader Render::Objects::UnsavedLevel::returnObjectHeader()
{
	// Temp Code to Set Header
	instance_with_changes.header = { 0 };
	instance_with_changes.header.number_of_loaded_objects = instance_with_changes.data_objects.size();
	for (DataClass::Data_Object* dobject : instance_with_changes.data_objects)
	{
		uint8_t* id = dobject->getObjectIdentifier();
		if (id[0] == Object::MASK)
		{
			if (id[1] == Object::Mask::FLOOR)
				instance_with_changes.header.floor_count++;

			else if (id[1] == Object::Mask::LEFT_WALL)
				instance_with_changes.header.left_count++;

			else if (id[1] == Object::Mask::RIGHT_WALL)
				instance_with_changes.header.right_count++;

			else if (id[1] == Object::Mask::CEILING)
				instance_with_changes.header.ceiling_count++;

			else if (id[1] == Object::Mask::TRIGGER)
				instance_with_changes.header.trigger_count++;
		}

		else if (id[0] == Object::TERRAIN)
			instance_with_changes.header.terrain_count++;

		else if (id[0] == Object::LIGHT)
		{
			if (id[1] == Object::Light::DIRECTIONAL)
				instance_with_changes.header.directional_count++;

			else if (id[1] == Object::Light::POINT)
				instance_with_changes.header.point_count++;

			else if (id[1] == Object::Light::SPOT)
				instance_with_changes.header.spot_count++;

			else if (id[1] == Object::Light::BEAM)
				instance_with_changes.header.beam_count++;
		}
	}

	//return slave_stack.returnInstance()->header;
	return instance_with_changes.header;
}

void Render::Objects::UnsavedLevel::drawVisualizer()
{
	// If Stack Size is Less Than or Equal to 1, Don't Draw Visualizer
	if (slave_stack.isEmpty() && !saved)
		return;

	// Bind Model Matrix
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	
	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 8);
	glBindVertexArray(0);
}

void Render::Objects::UnsavedLevel::write(bool& save)
{
	// If Unmodified Data is Selected, Don't Do Anything
	if (selected_unmodified) {}

	// Else, Write Currently Selected Instance 
	else 
	{
		save = true;
		writeInstance(*slave_stack.returnInstance());
	}

	// Delete Level
	delete this;
}

uint8_t Render::Objects::UnsavedLevel::revertChanges(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities)
{
	// Build Unmodified Data
	buildObjects(objects, index, physics, entities);

	// Set Selected Unmodified Flag
	selected_unmodified = true;

	// Increment Number of Times Unmodified Appears in Stack
	unmodified_data->master_stack_instances++;

	// Return Index of Unmodified Data in Stack Once it Appended
	return slave_stack.appendInstance(unmodified_data);
}

void Render::Objects::UnsavedLevel::generateChangeList()
{
	if (!making_changes)
		current_change_list = new Changes;
	current_change_list->change_count++;
	making_changes = true;
}

bool Render::Objects::UnsavedLevel::finalizeChangeList()
{
	if (making_changes)
	{
		making_changes = false;

		// Add Code to Store Change List Somewhere

		// Return True to Indicate Unsaved Level Has Been Changed
		// Causes Unsaved Level to be Added to Master Stack
		return true;
	}

	return false;
}

void Render::Objects::UnsavedLevel::createChangeAppend(Editor::Selector* selector)
{
	// Generate Change List if Not Generated Already
	generateChangeList();

	// Generate the New Change
	Change* change = new Change;
	change->change_type = CHANGE_TYPES::ADD;

	// Retrieve the Data Object from the Selector
	DataClass::Data_Object* data_object = selector->data_object;

	// Store Data Object in Change
	change->data = data_object;

	// Execute Change
	instance_with_changes.data_objects.push_back(data_object);
	instance_with_changes.header.number_of_loaded_objects++;
	std::cout << instance_with_changes.data_objects.size() << "   " << level_x << " " << level_y << "  yooo\n";

	// Store Change in Change List
	current_change_list->changes.push_back(change);
	current_change_list->change_count++;
}

void Render::Objects::UnsavedLevel::createChangePop(uint32_t object_index)
{
	// Generate Change List if Not Generated Already
	generateChangeList();

	// Generate the New Change
	Change* change = new Change;
	change->change_type = CHANGE_TYPES::REMOVE;

	// Find Data Object in the List of Data Objects, Remove it, and Store it in Change
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (instance_with_changes.data_objects.at(i)->getObjectIndex() == object_index)
		{
			change->data = instance_with_changes.data_objects.at(i);
			instance_with_changes.data_objects.at(i) = nullptr;
			break;
		}
	}

	// Execute Change
	instance_with_changes.header.number_of_loaded_objects--;

	// Store Change in Change List
	current_change_list->changes.push_back(change);
	current_change_list->change_count++;
}

void Render::Objects::UnsavedLevel::resetChangeList()
{
	// Only Execute if Changes Have Been Made to Unsaved Level
	if (making_changes)
	{
		// Undo All Changes in the Current Change List

		// Delete the Current Change List
		delete current_change_list;

		// Indicate Changes Have Been Reset
		making_changes = false;
	}
}

Render::Objects::UnsavedLevel::SlaveStack::SlaveStack()
{
	// Create Initial Allocation for Stack
	stack_array = new LevelInstance*[array_size];

	// Increment Size By 1 for Implementation of Unmodified Data
	stack_size++;
	head++;
}

uint8_t Render::Objects::UnsavedLevel::SlaveStack::appendInstance(LevelInstance* instance)
{
	// Increment Stack Index
	stack_index++;
	stack_size++;

	// If Current Index is Equal to the Size of Array, Reallocate Array
	if (stack_index == array_size)
	{
		// Create New Array With Size 1.5 X Size of Original Array
		array_size = (int)(array_size * 1.5f);
		LevelInstance** new_array = new LevelInstance*[array_size];

		// Move Data From Old Array to New Array
		for (uint8_t i = 0; i < stack_index; i++)
			new_array[i] = std::move(stack_array[i]);

		// Delete Old Array
		delete[] stack_array;

		// Store Pointer to New Array in Old Array Location
		stack_array = new_array;
	}

	// Insert Instance Into Array
	stack_array[stack_index] = instance;

	// Return Index of New Array in Stack
	return stack_index;
}

Render::Objects::UnsavedLevel::LevelInstance* Render::Objects::UnsavedLevel::SlaveStack::returnInstance()
{
	return stack_array[stack_index];
}

bool Render::Objects::UnsavedLevel::SlaveStack::isEmpty()
{
	return (stack_size == 0 || stack_size == 1);
}

void Render::Objects::UnsavedLevel::SlaveStack::storeUnmodified(LevelInstance* unmodified_data)
{
	stack_array[0] = unmodified_data;
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteInstance(LevelInstance* instance)
{
	// Delete Data Arrays
	for (DataClass::Data_Object* data_object : instance->data_objects)
	{
		delete  data_object;
	}

	// Delete Actual Object
	delete instance;
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteStack()
{
	// Iterate Through Array and Delete Instances
	for (int i = tail; i < head; i++)
	{
		// Don't Delete Unmodified
		if (stack_array[i]->unmodified)
			continue;

		// Delete Instance
		deleteInstance(stack_array[i]);
	}
}

uint8_t Render::Objects::UnsavedLevel::SlaveStack::returnCurrentIndex()
{
	return stack_index;
}

void Render::Objects::UnsavedLevel::SlaveStack::incrementStackInstances(uint8_t index)
{
	stack_array[index]->master_stack_instances++;
}

void Render::Objects::UnsavedLevel::SlaveStack::decrementStackInstances(uint8_t index)
{
	stack_array[index]->master_stack_instances--;

	// If Number of Instances is 0, Delete Instance
	if (stack_array[index]->master_stack_instances == 0)
		deleteInstance(stack_array[index]);
}

void Render::Objects::UnsavedLevel::SlaveStack::switchInstance(uint8_t instance_index)
{
	stack_index = instance_index;
}

