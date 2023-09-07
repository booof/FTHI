#include "SoftBody.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Visualizer/Visualizer.h"

// Editor Options
#include "Render/Editor/EditorOptions.h"

// Shader
#include "Render/Shader/Shader.h"

Object::Physics::Soft::SoftBody::~SoftBody()
{
	// Delete Nodes
	if (node_count)
		delete[] nodes;

	// Delete Springs
	if (spring_count)
		delete[] springs;
}

void Object::Physics::Soft::SoftBody::initializeVisualizer()
{
	// Create Visualizer for Nodes
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 1.0f, 1.0f, data.colors, vertices);

	// Generate Buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Store Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Object::Physics::Soft::SoftBody::updateObject()
{
	// Apply Forces
	applyForces();

	// Add Gravity
	applyGravity();

	// Add Spring Forces
	applySpring();
}

void Object::Physics::Soft::SoftBody::applyGravity()
{
	// Iterate Through Each Node
	for (int i = 0; i < node_count; i++)
	{
		// Apply force of weight
		nodes[i].Forces.y -= nodes[i].Mass * Constant::GRAVITATIONAL_ACCELERATION;

		// Apply terminal velocity if needed
		if (nodes[i].Velocity.y <= -Constant::TERMINALVELOCITY)
		{
			nodes[i].Velocity.y = -Constant::TERMINALVELOCITY;
		}
	}
}

void Object::Physics::Soft::SoftBody::applySpring()
{
	// Iterate Though Each Spring
	for (int i = 0; i < spring_count; i++)
	{
		// Get X and y Distances Between Nodes
		glm::vec2 distances = nodes[springs[i].Node1].Position - nodes[springs[i].Node2].Position;

		// Get Distance Between Nodes
		float distance = glm::distance(distances, glm::vec2(0.0f, 0.0f));

		// Determine if Spring Should Break
		if (distance > springs[i].MaxLength)
		{
			// Shift Proceding Springs Down
			for (int j = i; j < spring_count - 1; j++)
			{
				springs[j] = springs[j + 1];
			}

			// Decrement Spring Count
			spring_count--;
		}

		else
		{
			distances = glm::normalize(distances);
			float compression = distance - springs[i].RestLength;
			glm::vec2 velocity = nodes[springs[i].Node1].Velocity - nodes[springs[i].Node2].Velocity;

			float force = (compression * springs[i].Stiffness) + glm::dot(velocity, distances) * springs[i].Dampening;

			nodes[springs[i].Node2].Forces += distances * force;
			nodes[springs[i].Node1].Forces -= distances * force;
		}
	}
}

void Object::Physics::Soft::SoftBody::applyForces()
{
	// Iterate Through Each Node
	for (int i = 0; i < node_count; i++)
	{
		// Get Acceleration
		glm::vec2 Acceleration = nodes[i].Forces / nodes[i].Mass;
		nodes[i].Forces = glm::vec2(0.0f);

		// Apply Kinematics
		nodes[i].Velocity += Acceleration * Global::deltaTime;
		nodes[i].Position += nodes[i].Velocity * Global::deltaTime;
	}
}

void Object::Physics::Soft::SoftBody::drawObject()
{
	//// Bind VAO
	//glBindVertexArray(VAO);

	//// Draw Visualizer
	//if (Global::editing && shape == 6)
	//{
	//	glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(Initial_Position.x, Initial_Position.y, 0.0f)), glm::vec3(2.0f, 2.0f, 1.0f));
	//	glUniformMatrix4fv(modelLocColor, 1, GL_FALSE, glm::value_ptr(model));
	//	glDrawArrays(GL_TRIANGLES, 0, 6);
	//	glBindVertexArray(0);
	//	return;
	//}

	//// Unbind VAO
	//glBindVertexArray(0);

	// Draw Core
	Global::colorShaderStatic.Use();
	Vertices::Visualizer::visualizePoint(data.position, 2.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

	if (Global::editor_options->option_display_springmass_components || true)
	{
		// Bind VAO
		glBindVertexArray(VAO);

		Global::colorShaderStatic.Use();

		// Iterate for Every Single Node
		for (int i = 0; i < node_count; i++)
		{
			Vertices::Visualizer::visualizePoint(nodes[i].Position, 2.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

			// Calculate and Bind Model Matrix
			//glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(nodes[i].Position.x, nodes[i].Position.y, 0.0f)), glm::vec3(nodes[i].Radius * 2, nodes[i].Radius * 2, 1.0f));
			//glUniformMatrix4fv(Global::modelLocColor, 1, GL_FALSE, glm::value_ptr(model));

			// Draw Object
			//glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// Iterate Through Every Single Spring
		for (int i = 0; i < spring_count; i++)
		{
			Vertices::Visualizer::visualizeLine(nodes[springs[i].Node1].Position, nodes[springs[i].Node2].Position, 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		}

		Global::objectShader.Use();

		// Unbind VAO
		glBindVertexArray(0);
	}
}
