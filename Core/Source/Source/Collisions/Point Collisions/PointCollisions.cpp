#include "PointCollisions.h"
#include "Globals.h"
#include "Source/Vertices/Visualizer/Visualizer.h"

bool Source::Collisions::Point::testTrapCollisions(float xPos, float yPos, float width, float height, float width_offset, float height_offset)
{
	// Calculate Half Values
	float half_width = width / 2;
	float half_height = height / 2;

	// Values of Corners
	glm::vec2 bottom_left = glm::vec2(xPos - half_width, yPos - half_height);
	glm::vec2 bottom_right = glm::vec2(xPos + half_width, yPos - half_height + height_offset);
	glm::vec2 top_right = glm::vec2(xPos + half_width + width_offset, yPos + half_height + height_offset);
	glm::vec2 top_left = glm::vec2(xPos - half_width + width_offset, yPos + half_height);

	// Calculate Relative Values
	float relative_y_bottom = ((bottom_left.y - bottom_right.y) / (bottom_left.x - bottom_right.x)) * (Global::mouseRelativeX - bottom_left.x) + bottom_left.y;
	float relative_x_right = ((bottom_right.x - top_right.x) / (bottom_right.y - top_right.y)) * (Global::mouseRelativeY - bottom_right.y) + bottom_right.x;
	float relative_y_top = ((top_right.y - top_left.y) / (top_right.x - top_left.x)) * (Global::mouseRelativeX - top_right.x) + top_right.y;
	float relative_x_left = ((top_left.x - bottom_left.x) / (top_left.y - bottom_left.y)) * (Global::mouseRelativeY - top_left.y) + top_left.x;

	// Test Collisions
	if (Global::mouseRelativeY >= relative_y_bottom && Global::mouseRelativeX <= relative_x_right && Global::mouseRelativeY <= relative_y_top && Global::mouseRelativeX >= relative_x_left)
	{
		return true;
	}

	// Return False if no Collisions Occoured
	return false;
}

bool Source::Collisions::Point::testTriCollisionsHelper(glm::vec2 test_vertex, glm::vec2 vertex1, glm::vec2 vertex2)
{
	float angle1, angle2, angle_mouse;

	// Get Angle Between Test Vertex and Vertex1
	angle1 = atan((test_vertex.y - vertex1.y) / (test_vertex.x - vertex1.x));
	if (vertex1.x > test_vertex.x)
		angle1 += 3.14159f;
	if (angle1 < 0)
		angle1 += 6.2832f;

	// Get Angle Between Test Vertex and Vertex2
	angle2 = atan((test_vertex.y - vertex2.y) / (test_vertex.x - vertex2.x));
	if (vertex2.x > test_vertex.x)
		angle2 += 3.14159f;
	if (angle2 < 0)
		angle2 += 6.2832f;

	// Get Angle Between Test Vertex and Mouse
	angle_mouse = atan((test_vertex.y - Global::mouseRelativeY) / (test_vertex.x - Global::mouseRelativeX));
	if (Global::mouseRelativeX > test_vertex.x)
		angle_mouse += 3.14159f;
	if (angle_mouse < 0)
		angle_mouse += 6.2832f;


	//Vertices::Visualizer::visualizeRay(test_vertex, angle1, 0.2f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	//Vertices::Visualizer::visualizeRay(test_vertex, angle2, 0.2f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//Vertices::Visualizer::visualizeRay(test_vertex, angle_mouse, 0.2f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

	// Vertex1 Angle is Greater Than Vertex2 Angle
	if (angle1 > angle2)
	{
		// Vertex 1 is in Negative Plane
		if (angle1 - angle2 > 3.14159f)
		{
			// Mouse is in Positive Plane
			if (angle_mouse < 3.14159f)
			{
				return angle2 > angle_mouse && angle_mouse > (angle1 - 6.2832f);
			}

			// Mouse is in Negative Plane
			else
			{
				return (angle2 + 6.2832) > angle_mouse && angle_mouse > angle1;
			}
		}

		// Both are in Positive Plane
		else
		{
			return angle1 > angle_mouse && angle_mouse > angle2;
		}
	}

	// Vertex2 Angle is Greater Than Vertex1 Angle
	else
	{
		// Vertex 2 is in Negative Plane
		if (angle2 - angle1 > 3.14159f)
		{
			// Mouse is in Positive Plane
			if (angle_mouse < 3.14159f)
			{
				return angle1 > angle_mouse && angle_mouse > (angle2 - 6.2832f);
			}

			// Mouse is in Negative Plane
			else
			{
				return (angle1 + 6.2832) > angle_mouse && angle_mouse > angle2;
			}
		}

		// Both are in Positive Plane
		else
		{
			return angle2 > angle_mouse && angle_mouse > angle1;
		}
	}

	return false;
}

bool Source::Collisions::Point::testTriCollisions(glm::vec2 coords1, glm::vec2 coords2, glm::vec2 coords3)
{
	// Test Coords1 
	if (!testTriCollisionsHelper(coords1, coords2, coords3))
		return false;

	// Test Coords2 
	if (!testTriCollisionsHelper(coords2, coords1, coords3))
		return false;

	// Test Coords3 
	if (!testTriCollisionsHelper(coords3, coords1, coords2))
		return false;

	return true;
}

void Source::Collisions::Point::arrangeTriVertices(glm::vec2& coords1, glm::vec2& coords2, glm::vec2& coords3)
{
	int highest;
	int lowest;

	// Test if coords1 is Highest
	if (coords1.y > coords2.y && coords1.y > coords3.y)
	{
		highest = 1;

		// Test if coords2 is lowest
		if (coords2.y < coords3.y)
		{
			lowest = 2;
		}

		// Test if coords3 is Lowest
		else
		{
			lowest = 3;
		}
	}

	// Test if coords 2 is Highest
	else if (coords2.y > coords1.y && coords2.y > coords3.y)
	{
		highest = 2;

		// Test if coords1 is lowest
		if (coords1.y < coords3.y)
		{
			lowest = 1;
		}

		// Test if coords3 is lowest
		else
		{
			lowest = 3;
		}
	}

	// Test if coords3 is Highest
	else
	{
		highest = 3;

		// Test if coords1 is lowest
		if (coords1.y < coords2.y)
		{
			lowest = 1;
		}

		// Test if coords2 is lowest
		else
		{
			lowest = 2;
		}
	}

	// Test if The Highest is Not Already the First Coordintate
	if (highest != 1)
	{
		// Temp Variables to Allow Swaping of Variables
		glm::vec2 temp = coords1;

		// Test if Highest is the Second Coordinate
		if (highest == 2)
		{
			// Swap Coords2 and Coords1
			coords1 = coords2;
			coords2 = temp;

			// Swap Coords1 (now coords2) With Coords3
			if (lowest == 1)
			{
				temp = coords3;
				coords3 = coords2;
				coords2 = temp;
			}

			// Coords3 is Already Lowest
		}

		// Highest is Third Coordinate
		else
		{
			// Swap Coords3 and Coords1
			coords1 = coords3;
			coords3 = temp;

			// Coords1 Has Already Been Switched to Lowest

			// Swap Coords1 (now coords3) With Coords2
			if (lowest == 2)
			{
				temp = coords2;
				coords2 = coords3;
				coords2 = temp;
			}
		}
	}

	// Highest Coordinate is Already First Coordinate
	else
	{
		// Swap Coords2 and Coords3
		if (lowest == 2)
		{
			glm::vec2 temp = coords3;
			coords3 = coords2;
			coords2 = temp;
		}

		// Coords3 is Already Lowest
	}
}
