#include "TerrainBase.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Constants.h"
#include "Globals.h"

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

void Object::Terrain::TerrainBase::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(TERRAIN);
	object_file.put(layer);
	object_file.put(shape->shape);

	// Write Shape
	switch (shape->shape)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		Shape::Rectangle* temp_rect = static_cast<Shape::Rectangle*>(shape);
		object_file.write((char*)(temp_rect) + 8, sizeof(Shape::Rectangle) - 8);
		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		Shape::Trapezoid* temp_trap = static_cast<Shape::Trapezoid*>(shape);
		object_file.write((char*)(temp_trap) + 8 , sizeof(Shape::Trapezoid) - 8);
		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		Shape::Triangle* temp_tri = static_cast<Shape::Triangle*>(shape);
		object_file.write((char*)(temp_tri) + 8, sizeof(Shape::Triangle) - 8);
		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		Shape::Circle* temp_circle = static_cast<Shape::Circle*>(shape);
		object_file.write((char*)(temp_circle) + 8, sizeof(Shape::Circle) - 8);
		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		Shape::Polygon* temp_poly = static_cast<Shape::Polygon*>(shape);
		object_file.write((char*)(temp_poly) + 8, sizeof(Shape::Polygon) - 8);
		break;
	}

	}

	// Write Data
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
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

	data.zpos = -1.0f;

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

void Object::Terrain::TerrainBase::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = TERRAIN;
	selector.object_identifier[1] = layer;
	selector.object_identifier[2] = shape->shape;

	// Store Editing Values
	shape->selectShape(selector);
	selector.object_data = data;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data, shape);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

glm::vec2 Object::Terrain::TerrainBase::returnPosition()
{
	return data.position;
}

Shape::Shape* Object::Terrain::TerrainBase::returnShapePointer()
{
	return shape;
}

void Object::Terrain::TerrainBase::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, Shape::Shape* shape)
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

