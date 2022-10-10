#include "TriangleVertices.h"

void Vertices::Triangle::genTriObject(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, int data_index, float* vertices)
{
	// Store Position Data
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[5] = position2.x;
	vertices[6] = position2.y;
	vertices[10] = position3.x;
	vertices[11] = position3.y;

	// Store Texture Data
	vertices[2] = -1.0f;
	vertices[3] = -1.0f;
	vertices[7] = 1.0f;
	vertices[8] = -1.0f;
	vertices[12] = 1.0f;
	vertices[13] = 1.0f;

	// Store Data Index
	for (int i = 4; i < 15; i += 5)
	{
		vertices[i] = (float)data_index;
	}
}

void Vertices::Triangle::genTriObjectFull(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, glm::vec3 normal, float* vertices)
{
	// Store Position Data
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[12] = position2.x;
	vertices[13] = position2.y;
	vertices[24] = position3.x;
	vertices[25] = position3.y;

	// Store Texture Data
	vertices[10] = -1.0f;
	vertices[11] = -1.0f;
	vertices[22] = 1.0f;
	vertices[23] = -1.0f;
	vertices[34] = 1.0f;
	vertices[35] = 1.0f;

	// Store Z-Pos and Colors
	for (int i = 2; i < 36; i += 12)
	{
		vertices[i] = zPos;
		vertices[i + 1] = normal.x;
		vertices[i + 2] = normal.y;
		vertices[i + 3] = normal.z;
		vertices[i + 4] = color.r;
		vertices[i + 5] = color.g;
		vertices[i + 6] = color.b;
		vertices[i + 7] = color.a;
	}
}

void Vertices::Triangle::genTriColor(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, float* vertices)
{
	// Store Position Values
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[7] = position2.x;
	vertices[8] = position2.y;
	vertices[14] = position3.x;
	vertices[15] = position3.y;

	// Store Z-Pos and Color Values
	for (int i = 2; i < 21; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = color.r;
		vertices[i + 2] = color.g;
		vertices[i + 3] = color.b;
		vertices[i + 4] = color.a;
	}
}

void Vertices::Triangle::genTriColorNormal(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec4 color, glm::vec3 normal, float* vertices)
{
	// Store Position Values
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[10] = position2.x;
	vertices[11] = position2.y;
	vertices[20] = position3.x;
	vertices[21] = position3.y;

	// Store Z-Pos, Colors, and Normals
	for (int i = 2; i < 30; i += 10)
	{
		vertices[i] = zPos;
		vertices[i + 1] = normal.x;
		vertices[i + 2] = normal.y;
		vertices[i + 3] = normal.z;
		vertices[i + 4] = color.r;
		vertices[i + 5] = color.g;
		vertices[i + 6] = color.b;
		vertices[i + 7] = color.a;
	}
}

void Vertices::Triangle::genTriTexture(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, float* vertices)
{
	// Store Position Vertices
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[5] = position2.x;
	vertices[6] = position2.y;
	vertices[10] = position3.x;
	vertices[11] = position3.y;

	// Store Texture Coordinates
	vertices[3] = -1.0f;
	vertices[4] = -1.0f;
	vertices[8] = 1.0f;
	vertices[9] = -1.0f;
	vertices[13] = 1.0f;
	vertices[14] = 1.0f;

	// Store Z-Pos
	for (int i = 2; i < 15; i += 5)
	{
		vertices[2] = zPos;
	}
}

void Vertices::Triangle::genTriTextureNormal(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, glm::vec3 normal, float* vertices)
{
	// Store Position Vertices
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[8] = position2.x;
	vertices[9] = position2.y;
	vertices[16] = position3.x;
	vertices[17] = position3.y;

	// Store Texture Coordinates
	vertices[6] = -1.0f;
	vertices[7] = -1.0f;
	vertices[14] = 1.0f;
	vertices[15] = -1.0f;
	vertices[22] = 1.0f;
	vertices[23] = 1.0f;

	// Store Z-Pos and Normals
	for (int i = 2; i < 24; i += 8)
	{
		vertices[i] = zPos;
		vertices[i + 1] = normal.x;
		vertices[i + 2] = normal.y;
		vertices[i + 3] = normal.z;
	}
}

void Vertices::Triangle::genTriHighlighter(glm::vec2 position1, glm::vec2 position2, glm::vec2 position3, float zPos, float* vertices)
{
	// Store Position Vertices
	vertices[0] = position1.x;
	vertices[1] = position1.y;
	vertices[7] = position2.x;
	vertices[8] = position2.y;
	vertices[14] = position2.x;
	vertices[15] = position2.y;
	vertices[21] = position3.x;
	vertices[22] = position3.y;
	vertices[28] = position3.x;
	vertices[29] = position3.y;
	vertices[35] = position1.x;
	vertices[36] = position1.y;

	// Store Z-Pos and Colors
	for (int i = 2; i < 42; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = 1.0f;
		vertices[i + 2] = 1.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 1.0f;
	}
}

