#include "UnsavedLevel.h"
#include "Source/Algorithms/Common/Common.h"
#include "Class/Render/Editor/Selector.h"
#include "ChangeController.h"
#include "Globals.h"
#include "Render/Struct/DataClasses.h"
#include "UnsavedGroup.h"
#include "Level.h"

void Render::Objects::UnsavedLevel::constructUnmodifiedDataHelper(ObjectsInstance& instance)
{
	// Temporary Holder for Object Identifier
	uint8_t object_identifier[4] = { 0 };

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

	// Read Current Number of Objects from File
	uint16_t objects_size = 0;
	object_file.read((char*)&objects_size, sizeof(uint16_t));

	// Objects That Don't Belong
	struct InvalidObject {
		DataClass::Data_Object* data_object;
		int16_t object_x;
		int16_t object_y;
	}; 

	// An Array of Objects That Don't Belong In This Level
	std::vector<InvalidObject> invalid_location;

	// Level Pos of Object
	glm::i16vec2 object_level_pos;

	// Stack Used to Add Children
	AddChildrenStack add_child_stack;

	// Iterate Through Object File Until All is Read
	Global::object_index_counter++;
	while (object_file.tellg() < object_size)
	{
		// Read Object
		object_file.read((char*)&object_identifier, 4);
		DataClass::Data_Object* object = lambdaDataObject(object_identifier);
		object->readObject(object_file, editor_file);

		// Attempt to Add Child to Current Parent
		if (!add_child_stack.addChild(object))
		{
			// If Read Object's Position Does Not Match the Current Level,
			// Find a Way to Transfer Between Unsaved Levels
			if (main_container->getContainerType() == CONTAINER_TYPES::LEVEL)
				static_cast<Render::Objects::Level*>(main_container)->wrapObjectPos(object->getPosition());
			main_container->updateLevelPos(object->getPosition(), object_level_pos);
			if (object_level_pos.x != level_x || object_level_pos.y != level_y)
				invalid_location.push_back(InvalidObject(object, object_level_pos.x, object_level_pos.y));
			else
				instance.data_objects.push_back(object);
		}

		// If This Object is a Parent, Append to Add Child Stack
		if (object_identifier[3])
		{
			if (!add_child_stack.addObject(object, object_identifier[3]))
				throw "Cringe";
		}

		// Increment Object Index Counter
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

Shape::Shape* Render::Objects::UnsavedLevel::getShapePointer(Editor::Selector* selector)
{
	Shape::Shape* old_shape = static_cast<DataClass::Data_Shape*>(selector->data_objects.at(0))->getShape();
	switch (selector->data_objects.at(0)->getObjectIdentifier()[2])
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

void Render::Objects::UnsavedLevel::addWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset)
{
	instance_with_changes.data_objects.push_back(data_object);
	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		UnsavedGroup::enqueueLevelParent(data_object, offset);
}

void Render::Objects::UnsavedLevel::removeWhileTraversing(DataClass::Data_Object* data_object, glm::vec2 offset)
{
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (data_object == instance_with_changes.data_objects.at(i))
			instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
	}
}

void Render::Objects::UnsavedLevel::updatePostTraverse()
{

}

void Render::Objects::UnsavedLevel::removeChainListInstance()
{
	// Unsaved Levels Only Need to Remove Instance and Delete Data Objects
	slave_stack.removeRecentInstance();
}

bool Render::Objects::UnsavedLevel::testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child)
{
	// Don't Allow Connector Objects and Softbody Objects in Level

	return true;
}

