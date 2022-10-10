#include "CircleVertices.h"

void Vertices::Circle::genCircleObject(glm::vec2 position, float radius, int number_of_sides, int data_index, float* vertices)
{
	// Calculate Number of Floats
	int number_of_floats = number_of_sides * 15;

	// Calculate the Offset Angle
	float offset_angle = 6.2832f / number_of_sides;

	// Normalized Vertex Offets
	float current_angle;
	float normalized_x;
	float normalized_y;

	// Insert Origin In Vertices
	for (int i = 0; i < number_of_floats; i += 15)
	{
		vertices[i] = position.x;
		vertices[i + 1] = position.y;
		vertices[i + 2] = 0.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = (float)data_index;
	}

	// Calcualte Vertices and Texture Vertices Around Circle
	for (int i = 5, j = 1; i < number_of_floats; i += 15, j++)
	{
		// Calculate normalized offsets
		current_angle = j * offset_angle;
		normalized_x = cos(current_angle);
		normalized_y = sin(current_angle);

		// Insert First Vertices
		vertices[i] = position.x + radius * normalized_x;
		vertices[i + 1] = position.y + radius * normalized_y;
		vertices[i + 2] = normalized_x;
		vertices[i + 3] = normalized_y;
		vertices[i + 4] = (float)data_index;

		// Test if Second Vertices are Allowd
		if (i - 10 < 0)
		{
			// Insert Second Vertices at End
			vertices[number_of_floats - 5] = vertices[i];
			vertices[number_of_floats - 4] = vertices[i + 1];
			vertices[number_of_floats - 3] = normalized_x;
			vertices[number_of_floats - 2] = normalized_y;
			vertices[number_of_floats - 1] = (float)data_index;
		}

		else
		{
			// Insert Second Vertices
			vertices[i - 10] = vertices[i];
			vertices[i - 9] = vertices[i + 1];
			vertices[i - 8] = normalized_x;
			vertices[i - 7] = normalized_y;
			vertices[i - 6] = (float)data_index;
		}
	}
}

