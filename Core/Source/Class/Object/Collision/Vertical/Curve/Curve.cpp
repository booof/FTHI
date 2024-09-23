#include "Curve.h"
#include "Render/Struct/DataClasses.h"

// Line Vertex Generators
#include "Source/Vertices/Line/LineVertices.h"

Object::Mask::Curve::Curve(CurveData& data_, glm::vec2& offset)
{
	// Store Data
	data = std::move(data_);
	data.position += offset;

	// Calculate Moddified Coordinates
	modified_coordinates = glm::vec2(data.position.x - data.width / 2.0f, data.position.y - data.height / 2.0f);

	// Calculte Top Y Position of Curve
	y_top = modified_coordinates.y + data.height;

	// Calcualte Slope of Curve
	slope = data.height / 6.0f;

	// Calculate Amplitude of Curve
	amplitude = (1.0f + (6.0f / data.width));

	// Calculate X Offset
	x_offset = (21.0f * abs(data.width) - 25.0f) / 30.0f;
}

#ifdef EDITOR

void Object::Mask::Curve::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color, char sign)
{
	// Calculate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Get Vertices
	float vertices[154];
	Vertices::Line::genLineSimplifiedCurve2(-data.width / 2 * sign, -data.height / 2, -1.6f, data.width, slope, amplitude, x_offset, sign, color, 11, vertices);

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

void DataClass::Data_Curve::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
}

Object::Mask::CurveData& DataClass::Data_Curve::getCurveData()
{
	return data;
}

int& DataClass::Data_Curve::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Curve::getPosition()
{
	return data.position;
}

void DataClass::Data_Curve::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size;
	data.height = size;
	data.script = 0;
	data.material = 0;
}

void DataClass::Data_Curve::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}

