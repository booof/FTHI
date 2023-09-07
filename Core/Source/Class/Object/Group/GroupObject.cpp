#include "GroupObject.h"
#include "Render/Struct/DataClasses.h"
#include "Render/Editor/ObjectInfo.h"
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Render/Objects/ChangeController.h"
#include "Render/Objects/UnsavedComplex.h"
#include "Source/Algorithms/Common/Common.h"
#include "Globals.h"

void Object::Group::GroupObject::initializeVisualizer()
{
	// Generate Model Matrix
	updateModelMatrix();

	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Generate Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 2.0f, 2.0f, glm::vec4(0.0f, 0.8f, 0.6f, 0.9f), vertices);

	// Store Vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Bind Color Vertices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Object::Group::GroupObject::updateObject()
{

}

glm::vec2* Object::Group::GroupObject::pointerToPosition()
{
	return &data.position;
}

Object::Group::GroupObject::GroupObject(GroupData& data_, std::string file_path_, Render::Objects::UnsavedComplex* complex_object, glm::vec2& offset)
{
	// Store Group Data
	data = data_;
	data.position += offset;

	// Store File Name and Path to File
	path = file_path_;
	file_name = Source::Algorithms::Common::getFileName(path, false);

	// For Editing, Add This Instance to Complex Object Instances
	complex_object->addComplexInstance(this);

	// Store Storage Type
	storage_type = GROUP_COUNT;

	// Initiailize Visualizer
	initializeVisualizer();

#ifdef EDITOR

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		data.script = 0;
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif
}

Object::Group::GroupObject::~GroupObject()
{
	// Remove This Object From Complex Instances
	static_cast<Render::Objects::UnsavedComplex*>(data_object->getGroup())->removeComplexInstance(this);
}

bool Object::Group::GroupObject::testMouseCollisions(float x, float y)
{
	return x < data.position.x + 1.0f &&
		x > data.position.x - 1.0f &&
		y > data.position.y - 1.0f &&
		y < data.position.y + 1.0f;
}

glm::vec2 Object::Group::GroupObject::returnPosition()
{
	return data.position;
}

Render::Objects::UnsavedComplex* Object::Group::GroupObject::getComplexGroup()
{
	return static_cast<Render::Objects::UnsavedComplex*>(group_object);
}

void Object::Group::GroupObject::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position += glm::vec2(deltaX, deltaY);
	updateModelMatrix();
}

void Object::Group::GroupObject::drawObject()
{
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Object::Group::GroupObject::updateModelMatrix()
{
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
}

Object::Object* DataClass::Data_GroupObject::genObject(glm::vec2& offset)
{
	return new Object::Group::GroupObject(data, file_path, static_cast<Render::Objects::UnsavedComplex*>(group_object), offset);
}

void DataClass::Data_GroupObject::writeObjectData(std::ofstream& object_file)
{
	data.file_path_size = file_path.size();
	object_file.write((char*)&data, sizeof(Object::Group::GroupData));
	object_file.write(file_path.c_str(), data.file_path_size);
}

void DataClass::Data_GroupObject::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Group::GroupData));
	file_path.resize(data.file_path_size);
	object_file.read(&file_path[0], data.file_path_size);
	file_name = Source::Algorithms::Common::getFileName(file_path, false);
	group_layer = -1;

	// Get Unsaved Complex Object
	group_object = reinterpret_cast<Render::Objects::UnsavedCollection*>(change_controller->getUnsavedComplex(file_path));
}

int& DataClass::Data_GroupObject::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_GroupObject::getPosition()
{
	return data.position;
}

void DataClass::Data_GroupObject::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	// Update Position
	data.position.x += deltaX;
	data.position.y += deltaY;

	// Update Model Matrix of All Children
	if (update_real)
	{
		for (Object::Object* instance : *object_pointers)
			instance->updateSelectedPosition(deltaX, deltaY);
	}

	// Don't Use The Helper Function. Instead, Use the Complex Alternative
	for (DataClass::Data_Object* child : group_object->getChildren())
	{
		for (Object::Object* instance : child->getObjects())
		{
			if (instance->parent->object_index == object_index)
				instance->updateSelectedComplexPosition(deltaX, deltaY);
		}
	}
}

DataClass::Data_GroupObject::Data_GroupObject()
{
	// Set Object Identifier
	object_identifier[0] = Object::GROUP;
	object_identifier[1] = 0;
	object_identifier[2] = 0;
	object_identifier[3] = 0;

	// Set Group Layer to -1
	// Complex Objects Are Not Considered Part of a Group
	group_layer = -1;
}

void DataClass::Data_GroupObject::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("GroupObject", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addTextValue("File: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &file_name, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_GroupObject::makeCopy()
{
	return new Data_GroupObject(*this);
}

Object::Group::GroupData& DataClass::Data_GroupObject::getGroupData()
{
	return data;
}

void DataClass::Data_GroupObject::generateInitialData(glm::vec2& position)
{
	file_name = "NULL";
	file_path = Global::project_resources_path + "\\Models\\Groups\\NULL";
	data.position = position;
	data.script = 0;
	data.file_path_size = sizeof(file_path);
	group_object = reinterpret_cast<Render::Objects::UnsavedCollection*>(change_controller->getUnsavedComplex(file_path));
	group_layer = -1;
}

void DataClass::Data_GroupObject::writeObject(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object
	object_file.write((char*)object_identifier, 4);
	writeObjectData(object_file);
	writeEditorData(editor_file);

	// Children Will Not be Written Here, Will Instead
	// Be Written In Write Function in Change Controller
	// Alongside the Writing of the Levels
}

void DataClass::Data_GroupObject::setGroupLayer(int8_t new_layer)
{
	group_layer = -1;
}

void DataClass::Data_GroupObject::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}
