#include "UnsavedComplex.h"
#include "UnsavedGroup.h"
#include "Render/Struct/DataClasses.h"
#include "Globals.h"
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Vertices/Visualizer/Visualizer.h"
#include "Source/Algorithms/Common/Common.h"
#include "Render/Editor/Notification.h"

void Render::Objects::UnsavedComplex::constructUnmodifiedDataHelper(ObjectsInstance& instance)
{
	// Temporary Holder for Object Identifier
	uint8_t object_identifier[4] = { 0 };

	// Size of Object File
	uint32_t object_size = 0;

	// If Editor File is Empty, Load Empty Headers
	std::filesystem::path temp = file_path + ".edt";
	std::error_code ec;
	if ((int)std::filesystem::file_size(temp, ec) == 0)
		return;
	if (ec)
		return;

	// Open Object Data File
	std::ifstream object_file;
	object_file.open(file_path + ".dat", std::ios::binary);

	// Open Editor Data File
	std::ifstream editor_file;
	editor_file.open(file_path + ".edt", std::ios::binary);

	// Get Size of Object File
	object_file.seekg(0, std::ios::end);
	object_size = (uint32_t)object_file.tellg();
	object_file.seekg(0, std::ios::beg);
	editor_file.seekg(0, std::ios::beg);

	// Read Current Number of Objects from File
	uint16_t objects_size = 0;
	object_file.read((char*)&objects_size, sizeof(uint16_t));

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
			// Else, Add to Instances and Set Complex Parent
			instance.data_objects.push_back(object);
			object->setParent(&complex_parent);
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

	// Generate Unmodified Visualizer
	updateVisualizer();
}

void Render::Objects::UnsavedComplex::addWhileTraversing(DataClass::Data_Object* data_object, MOVE_WITH_PARENT move_with_parent)
{
	// Add Object To Change List
	instance_with_changes.data_objects.push_back(data_object);

	// If Object is a Parent, Update Group
	UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		static_cast<UnsavedGroup*>(data_object->getGroup())->setParent(data_object, MOVE_WITH_PARENT::MOVE_ENABLED);

	// Determine if Object Should Move With Parent
	data_object->disableMoveWithParent(move_with_parent);
}

void Render::Objects::UnsavedComplex::removeWhileTraversing(DataClass::Data_Object* data_object)
{
	// Remove Object From Change List
	for (int i = 0; i < instance_with_changes.data_objects.size(); i++)
	{
		if (data_object == instance_with_changes.data_objects.at(i))
			instance_with_changes.data_objects.erase(instance_with_changes.data_objects.begin() + i);
	}
}

void Render::Objects::UnsavedComplex::updatePostTraverse()
{

}

void Render::Objects::UnsavedComplex::removeChainListInstance()
{
	// No Need to Delete Instance Since DataObjects Don't Belong to This Object
	// Only Need to Decrement Stack Size
	slave_stack.removeRecentInstanceWithoutDeletion();
}

bool Render::Objects::UnsavedComplex::testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child)
{
	// If File Name is Null, Prevent Objects from Being Added
	if (Source::Algorithms::Common::getFileName(file_path, false)._Equal("NULL"))
	{
		std::string message = "INVALID OPERATION:\n\nCannot Assign an Object to a Group\nWith the \"NULL\" FileName.\n\nPlease Change the FileName in the\nSelected Object's Editor Window\n";
		notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
		return false;
	}

	// If Attempting to Create a Circular Group, Stop That from Happening
	if (!preventCircularGroups(parent, test_child))
		return false;

	return true;
}

void Render::Objects::UnsavedComplex::updateVisualizer()
{
	// Temp Vertices for Testing
	float vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.0f, 5.0f, 5.0f, vertices);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Store Vertex Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Render::Objects::UnsavedComplex::UnsavedComplex()
{
	// Generate the Parent Object
	complex_parent.setGroup(this);

	// Generate Vertex Objects for Outline
	
	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Default Vertices for Object
	float vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.0f, 5.0f, 5.0f, vertices);

	// Allocate Buffer Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

void Render::Objects::UnsavedComplex::write(bool& save)
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
		object_file.open(file_path + ".dat", std::ios::binary);

		// Open Editor Data File
		std::ofstream editor_file;
		editor_file.open(file_path + ".edt", std::ios::binary);

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

void Render::Objects::UnsavedComplex::constructUnmodifiedData(std::string file_path_)
{
	// Store File Path
	file_path = file_path_;

	// Read Unmodified Data
	constructUnmodifiedDataHelper(instance_with_changes);
}

void Render::Objects::UnsavedComplex::recursiveSetGroupLayer(int8_t layer)
{
	// Update All Children and Their Children
	for (DataClass::Data_Object* data_object : instance_with_changes.data_objects)
	{
		data_object->setGroupLayer(layer);
		if (data_object->getGroup() != nullptr)
			data_object->getGroup()->recursiveSetGroupLayer(0); // All Children Will Have a Layer of 0
	}
}

