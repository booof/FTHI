#include "Rectangle.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"

void Shape::Rectangle::initializeShape(int& number_of_vertices)
{
	// Store Shape
	shape = RECTANGLE;

	// Store Number of Vertices
	number_of_vertices = 6;
}

void Shape::Rectangle::initializeVertices(Object::ObjectData& data, int& offset, int instance)
{
	float vertices[30];

	// Get Vertices
	Vertices::Rectangle::genRectObject(glm::vec2(0.0f), width, height, instance, vertices);

	// Store Data
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);

	// Increment Offset
	offset += sizeof(vertices);
}

#ifdef EDITOR

void Shape::Rectangle::writeShape(std::ofstream& object_file)
{
	object_file.write((char*)(this) + 8, sizeof(Rectangle) - 8);
}

void Shape::Rectangle::selectInfo(Editor::ObjectInfo& object_info)
{
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &width, &height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

bool Shape::Rectangle::testMouseCollisions(float x, float y, float object_x, float object_y)
{
	//std::cout << width << " " << height << "   " << object_x << " " << object_y << "    reee\n";

	if (width < 0)
		width *= -1.0f;

	if (height < 0)
		height *= -1.0f;

	float half_width = width * 0.5f;
	float half_height = height * 0.5f;

	if (x >= object_x - half_width && x <= object_x + half_width)
	{
		if (y >= object_y - half_height && y <= object_y + half_height)
		{
			return true;
		}
	}

	return false;
}

Shape::Rectangle::Rectangle(float width_, float height_)
{
	width = width_;
	height = height_;
}

Shape::Rectangle::Rectangle(std::ifstream& object_file)
{
	object_file.read((char*)(this) + 8, sizeof(Rectangle) - 8);
}

float* Shape::Rectangle::pointerToWidth()
{
	return &width;
}

float* Shape::Rectangle::pointerToHeight()
{
	return &height;
}

void Shape::Rectangle::printName()
{
	std::cout << "RECTANGLE\n";
}

Shape::Shape* Shape::Rectangle::makeCopy()
{
	return new Rectangle(*this);
}

#endif

