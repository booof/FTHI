#include "LineVertices.h"

void Vertices::Line::genLineColor(float x1, float x2, float y1, float y2, float zPos, float width, glm::vec4 color, float* vertices)
{
	// Calculate Angle of Line
	float angle = 1.57f - atan((y1 - y2) / (x1 - x2));

	// Calculate Simplified Coordinates of Line
	float simplifiedX = -(width / 2) * cos(angle);
	float simplifiedY = (width / 2) * sin(angle);

	// Store Position Values
	vertices[0] = x1 - simplifiedX;
	vertices[1] = y1 - simplifiedY;
	vertices[7] = x2 - simplifiedX;
	vertices[8] = y2 - simplifiedY;
	vertices[14] = x2 + simplifiedX;
	vertices[15] = y2 + simplifiedY;
	vertices[21] = x2 + simplifiedX;
	vertices[22] = y2 + simplifiedY;
	vertices[28] = x1 + simplifiedX;
	vertices[29] = y1 + simplifiedY;
	vertices[35] = x1 - simplifiedX;
	vertices[36] = y1 - simplifiedY;

	// Store Z-Pos and Color Values
	for (int i = 2; i < 42; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = color.r;
		vertices[i + 2] = color.g;
		vertices[i + 3] = color.b;
		vertices[i + 4] = color.a;
	}
}

void Vertices::Line::genLineTexture(float x1, float x2, float y1, float y2, float zPos, float width, float* vertices)
{
	// Calculate Angle of Line
	float angle = 1.57f - atan((y1 - y2) / (x1 - x2));

	// Calculate Simplified Coordinates of Line
	float simplifiedX = -(width / 2) * cos(angle);
	float simplifiedY = (width / 2) * sin(angle);

	// Store Position Vertices
	vertices[0] = x1 - simplifiedX;
	vertices[1] = y1 - simplifiedY;
	vertices[5] = x2 - simplifiedX;
	vertices[6] = y2 - simplifiedY;
	vertices[10] = x2 + simplifiedX;
	vertices[11] = y2 + simplifiedY;
	vertices[15] = x2 + simplifiedX;
	vertices[16] = y2 + simplifiedY;
	vertices[20] = x1 + simplifiedX;
	vertices[21] = y1 + simplifiedY;
	vertices[25] = x1 - simplifiedX;
	vertices[26] = y1 - simplifiedY;

	// Store Texture Coordinates
	vertices[3] = 0.0f;
	vertices[4] = 0.0f;
	vertices[8] = 1.0f;
	vertices[9] = 0.0f;
	vertices[13] = 1.0f;
	vertices[14] = 1.0f;
	vertices[18] = 1.0f;
	vertices[19] = 1.0f;
	vertices[23] = 0.0f;
	vertices[24] = 1.0f;
	vertices[28] = 0.0f;
	vertices[29] = 0.0f;

	// Store Z-Pos
	for (int i = 2; i < 30; i += 5)
	{
		vertices[i] = zPos;
	}
}

void Vertices::Line::genLineHighlighter(float x1, float x2, float y1, float y2, float zPos, float* vertices)
{
	// Calculate Angle of Line
	float angle = 1.57f - atan((y1 - y2) / (x1 - x2));

	// Calculate Simplified Coordinates of Line
	float simplifiedX = -0.75f * cos(angle);
	float simplifiedY = 0.75f * sin(angle);

	// Store Position
	vertices[0] = x1 - simplifiedX;
	vertices[1] = y1 - simplifiedY;
	vertices[7] = x2 - simplifiedX;
	vertices[8] = y2 - simplifiedY;
	vertices[14] = x2 - simplifiedX;
	vertices[15] = y2 - simplifiedY;
	vertices[21] = x2 + simplifiedX;
	vertices[22] = y2 + simplifiedY;
	vertices[28] = x2 + simplifiedX;
	vertices[29] = y2 + simplifiedY;
	vertices[35] = x1 + simplifiedX;
	vertices[36] = y1 + simplifiedY;
	vertices[42] = x1 + simplifiedX;
	vertices[43] = y1 + simplifiedY;
	vertices[49] = x1 - simplifiedX;
	vertices[50] = y1 - simplifiedY;

	// Store Z-Pos and Colors
	for (int i = 2; i < 56; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = 1.0f;
		vertices[i + 2] = 1.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 1.0f;
	}
}

