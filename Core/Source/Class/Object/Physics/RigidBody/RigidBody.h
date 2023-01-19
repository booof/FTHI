#pragma once
#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#ifndef DLL_HEADER
#include "Class/Object/Physics/PhysicsBase.h"
#endif

namespace Shape
{
	class Shape;
}

namespace Object::Physics::Rigid
{
	// Definition for Rigid Body Physics Variables
	struct RigidPhysics {
		glm::vec2 Position = glm::vec2(0.0f, 0.0f); // Position of Object in World (Object Position Always in Center of Object)
		glm::vec2 Velocity = glm::vec2(0.0f, 0.0f); // How Fast it is Moving
		float Mass = 1; // How Heavy the Object Is
		float Max_Health = 1; // How Much Damage Until Object is Destroyed
		float Health = 1; // Current Health of Object
		glm::vec2 Center_of_Gravity = glm::vec2(0.0f, 0.0f);; // Offset of Position
		glm::vec2 Center_of_Mass = glm::vec2(0.0f, 0.0f);; // Center of Gravity but Updated to Current Rotation
		float Center_of_Mass_Distance = 0.0f; // Distance from Origin to Center of Mass
		float Center_of_Mass_Angle = 0.0f; // Angle of Center of Mass when Rotation is 0
		bool fluid = false; // Center of Mass is independant of rotation
		bool grounded = false; // Object Is Currently On a Floor
		float rotation = 0; // Rotation of Object in radians
		float rotation_velocity = 0; // How Fast Object is Rotating in radians/second
		float torque = 0; // Angular Forces on Object
		glm::vec2 Rotation_Vertex = glm::vec2(0.0f, 0.0f);; // The Point the Object is Rotating Around
		glm::vec2 IAR = glm::vec2(0.0f, 0.0f);; // Instantanious Axis of Rotation
		glm::vec2 Forces = glm::vec2(0.0f, 0.0f);; // Kinematic Forces on Object
	};

	// Initialization Data for Rigid Bodies
	struct RigidBodyData
	{
		// Mass of Object
		float mass = 1.0f;

		// Max Health of Object
		float max_health = 0.0f;

		// Center of Gravity of Object
		glm::vec2 center_of_gravity = glm::vec2(0.0f, 0.0f);

		// Determines if The Object is Filled With a Fluid
		bool fluid = false;

		// Initial Rotation of Object
		float initial_rotation = 0.0f;

		// Physics Material
		int material = 0;
	};

	// Named Node
	struct Named_Node
	{
		float value;
		int name;
		Named_Node(float Value, int Name) { value = Value; name = Name; }
		Named_Node() { value = 0.0f; name = 0; }
	};

	// Definition for Rigid Body Base Class
	class RigidBody : public PhysicsBase
	{
	protected:

		// Initialize Object
		RigidBody(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, int vertex_count);

	public:

		// Object Characteristics
		Rigid::RigidPhysics physics;

		// Initial Rigid Body Data
		RigidBodyData rigid;

		// Rigid Body Shape
		Shape::Shape* shape = nullptr;

		// Vertex Objects
		GLuint VAO = 0, VBO = 0;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Instance of Object
		int instance = 0;

		// The longest edge of object
		float longest_edge = 0;

		// Array of Vertices at Given Instance
		glm::vec2* vertices = nullptr;

		// Angle Offsets of Vertices
		Named_Node* angle_offsets = nullptr;

		// Vertex Maps
		uint8_t* angle_to_vertex_left = nullptr;
		uint8_t* angle_to_vertex_right = nullptr;

		// Array of possible rotation vertices
		std::vector<glm::vec2> possible_rotation_vertices;

		// Extreme Vertices Lists
		short extreme_vertex_lower[2] = {0}; // Lower Left - Lower Right
		short extreme_vertex_upper[2] = {0}; // Upper Left - Upper Right
		short extreme_vertex_left[2] = {0};  // Upper Left - Lower Left
		short extreme_vertex_right[2] = {0}; // Upper Right - Lower Right

		// Angle Between Angle Offsets
		float angle_between_offsets = 1.5708f;

		// Creates Vertices and Lines For Collision Detection
		virtual void prepCollisions() = 0;

		// Calculates the Potenital Energy of an Object and Sets the Angular Velocity
		virtual void calcPotentialEnergy() = 0;

		// Calculates the Moment of Inertia
		virtual float calcMomentOfInertia() = 0;

		// Find Extreme Vertices
		virtual void findExtremeVertices() = 0;

		// Initialize Vertices
		void initializeVertices(int & instance_, int & instance_index_);
	
		// Update Object
		void updateObject();

		// Apply Gravity
		void applyGravity();

		// Calculate Instantanious Axis of Rotation
		void calculateIAR();

		// Apply Forces
		void applyForces();

		// Draw Object
		void drawObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();


#ifdef EDITOR

		// Write Object to File
		void write(std::ofstream& object_file, std::ofstream& editor_file);

		// Select Object
		void select(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

		// Select Object Info
		static void info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, Shape::Shape* shape);

#endif

	};
}

// Compare Values of Named Nodes
bool operator==(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2);

// Compare Values of Named Node
bool operator>(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2);

// Compare Values of Named Nodes
bool operator<(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2);

// Compare Values of Named Nodes
bool operator==(const Object::Physics::Rigid::Named_Node node1, const float value);

// Compare Values of Named Node
bool operator>(const Object::Physics::Rigid::Named_Node node1, const float value);

// Compare Values of Named Nodes
bool operator<(const Object::Physics::Rigid::Named_Node node1, const float value);

// Compare Values of Named Nodes
bool operator==(const float value, const Object::Physics::Rigid::Named_Node node1);

// Compare Values of Named Node
bool operator>(const float value, const Object::Physics::Rigid::Named_Node node1);

// Compare Values of Named Nodes
bool operator<(const float value, const Object::Physics::Rigid::Named_Node node1);

#endif
