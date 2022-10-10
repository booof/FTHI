#include "FloorMask.h"

#include "Source/Vertices/Visualizer/Visualizer.h"

bool Object::Mask::Floor::FloorMask::testCollisions(glm::vec2 test_position, float error)
{
	return testCollisions(test_position, error, false);
}

bool Object::Mask::Floor::FloorMask::testEdgeCollisions(glm::vec2& left_vertex, glm::vec2& right_vertex, float error, glm::vec2& returned_edge_pos)
{
	// Parameterize Vertices
	float mx = right_vertex.x - left_vertex.x;
	float bx = left_vertex.x;
	float my = right_vertex.y - left_vertex.y;
	float by = left_vertex.y;

	// Get Left and Right Corners of Object
	glm::vec2 left_edge, right_edge;
	getLeftRightEdgeVertices(left_edge, right_edge);

	//Vertices::Visualizer::visualizePoint(left_edge, 1.0f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	// Find Object Edge Position at X-Pos of Mask Edge
	float left_pos_y, right_pos_y;
	float t1 = (left_edge.x - bx) / mx;
	float t2 = (right_edge.x - bx) / mx;
	left_pos_y = my * t1 + by;
	right_pos_y = my * t2 + by;

	// Test if Object Edge Y-Pos is Below Mask Edge y-Pos
	if ((t1 > 0 && t1 < 1) && (left_edge.y - error < left_pos_y && left_edge.y + 0.01f > left_pos_y))
	{
		returned_edge_pos = glm::vec2(left_edge.x, left_pos_y);
		if (type)
			returned_value = left_edge.y;
		return true;
	}

	if ((t2 > 0 && t2 < 1) && (right_edge.y - error < right_pos_y && right_edge.y + 0.01f > right_pos_y))
	{
		returned_edge_pos = glm::vec2(right_edge.x, right_pos_y);
		if (type)
			returned_value = right_edge.y;
		return true;
	}

	return false;
}
