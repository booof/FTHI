#include "UnsavedLevel.h"
#include "Source/Algorithms/Common/Common.h"
#include "Class/Render/Editor/Selector.h"
#include "ChangeController.h"
#include "Globals.h"
#include "Render/Struct/DataClasses.h"

DataClass::Data_Object* Render::Objects::UnsavedLevel::lambdaDataObject(uint8_t object_identifier[3])
{  

#define ENABLE_LAG2

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
		return nullptr;
	};

	auto readTriggers = [&object_identifier]()->DataClass::Data_Object* {
		return new DataClass::Data_TriggerMask();
	};

	auto readMasks = [&object_identifier, &readFloors, &readLeftWalls, &readRightWalls, &readCeilings, &readTriggers]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object*()> masks[5] = { readFloors, readLeftWalls, readRightWalls, readCeilings, readTriggers };
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
		return nullptr;
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
		return nullptr;
	};

	auto readHinge = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[2])
		{
		case ((int)Object::Physics::HINGES::ANCHOR): return new DataClass::Data_Anchor();
		case ((int)Object::Physics::HINGES::HINGE): return new DataClass::Data_Hinge();
		}
		return nullptr;
	};

	auto readPhysics = [&object_identifier, &readRigid, &readSoft, &readHinge]()->DataClass::Data_Object* {
		std::function<DataClass::Data_Object*()> physics[3] = { readRigid, readSoft, readHinge };
		return physics[object_identifier[1]]();
	};

	auto readEntities = [&object_identifier]()->DataClass::Data_Object* {
		switch (object_identifier[1])
		{
		case ((int)Object::Entity::ENTITY_NPC): return new DataClass::Data_NPC();
		case ((int)Object::Entity::ENTITY_CONTROLLABLE): return new DataClass::Data_Controllable();
		case ((int)Object::Entity::ENTITY_INTERACTABLE): return new DataClass::Data_Interactable();
		case ((int)Object::Entity::ENTITY_DYNAMIC): return new DataClass::Data_Dynamic();
		}
		return nullptr;
	};

	std::function<DataClass::Data_Object*()> objects[5] = {readMasks, readTerrain, readLights, readPhysics, readEntities};
	return objects[object_identifier[0]]();

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

	// Objects That Don't Belong
	struct InvalidObject {
		DataClass::Data_Object* data_object;
		int16_t object_x;
		int16_t object_y;
	}; 

	// An Array of Objects That Don't Belong In This Level
	std::vector<InvalidObject> invalid_location;

	// Level Pos of Object
	int16_t object_x = 0;
	int16_t object_y = 0;

	// Iterate Through Object File Until All is Read
	Global::object_index_counter++;
	while (object_file.tellg() < object_size)
	{
		object_file.read((char*)&object_identifier, 3);
		DataClass::Data_Object* object = lambdaDataObject(object_identifier);
		object->readObject(object_file, editor_file);

		// If Read Object's Position Does Not Match the Current Level,
		// Find a Way to Transfer Between Unsaved Levels
		object_x = floor(object->getPosition().x / 128.0f);
		object_y = floor(object->getPosition().y / 64.0f);
		if (object_x != level_x || object_y != level_y)
			invalid_location.push_back(InvalidObject(object, object_x, object_y));
		else
			instance.data_objects.push_back(object);

		Global::object_index_counter++;
	}
	Global::object_index_counter++;

	// Close Files
	object_file.close();
	editor_file.close();

	// Deal With Invalid Objects
	for (InvalidObject& object : invalid_location)
		change_controller->transferObject(object.data_object, object.object_x, object.object_y, 0);
}

