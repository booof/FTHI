#include "TerrainBase.h"
#include "Render/Struct/DataClasses.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Constants.h"
#include "Globals.h"

#include "Backdrop.h"
#include "Background.h"
#include "Foreground.h"
#include "Formerground.h"

// Initialize Terrain
Object::Terrain::TerrainBase::TerrainBase(Shape::Shape* shape_, ObjectData data_)
{
	// Store Information
	shape = shape_;
	data = data_;
	//data.zpos = -60.0f;

	// Store Storage Type
	storage_type = TERRAIN_COUNT;

	// Store Z-Position for Easy Access
	zpos = data.zpos;

	// Initialize Shape
	shape->initializeShape(number_of_vertices);

#ifdef EDITOR

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
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

void Object::Terrain::TerrainBase::updateObject()
{
}

glm::vec2* Object::Terrain::TerrainBase::pointerToPosition()
{
	return &data.position;
}

bool Object::Terrain::TerrainBase::testMouseCollisions(float x, float y)
{
	return shape->testMouseCollisions(x, y, data.position.x, data.position.y);
}

void Object::Terrain::TerrainBase::initializeTerrain(int& offset_, int& instance_, int& instance_index_)
{
	static glm::mat4 model;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Store Offset and Instance
	offset = offset_;
	instance = instance_;

	// Assign Vertices
	shape->initializeVertices(data, offset_, instance_index_);

	//data.zpos = -1.0f;

	// Assign Instance
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, instance, 64, glm::value_ptr(model));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 64, 16, glm::value_ptr(data.colors));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 80, 12, glm::value_ptr(data.normals));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 92, 4, &data.texture_name);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 96, 4, &data.material_name);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 100, 4, &data.zpos);

	// Increment Instance
	instance_ += Constant::INSTANCE_SIZE;
	instance_index_++;
}

#ifdef EDITOR

glm::vec2 Object::Terrain::TerrainBase::returnPosition()
{
	return data.position;
}

Shape::Shape* Object::Terrain::TerrainBase::returnShapePointer()
{
	return shape;
}

Object::ObjectData& Object::Terrain::TerrainBase::returnObjectData()
{
	return data;
}

Shape::Shape* Object::Terrain::TerrainBase::returnShapeData()
{
	return shape;
}

void Object::Terrain::TerrainBase::updateModel()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::InstanceBuffer);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, instance, 64, glm::value_ptr(model));
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

#endif

Object::Object* DataClass::Data_Terrain::genObject()
{
	switch (object_identifier[1])
	{
	case Object::Terrain::BACKDROP: return new Object::Terrain::Backdrop(shape, data); break;
	case Object::Terrain::BACKGROUND_3:
	case Object::Terrain::BACKGROUND_2:
	case Object::Terrain::BACKGROUND_1: return new Object::Terrain::Background(shape, data, layer); break;
	case Object::Terrain::FOREGROUND: return new Object::Terrain::Foreground(shape, data); break;
	default: return new Object::Terrain::Formerground(shape, data);
	}
}

void DataClass::Data_Terrain::writeObjectData(std::ofstream& object_file)
{
	shape->writeShape(object_file);
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	if (testIfBackground())
		object_file.write((char*)&layer, sizeof(uint8_t));
}

void DataClass::Data_Terrain::readObjectData(std::ifstream& object_file)
{
	shape = shapes[object_identifier[2]](object_file);
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	if (testIfBackground())
		object_file.read((char*)&layer, sizeof(uint8_t));
}

bool DataClass::Data_Terrain::testIfBackground()
{
	return object_identifier[1] == Object::Terrain::BACKGROUND_1 ||
		object_identifier[1] == Object::Terrain::BACKGROUND_2 ||
		object_identifier[1] == Object::Terrain::BACKGROUND_3;
}

DataClass::Data_Terrain::Data_Terrain(uint8_t layer_identifier, uint8_t shape_identifier)
{
	// Set Object Identifier
	object_identifier[0] = Object::TERRAIN;
	object_identifier[1] = layer_identifier;
	object_identifier[2] = shape_identifier;
}

void DataClass::Data_Terrain::info(Editor::ObjectInfo& object_info)
{
	// Map to Shape Names
	static std::string shape_name_map[] = { "Rectangle", "Trapezoid", "Triangle", "Circle", "Polygon" };

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Terrain", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addTextValue("Shape: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &shape_name_map[shape->shape], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	shape->selectInfo(object_info);
}

DataClass::Data_Object* DataClass::Data_Terrain::makeCopy()
{
	Data_Terrain* new_terrain = new Data_Terrain(*this);
	new_terrain->shape = new_terrain->shape->makeCopy();
	return new_terrain;
}

void DataClass::Data_Terrain::generateInitialValues(glm::vec2& position, glm::vec4 color, Shape::Shape* new_shape)
{
	data.position = position;
	data.zpos = -1.0f;
	data.colors = color;
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	shape = new_shape;
}

