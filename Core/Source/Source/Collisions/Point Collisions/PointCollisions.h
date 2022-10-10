#ifndef POINT_COLLISIONS_H
#define POINT_COLLISIONS_H

#include "ExternalLibs.h"

namespace Source::Collisions::Point
{
	// Test if Mouse is Inside of a Trapezoid Object
	bool testTrapCollisions(float xPos, float yPos, float width, float height, float width_offset, float height_offset);

	// Triangle Collision Helper
	bool testTriCollisionsHelper(glm::vec2 test_vertex, glm::vec2 vertex1, glm::vec2 vertex2);

	// Test if Mouse is Inside of a Triangle Object
	bool testTriCollisions(glm::vec2 coords1, glm::vec2 coords2, glm::vec2 coords3);

	// Sort Triangle Vertices
	void arrangeTriVertices(glm::vec2& coords1, glm::vec2& coords2, glm::vec2& coords3);
}

#endif
