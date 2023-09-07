#include "RigidPolygon.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Polygon
#include "Render/Shape/Polygon.h"

// Sorting Algorithms
#include "Source/Algorithms/Sorting/SortingAlgorithms.h"

// Math Functions
#include "Source/Algorithms/Quick Math/QuickMath.h"

Object::Physics::Rigid::RigidPolygon::RigidPolygon(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset) : RigidBody(uuid_, data_, rigid_, shape_, *static_cast<Shape::Polygon*>(shape_)->pointerToNumberOfSides() * 60, offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_POLYGON;

	// Store Polygon Data
	Shape::Polygon& temp_poly = *static_cast<Shape::Polygon*>(shape);
	radius = *temp_poly.pointerToRadius();
	number_of_sides = *temp_poly.pointerToNumberOfSides();
	number_of_vertices = 3 * number_of_sides;
	longest_edge = radius;

	// Determine Angle Between Offsets
	angle_between_offsets = 6.283185f / number_of_sides;

	// Establish Collision Arrays
	angle_offsets = new Named_Node[number_of_sides];
	vertices = new glm::vec2[number_of_sides];
	angle_to_vertex_left = new uint8_t[number_of_sides];
	angle_to_vertex_right = new uint8_t[number_of_sides];

	// Determine Angle to Vertex Values
	for (uint8_t i = 0; i < number_of_sides; i++)
	{
		angle_to_vertex_left[i] = i - 1;
		angle_to_vertex_right[i] = i;
	}
	//angle_to_vertex_right[0] = number_of_sides - 1;
	angle_to_vertex_left[0] = number_of_sides - 1;

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

void Object::Physics::Rigid::RigidPolygon::prepCollisions()
{
	// Calculate Rotation Offset
	rotation_offset = physics.rotation;
	while (rotation_offset >= angle_between_offsets)
		rotation_offset -= angle_between_offsets;

	// Calulate Angle Offsets and Vertices
	for (int i = 0; i < number_of_sides; i++)
	{
		angle_offsets[i] = Named_Node(rotation_offset + angle_between_offsets * i, i);
		vertices[i] = radius * Algorithms::Math::parameterizeAngle(angle_offsets[i].value);
	}

	// Calculate Model Matrix
	model = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position, 0.0f)), physics.rotation, glm::vec3(0, 0, 1));
}

void Object::Physics::Rigid::RigidPolygon::calcPotentialEnergy()
{
	physics.torque -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION * (physics.Center_of_Mass.x - physics.Rotation_Vertex.x) * Constant::POTENTIAL_ENERGY_CONSTANT;
}

float Object::Physics::Rigid::RigidPolygon::calcMomentOfInertia()
{
	return 5.0f;
}

void Object::Physics::Rigid::RigidPolygon::findExtremeVertices()
{
	// Use math to get float index at angles 0, 90, 180, 270
	// floor index to get left, ceil for right

	// theta = rotation + angle_offset * i
	// i = (theta - rotation) / angle_offset

	// Right Vertices
	float floating_index = -rotation_offset / angle_between_offsets;
	extreme_vertex_right[0] = (short)floor(floating_index);
	extreme_vertex_right[1] = (short)ceil(floating_index);

	// Upper Vertices
	floating_index = (1.5708f - rotation_offset) / angle_between_offsets;
	extreme_vertex_upper[0] = (short)floor(floating_index);
	extreme_vertex_upper[1] = (short)ceil(floating_index);

	// Left Vertices
	floating_index = (3.14159f - rotation_offset) / angle_between_offsets;
	extreme_vertex_left[0] = (short)floor(floating_index);
	extreme_vertex_left[1] = (short)ceil(floating_index);

	// Lower Vertices
	floating_index = (4.7124f - rotation_offset) / angle_between_offsets;
	extreme_vertex_lower[0] = (short)floor(floating_index);
	extreme_vertex_lower[1] = (short)ceil(floating_index);
}
