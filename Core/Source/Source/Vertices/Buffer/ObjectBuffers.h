#pragma once
#ifndef OBJECT_BUFFERS_H
#define OBJECT_BUFFERS_H

#include "Source/Vertices/Vertices.h"

namespace Vertices::Buffer
{
	// Establish Object Vertex Array Object
	void genObjectVAO(GLuint& VAO, GLuint& VBO, int total_number_of_vertices);

	// Establish Buffers
	void genBuffers(GLuint instance_buffer, GLuint& directional_buffer, GLuint& point_buffer, GLuint& spot_buffer, GLuint& beam_buffer);

	// Clear Object Vertex Array Object
	void clearObjectVAO(GLuint& VAO, GLuint& VBO, int total_number_of_vertices);

	// Clear Object Data Buffer
	void clearObjectDataBuffer(GLuint& buffer, int instances);

	// Clear Light Buffer
	void clearLightBuffer(GLuint& directional_buffer, GLuint& point_buffer, GLuint& spot_buffer, GLuint& beam_buffer, int directional, int point, int spot, int beam);
}

#endif
