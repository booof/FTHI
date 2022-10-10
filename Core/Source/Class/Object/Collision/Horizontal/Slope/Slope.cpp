#include "Slope.h"

// Line Vertex Generators
#include "Source/Vertices/Line/LineVertices.h"

#include "Source/Algorithms/Quick Math/QuickMath.h"

Object::Mask::Slope::Slope(SlopeData& data_)
{
	// Store Data
	data = std::move(data_);

	// Calculate X Positions of Edges
	float half_width = abs(data.width * 0.5f);
	x_left = data.position.x - half_width;
	x_right = data.position.x + half_width;

	// Calculate Slope of Object
	slope = (float)tan(1) / half_width;

	// Calculate Sign of the Height of Object
	height_sign = Algorithms::Math::getSign(data.height);

	// Calculate Half Height
	half_height = data.height * 0.5f;
}

#ifdef EDITOR

void Object::Mask::Slope::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color)
{
	// Calculate View Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Get Vertices
	float vertices[154];
	Vertices::Line::genLineSimplifiedCurve1(0, 0, -3.0f, data.height, data.width, color, 11, vertices);

	// Initialize VBO and VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Array Object
	glBindVertexArray(VAO);

	// Bind Buffer Object and Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Assign Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Array Object
	glBindVertexArray(0);
}

#endif