void Vertices::Circle::genCircleObjectFull(glm::vec2 position, float zPos, float radius, int number_of_sides, glm::vec4 color, glm::vec3 normal, float* vertices)
{
	// Calculate Number of Floats
	int number_of_floats = number_of_sides * 36;

	// Calculate the Offset Angle
	float offset_angle = 6.2832f / number_of_sides;

	// Normalized Vertex Offets
	float current_angle;
	float normalized_x;
	float normalized_y;

	// Insert Origin In Vertices
	for (int i = 0; i < number_of_floats; i += 36)
	{
		vertices[i] = position.x;
		vertices[i + 1] = position.y;
		vertices[i + 10] = 0.0f;
		vertices[i + 11] = 0.0f;
	}

	// Calcualte Vertices and Texture Vertices Around Circle
	for (int i = 12, j = 1; i < number_of_floats; i += 36, j++)
	{
		// Calculate normalized offsets
		current_angle = j * offset_angle;
		normalized_x = cos(current_angle);
		normalized_y = sin(current_angle);

		// Insert First Vertices
		vertices[i] = position.x + radius * normalized_x;
		vertices[i + 1] = position.y + radius * normalized_y;
		vertices[i + 10] = normalized_x;
		vertices[i + 11] = normalized_y;

		// Test if Second Vertices are Allowd
		if (i - 18 < 0)
		{
			// Insert Second Vertices at End
			vertices[number_of_floats - 12] = vertices[i];
			vertices[number_of_floats - 11] = vertices[i + 1];
			vertices[number_of_floats - 2] = normalized_x;
			vertices[number_of_floats - 1] = normalized_y;
		}

		else
		{
			// Insert Second Vertices
			vertices[i - 24] = vertices[i];
			vertices[i - 23] = vertices[i + 1];
			vertices[i - 14] = normalized_x;
			vertices[i - 13] = normalized_y;
		}
	}

	// Insert Z-Pos and Color Values
	for (int i = 2; i < number_of_floats; i += 12)
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

void Vertices::Circle::genCircleColorFull(float xPos, float yPos, float zPos, float radius, int number_of_sides, glm::vec4 color, float* vertices)
{
	// Calculate Number of Verticies
	int NumberOfVertices = number_of_sides * 21;

	// Calculate the Correct Version of PI
	float psi = 6.28319f;

	// Unpack Color Variables
	float R = color.x;
	float G = color.y;
	float B = color.z;
	float A = color.a;

	// Create X and Y Vertex Array
	float* VerticesX = new float[NumberOfVertices];
	float* VerticesY = new float[NumberOfVertices];

	// This is to Make the Compiler Happy
	VerticesX[0] = xPos + radius;
	VerticesY[0] = yPos + radius;

	// Calculate Each Vertex on Circle
	for (int i = 1; i < number_of_sides + 1; i++)
	{
		VerticesX[i - 1] = xPos + (radius * cos(i * psi / number_of_sides));
		VerticesY[i - 1] = yPos + (radius * sin(i * psi / number_of_sides));
	}

	// The Index that Determines Which Vertex is Being Calculated
	int VertexCount = 0;

	// Calculate Each Position Vertex Around Origin of Circle
	for (int i = 0; i < NumberOfVertices; i += 21)
	{
		// Assign Origin Coordinates
		vertices[i] = xPos;
		vertices[i + 1] = yPos;
		vertices[i + 2] = zPos;

		// Assign Previous Triangle Vertex
		vertices[i + 7] = VerticesX[VertexCount];
		vertices[i + 8] = VerticesY[VertexCount];
		vertices[i + 9] = zPos;

		// Assign New Triangle Vertex 
		if (VertexCount + 1 < number_of_sides)
		{
			vertices[i + 14] = VerticesX[VertexCount + 1];
			vertices[i + 15] = VerticesY[VertexCount + 1];
		}

		// If Vertex Index is Beyond Array Size, Restart From Begining of Array
		else
		{
			vertices[i + 14] = VerticesX[0];
			vertices[i + 15] = VerticesY[0];
		}

		// New Triangle zPos
		vertices[i + 16] = zPos;

		// Increase Vertex Index
		VertexCount++;
	}

	// Calculate Each Color Vertex Around Origin of Circle
	for (int i = 3; i < NumberOfVertices; i += 7)
	{
		vertices[i] = R;
		vertices[i + 1] = G;
		vertices[i + 2] = B;
		vertices[i + 3] = A;
	}

	// Delete Arrays
	delete[] VerticesX;
	delete[] VerticesY;
}

void Vertices::Circle::genCircleTextureFull()
{
	// lul how will this work?
}

void Vertices::Circle::genCircleColorHollow()
{

}

void Vertices::Circle::genCircleColorWheel(float xPos, float yPos, float zPos, float radius, int number_of_sides, float* vertices)
{
	// Calculate the Number of Vertices
	int NumberOfVertices = number_of_sides * 21;

	// Calculate the True Version of PI
	float psi = 6.28319f;

	// Calculate the Color Scale Factor
	float ColorScale = 1530.0f / (number_of_sides + 1);

	// Calculate Color Ratio
	float ColorRatio = 1 / 255.0f;

	// Create X and Y Vertex Arrays
	float* VerticesX = new float[number_of_sides];
	float* VerticesY = new float[number_of_sides];

	// Create R G B Vertex Arrays
	float** Colors = new float*[number_of_sides];
	for (int i = 0; i < number_of_sides; i++)
	{
		Colors[i] = new float[3];
	}

	// This is to Make the Compiler Happy
	VerticesX[0] = xPos + radius;
	VerticesY[0] = yPos + radius;

	// Color Mode to Calculate Colors
	int ColorMode = 0;

	// The Color Index
	float ColorCount = 0;

	// Calculate Each Vertex On Circle
	for (int i = 1; i < number_of_sides + 1; i++)
	{
		// Position Vertex
		VerticesX[i - 1] = xPos + (radius * cos(i * psi / number_of_sides));
		VerticesY[i - 1] = yPos + (radius * sin(i * psi / number_of_sides));

		// Color Vertex
		switch (ColorMode)
		{

		// Constant Red, Rising Green (0 to 255)
		case 0:
		{
			// Calculate Colors
			Colors[i - 1][0] = 1.0f;
			Colors[i - 1][1] = ColorCount * ColorRatio;
			Colors[i - 1][2] = 0.0f;

			// If Color Index Excedes Boundry, Increase Color Mode
			if (ColorCount > 255)
			{
				ColorMode++;
			}

			break;
		}

		// Constant Green, Falling Red (255 to 510)
		case 1:
		{
			// Calculate Colors
			Colors[i - 1][0] = (510 - ColorCount) * ColorRatio;
			Colors[i - 1][1] = 1.0f;
			Colors[i - 1][2] = 0.0f;

			// If Color Index Excedes Boundry, Increase Color Mode
			if (ColorCount > 510)
			{
				ColorMode++;
			}

			break;
		}

		// Constant Green, Rising Blue (510 to 765)
		case 2:
		{
			// Calculate Colors
			Colors[i - 1][0] = 0.0f;
			Colors[i - 1][1] = 1.0f;
			Colors[i - 1][2] = (ColorCount - 510) * ColorRatio;

			// If Color Index Excedes Boundry, Increase Color Mode
			if (ColorCount > 765)
			{
				ColorMode++;
			}

			break;
		}

		// Constant Blue, Falling Green (765 to 1020)
		case 3:
		{
			// Calculate Colors
			Colors[i - 1][0] = 0.0f;
			Colors[i - 1][1] = (1020 - ColorCount) * ColorRatio;
			Colors[i - 1][2] = 1.0f;

			// If Color Index Excedes Boundry, Increase Color Mode
			if (ColorCount > 1020)
			{
				ColorMode++;
			}

			break;
		}

		// Constant Blue, Rising Red (1020 to 1275)
		case 4:
		{
			// Calculate Colors
			Colors[i - 1][0] = (ColorCount - 1020) * ColorRatio;
			Colors[i - 1][1] = 0.0f;
			Colors[i - 1][2] = 1.0f;

			// If Color Index Excedes Boundry, Increase Color Mode
			if (ColorCount > 1275)
			{
				ColorMode++;
			}

			break;
		}

		// Constant Red, Falling Blue (1275 to 1530)
		case 5:
		{
			// Calculate Colors
			Colors[i - 1][0] = 1.0f;
			Colors[i - 1][1] = 0.0f;
			Colors[i - 1][2] = (1530 - ColorCount) * ColorRatio;

			break;
		}

		}

		// Update Color Count
		ColorCount += ColorScale;
	}

	// The Index that Determines Which Vertex Should be Calculated
	int VertexCount = 0;

	// Calculate Each Position Vertex Around Circle
	for (int i = 0; i < NumberOfVertices - 1; i += 21)
	{
		// Assign Origin Position Vertex
		vertices[i] = xPos;
		vertices[i + 1] = yPos;
		vertices[i + 2] = zPos;

		// Assign White Origin Color Vertex
		vertices[i + 3] = 1.0f;
		vertices[i + 4] = 1.0f;
		vertices[i + 5] = 1.0f;
		vertices[i + 6] = 1.0f;

		// Assign Previous Triangle Position Vertex
		vertices[i + 7] = VerticesX[VertexCount];
		vertices[i + 8] = VerticesY[VertexCount];
		vertices[i + 9] = zPos;

		// Assign Previous Triangle Color Vertex
		vertices[i + 10] = Colors[VertexCount][0];
		vertices[i + 11] = Colors[VertexCount][1];
		vertices[i + 12] = Colors[VertexCount][2];
		vertices[i + 13] = 1.0f;

		// Assign New Triangle Vertex
		if (VertexCount + 1 < number_of_sides)
		{
			// Assign New Triangle Position Vertex
			vertices[i + 14] = VerticesX[VertexCount + 1];
			vertices[i + 15] = VerticesY[VertexCount + 1];

			// Assign New Triangle Color Vertex
			vertices[i + 17] = Colors[VertexCount + 1][0];
			vertices[i + 18] = Colors[VertexCount + 1][1];
			vertices[i + 19] = Colors[VertexCount + 1][2];
		}

		// If Vertex Index is Beyond Array Size, Restart From Begining of Array
		else
		{
			// Assign First Triangle Position Vertex
			vertices[i + 14] = VerticesX[0];
			vertices[i + 15] = VerticesY[0];

			// Assign First Triangle Color Vertex
			vertices[i + 17] = Colors[0][0];
			vertices[i + 18] = Colors[0][1];
			vertices[i + 19] = Colors[0][2];
		}

		// Assign New Triangle zPos
		vertices[i + 16] = zPos;

		// Assign New Alpha Value
		vertices[i + 20] = 1.0f;

		// Update Vertex Index
		VertexCount++;
	}

	// Delete Sub-Arrays in Color Array
	for (int i = 0; i < number_of_sides; i++)
	{
		delete[] Colors[i];
	}

	// Delete Arrays
	delete[] VerticesX;
	delete[] VerticesY;
	delete[] Colors;
}

void Vertices::Circle::genCircleHighlighter(float xPos, float yPos, float zPos, float radius, int number_of_sides, float* vertices)
{
	// Calculate Number of Floats
	int number_of_floats = number_of_sides * 14;

	// Calculate the Offset Angle
	float offset_angle = 6.2832f / number_of_sides;

	// Normalized Vertex Offets
	float current_angle;
	float normalized_x;
	float normalized_y;

	// Calculate Vertex Positions
	for (int i = 0, j = 0; i < number_of_floats; i += 14, j++)
	{
		// Calculate normalized offsets
		current_angle = j * offset_angle;
		normalized_x = cos(current_angle);
		normalized_y = sin(current_angle);

		// Insert First Vertices
		vertices[i] = xPos + radius * normalized_x;
		vertices[i + 1] = yPos + radius * normalized_y;

		// Test if Second Vertices are Allowd
		if (i == 0)
		{
			// Insert Second Vertices at End
			vertices[number_of_floats - 7] = vertices[i];
			vertices[number_of_floats - 6] = vertices[i + 1];
		}

		else
		{
			// Insert Second Vertices
			vertices[i - 7] = vertices[i];
			vertices[i - 6] = vertices[i + 1];
		}
	}

	// Insert Z-Pos and Color Values
	for (int i = 2; i < number_of_floats; i += 7)
	{
		vertices[i] = zPos;
		vertices[i + 1] = 1.0f;
		vertices[i + 2] = 1.0f;
		vertices[i + 3] = 0.0f;
		vertices[i + 4] = 1.0f;
	}
}

