#include "ObjectBuffers.h"
#include "Constants.h"

void Vertices::Buffer::genObjectVAO(GLuint& VAO, GLuint& VBO, int total_number_of_vertices)
{
	// Generate Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocate Memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 5 * total_number_of_vertices, NULL, GL_DYNAMIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Enable Data Index
	glVertexAttribIPointer(2, 1, GL_INT, 5 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertices::Buffer::genBuffers(GLuint instance_buffer, GLuint& directional_buffer, GLuint& point_buffer, GLuint& spot_buffer, GLuint& beam_buffer)
{
	// Object Data Buffer
	glGenBuffers(1, &instance_buffer);

	// Light Buffer
	glGenBuffers(1, &directional_buffer);
	glGenBuffers(1, &point_buffer);
	glGenBuffers(1, &spot_buffer);
	glGenBuffers(1, &beam_buffer);
}

void Vertices::Buffer::clearObjectVAO(GLuint& VAO, GLuint& VBO, int total_number_of_vertices)
{
	// Bind Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set Buffer Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * total_number_of_vertices, NULL, GL_DYNAMIC_DRAW);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertices::Buffer::clearObjectDataBuffer(GLuint& buffer, int instances)
{
	// Bind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);

	// Set Buffer Data
	glBufferData(GL_SHADER_STORAGE_BUFFER, Constant::INSTANCE_SIZE * instances, NULL, GL_DYNAMIC_DRAW);

	// Bind Buffer Index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffer);

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Vertices::Buffer::clearLightBuffer(GLuint& directional_buffer, GLuint& point_buffer, GLuint& spot_buffer, GLuint& beam_buffer, int directional, int point, int spot, int beam)
{
	// Bind Directional Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, directional_buffer);

	// Set Directional Data
	glBufferData(GL_SHADER_STORAGE_BUFFER, 96 * directional + 16, NULL, GL_DYNAMIC_DRAW);

	// Store Number of Directional Objects
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &directional);

	// Bind Directional Buffer Index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, directional_buffer);

	// Bind Point Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, point_buffer);

	// Set Point Data
	glBufferData(GL_SHADER_STORAGE_BUFFER, 80 * point + 16, NULL, GL_DYNAMIC_DRAW);

	// Store Number of Point Objects
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &point);

	// Bind Point Buffer Index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, point_buffer);

	// Bind Spot Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, spot_buffer);

	// Set Spot Data
	glBufferData(GL_SHADER_STORAGE_BUFFER, 96 * spot + 16, NULL, GL_DYNAMIC_DRAW);

	// Store Number of Spot Objects
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &spot);

	// Bind Spot Buffer Index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, spot_buffer);

	// Bind Beam Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, beam_buffer);
	
	// Set Beam Data
	glBufferData(GL_SHADER_STORAGE_BUFFER, 96 * beam + 16, NULL, GL_DYNAMIC_DRAW);

	// Store Number of Beam Objects
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &beam);

	// Bind Beam Buffer Index
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, beam_buffer);

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

