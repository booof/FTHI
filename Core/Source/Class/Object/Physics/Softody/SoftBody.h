#ifndef SOFT_BODY_H
#define SOFT_BODY_H

#ifndef DLL_HEADER
#include "Object/Physics/PhysicsBase.h"
#endif

namespace Object::Physics::Soft
{
	// Node Data
	struct NodeData
	{
		// Name of the Node
		int name = 0;

		// Position Offset of Node
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Mass of the Node
		float mass = 1.0f;

		// Health of the Node
		float health = 1.0f;

		// Radius of the Node
		float radius = 1.0f;

		// Material of the Node
		int material = 1;
	};

	// Node Object
	struct Node
	{
		int Name = 0; // Identifier for Object
		glm::vec2 Position = glm::vec2(0.0f, 0.0f); // Position of Node in World Space
		glm::vec2 Velocity = glm::vec2(0.0f, 0.0f);; // Motion of Node
		glm::vec2 Forces = glm::vec2(0.0f, 0.0f);; // Forces Applied to Node
		float Mass = 1.0f; // Mass of Node
		float Health = 1.0f; // Health of the Node
		float Radius = 1.0f; // Radius of the Node for Collision Detection
		int material = 1; // Physics Material of Node

		Node(int name, float x, float y, float mass, float health, float radius)
		{
			Name = name;
			Position = glm::vec2(x, y);
			Velocity = glm::vec2(0.0f, 0.0f);
			Forces = glm::vec2(0.0f, 0.0f);
			Mass = mass;
			Health = health;
			Radius = radius;
			material = 0;
		}

		Node(NodeData data)
		{
			Name = data.name;
			Position = data.position;
			Velocity = glm::vec2(0.0f, 0.0f);
			Forces = glm::vec2(0.0f, 0.0f);
			Mass = data.mass;
			Health = data.health;
			Radius = data.radius;
			material = data.material;
		}

		Node() {}
	};

	// Spring Object
	struct Spring
	{
		int Node1 = 0; // Pointer to First Connected Node
		int Node2 = 0; // Pointer to Second Connected Node
		float RestLength = 0.0f; // Length of Spring That Produces no Tension
		float Stiffness = 0.0f; // Stiffness of Spring
		float Dampening = 0.0f; // Dampening Constant
		float MaxLength = 0.0f; // Length of Spring Before It Breaks

		Spring(int node1, int node2, float rest, float stiff, float damp, float max)
		{
			Node1 = node1;
			Node2 = node2;
			RestLength = rest;
			Stiffness = stiff;
			Dampening = damp;
			MaxLength = max;
		}

		Spring() {}
	};

	// The Base Class for Soft Body Physics Objects
	class SoftBody : public PhysicsBase
	{
		// Vertex Object
		GLuint VAO = 0, VBO = 0;

	public:

		// Array of Nodes
		Node* nodes = nullptr;

		// Array of Springs
		Spring* springs = nullptr;

		// Number of Nodes
		uint8_t node_count = 0;

		// Number of Springs
		uint8_t spring_count = 0;

		// Delete Object
		~SoftBody();

		// Initialize Visuailzer
		void initializeVisualizer();

		// Update Object
		void updateObject();

		// Apply Gravity
		void applyGravity();

		// Apply Spring Forces
		void applySpring();

		// Apply Forces
		void applyForces();

		// Draw Object
		void drawObject();
	};
}

#endif
