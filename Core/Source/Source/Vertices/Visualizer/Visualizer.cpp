#include "Visualizer.h"

// Globals
#include "Globals.h"

// Shader
#include "Class/Render/Shader/Shader.h"

void Vertices::Visualizer::visualizePoint(float xPos, float yPos, float size, glm::vec4 color)
{
	// Set Shader
	Global::colorShaderStatic.Use();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	//glUniform4fv(Global::brightnessLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	// Temp VAO and VBO
	static GLuint VAO, VBO;
	static bool first = true;

	// If first execution of function, generate VAO and VBO
	if (first)
	{
		// Generate Objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		first = false;

		// Set Location of Vertices
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Generate Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, size, size, color, vertices);

	// Bind Buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Draw Object
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unbind Buffer Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertices::Visualizer::visualizePoint(glm::vec2 position, float size, glm::vec4 color)
{
	visualizePoint(position.x, position.y, size, color);
}

void Vertices::Visualizer::visualizeLine(float x1, float y1, float x2, float y2, float size, glm::vec4 color)
{
	// Set Shader
	Global::colorShaderStatic.Use();
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(Global::brightnessLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

	// Temp VAO and VBO
	static GLuint VAO, VBO;
	static bool first = true;

	// If first execution of function, generate VAO and VBO
	if (first)
	{
		// Generate Objects
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		first = false;

		// Set Location of Vertices
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Generate Vertices
	float vertices[42];
	Vertices::Line::genLineColor(x1, x2, y1, y2, -0.5f, size, color, vertices);

	// Bind Buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Draw Object
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unbind Buffer Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Vertices::Visualizer::visualizeLine(glm::vec2 point1, glm::vec2 point2, float size, glm::vec4 color)
{
	visualizeLine(point1.x, point1.y, point2.x, point2.y, size, color);
}

void Vertices::Visualizer::visualizeRay(float x, float y, float theta, float size, glm::vec4 color)
{
	visualizeLine(glm::vec2(x, y), glm::vec2(x, y) + 100.0f * glm::vec2(cos(theta), sin(theta)), size, color);
}

void Vertices::Visualizer::visualizeRay(glm::vec2 origin, float theta, float size, glm::vec4 color)
{
	visualizeLine(origin, origin + 100.0f * glm::vec2(cos(theta), sin(theta)), size, color);
}