std::string& Render::Objects::UnsavedComplex::getFilePath()
{
	return file_path;
}

Render::Objects::UNSAVED_COLLECTIONS Render::Objects::UnsavedComplex::getCollectionType()
{
	return UNSAVED_COLLECTIONS::COMPLEX;
}

DataClass::Data_Object* Render::Objects::UnsavedComplex::getComplexParent()
{
	return &complex_parent;
}

void Render::Objects::UnsavedComplex::addComplexInstance(Object::Object* new_instance)
{
	instance_vector.push_back(new_instance);
}

void Render::Objects::UnsavedComplex::removeComplexInstance(Object::Object* pointer_to_be_removed)
{
	for (std::vector<Object::Object*>::iterator it = instance_vector.begin(); it != instance_vector.end(); it++)
	{
		if (*it == pointer_to_be_removed)
		{
			instance_vector.erase(it);
			break;
		}
	}
}

bool Render::Objects::UnsavedComplex::testForInstance(uint32_t test_index)
{
	for (Object::Object* instance : instance_vector)
	{
		if (instance->object_index == test_index)
			return true;
	}

	return false;
}

bool Render::Objects::UnsavedComplex::testForSelectedInstance()
{
	for (Object::Object* instance : instance_vector)
	{
		if (instance->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
			return true;
	}

	return false;
}

glm::vec2* Render::Objects::UnsavedComplex::getSelectedPosition()
{
	// Since Only One Object Should be Selected, Return Selected Position of First Temp Object
	for (Object::Object* object : instance_vector)
	{
		if (object->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
			return static_cast<Object::TempObject*>(object)->pointerToSelectedPosition();
	}

	// Should Never Reach Here, Return NULLPTR Anyways
	return nullptr;
}

std::vector<Object::Object*>& Render::Objects::UnsavedComplex::getInstances()
{
	return instance_vector;
}

void Render::Objects::UnsavedComplex::drawVisualizer()
{
	// Memory for Temp Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Bind Vertex Array
	glBindVertexArray(VAO);

	// Iterate Through Each Instance and Draw Visualizer at That Position
	for (std::vector<Object::Object*>::iterator it = instance_vector.begin(); it != instance_vector.end(); it++)
	{
		// If Object is a Temp Object, Skip
		if ((*it)->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
			continue;

		// Get Position of Instance
		glm::vec2& position = *(*it)->pointerToPosition();

		// Generate Model Matrix for Instance
		model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));

		// Send Model Matrix to Shaders
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Draw Vertex Object
		glDrawArrays(GL_LINES, 0, 8);
	}

	// Unbind Vertex Array
	glBindVertexArray(0);
}

void Render::Objects::UnsavedComplex::drawSelected(int object_vertex_count, int mode, GLuint model_loc, glm::vec3 delta_pos, Object::Object* instance_to_skip)
{
	// Memory for Temp Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Iterate Through Each Instance and Draw Visualizer at That Position
	for (std::vector<Object::Object*>::iterator it = instance_vector.begin(); it != instance_vector.end(); it++)
	{
		// If Object is a Temp Object, Skip
		if ((*it)->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
			continue;

		// Test if Instance Should be Skipped
		if (*it != instance_to_skip)
		{
			// Get Position of Instance
			glm::vec2& position = *(*it)->pointerToPosition();

			// Generate Model Matrix for Instance
			model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f) + delta_pos);

			// Send Model Matrix to Shaders
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

			// Draw Vertex Object
			glDrawArrays(mode, 0, object_vertex_count);
		}
	}
}

