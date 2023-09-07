#include "RigidRectangle.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Rectangle
#include "Render/Shape/Rectangle.h"

// Sorting Algorithms
#include "Source/Algorithms/Sorting/SortingAlgorithms.h"

Object::Physics::Rigid::RigidRectangle::RigidRectangle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset) : RigidBody(uuid_, data_, rigid_, shape_, 120, offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_RECTANGLE;

	// Store Number of Vertices
	number_of_vertices = 6;

	// Store Longest Edge
	longest_edge = fmax(half_width, half_height);

	// Allocate Vertex Arrays
	vertices = new glm::vec2[4];
	angle_offsets = new Named_Node[4];
	angle_to_vertex_left = new uint8_t[4]{ 1, 2, 3, 0 };
	angle_to_vertex_right = new uint8_t[4]{2, 3, 0, 1};

	// Store Rectangle Data
	Shape::Rectangle& temp_rect = *static_cast<Shape::Rectangle*>(shape);
	half_width = *temp_rect.pointerToWidth() * 0.5f;
	half_height = *temp_rect.pointerToHeight() * 0.5f;

	// Center of Mass Initialization
	physics.Center_of_Mass_Distance = glm::distance(physics.Center_of_Mass, glm::vec2(0, 0));
	physics.Center_of_Mass_Angle = atan(physics.Center_of_Gravity.y / physics.Center_of_Gravity.x);
	if (physics.Center_of_Gravity.x < 0)
		physics.Center_of_Mass_Angle += 3.14159f;
	if (physics.Center_of_Gravity.x == 0)
		physics.Center_of_Mass_Angle = 3.14159f;

	// Determine Angles for Quadrants
	if (half_height > half_width)
	{
		float angle_offset = half_width / glm::sqrt(half_height * half_height + half_width * half_width);
		angle_initials[0] = 1.5708f - angle_offset;
		angle_initials[1] = 1.5708f + angle_offset;
		angle_initials[2] = 4.7124f - angle_offset;
		angle_initials[3] = 4.7124f + angle_offset;
	}

	else
	{
		float angle_offset = half_height / glm::sqrt(half_height * half_height + half_width * half_width);
		angle_initials[0] = angle_offset;
		angle_initials[1] = 3.14159f - angle_offset;
		angle_initials[2] = 3.14159f + angle_offset;
		angle_initials[3] = 6.2831f - angle_offset;
	}

	// Perform Initial Collision Preparation
	prepCollisions();

#ifdef EDITOR

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif

}

void Object::Physics::Rigid::RigidRectangle::prepCollisions()
{
	// All Vertices Calulated are Based On Position when Angle is 0
	// Vertices Start at Lower-Left Corner and Traverse Counterclockwise
	glm::vec2 pivot = glm::vec2(half_height * sin(physics.rotation), half_height * sin(physics.rotation - 1.5708f));
	glm::vec2 offset = glm::vec2(half_width * cos(physics.rotation), half_width * sin(physics.rotation));
	vertices[0] = glm::vec2(pivot - offset); // Lower Left
	vertices[1] = glm::vec2(pivot + offset); // Lower Right
	vertices[2] = -vertices[0]; // Top Right
	vertices[3] = -vertices[1]; // Top Left

	// Set Object Angles for Quadrants
	for (int i = 0; i < 4; i++)
	{
		angle_offsets[i] = Named_Node(angle_initials[i] + physics.rotation, i);
		if (angle_offsets[i].value > 6.2832f)
		{
			angle_offsets[i].value -= 6.2832f;
		}
	}
	Algorithms::Sorting::sortNamedNodes(&angle_offsets, 4);
	//Algorithms::Sorting::bubbleSort(angle_offsets, 4);

	// Calculate Model Matrix
	model = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position, 0.0f)), physics.rotation, glm::vec3(0, 0, 1));

	// Set axis of rotation to center of mass
	//physics.Rotation_Vertex = physics.Position + physics.Center_of_Gravity;
}

void Object::Physics::Rigid::RigidRectangle::calcPotentialEnergy()
{
	// Calculate Gravitational Torque
	physics.torque -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION * (physics.Center_of_Mass.x - physics.Rotation_Vertex.x) * Constant::POTENTIAL_ENERGY_CONSTANT;
	//Visualize_Point(physics.Rotation_Vertex.x + physics.Position.x, physics.Rotation_Vertex.y + physics.Position.y, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

float Object::Physics::Rigid::RigidRectangle::calcMomentOfInertia()
{
	return physics.Mass * (
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[0]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[1]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[2]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[3]), 2)
		);
}

void Object::Physics::Rigid::RigidRectangle::findExtremeVertices()
{
	// Find Lower Left Vertex
	int temp_vertex_value = 4 - (int)floor((physics.rotation + 0.7834f) / 1.5708f);
	if (temp_vertex_value > 3)
		temp_vertex_value = 0;

	// Store Lower Left Vertex
	extreme_vertex_left[1] = temp_vertex_value;
	extreme_vertex_lower[0] = temp_vertex_value;

	// Find Lower Right Vertex
	temp_vertex_value = shift_vertex_right[temp_vertex_value];

	// Store Lower Right Vertex
	extreme_vertex_lower[1] = temp_vertex_value;
	extreme_vertex_right[1] = temp_vertex_value;

	// Find Upper Right Vertex
	temp_vertex_value = shift_vertex_right[temp_vertex_value];

	// Store Upper Right Vertex
	extreme_vertex_right[0] = temp_vertex_value;
	extreme_vertex_upper[1] = temp_vertex_value;

	// Find Upper Left Vertex
	temp_vertex_value = shift_vertex_right[temp_vertex_value];

	// Store Upper Left Vertex
	extreme_vertex_upper[0] = temp_vertex_value;
	extreme_vertex_left[0] = temp_vertex_value;
}