void Vertices::Line::genLineHighlighterWidth(float x1, float x2, float y1, float y2, float zPos, float width, float* vertices)
{
	// Calculate Angle of Line
	float angle = 1.57f - atan((y1 - y2) / (x1 - x2));

	// Calculate Simplified Coordinates of Line
	float simplifiedX = -(width / 2) * cos(angle);
	float simplifiedY = (width / 2) * sin(angle);

	// Store Position
	vertices[0] = x1 - simplifiedX;
	vertices[1] = y1 - simplifiedY;
	vertices[7] = x2 - simplifiedX;
	vertices[8] = y2 - simplifiedY;
	vertices[14] = x2 - simplifiedX;
	vertices[15] = y2 - simplifiedY;
	vertices[21] = x2 + simplifiedX;
	vertices[22] = y2 + simplifiedY;
	vertices[28] = x2 + simplifiedX;
	vertices[29] = y2 + simplifiedY;
	vertices[35] = x1 + simplifiedX;
	vertices[36] = y1 + simplifiedY;
	vertices[42] = x1 + simplifiedX;
	vertices[43] = y1 + simplifiedY;
	vertices[49] = x1 - simplifiedX;
	vertices[50] = y1 - simplifiedY;

	// Store Z-Pos and Colors
	for (int i = 2; i < 56; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = 1.0f;
		vertices[i + 2] = 1.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 1.0f;
	}
}

void Vertices::Line::genLineOutlineRect(float x1, float x2, float y1, float y2, float zPos, float width, glm::vec4 color, float* vertices)
{
	// Store Position Values of Top Line
	vertices[0] = x1;
	vertices[1] = y1 - width;
	vertices[7] = x2 - width;
	vertices[8] = y1 - width;
	vertices[14] = x2 - width;
	vertices[15] = y1;

	vertices[21] = x2 - width;
	vertices[22] = y1;
	vertices[28] = x1;
	vertices[29] = y1;
	vertices[35] = x1;
	vertices[36] = y1 - width;

	// Store Position Values of Right Line
	vertices[42] = x2 - width;
	vertices[43] = y2 + width;
	vertices[49] = x2;
	vertices[50] = y2 + width;
	vertices[56] = x2;
	vertices[57] = y1;

	vertices[63] = x2;
	vertices[64] = y1;
	vertices[70] = x2 - width;
	vertices[71] = y1;
	vertices[77] = x2 - width;
	vertices[78] = y2 + width;

	// Store Position Values of Bottom Line
	vertices[84] = x1 + width;
	vertices[85] = y2;
	vertices[91] = x2;
	vertices[92] = y2;
	vertices[98] = x2;
	vertices[99] = y2 + width;

	vertices[105] = x2;
	vertices[106] = y2 + width;
	vertices[112] = x1 + width;
	vertices[113] = y2 + width;
	vertices[119] = x1 + width;
	vertices[120] = y2;

	// Store Position Values of Left Line
	vertices[126] = x1;
	vertices[127] = y2;
	vertices[133] = x1 + width;
	vertices[134] = y2;
	vertices[140] = x1 + width;
	vertices[141] = y1 - width;

	vertices[147] = x1 + width;
	vertices[148] = y1 - width;
	vertices[154] = x1;
	vertices[155] = y1 - width;
	vertices[161] = x1;
	vertices[162] = y2;

	// Store Z-Pos and Colors
	for (int i = 2; i < 168; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = 1.0f;
		vertices[i + 2] = 1.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 1.0f;
	}
}

