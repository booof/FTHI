// Modifies Vertices of Objects

// Changes the Size of an Object Through a Scale Factor
void Quad_Dialate(GLuint VAO, GLuint VBO, float ScaleFactor, bool textured, bool normalized, float zPos)
{
	// Data for Vertices if it is Colored or Textured
	int vertexCount = (textured) ? 30 : 36;
	int vertexLength = (textured) ? 5 : 6;
	if (normalized) { vertexLength += 3; vertexCount += 18; }

	// Bind Buffer Data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Storage of Vertices
	float vertices[54];

	// Return Buffer Data
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// Apply Dialation to Vertices
	for (int i = 0; i < vertexCount; i++)
	{
		// Change only the X and Y Vertices
		if (i % vertexLength == 0 || i % vertexLength == 1)
		{
			// Apply Scale Factor
			vertices[i] *= ScaleFactor;
		}

		// Set Z Value
		else if (i % vertexLength == 2)
		{
			vertices[i] = zPos;
		}
	}

	// Send New Buffer Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// Unbind Buffer Data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Changes Color Vertices to Texture Vertices
void Quad_Color_To_Texture(GLuint VAO, GLuint VBO, bool normalized)
{
	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Storage For Old Vertices
	float verticesOld[54];
	float verticesOld_size = normalized ? 54 * sizeof(GL_FLOAT) : 36 * sizeof(GL_FLOAT);

	// Temporary Storage For New Vertices
	float verticesNew[48];
	float verticesNew_size = normalized ? 48 * sizeof(GL_FLOAT) : 30 * sizeof(GL_FLOAT);

	// How Much to Shift Until Next Layout
	float vertexOffset = normalized ? 8 : 5;

	// Texture Coordinates
	float verticesTexture[12] =
	{
		0.0f, 0.0f,  // Bottom Left
		1.0f, 0.0f,	 // Bottom Right
		1.0f, 1.0f,	 // Top Right

		1.0f, 1.0f,	 // Top Right
		0.0f, 1.0f,	 // Top Left
		0.0f, 0.0f	 // Bottom Left
	};

	// Several Counting Variables Used During Calculations
	int positionCount = 0;
	int textureCount = 0;

	// Return Buffer Data
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, verticesOld_size, verticesOld);

	// Iterate Between Each Vertex and Apply Texture Coordinates
	for (int i = 0; i < vertexOffset * 6; i += vertexOffset)
	{
		// Position Coordinates
		verticesNew[i] = verticesOld[positionCount];
		verticesNew[i + 1] = verticesOld[positionCount + 1];
		verticesNew[i + 2] = verticesOld[positionCount + 2];

		// Texture Coordinates
		verticesNew[i + 3] = verticesTexture[textureCount];
		verticesNew[i + 4] = verticesTexture[textureCount + 1];

		// Normalized Coordinates
		if (normalized)
		{
			verticesNew[i + 5] = verticesOld[positionCount + 5];
			verticesNew[i + 6] = verticesOld[positionCount + 6];
			verticesNew[i + 7] = verticesOld[positionCount + 7];
		}

		// Shift Counting Variables For Next Iteration
		positionCount += vertexOffset + 1;
		textureCount += 2;
	}

	// Send New Vertices to Buffer
	glBufferData(GL_ARRAY_BUFFER, verticesNew_size, verticesNew, GL_STATIC_DRAW);

	// Send New Position Data to Shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send New Texture Data to Shaders
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Send New Normalized Data to Shaders
	if (normalized)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Changes Texture Vertices to Color Vertices
void Quad_Texture_To_Color(GLuint VAO, GLuint VBO, bool normalized, glm::vec3 color)
{
	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Storage For Old Vertices
	float verticesOld[48];
	float verticesOld_size = normalized ? 48 * sizeof(GL_FLOAT) : 30 * sizeof(GL_FLOAT);

	// Temporary Storage For New Vertices
	float verticesNew[54];
	float verticesNew_size = normalized ? 54 * sizeof(GL_FLOAT) : 36 * sizeof(GL_FLOAT);

	// How Much to Shift Until Next Layout
	float vertexOffset = normalized ? 9 : 6;

	// Unpack Color Data
	float R = color.x;
	float G = color.y;
	float B = color.z;

	// Get Object Coordinates and Save them in Old Vertices Array
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, verticesOld_size, verticesOld);

	// Offset For Calculating Position Vertices
	int positionCount = 0;

	// Iterate Between Each Coordinate and Apply New Color Coordinates
	for (int i = 0; i < vertexOffset * 6; i += vertexOffset)
	{
		// Assign Position Vertices
		verticesNew[i] = verticesOld[positionCount];
		verticesNew[i + 1] = verticesOld[positionCount + 1];
		verticesNew[i + 2] = verticesOld[positionCount + 2];

		// Assign Color Vertices
		verticesNew[i + 3] = R;
		verticesNew[i + 4] = G;
		verticesNew[i + 5] = B;

		// Normalized Coordinates
		if (normalized)
		{
			verticesNew[i + 6] = verticesOld[positionCount + 6];
			verticesNew[i + 7] = verticesOld[positionCount + 7];
			verticesNew[i + 8] = verticesOld[positionCount + 8];
		}

		// Update Position Offset
		positionCount += vertexOffset - 1;
	}

	// Send New Vertices to Buffer
	glBufferData(GL_ARRAY_BUFFER, verticesNew_size, verticesNew, GL_STATIC_DRAW);

	// Send New Position Data to Shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send New Color Data to Shaders
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Send New Normalized Data to Shaders
	if (normalized)
	{
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexOffset * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Changes The Size of an Object Through a Scale Factor
void Tri_Dialate(GLuint VAO, GLuint VBO, float ScaleFactor, bool textured, bool normalized, float zPos)
{
	// Data for Vertices if it is Colored or Textured
	int vertexCount = (textured) ? 15 : 18;
	int vertexLength = (textured) ? 5 : 6;
	if (normalized) { vertexLength += 3; vertexCount += 9; }

	// Bind Buffer Data
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Storage for Vertices
	float vertices[27];

	// Return Buffer Data
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GLfloat), vertices);

	// Apply Dialation to Vertices
	for (int i = 0; i < vertexCount; i++)
	{
		// Change X and Y Vertices Only
		if (i % vertexLength == 0 || i % vertexLength == 1)
		{
			// Apply Scale Factor
			vertices[i] *= ScaleFactor;
		}

		// Set Z Value
		else if (i % vertexLength == 2)
		{
			vertices[i] = zPos;
		}
	}

	// Send New Buffer Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GLfloat), vertices);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Determine Order of Triange Vertices
void Tri_ArrangeVertices(glm::vec2& coords1, glm::vec2& coords2, glm::vec2& coords3)
{
	int highest;
	int lowest;

	// Test if coords1 is Highest
	if (coords1.y >= coords2.y && coords1.y >= coords3.y)
	{
		highest = 1;

		// Test if coords2 is lowest
		if (coords2.y <= coords3.y)
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
	else if (coords2.y > coords1.y && coords2.y >= coords3.y)
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
		if (coords1.y <= coords2.y)
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

// Test Collisions of Trapeziods
bool Trap_TestCollisions(float xPos, float yPos, float width, float height, float widthOffset, float heightOffset)
{
	// Calculate Half Values
	float halfWidth = width / 2;
	float halfHeight = height / 2;

	// Values of Corners
	glm::vec2 bottom_left = glm::vec2(xPos - halfWidth, yPos - halfHeight);
	glm::vec2 bottom_right = glm::vec2(xPos + halfWidth, yPos - halfHeight + heightOffset);
	glm::vec2 top_right = glm::vec2(xPos + halfWidth + widthOffset, yPos + halfHeight + heightOffset);
	glm::vec2 top_left = glm::vec2(xPos - halfWidth + widthOffset, yPos + halfHeight);

	// Calculate Relative Values
	float relative_y_bottom = ((bottom_left.y - bottom_right.y) / (bottom_left.x - bottom_right.x)) * (mouseRelativeX - bottom_left.x) + bottom_left.y;
	float relative_x_right = ((bottom_right.x - top_right.x) / (bottom_right.y - top_right.y)) * (mouseRelativeY - bottom_right.y) + bottom_right.x;
	float relative_y_top = ((top_right.y - top_left.y) / (top_right.x - top_left.x)) * (mouseRelativeX - top_right.x) + top_right.y;
	float relative_x_left = ((top_left.x - bottom_left.x) / (top_left.y - bottom_left.y)) * (mouseRelativeY - top_left.y) + top_left.x;

	// Test Collisions
	if (mouseRelativeY >= relative_y_bottom && mouseRelativeX <= relative_x_right && mouseRelativeY <= relative_y_top && mouseRelativeX >= relative_x_left)
	{
		return true;
	}

	// Return False if no Collisions Occoured
	return false;
}

// Test Collisions of Triangles
bool Tri_TestCollisions(glm::vec2 coords1, glm::vec2 coords2, glm::vec2 coords3)
{
	// Test Collisions if coords2 is to the Right of The Rest of the Triangle
	if (coords2.x >= coords1.x && coords2.x >= coords3.x)
	{
		// Inverse Line Between Highest and Lowest Vertex
		float m = (coords1.x - coords3.x) / (coords1.y - coords3.y);
		float b = coords1.x - m * coords1.y;

		// Test if mouseX is Inside Triangle
		if (m * mouseRelativeY + b <= mouseRelativeX && mouseRelativeX <= coords2.x)
		{
			// Line Between Lowest and Middle Vertex
			m = (coords2.y - coords3.y) / (coords2.x - coords3.x);
			b = coords2.y - m * coords2.x;

			// Test if mouseY is Above the Line
			if (m * mouseRelativeX + b <= mouseRelativeY)
			{
				// Line Between Highest and Middle Vertex
				m = (coords2.y - coords1.y) / (coords2.x - coords1.x);
				b = coords2.y - m * coords2.x;

				// Test if mouseY is Bellow the Line
				if (m * mouseRelativeX + b >= mouseRelativeY)
				{
					return true;
				}
			}
		}
	}

	// Test Collisions if coords2 is to the Left of the Rest of the Triangle
	else if (coords2.x < coords1.x && coords2.x < coords3.x)
	{
		// Inverse Line Between Highest and Lowest Vertex
		float m = (coords1.x - coords3.x) / (coords1.y - coords3.y);
		float b = coords1.x - m * coords1.y;

		// Test if mouseX is Inside Triangle
		if (m * mouseRelativeY + b >= mouseRelativeX && mouseRelativeX >= coords2.x)
		{
			// Line Between Lowest and Middle Vertex
			m = (coords2.y - coords3.y) / (coords2.x - coords3.x);
			b = coords2.y - m * coords2.x;

			// Test if mouseY is Above the Line
			if (m * mouseRelativeX + b <= mouseRelativeY)
			{
				// Line Between Highest and Middle Vertex
				m = (coords2.y - coords1.y) / (coords2.x - coords1.x);
				b = coords2.y - m * coords2.x;

				// Test if mouseY is Bellow the Line
				if (m * mouseRelativeX + b >= mouseRelativeY)
				{
					return true;
				}
			}
		}
	}

	// Test Collisions if coords2 is Between coords1 and coords3
	else
	{
		// Test if mouseX is Inside Triangle
		if ((coords1.x < mouseRelativeX && coords3.x > mouseRelativeX) || (coords3.x < mouseRelativeX && coords1.x > mouseRelativeX))
		{
			// Line Between Highest and Lowest Vertex
			float m = (coords1.y - coords3.y) / (coords1.x - coords3.x);
			float b = coords1.y - m * coords1.x;

			// Test if coords2 is Above the Line
			if (m * coords2.x + b <= coords2.y)
			{
				// Test if mouseY is Above the Line
				if (m * mouseRelativeX + b <= mouseRelativeY)
				{
					// Line Between Lowest and Middle Vertex
					m = (coords2.y - coords3.y) / (coords2.x - coords3.x);
					b = coords2.y - m * coords2.x;

					// Test if mouseY is Below the Line
					if (m * mouseRelativeX + b >= mouseRelativeY)
					{
						// Line Between Highest and Middle Vertex
						m = (coords2.y - coords1.y) / (coords2.x - coords1.x);
						b = coords2.y - m * coords2.x;

						// Test if mouseY is Below the Line
						if (m * mouseRelativeX + b >= mouseRelativeY)
						{
							return true;
						}
					}
				}
			}

			// Test if coords2 is Below the Line
			else
			{
				// Test if mouseY is Below the Line
				if (m * mouseRelativeX + b >= mouseRelativeY)
				{
					// Line Between Lowest and Middle Vertex
					m = (coords2.y - coords3.y) / (coords2.x - coords3.x);
					b = coords2.y - m * coords2.x;

					// Test if mouseY is Above the Line
					if (m * mouseRelativeX + b <= mouseRelativeY)
					{
						// Line Between Highest and Middle Vertex
						m = (coords2.y - coords1.y) / (coords2.x - coords1.x);
						b = coords2.y - m * coords2.x;

						// Test if mouseY is Above the Line
						if (m * mouseRelativeX + b <= mouseRelativeY)
						{
							return true;
						}
					}
				}
			}
		}
	}

	// Return False if no Collisions Occoured
	return false;
}

// Swap Colors of an Object
void SwapColors(GLuint VAO, GLuint VBO, glm::vec3 Color, bool rectangular, bool normalized)
{
	// Determine the Number of Vertices of Object
	int vertexCount = rectangular ? 36 : 18;
	if (normalized) { vertexCount += vertexCount / 2; }

	// Determine The Number of Vertices Until Next Layout
	int vertexOffset = normalized ? 9 : 6;

	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Vertices Object
	float vertices[54];

	// Get Vertex Data From Object
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// For Every Color Vertex, Exchange Color
	for (int i = 3; i < vertexCount; i += vertexOffset)
	{
		vertices[i] = Color.x;
		vertices[i + 1] = Color.y;
		vertices[i + 2] = Color.z;
	}

	// Send Modified Data Back to Object
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// Swap Normals of an Object
void SwapNormals(GLuint VAO, GLuint VBO, glm::vec3 Normal, bool rectangular, bool textured)
{
	// Determine the Number of Vertices of Object
	int vertexCount = textured ? 48 : 54;
	if (!rectangular) { vertexCount /= 2; }

	// Determine The Number of Vertices Until Next Layout
	int vertexOffset = textured ? 8 : 9;

	// Location Where Normals Start
	int normalsOffset = textured ? 5 : 6;

	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Temporary Vertices Object
	float vertices[54];

	// Get Vertex Data From Object
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// For Every Color Vertex, Exchange Normals
	for (int i = normalsOffset; i < vertexCount; i += vertexOffset)
	{
		vertices[i] = Normal.x;
		vertices[i + 1] = Normal.y;
		vertices[i + 2] = Normal.z;
	}

	// Send Modified Data Back to Offset
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * sizeof(GL_FLOAT), vertices);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
