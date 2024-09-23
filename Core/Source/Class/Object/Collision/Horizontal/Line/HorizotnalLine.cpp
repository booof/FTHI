#include "HorizotnalLine.h"
#include "Render/Struct/DataClasses.h"
#include "FloorMaskLine.h"
#include "CeilingMaskLine.h"

Object::Mask::HorizontalLine::HorizontalLine(HorizontalLineData& data_, glm::vec2& offset)
{
	// Store Data
	data = std::move(data_);
	data.position += offset;

	// Calculate Left and Right Vertex X Position
	float half_width = data.width * 0.5f;
	x_left = data.position.x - half_width;
	x_right = data.position.x + half_width;
}

#ifdef EDITOR

void Object::Mask::HorizontalLine::initializeVisualizer(GLuint& VAO, GLuint& VBO, glm::mat4& model, glm::vec4 color)
{
	// Calculate View Matrix
	float half_width = data.width * 0.5f;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
	
	// Vertices
	float vertices[] =
	{
		0.0f - half_width, 0.0f, -1.6f,  color.r, color.g, color.b, color.a,
		0.0f + half_width, 0.0f, -1.6f,  color.r, color.g, color.b, color.a,
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

void DataClass::Data_HorizontalLine::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
}

Object::Mask::HorizontalLineData& DataClass::Data_HorizontalLine::getHorizontalLineData()
{
	return data;
}

int& DataClass::Data_HorizontalLine::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_HorizontalLine::getPosition()
{
	return data.position;
}

void DataClass::Data_HorizontalLine::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size;
	data.script = 0;
	data.material = 0;
}

void DataClass::Data_HorizontalLine::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}
