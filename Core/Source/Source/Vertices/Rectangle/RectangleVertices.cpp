#include "RectangleVertices.h"

void Vertices::Rectangle::genRectObject(glm::vec2 position, float width, float height, int data_index, float* vertices)
{
	// Half Values
	float half_width = width * 0.5f;
	float half_height = height * 0.5f;

	// Store Position Data
	vertices[0] = position.x - half_width;
	vertices[1] = position.y - half_height;
	vertices[5] = position.x + half_width;
	vertices[6] = position.y - half_height;
	vertices[10] = position.x + half_width;
	vertices[11] = position.y + half_height;
	vertices[15] = position.x + half_width;
	vertices[16] = position.y + half_height;
	vertices[20] = position.x - half_width;
	vertices[21] = position.y + half_height;
	vertices[25] = position.x - half_width;
	vertices[26] = position.y - half_height;

	// Store Texture Data
	vertices[2] = 0.0f;
	vertices[3] = 0.0f;
	vertices[7] = 1.0f;
	vertices[8] = 0.0f;
	vertices[12] = 1.0f;
	vertices[13] = 1.0f;
	vertices[17] = 1.0f;
	vertices[18] = 1.0f;
	vertices[22] = 0.0f;
	vertices[23] = 1.0f;
	vertices[27] = 0.0f;
	vertices[28] = 0.0f;

	// Store Data Index
	for (int i = 4; i < 30; i += 5)
	{
		vertices[i] = (float)data_index;
	}
}

void Vertices::Rectangle::genRectObjectFull(glm::vec2 position, float zPos, float width, float height, glm::vec4 color, glm::vec3 normal, float* vertices)
{
	// Half Values
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position Data
	vertices[0] = position.x - half_width;
	vertices[1] = position.y - half_height;
	vertices[12] = position.x + half_width;
	vertices[13] = position.y - half_height;
	vertices[24] = position.x + half_width;
	vertices[25] = position.y + half_height;
	vertices[36] = position.x + half_width;
	vertices[37] = position.y + half_height;
	vertices[48] = position.x - half_width;
	vertices[49] = position.y + half_height;
	vertices[60] = position.x - half_width;
	vertices[61] = position.y - half_height;

	// Store Texture Data
	vertices[10] = -1.0f;
	vertices[11] = -1.0f;
	vertices[22] = 1.0f;
	vertices[23] = -1.0f;
	vertices[34] = 1.0f;
	vertices[35] = 1.0f;
	vertices[46] = 1.0f;
	vertices[47] = 1.0f;
	vertices[58] = -1.0f;
	vertices[59] = 1.0f;
	vertices[70] = -1.0f;
	vertices[71] = -1.0f;

	// Store Z-Pos and Colors
	for (int i = 2; i < 72; i += 12)
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

void Vertices::Rectangle::genRectColor(float xPos, float yPos, float zPos, float width, float height, glm::vec4 color, float* vertices)
{
	// Create Half Values of Width and Height to Make Calculations Easier
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position Values
	vertices[0] = xPos - half_width;
	vertices[1] = yPos - half_height;
	vertices[7] = xPos + half_width;
	vertices[8] = yPos - half_height;
	vertices[14] = xPos + half_width;
	vertices[15] = yPos + half_height;
	vertices[21] = xPos + half_width;
	vertices[22] = yPos + half_height;
	vertices[28] = xPos - half_width;
	vertices[29] = yPos + half_height;
	vertices[35] = xPos - half_width;
	vertices[36] = yPos - half_height;

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

void Vertices::Rectangle::genRectColorNormal(float xPos, float yPos, float zPos, float width, float height, glm::vec4 color, glm::vec3 normal, float* vertices)
{
	// Create Half Values of Width and Height to Make Calculations Easier
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position Values
	vertices[0] = xPos - half_width;
	vertices[1] = yPos - half_height;
	vertices[10] = xPos + half_width;
	vertices[11] = yPos - half_height;
	vertices[20] = xPos + half_width;
	vertices[21] = yPos + half_height;
	vertices[30] = xPos + half_width;
	vertices[31] = yPos + half_height;
	vertices[40] = xPos - half_width;
	vertices[41] = yPos + half_height;
	vertices[50] = xPos - half_width;
	vertices[51] = yPos - half_height;

	// Store Z-Pos, Colors, and Normals
	for (int i = 2; i < 60; i += 10)
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

void Vertices::Rectangle::genRectTexture(float xPos, float yPos, float zPos, float width, float height, float* vertices)
{
	// Create Half Values of Width and Height to Make Calculations Easier
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position Vertices
	vertices[0] = xPos - half_width;
	vertices[1] = yPos - half_height;
	vertices[5] = xPos + half_width;
	vertices[6] = yPos - half_height;
	vertices[10] = xPos + half_width;
	vertices[11] = yPos + half_height;
	vertices[15] = xPos + half_width;
	vertices[16] = yPos + half_height;
	vertices[20] = xPos - half_width;
	vertices[21] = yPos + half_height;
	vertices[25] = xPos - half_width;
	vertices[26] = yPos - half_height;

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

void Vertices::Rectangle::genRectTextureNormal(float xPos, float yPos, float zPos, float width, float height, glm::vec3 normal, float* vertices)
{
	// Calculate Half Values of Width and Height to Make Calculations Easier
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position Vertices
	vertices[0] = xPos - half_width;
	vertices[1] = yPos - half_height;
	vertices[8] = xPos + half_width;
	vertices[9] = yPos - half_height;
	vertices[16] = xPos + half_width;
	vertices[17] = yPos + half_height;
	vertices[24] = xPos + half_width;
	vertices[25] = yPos + half_height;
	vertices[32] = xPos - half_width;
	vertices[33] = yPos + half_height;
	vertices[40] = xPos - half_width;
	vertices[41] = yPos - half_height;

	// Store Texture Coordinates
	vertices[6] = -1.0f;
	vertices[7] = -1.0f;
	vertices[14] = 1.0f;
	vertices[15] = -1.0f;
	vertices[22] = 1.0f;
	vertices[23] = 1.0f;
	vertices[30] = 1.0f;
	vertices[31] = 1.0f;
	vertices[38] = -1.0f;
	vertices[39] = 1.0f;
	vertices[46] = -1.0f;
	vertices[47] = -1.0f;

	// Store Z-Pos and Normals
	for (int i = 2; i < 48; i += 8)
	{
		vertices[i] = zPos;
		vertices[i + 1] = normal.x;
		vertices[i + 2] = normal.y;
		vertices[i + 3] = normal.z;
	}
}

void Vertices::Rectangle::genRectHilighter(float xPos, float yPos, float zPos, float width, float height, float* vertices)
{
	// Calculate Half Values of Width and Height to Make Calculations Easier
	float half_width = width / 2.0f;
	float half_height = height / 2.0f;

	// Store Position
	vertices[0] = xPos - half_width;
	vertices[1] = yPos - half_height;
	vertices[7] = xPos + half_width;
	vertices[8] = yPos - half_height;
	vertices[14] = xPos + half_width;
	vertices[15] = yPos - half_height;
	vertices[21] = xPos + half_width;
	vertices[22] = yPos + half_height;
	vertices[28] = xPos + half_width;
	vertices[29] = yPos + half_height;
	vertices[35] = xPos - half_width;
	vertices[36] = yPos + half_height;
	vertices[42] = xPos - half_width;
	vertices[43] = yPos + half_height;
	vertices[49] = xPos - half_width;
	vertices[50] = yPos - half_height;

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
