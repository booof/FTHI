#include "Curve.h"

// Line Vertex Generators
#include "Source/Vertices/Line/LineVertices.h"

Object::Mask::Curve::Curve(CurveData& data_)
{
	// Store Data
	data = std::move(data_);

	// Calculate Moddified Coordinates
	modified_coordinates = glm::vec2(data.position.x - data.width / 2.0f, data.position.y - data.height / 2.0f);

	// Calculte Top Y Position of Curve
	y_top = modified_coordinates.y + data.height;

	// Calcualte Slope of Curve
	slope = data.height / 6.0f;

	// Calculate Amplitude of Curve
	amplitude = (1.0f + (6.0f / data.width));

	// Calculate X Offset
	x_offset = (21.0f * data.width - 25.0f) / 30.0f;
}

#ifdef EDITOR

void Object::Mask::Curve::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color, char sign)
{
	// Calculate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Get Vertices
	float vertices[154];
	Vertices::Line::genLineSimplifiedCurve2(-data.width / 2 * sign, -data.height / 2, -3.0f, data.width, slope, amplitude, x_offset, sign, color, 11, vertices);

	// Initialize VAO and VBO
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

#endif

