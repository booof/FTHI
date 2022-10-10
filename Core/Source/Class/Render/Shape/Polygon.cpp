#include "Polygon.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Circle/CircleVertices.h"

void Shape::Polygon::initializeShape(int& number_of_vertices)
{
	// Store Shape
	shape = POLYGON;

	// Store Number of Vertices
	number_of_vertices = number_of_sides * 3;
}

void Shape::Polygon::initializeVertices(Object::ObjectData& data, int& offset, int instance)
{
	// Set Up Vertices
	int vertex_count = 15 * number_of_sides;
	float* vertices = new float[vertex_count];

	// Get Vertices
	Vertices::Circle::genCircleObject(glm::vec2(0.0f), radius, number_of_sides, instance, vertices);

	// Store Vertices
	glBufferSubData(GL_ARRAY_BUFFER, offset, vertex_count * 4, vertices);

	// Increment Offset
	offset += vertex_count * 4;

	// Delete Vertices
	delete[] vertices;
}

#ifdef EDITOR

void Shape::Polygon::selectShape(Editor::Selector& selector)
{
	selector.polygon_data = *this;
}

void Shape::Polygon::selectInfo(Editor::ObjectInfo& object_info)
{
	object_info.addSingleValue("Radus: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &radius, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
	object_info.addSingleValue("NumberOfSides: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &number_of_sides, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
}

bool Shape::Polygon::testMouseCollisions(float x, float y, float object_x, float object_y)
{
	return glm::distance(glm::vec2(object_x, object_y), glm::vec2(x, y)) <= radius;
}

Shape::Polygon::Polygon(float radius1, float radius2, unsigned char sides, float a_offset)
{
	radius = radius1;
	radius_inner = radius2;
	number_of_sides = sides;
	angle_offset = a_offset;
	shape = POLYGON;
}

float* Shape::Polygon::pointerToRadius()
{
	return &radius;
}

float* Shape::Polygon::pointerToRaidusInner()
{
	return &radius_inner;
}

unsigned char* Shape::Polygon::pointerToNumberOfSides()
{
	return &number_of_sides;
}

float* Shape::Polygon::pointerToAngleOffset()
{
	return &angle_offset;
}

void Shape::Polygon::printName()
{
	std::cout << "POLYGON\n";
}

#endif