void Render::Objects::UnsavedLevel::buildObjectsHelper(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, LevelInstance& instance)
{
	// Allocate Memory for Active Array
	*active_objects = new Object::Object*[instance.header.number_of_loaded_objects];
	Object::Object** active_array = *active_objects;
	uint16_t active_index = 0;

	// Generate Objects
	for (DataClass::Data_Object* data_object : instance.data_objects)
	{
		// Generate Object and Attach Data Object
		Object::Object* new_object = data_object->generateObject();
		new_object->data_object = data_object;

		switch (data_object->getObjectIdentifier()[0])
		{

		// Generate a Physics Objects
		case (Object::PHYSICS):
		{
			physics.appendStatic((Object::Physics::PhysicsBase*)new_object);
			break;
		}

		// Generate an Entity
		case (Object::ENTITY):
		{
			entities.appendStatic((Object::Entity::EntityBase*)new_object);
			break;
		}

		// Generate Normal, Stationary Object
		default:
		{
			objects[index] = new_object;
			index++;
			active_array[active_index] = new_object;
			active_index++;
		}

		}
	}
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

	// Read Unmodified Data
	constructUnmodifiedDataHelper(instance_with_changes);
}

void Render::Objects::UnsavedLevel::buildObjects(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities)
{
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

void Render::Objects::UnsavedLevel::moveForwardsThroughChanges(Changes* changes)
{
	for (Change* change : changes->changes)
	{
		// If this is an Add Change, Add Change to Current Data Object List
		if (change->change_type == ADD)
		{
			instance_with_changes.data_objects.push_back(change->data);
		}

		// If this is a Remove Change Remove Change from Current Data Object List
		else
		{
			for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
			{
				if (change->data == instance_with_changes.data_objects.at(i))
					instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
			}
		}
	}
}

void Render::Objects::UnsavedLevel::moveBackwardsThroughChanges(Changes* changes)
{
	for (Change* change : changes->changes)
	{
		// If this is a Remove Change, Add Change to Current Data Object List
		if (change->change_type == REMOVE)
		{
			instance_with_changes.data_objects.push_back(change->data);
		}

		// If this is an Add Change Remove Change from Current Data Object List
		else
		{
			for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
			{
				if (change->data == instance_with_changes.data_objects.at(i))
					instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
			}
		}
	}
}

Render::Objects::LevelHeader Render::Objects::UnsavedLevel::returnObjectHeader()
{
	// Temp Code to Set Header
	instance_with_changes.header = { 0 };
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

		if (id[0] != Object::PHYSICS && id[0] != Object::ENTITY)
			instance_with_changes.header.number_of_loaded_objects++;
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
		// Let the Change Controller Know Level was Saved
		save = true;

		// Open Object Data File
		std::ofstream object_file;
		object_file.open(object_path, std::ios::binary);

		// Open Editor Data File
		std::ofstream editor_file;
		editor_file.open(editor_path, std::ios::binary);

		// Write All Current Data Objects to Files
		for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
			data_object->writeObject(object_file, editor_file);

		// Close Files
		object_file.close();
		editor_file.close();

		// Level Has Been Saved
		selected_unmodified = true;
		changeToSaved();
	}
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
		// Disabling this Flag Lets the Engine Know That New
		// Changes Should be Made
		making_changes = false;

		// Add Code to Store Change List Somewhere
		slave_stack.appendInstance(current_change_list);

		// Return True to Indicate Unsaved Level Has Been Changed
		// Causes Unsaved Level to be Added to Master Stack
		return true;
	}

	return false;
}

void Render::Objects::UnsavedLevel::transferObject(DataClass::Data_Object* data_object)
{
	instance_with_changes.data_objects.push_back(data_object);
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

	// Store Change in Change List
	current_change_list->changes.push_back(change);

	// Level Has Been Modified
	selected_unmodified = false;
	changeToModified();
}

