#include "Slant.h"

Object::Mask::Slant::Slant(SlantData& data_)
{
	// Store Data
	data = std::move(data_);

	// Calculate Slope of Linear Function
	slope = (data.position2.y - data.position.y) / (data.position2.x - data.position.x);

	// Calculate Y-Intercept of Linear Function
	intercept = data.position.y - (slope * data.position.x);
}

#ifdef EDITOR

void Object::Mask::Slant::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color)
{
	// Calculate View Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Vertices
	float vertices[] =
	{
		0.0f,                               0.0f,                               -3.0f,  color.r, color.g, color.b, color.a,
		data.position2.x - data.position.x, data.position2.y - data.position.y, -3.0f,  color.r, color.g, color.b, color.a
	};

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