void Vertices::Line::genLineSimplifiedCurve1(float xPos, float yPos, float zPos, float angle, float scale, glm::vec4 color, char samples, float* vertices)
{
	// Calculate the Distance Between Each xPos on the Line
	float distance = scale / samples;

	// Modify Distances
	scale *= 0.5f;
	angle *= 0.5f;

	// Calculate Slope of Curve
	float Slope = (float)tan(1) / scale;

	// Calculate Left xPos
	float xPos_Left = xPos - scale;

	// Create Samples
	for (int i = 0, j = 0; j < samples; i += 14, j++)
	{
		// First Vertex
		vertices[i] = xPos_Left + distance * j;
		vertices[i + 1] = angle * atan(Slope * (xPos_Left + distance * j)) + yPos;
		vertices[i + 2] = zPos;
		vertices[i + 3] = color.r;
		vertices[i + 4] = color.g;
		vertices[i + 5] = color.b;
		vertices[i + 6] = color.a;

		// Second Vertex
		vertices[i + 7] = xPos_Left + distance * (j + 1);
		vertices[i + 8] = angle * atan(Slope * (xPos_Left + distance * (j + 1))) + yPos;
		vertices[i + 9] = zPos;
		vertices[i + 10] = color.r;
		vertices[i + 11] = color.g;
		vertices[i + 12] = color.b;
		vertices[i + 13] = color.a;
	}
}

void Vertices::Line::genLineSimplifiedCurve2(float xPos, float yPos, float zPos, float scale, float slope, float amlitude, float x_offset, int sign, glm::vec4 color, char samples, float* vertices)
{
	// Calculate Distance Between Each xPos on the Line
	float distance = scale / samples;

	// Create Samples
	for (int i = 0, j = 0; j < samples; i += 14, j++)
	{
		// First Vertex
		vertices[i] = xPos + distance * j * sign;
		vertices[i + 1] = slope * pow(amlitude, (distance * j - x_offset)) + yPos;
		vertices[i + 2] = zPos;
		vertices[i + 3] = color.r;
		vertices[i + 4] = color.g;
		vertices[i + 5] = color.b;
		vertices[i + 6] = color.a;

		// Second Vertex
		vertices[i + 7] = xPos + distance * (j + 1) * sign;
		vertices[i + 8] = slope * pow(amlitude, (distance * (j + 1) - x_offset)) + yPos;
		vertices[i + 9] = zPos;
		vertices[i + 10] = color.r;
		vertices[i + 11] = color.g;
		vertices[i + 12] = color.b;
		vertices[i + 13] = color.a;
	}
}

void Vertices::Line::genLineDetailedCurve1(float xPos, float yPos, float zPos, float angle, float scale, float size, glm::vec4 color, char samples, float* vertices)
{
	// Calculate the Distance Between Each xPos on the Line
	float distance = (2 * scale) / samples;

	// Calculate Slope of Curve
	float Slope = (float)tan(1) / scale;

	// Calculate Left xPos
	float xPos_Left = xPos - scale;

	// Create Samples
	for (int i = 0, j = 0; j < samples; i += 42, j++)
	{
		genLineColor(xPos_Left + distance * j, xPos_Left + distance * (j + 1), angle * atan(Slope * (xPos_Left + distance * j)) + yPos, angle * atan(Slope * (xPos_Left + distance * (j + 1))) + yPos, zPos, size, color, vertices + i);
	}
}

void Vertices::Line::genLineDetailedCurve2(float xPos, float yPos, float zPos, float scale, float slope, float amlitude, float x_offset, int sign, float size, glm::vec4 color, char samples, float* vertices)
{
	// Calculate Distance Between Each xPos on the Line
	float distance = scale / samples;

	// Create Samples
	for (int i = 0, j = 0; j < samples; i += 42, j++)
	{
		genLineColor(xPos + distance * j * sign, xPos + distance * (j + 1) * sign, slope * pow(amlitude, (distance * j - x_offset)) + yPos, slope * pow(amlitude, (distance * (j + 1) - x_offset)) + yPos, zPos, size, color, vertices + i);
	}
}
