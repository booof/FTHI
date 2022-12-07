#include "Trapezoid.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Trapezoid/TrapezoidVertices.h"
#include "Source/Collisions/Point Collisions/PointCollisions.h"

void Shape::Trapezoid::initializeShape(int& number_of_vertices)
{
	// Store Shape
	shape = TRAPEZOID;

	// Store 
	number_of_vertices = 6;
}

void Shape::Trapezoid::initializeVertices(Object::ObjectData& data, int& offset, int instance)
{
	static float vertices[30];

	// Get Vertices
	Vertices::Trapezoid::genTrapObject(glm::vec2(0.0f), width, height, width_offset, height_offset, instance, vertices);

	// Store Data
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);

	// Increment Offset
	offset += sizeof(vertices);
}

#ifdef EDITOR

void Shape::Trapezoid::writeShape(std::ofstream& object_file)
{
	object_file.write((char*)(this) + 8, sizeof(Trapezoid) - 8);
}

void Shape::Trapezoid::selectShape(Editor::Selector& selector)
{
	selector.trapezoid_data = *this;
}

void Shape::Trapezoid::selectInfo(Editor::ObjectInfo& object_info)
{
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &width, &height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Offset: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &width_offset, &height_offset, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

bool Shape::Trapezoid::testMouseCollisions(float x, float y, float object_x, float object_y)
{
	return Source::Collisions::Point::testTrapCollisions(object_x, object_y, width, height, width_offset, height_offset);
}

Shape::Trapezoid::Trapezoid(float width_, float height_, float w_offest, float h_offset)
{
	width = width_;
	height = height_;
	width_offset = w_offest;
	height_offset = h_offset;
	shape = TRAPEZOID;
}

Shape::Trapezoid::Trapezoid(std::ifstream& object_file)
{
	object_file.read((char*)(this) + 8, sizeof(Trapezoid) - 8);
}

float* Shape::Trapezoid::pointerToWidth()
{
	return &width;
}

float* Shape::Trapezoid::pointerToHeight()
{
	return &height;
}

float* Shape::Trapezoid::pointerToWidthOffset()
{
	return &width_offset;
}

float* Shape::Trapezoid::pointerToHeightOffset()
{
	return &height_offset;
}

void Shape::Trapezoid::printName()
{
	std::cout << "TRAPEZOID\n";
}

#endif
