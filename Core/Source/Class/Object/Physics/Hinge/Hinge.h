#pragma once
#ifndef HINGE_H
#define HINGE_H

#ifndef DLL_HEADER
#include "Object/Physics/PhysicsBase.h"
#endif

namespace Object::Physics::Hinge
{
	// Hinge Data
	struct HingeData
	{
		// Position of Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Script
		int script;
	};

	// The Hinge Object Used to Combine Individual Physics Objects Into Complex Physics Objects
	class Hinge : public PhysicsBase
	{
		// Limb Object
		struct Limb
		{
			float offset_x = 0.0f;
			float offset_y = 0.0f;
			float theta_max = 0.0f;
			float theta_min = 0.0f;
			float initial_distance = 1.0f;
			float initial_angle = 0.0f;
			float pos_to_offset_distance = 0.0f;
			float pos_to_offset_angle = 0.0f;
			int node_index = 0; // Only For Soft Bodies
			glm::vec2* current_position = nullptr; // Pointer to Position of Object
			float mass = 0.0f; // Mass of Object
			Limb(float x, float y, float max, float min)
			{
				offset_x = x;
				offset_y = y;
				theta_max = max;
				theta_min = min;
			}
			Limb() {}
		};

		// Hinge Data
		HingeData data;

		// Determines if Limb Visualizer Should Point to Object Position Instead of Offset
		bool limb_to_position = false;

		// Rotation of Object
		float rotation = 0.0f;

		// Rotation Velocity
		float rotation_velocity = 0.0f;

		// Axis of Rotation of All Objects
		glm::vec2 Axis_of_Rotation = glm::vec2(0.0f);

		// Combined Mass of All Objects
		float total_mass = 0.0f;

		// Determines if the Object Has Been Initialized
		bool initialized = false;

		// The Visualizer Vertex Object of the Hinge
		GLuint VAO, VBO;

		// Create Visualizer
		void initializeVisualizer();

		// Establish Limb
		void establishLib(int index);

		// Determine Position of Hinge From Object
		void getHingePosFromObject(PhysicsBase& object, Limb& limb, glm::vec2& total_pos, float& total_y_pos_grounded, int& grounded_count);

		// Determine Position of Object From Hinge
		void getObjectPosFromHinge(PhysicsBase& object, Limb& limb, bool grounded);

		// Calculate Mass
		void getMass();

		// Determine the Moment of Inertia
		float calcMomentOfInertia();

		// Calculate Center of Mass
		glm::vec2 calcCenterOfMass();

		// Rotate Object
		void rotate();

		// Move Position of Objects Through Slight Change in Rotation
		void perambulate(glm::vec2& object_position, float& rotation_step);

	public:

		// File Path
		std::string file_name = "NULL";
		std::string path;

		// Hinge Position
		glm::vec2 initial_position = glm::vec2(0.0f, 0.0f);

		// List of Bound Objects
		PhysicsBase** objects;

		// List of Limbs
		Limb** limbs;

		// List of Child Hinges
		// Hinge That is a Part of the Main Hinge Object but Moves Independantly
		// Connects to An Object in Objects List; That Object will Be First in Objects List to Avoid Being Rendered Twice
		// Useful for Making Large, Complex Objects With Moving Parts
		Hinge** children;

		// Number of Objects in Hinge
		int number_of_objects;

		// Number of Children in Hinge
		int number_of_children;

		// Initialize Object
		Hinge(uint32_t& uuid_, HingeData& data_, std::string& file_name_);

		// Delete Object
		~Hinge();

		// Update Object
		void updateObject();

		// Draw Object
		void drawObject();

		// Test Mouse Collisions
		bool Test_Mouse_Collisions();

		// Read the File
		void readFile();

		// Write to the File
		void writeFile();

		// Append New Object
		void append(PhysicsBase& object, bool insert_limb, Limb limb);

		// Pop Object and Return to Level
		PhysicsBase* pop(int index, bool hinge, Limb& limb);

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

#ifdef EDITOR

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Return Position of Object
		glm::vec2 returnPosition();

#endif
	};
}

#endif