Render::Objects::UnsavedLevel::UnsavedLevel(glm::vec2& sizes, Container* container)
{
	// Store Level Object
	main_container = container;

	// Decrement Sizes by 0.1
	sizes.x -= 0.1f;
	sizes.y -= 0.1f;

	// Constant Vertex Positions
	const float vertices[] = {
		0.1f,   0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		sizes.x, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		sizes.x, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		sizes.x, sizes.y, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		sizes.x, sizes.y, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   sizes.y, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   sizes.y, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
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

void Render::Objects::UnsavedLevel::constructUnmodifiedDataLevel(int16_t x, int16_t y, uint8_t z, float width, float height, std::string level_data_path, std::string editor_level_data_path)
{
	// Store Coordinates of Level
	level_x = x;
	level_y = y;
	level_version = z;

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(width *level_x, height * level_y, 0.0f));

	// Generate File Name
	std::string file_name = "";
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(x)));
	file_name.append(",");
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(y)));

	// Get Paths of Level
	object_path = level_data_path + file_name;
	editor_path = editor_level_data_path + file_name;

	// Read Unmodified Data
	constructUnmodifiedDataHelper(instance_with_changes);
}

void Render::Objects::UnsavedLevel::contructUnmodifiedDataGUI(std::string gui_path)
{
	// Coordinates of Level are All 0
	level_x = 0;
	level_y = 0;
	level_version = 0;

	// Model Matrix is Unused
	model = glm::mat4(1.0f);

	// Get Paths for Data Files
	object_path = gui_path + "Object.dat";
	editor_path = gui_path + "Editor.dat";

	// Read Unmodified Data
	constructUnmodifiedDataHelper(instance_with_changes);
}

void Render::Objects::UnsavedLevel::buildObjects(uint16_t& index, glm::vec2& object_offset)
{
	// Allocate Memory for Active Array
	if (instance_with_changes.data_objects.size())
		*active_objects = new Object::Active[instance_with_changes.number_of_loaded_objects];
	Object::Active* active_array = *active_objects;
	active_size = instance_with_changes.number_of_loaded_objects;
	uint16_t active_index = 0;

	// Generate the Objects in the Container
	change_controller->getCurrentContainer()->buildObjectsGenerator(instance_with_changes.data_objects, index, nullptr, object_offset, active_index, *this);
}

uint16_t Render::Objects::UnsavedLevel::returnObjectHeader()
{
	instance_with_changes.number_of_loaded_objects = 0;
	for (DataClass::Data_Object* dobject : instance_with_changes.data_objects)
	{
		returnObjectHeaderHelper(dobject);
		uint8_t* id = dobject->getObjectIdentifier();
		if (id[0] != Object::PHYSICS && id[0] != Object::ENTITY)
			instance_with_changes.number_of_loaded_objects++;
	}
	return instance_with_changes.number_of_loaded_objects;
}

