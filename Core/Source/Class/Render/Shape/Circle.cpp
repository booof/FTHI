#include "Circle.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Circle/CircleVertices.h"

void Shape::Circle::initializeShape(int& number_of_vertices)
{
	// Store Shape
	shape = CIRCLE;
	
	// Store Number of Vertices
	number_of_vertices = 60;
}

void Shape::Circle::initializeVertices(Object::ObjectData& data, int& offset, int instance)
{
	// 20 Sides for Circle
	static float vertices[300];

	// Get Vertices
	Vertices::Circle::genCircleObject(glm::vec2(0.0f), radius, 20, instance, vertices);

	// Store Vertices
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);

	// Increment Offset
	offset += sizeof(vertices);
}

#ifdef EDITOR

void Shape::Circle::selectShape(Editor::Selector& selector)
{
	selector.circle_data = *this;
}

void Shape::Circle::selectInfo(Editor::ObjectInfo& object_info)
{
	object_info.addSingleValue("Width: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &radius, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
}

bool Shape::Circle::testMouseCollisions(float x, float y, float object_x, float object_y)
{
	return glm::distance(glm::vec2(object_x, object_y), glm::vec2(x, y)) <= radius;
}

Shape::Circle::Circle(float radius1, float radius2)
{
	radius = radius1;
	radius_inner = radius2;
	shape = CIRCLE;
}

float* Shape::Circle::pointerToRadius()
{
	return &radius;
}

float* Shape::Circle::pointerToRadiusInner()
{
	return &radius_inner;
}

void Shape::Circle::printName()
{
	std::cout << "CIRCLE\n";
}

#endif

