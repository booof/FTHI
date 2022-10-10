#ifndef LINE_VERTICES_H
#define LINE_VERTICES_H

#include "Vertices/Vertices.h"

namespace Vertices::Line
{
	// Draws Straight Lines
	// Vertices = 42f
	void genLineColor(float x1, float x2, float y1, float y2, float zPos, float width, glm::vec4 color, float* vertices);

	// Draws Straight Lines with Texture
	// Vertices = 30f
	void genLineTexture(float x1, float x2, float y1, float y2, float zPos, float width, float* vertices);

	// Generate Vertices that Highlight a Line
	// Vertices = 56f
	void genLineHighlighter(float x1, float x2, float y1, float y2, float zPos, float* vertices);

	// Generate Vertices that Highlight a Line With Specified Width
	// Vertices = 56f
	void genLineHighlighterWidth(float x1, float x2, float y1, float y2, float zPos, float width, float* vertices);

	// Draws Lines that Outline a Rectangle
	// Vertiecs = 168f
	void genLineOutlineRect(float x1, float x2, float y1, float y2, float zPos, float width, glm::vec4 color, float* vertices);

	// Draws Curved Lines
	// Vertices = (samples * 14)f
	void genLineSimplifiedCurve1(float xPos, float yPos, float zPos, float angle, float scale, glm::vec4 color, char samples, float* vertices);

	// Draws Curved Lines
	// Vertices = (samples * 14)f
	void genLineSimplifiedCurve2(float xPos, float yPos, float zPos, float scale, float slope, float amlitude, float x_offset, int sign, glm::vec4 color, char samples, float* vertices);

	// Like Simplified Lines, but Should Only be Used if Curve NEEDS to be WIDER Than ONE Pixel
	// Vertices = (samples * 42)f
	void genLineDetailedCurve1(float xPos, float yPos, float zPos, float angle, float scale, float size, glm::vec4 color, char samples, float* vertieces);

	// Like Simplified Lines, but Should Only be Used if Curve NEEDS to be WIDER Than ONE Pixel
	// Vertices = (samples * 42)f
	void genLineDetailedCurve2(float xPos, float yPos, float zPos, float scale, float slope, float amlitude, float x_offset, int sign, float size, glm::vec4 color, char samples, float* vertices);
}

#endif