void Render::Objects::UnsavedLevel::returnObjectHeaderHelper(DataClass::Data_Object* dobject)
{
	if (dobject->getGroup() != nullptr)
	{
		// Get Children Vector and Increment Children Count
		std::vector<DataClass::Data_Object*>& children_vector = dobject->getGroup()->getChildren();
		instance_with_changes.number_of_loaded_objects += children_vector.size();

		// Add Direct Children of Object
		for (DataClass::Data_Object* dobject2 : children_vector)
			returnObjectHeaderHelper(dobject2);
	}
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

void Render::Objects::UnsavedLevel::updateModelMatrix()
{
	// GUIs Don't Utilize Level Borders, Return Early
	if (main_container->getContainerType() == CONTAINER_TYPES::GUI)
		return;

	// Get Level Object
	Render::Objects::Level* main_level = static_cast<Render::Objects::Level*>(main_container);

	// Get the Object Position of Level
	glm::vec2 object_pos = main_level->getObjectPos(glm::i16vec2(level_x, level_y));

	// Unwrap the Positions
	main_level->unwrapObjectPos(object_pos);

	// Update Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(object_pos.x, object_pos.y, 0.0f));
}

int16_t Render::Objects::UnsavedLevel::reallocateActivesList(int16_t delta_size)
{
	//return active_size;

	//std::cout << " level: " << level_x << " " << level_y << "   size: " << active_size << "   delta: " << delta_size << "\n";

	// Get Active Array
	Object::Active* active_array = *active_objects;

	//for (int i = 0; i < active_size; i++)
	//	std::cout << "   active " << i << ": " << active_array[i].active << " " << active_array[i].alive << " " << active_array[i].object_ptr << "\n";

	// Store Backup of Number of Objects
	int16_t old_active_count = active_size;

	// Update the New Size of the Actives Array
	active_size += delta_size;
	instance_with_changes.number_of_loaded_objects = active_size;

	// Allocate New Array for Active Objects
	Object::Active* new_active_array = new Object::Active[active_size];

	// Copy Old Objects Into New Array, Ignore Inactive Objects
	int last_active_index = 0;
	for (int i = 0; i < old_active_count; i++)
	{
		// Add Active Object Into New Array
		if (active_array[i].active) {
			new_active_array[last_active_index] = active_array[i];
			new_active_array[last_active_index].object_ptr->active_ptr = &new_active_array[last_active_index];
			last_active_index++;
		}
	}

	// Add in Temp Null Data for New Objects, if They Exist
	for (int i = last_active_index; i < active_size; i++)
		new_active_array[i] = Object::Active{ 0 };

	// Delete the Old Active Array
	delete[] active_array;

	// Store New Active Array
	*active_objects = new_active_array;

	// Return the Index to Start Adding Objects Into
	//return old_active_count;
	return last_active_index;
}

void Render::Objects::UnsavedLevel::addToActivesList(Object::Object* new_object, uint16_t& active_index)
{
	Object::Active* active_array = *active_objects;
	active_array[active_index] = Object::Active(true, true, glm::i16vec2(level_x, level_y), new_object);
	new_object->active_ptr = &active_array[active_index];
	active_index++;

	if (level_x == 0 && level_y == 0)
		std::cout << "adding to actives: " << active_index << " " << active_size << "\n";
}

void Render::Objects::UnsavedLevel::write(bool& save)
{
	// If Unmodified Data is Selected, Don't Do Anything
	if (selected_unmodified && false) {}

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

		// Write Current Number of Objects to File
		uint16_t objects_size = (uint16_t)instance_with_changes.data_objects.size();
		object_file.write((char*)&objects_size, sizeof(uint16_t));

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

void Render::Objects::UnsavedLevel::transferObject(DataClass::Data_Object* data_object)
{
	instance_with_changes.data_objects.push_back(data_object);
}

void Render::Objects::UnsavedLevel::returnMasks(DataClass::Data_Object*** masks, int& masks_size, uint8_t type, DataClass::Data_Object* match)
{
	// Initial Iteration: Count the Number of Masks Objects
	masks_size = 0;
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		if (data_object->getObjectIdentifier()[0] == Object::MASK && data_object->getObjectIdentifier()[1] == type && data_object != match)
			masks_size++;
	}

	// Allocate Memory for Array
	(*masks) = new DataClass::Data_Object*[masks_size];
	DataClass::Data_Object** masks_array = *masks;

	// Second Iteration: Send Data Object Into Array
	int masks_index = 0;
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		if (data_object->getObjectIdentifier()[0] == Object::MASK && data_object->getObjectIdentifier()[1] == type && data_object != match)
		{
			masks_array[masks_index] = data_object;
			masks_index++;
		}
	}
}

void Render::Objects::UnsavedLevel::returnTerrainObjects(DataClass::Data_Terrain*** terrain_objects, int& terrain_objects_size, uint8_t layer, DataClass::Data_Object* match)
{
	// Initial Iteration: Count the Number of Terrain Objects
	terrain_objects_size = 0;
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		if (data_object->getObjectIdentifier()[0] == Object::TERRAIN && data_object != match && static_cast<DataClass::Data_Terrain*>(data_object)->getLayer() == layer)
			terrain_objects_size++;
	}

	// Allocate Memory for Array
	(*terrain_objects) = new DataClass::Data_Terrain * [terrain_objects_size];
	DataClass::Data_Terrain** terrain_objects_array = *terrain_objects;

	// Second Iteration: Send Data Object Into Array
	int terrain_objects_index = 0;
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		if (data_object->getObjectIdentifier()[0] == Object::TERRAIN && data_object != match && static_cast<DataClass::Data_Terrain*>(data_object)->getLayer() == layer)
		{
			terrain_objects_array[terrain_objects_index] = static_cast<DataClass::Data_Terrain*>(data_object);
			terrain_objects_index++;
		}
	}
}

