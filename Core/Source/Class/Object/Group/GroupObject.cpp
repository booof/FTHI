#include "GroupObject.h"
#include "Render/Struct/DataClasses.h"
#include "Render/Editor/ObjectInfo.h"
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Render/Objects/ChangeController.h"
#include "Render/Objects/UnsavedComplex.h"
#include "Globals.h"

void Object::Group::GroupObject::initializeVisualizer()
{
	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Generate Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.2f, 2.0f, 2.0f, glm::vec4(0.0f, 0.8f, 0.6f, 0.9f), vertices);

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

Object::Group::GroupObject::GroupObject(GroupData& data_, std::string file_name_, Render::Objects::UnsavedComplex* complex_object)
{
	// Store Group Data
	data = data_;

	// Store File Name and Path to File
	file_name = file_name_;
	path = Global::project_resources_path + "/Models/SoftBodies/" + file_name;

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
}

void Object::Group::GroupObject::drawObject()
{
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

Object::Object* DataClass::Data_GroupObject::genObject()
{
	return new Object::Group::GroupObject(data, file_name, static_cast<Render::Objects::UnsavedComplex*>(group_object));
}

void DataClass::Data_GroupObject::writeObjectData(std::ofstream& object_file)
{
	data.file_name_size = file_name.size();
	object_file.write((char*)&data, sizeof(Object::Group::GroupData));
	object_file.write(file_name.c_str(), sizeof(data.file_name_size));
}

void DataClass::Data_GroupObject::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Group::GroupData));
	file_name.resize(data.file_name_size);
	object_file.read(&file_name[0], sizeof(data.file_name_size));
	file_name = "NULL";
	file_path = Global::project_resources_path + "\\Models\\Groups\\" + file_name;

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
	data.position.x += deltaX;
	data.position.y += deltaY;
}

DataClass::Data_GroupObject::Data_GroupObject()
{
	// Set Object Identifier
	object_identifier[0] = Object::GROUP;
	object_identifier[1] = 0;
	object_identifier[2] = 0;
	object_identifier[3] = 0;
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
	data.position = position;
	data.script = 0;
	data.file_name_size = 4;
	file_name = "NULL";
	file_path = Global::project_resources_path + "\\Models\\Groups\\NULL";
	group_object = reinterpret_cast<Render::Objects::UnsavedCollection*>(change_controller->getUnsavedComplex(file_path));
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

std::string& DataClass::Data_GroupObject::getFilePath()
{
	return file_path;
}