void Render::Objects::UnsavedLevel::createChangePop(DataClass::Data_Object* data_object_to_remove)
{
	// Generate Change List if Not Generated Already
	generateChangeList();

	// Generate the New Change
	Change* change = new Change;
	change->change_type = CHANGE_TYPES::REMOVE;

	// Find Data Object in the List of Data Objects, Remove it, and Store it in Change
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (instance_with_changes.data_objects.at(i) == data_object_to_remove)
		{
			change->data = instance_with_changes.data_objects.at(i);
			instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
			break;
		}
	}

	// Execute Change
	instance_with_changes.header.number_of_loaded_objects--;

	// Store Change in Change List
	current_change_list->changes.push_back(change);

	// Level Has Been Modified
	selected_unmodified = false;
	changeToModified();
}

void Render::Objects::UnsavedLevel::resetChangeList()
{
	// Only Execute if Changes Have Been Made to Unsaved Level
	if (making_changes)
	{
		// Undo All Changes in the Current Change List
		moveBackwardsThroughChanges(current_change_list);

		// Delete the Current Change List
		delete current_change_list;

		// Indicate Changes Have Been Reset
		making_changes = false;
	}
}

void Render::Objects::UnsavedLevel::traverseChangeList(bool backward)
{
	// Move Backwards Through Change List
	if (backward)
	{
		moveBackwardsThroughChanges(slave_stack.returnInstance());
		if (slave_stack.moveBackward())
			changeToUnmodified();
	}

	// Move Forwards Through Change List
	else
	{
		slave_stack.moveForward();
		moveForwardsThroughChanges(slave_stack.returnInstance());
		changeToModified();
	}
}

void Render::Objects::UnsavedLevel::removeChainListInstance()
{
	slave_stack.removeRecentInstance();
}

Render::Objects::UnsavedLevel::SlaveStack::SlaveStack()
{
	// Create Initial Allocation for Stack
	stack_array = new Changes*[array_size];

	// Increment Size By 1 for Implementation of Unmodified Data
	stack_size++;
	head++;
}

uint8_t Render::Objects::UnsavedLevel::SlaveStack::appendInstance(Changes* instance)
{
	// Increment Stack Index
	stack_index++;
	stack_size++;

	// If Current Index is Equal to the Size of Array, Reallocate Array
	if (stack_index == array_size)
	{
		// Create New Array With Size 1.5 X Size of Original Array
		array_size = (int)(array_size * 1.5f);
		Changes** new_array = new Changes*[array_size];

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

Render::Objects::UnsavedLevel::Changes* Render::Objects::UnsavedLevel::SlaveStack::returnInstance()
{
	return stack_array[stack_index];
}

bool Render::Objects::UnsavedLevel::SlaveStack::isEmpty()
{
	return (stack_size == 0 || stack_size == 1);
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteInstance(Changes* instance)
{
	// Delete Each Change and Respective Data Object in Change Array
	for (Change* change : instance->changes)
	{
		// Data Classes may Only be Deleted When Deleting an Add Operation. This is because 
		// A Data Class can Only Appear Twice, Once for Each Operation, and the Add Will
		// ALLWAYS Happen First. This is Because an Add Operation is Only Completed When an
		// Object is Created From the Editor Window, or a Copy is Made When Selecting an Object,
		// Which Triggers the Removal of the Old Object and the Adding of the New Object
		if (change->change_type == ADD)
			delete change->data;

		// Every Change is Unique, Safe to Delete at Any Point
		delete change;
	}

	// Delete Actual Object
	delete instance;
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteStack()
{
	// Iterate Through Array and Delete Instances
	for (int i = tail; i < head; i++)
	{
		// Delete Instance
		deleteInstance(stack_array[i]);
	}
}

void Render::Objects::UnsavedLevel::SlaveStack::removeRecentInstance()
{
	deleteInstance(stack_array[stack_size - 1]);
	stack_size--;
}

void Render::Objects::UnsavedLevel::SlaveStack::moveForward()
{
	stack_index++;
}

bool Render::Objects::UnsavedLevel::SlaveStack::moveBackward()
{
	stack_index--;
	return stack_index == 0;
}