void Render::Objects::UnsavedComplex::drawSelectedConnection(DataClass::Data_Object* selected_object, glm::vec2 position_offset)
{
	// Iterate Through Each Instance and Draw the Connection to Selected Object
	for (std::vector<Object::Object*>::iterator it = instance_vector.begin(); it != instance_vector.end(); it++)
	{
		// Get Position of Instance
		glm::vec2& position = *(*it)->pointerToPosition();

		// Draw Connection With Parent
		glm::vec4& parent_connection_color = selected_object->returnLineColor(selected_object->getParent()->getGroupLayer());
		if (selected_object->getParent()->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
			Vertices::Visualizer::visualizeLine(position, selected_object->getPosition() - position_offset + position, 0.5f, parent_connection_color);
		else
			Vertices::Visualizer::visualizeLine(selected_object->getParent()->getPosition() + position, selected_object->getPosition() - position_offset + position, 0.5f, parent_connection_color);

		// Draw Connection Point of Object
		Vertices::Visualizer::visualizePoint(selected_object->getPosition() - position_offset + position, 1.0f, selected_object->returnLineColor(selected_object->getGroupLayer()));

		// Draw Children Connection With the Current Offset
		if (selected_object->getGroup() != nullptr)
		{
			std::vector<DataClass::Data_Object*>& children = selected_object->getGroup()->getChildren();
			for (std::vector<DataClass::Data_Object*>::iterator it2 = children.begin(); it2 != children.end(); it2++)
			{
				// Draw Connection With Child
				Vertices::Visualizer::visualizeLine((*it2)->getPosition() + position, selected_object->getPosition() - position_offset + position, 0.5f, selected_object->returnLineColor(selected_object->getGroupLayer()));

				// Draw Connection Point of Child
				Vertices::Visualizer::visualizePoint((*it2)->getPosition() + position, 1.0f, selected_object->returnLineColor(selected_object->getGroupLayer() + 1));

				// Recursively Draw Connections With Other Children
				(*it2)->drawGroupVisualizer(position);
			}
		}
	}
}

void Render::Objects::UnsavedComplex::drawSelectedConnectionParentOnly(DataClass::Data_Object* selected_object, glm::vec2 position_offset)
{
	// Iterate Through Each Instance and Draw the Connection to Selected Object
	for (std::vector<Object::Object*>::iterator it = instance_vector.begin(); it != instance_vector.end(); it++)
	{
		// Get Position of Instance
		glm::vec2& position = *(*it)->pointerToPosition();

		// Draw Connection With Parent
		glm::vec4& parent_connection_color = selected_object->returnLineColor(selected_object->getParent()->getGroupLayer());
		if (selected_object->getParent()->getGroup()->getCollectionType() == UNSAVED_COLLECTIONS::COMPLEX)
			Vertices::Visualizer::visualizeLine(position, selected_object->getPosition() - position_offset + position, 0.5f, parent_connection_color);
		else
			Vertices::Visualizer::visualizeLine(selected_object->getParent()->getPosition() + position, selected_object->getPosition() - position_offset + position, 0.5f, parent_connection_color);

		// Draw Connection Point of Object
		Vertices::Visualizer::visualizePoint(selected_object->getPosition() - position_offset + position, 1.0f, selected_object->returnLineColor(selected_object->getParent()->getGroupLayer() + 1));
	}
}

// Get the File Path
std::string& DataClass::Data_Complex::getFilePath()
{
	return file_path;
}

void DataClass::Data_Complex::setGroup(Render::Objects::UnsavedComplex* new_group)
{
	// Store the New Group Object
	group_object = reinterpret_cast<Render::Objects::UnsavedCollection*>(new_group);

	// Force Get the New File Name
	file_name = Source::Algorithms::Common::getFileName(file_path, false);
}

Object::Object* DataClass::Data_ComplexParent::genObject(glm::vec2& offset)
{
	return nullptr;
}

void DataClass::Data_ComplexParent::writeObjectData(std::ofstream& object_file)
{
}

void DataClass::Data_ComplexParent::readObjectData(std::ifstream& object_file)
{
}

int& DataClass::Data_ComplexParent::getScript()
{
	return null_value;
}

glm::vec2& DataClass::Data_ComplexParent::getPosition()
{
	return position_offset;
}

void DataClass::Data_ComplexParent::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
}

void DataClass::Data_ComplexParent::info(Editor::ObjectInfo& object_info)
{
}

DataClass::Data_Object* DataClass::Data_ComplexParent::makeCopy()
{
	// Singleton, Copies Should Never be Made
	return this;
}

DataClass::Data_ComplexParent::Data_ComplexParent()
{
	// Set Group Layer to -1 For Same Reason as Group Object
	group_layer = -1;
}

void DataClass::Data_ComplexParent::setGroup(Render::Objects::UnsavedComplex* complex_group)
{
	// Store Complex Group Object
	group_object = reinterpret_cast<Render::Objects::UnsavedCollection*>(complex_group);
}

void DataClass::Data_ComplexParent::setPositionOffset(glm::vec2 new_offset)
{
	position_offset = new_offset;
}

glm::vec2 DataClass::Data_ComplexParent::getPositionOffset()
{
	return position_offset;
}

bool DataClass::Data_ComplexParent::isActive()
{
	return active;
}

void DataClass::Data_ComplexParent::setActive()
{
	active = true;
}

void DataClass::Data_ComplexParent::setInactive()
{
	active = false;
}

void DataClass::Data_ComplexParent::storeRootParent(Object::Object* parent)
{
	root_parent = parent;
}

void DataClass::Data_ComplexParent::setGroupLayer(int8_t new_layer)
{
	group_layer = -1;
}

void DataClass::Data_ComplexParent::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Literally Nothing is Used
	*position1 = &Global::dummy_vec2;
	index1 = -1;
	position23Null(index2, index3, position2, position3);
}

Object::Object* DataClass::Data_ComplexParent::getRootParent()
{
	return root_parent;
}

int DataClass::Data_ComplexParent::null_value = 0;
