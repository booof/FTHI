#include "Slant.h"
#include "Render/Struct/DataClasses.h"

Object::Mask::Slant::Slant(SlantData& data_, glm::vec2& offset)
{
	// Store Data
	data = std::move(data_);
	data.position += offset;
	data.position2 += offset;

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
		0.0f,                               0.0f,                               -1.6f,  color.r, color.g, color.b, color.a,
		data.position2.x - data.position.x, data.position2.y - data.position.y, -1.6f,  color.r, color.g, color.b, color.a
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

void DataClass::Data_Slant::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	data.position2.x += deltaX;
	data.position2.y += deltaY;
}

Object::Mask::SlantData& DataClass::Data_Slant::getSlantData()
{
	return data;
}

int& DataClass::Data_Slant::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Slant::getPosition()
{
	return data.position;
}

void DataClass::Data_Slant::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.position2 = position + glm::vec2(size, 0.0f);
	data.script = 0;
	data.material = 0;
}


void DataClass::Data_Slant::offsetPosition(glm::vec2& offset)
{
	// Update the Positions of Both Bvertices
	data.position += offset;
	data.position2 += offset;
}

void DataClass::Data_Slant::offsetOppositePosition(glm::vec2& offset)
{
	// Only Update Second Position
	data.position2 += offset;
}

void DataClass::Data_Slant::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Position2 is at Index 3
	*position2 = &data.position2;
	index2 = 3;

	// Position 3 Not Important
	position3Null(index3, position3);
}
