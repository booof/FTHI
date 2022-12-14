#include "VerticalLine.h"

Object::Mask::VerticalLine::VerticalLine(VerticalLineData data_)
{
	// Store Data
	data = std::move(data_);

	// Find Upper Y Position
	y_top = data.position.y + data.height;
}

void Object::Mask::VerticalLine::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color)
{
	// Calculate Model Matrix
	float half_height = data.height * 0.5f;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Vertices
	float vertices[] =
	{
		0.0f, 0.0f + half_height, -3.0f, color.r, color.g, color.b, color.a,
		0.0f, 0.0f - half_height, -3.0f, color.r, color.g, color.b, color.a
	};

	// Initialize VBO and VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Bind Vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Assign Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
