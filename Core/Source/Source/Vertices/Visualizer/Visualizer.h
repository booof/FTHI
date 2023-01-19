#pragma once
#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "Source/Vertices/Vertices.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Line/LineVertices.h"

namespace Vertices::Visualizer
{
	// Generate a Rectangle to visualize a point
	void visualizePoint(float xPos, float yPos, float size, glm::vec4 color);

	// Generate a Rectangle to Visualize a Point
	void visualizePoint(glm::vec2 position, float size, glm::vec4 color);

	// Generate a Line to Visualize Relationship Between Two Points
	void visualizeLine(float x1, float y1, float x2, float y2, float size, glm::vec4 color);

	// Generate a Line to Visualize Relationship Between Two Points
	void visualizeLine(glm::vec2 point1, glm::vec2 point2, float size, glm::vec4 color);

	// Generate a Line to Visualize a Ray
	void visualizeRay(float x, float y, float theta, float size, glm::vec4 color);

	// Generate a Line to Visualize a Ray
	void visualizeRay(glm::vec2 origin, float theta, float size, glm::vec4 color);
}

#endif
