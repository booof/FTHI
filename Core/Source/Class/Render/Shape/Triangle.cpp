#include "Triangle.h"
#include "Class/Object/Object.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Triangle/TriangleVertices.h"
#include "Source/Collisions/Point Collisions/PointCollisions.h"
#include "Source/Vertices/Visualizer/Visualizer.h"

void Shape::Triangle::initializeShape(int& number_of_vertices)
{
	// Store Shape
	shape = TRIANGLE;

	// Store Number of Vertices
	number_of_vertices = 3;
}

void Shape::Triangle::initializeVertices(Object::ObjectData& data, int& offset, int instance)
{
	static float vertices[15];

	// Get Vertices
	Vertices::Triangle::genTriObject(glm::vec2(0.0f), position2 - data.position, position3 - data.position, instance, vertices);

	// Store Vertices
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);

	// Increment Offset
	offset += sizeof(vertices);
}

#ifdef EDITOR

void Shape::Triangle::writeShape(std::ofstream& object_file)
{
	object_file.write((char*)(this) + 8, sizeof(Triangle) - 8);
}

void Shape::Triangle::selectInfo(Editor::ObjectInfo& object_info)
{
	object_info.addDoubleValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &position2.x, &position2.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Pos3: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &position3.x, &position3.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

bool Shape::Triangle::testMouseCollisions(float x, float y, float object_x, float object_y)
{
	// Get Vertices
	glm::vec2 origin = glm::vec2(object_x, object_y);
	glm::vec2 coords1 = position2;
	glm::vec2 coords2 = position3;

	return Source::Collisions::Point::testTriCollisions(origin, coords1, coords2);
}

Shape::Triangle::Triangle(glm::vec2 pos2, glm::vec2 pos3)
{
	position2 = pos2;
	position3 = pos3;
	shape = TRIANGLE;
}

Shape::Triangle::Triangle(std::ifstream& object_file)
{
	object_file.read((char*)(this) + 8, sizeof(Triangle) - 8);
}

glm::vec2* Shape::Triangle::pointerToSecondPosition()
{
	return &position2;
}

glm::vec2* Shape::Triangle::pointerToThirdPosition()
{
	return &position3;
}

void Shape::Triangle::printName()
{
	std::cout << "TRIANGLE\n";
}

Shape::Shape* Shape::Triangle::makeCopy()
{
	return new Triangle(*this);
}

void Shape::Triangle::updateSelectedPosition(float deltaX, float deltaY)
{
	position2.x += deltaX;
	position2.y += deltaY;
	position3.x += deltaX;
	position3.y += deltaY;
}

void Shape::Triangle::setInfoPointers(int& index2, int& index3, glm::vec2** position2, glm::vec2** position3)
{
	// Match Position Pointers
	*position2 = &this->position2;
	*position3 = &this->position3;

	// Positions 2 and 3 are at Indicies 4 and 5 Respectively
	index2 = 4;
	index3 = 5;
}

#endif

