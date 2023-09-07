#include "RigidTriangle.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Triangle
#include "Render/Shape/Triangle.h"

// Sorting Algorithms
#include "Source/Algorithms/Sorting/SortingAlgorithms.h"

// Math Functions
#include "Source/Algorithms/Quick Math/QuickMath.h"

// Functions to Arrange Vertices
#include "Source/Collisions/Point Collisions/PointCollisions.h"

#include "Source/Vertices/Visualizer/Visualizer.h"

short Object::Physics::Rigid::RigidTriangle::find_min(float x, float y, float z)
{
	// X is the smallest
	if (x < y && x < z)
	{
		return 0;
	}

	// Y is the smallest
	if (y < z)
	{
		return 1;
	}

	// Z is the smallest
	return 2;
}

short Object::Physics::Rigid::RigidTriangle::find_max(float x, float y, float z)
{
	// X is the greatest
	if (x > y && x > z)
	{
		return 0;
	}

	// Y is the greatest
	if (y > z)
	{
		return 1;
	}

	// Z is the greatest
	return 2;
}

Object::Physics::Rigid::RigidTriangle::RigidTriangle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset) : RigidBody(uuid_, data_, rigid_, shape_, 60, offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_TRIANGLE;

	// Store Number of Vertices
	number_of_vertices = 3;

	// Allocate Vertex Array
	vertices = new glm::vec2[3];
	angle_offsets = new Named_Node[3];
	angle_to_vertex_left = new uint8_t[3]{ 0, 1, 2 };
	angle_to_vertex_right = new uint8_t[3]{ 1, 2, 0 };

	// Store Initial Coordinates
	Shape::Triangle& temp_tri = *static_cast<Shape::Triangle*>(shape);
	temp_tri.updateSelectedPosition(offset.x, offset.y);
	coords1 = data.position;
	coords2 = *temp_tri.pointerToSecondPosition();
	coords3 = *temp_tri.pointerToThirdPosition();

	// Sort Initial Coordinates
	float angle_1_to_2 = Algorithms::Math::angle_from_vector(coords2 - coords1);
	float angle_1_to_3 = Algorithms::Math::angle_from_vector(coords3 - coords1);
	//Vertices::Visualizer::visualizeRay(coords1, angle_1_to_2, 0.1f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	//Vertices::Visualizer::visualizeRay(coords1, angle_1_to_3, 0.1f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	if (angle_1_to_2 > angle_1_to_3 && angle_1_to_2 - angle_1_to_3 < 3.14159f)
	{
		glm::vec2 temp_vert = coords2;
		coords2 = coords3;
		coords3 = temp_vert;
	}

	//Source::Collisions::Point::arrangeTriVertices(coords1, coords2, coords3);

	// Store Triangle Data
	//height = glm::distance(*temp_tri.pointerToSecondPosition(), data.position);
	//width = glm::distance(*temp_tri.pointerToThirdPosition(), data.position);
	//height_angle = Algorithms::Math::angle_from_vector(*temp_tri.pointerToSecondPosition() - data.position);
	//width_angle = Algorithms::Math::angle_from_vector(*temp_tri.pointerToThirdPosition() - data.position);

	// Store Triangle Data
	width = glm::distance(coords2, coords1);
	height = glm::distance(coords3, coords1);
	width_angle = Algorithms::Math::angle_from_vector(coords2 - coords1);
	height_angle = Algorithms::Math::angle_from_vector(coords3 - coords1);

	// Set Initial Vertex Coordinates
	vertices[0] = glm::vec2(0.0f, 0.0f);
	vertices[1] = width * Algorithms::Math::parameterizeAngle(width_angle);
	vertices[2] = height * Algorithms::Math::parameterizeAngle(height_angle);
	//Source::Collisions::Point::arrangeTriVertices(vertices[0], vertices[1], vertices[2]);

	// Determine Coords for Mouse Collisions
	//coords1 = vertices[0] + physics.Position;
	//coords2 = vertices[1] + physics.Position;
	//coords3 = vertices[2] + physics.Position;
	//Source::Collisions::Point::arrangeTriVertices(coords1, coords2, coords3);

	// Find Position Offset
	position_offset = (vertices[0] + vertices[1] + vertices[2]) / 4.0f;

	// Find Angle Offsets
	short vertex_to_angle_map[3] = { 2, 0, 1 };
	for (int i = 0; i < 3; i++)
	{
		vertices[i] -= position_offset;
		angle_initials[vertex_to_angle_map[i]] = atan(vertices[i].y / vertices[i].x);
		if (vertices[i].x < 0)
		{
			angle_initials[vertex_to_angle_map[i]] += 3.14159f;
		}
		if (angle_initials[vertex_to_angle_map[i]] < 0)
		{
			angle_initials[vertex_to_angle_map[i]] += 6.2832f;
		}
	}

	offset_angle = Algorithms::Math::angle_from_vector(position_offset);
	offset_distance = glm::distance(position_offset, glm::vec2(0.0f, 0.0f));

	// Perform Initial Collision Preparation
	physics.Position += position_offset;
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

void Object::Physics::Rigid::RigidTriangle::prepCollisions()
{
	// Position Offset Calculation
	physics.Position -= position_offset;
	position_offset = offset_distance * Algorithms::Math::parameterizeAngle(offset_angle + physics.rotation);

	// Calculate Vertices
	vertices[0] = glm::vec2(0.0f, 0.0f);
	vertices[1] = glm::vec2(width * cos(width_angle + physics.rotation), width * sin(width_angle + physics.rotation));
	vertices[2] = glm::vec2(height * cos(height_angle + physics.rotation), height * sin(height_angle + physics.rotation));

	// Find Angle Offsets
	for (int i = 0; i < 3; i++)
	{
		angle_offsets[i] = Named_Node(angle_initials[i] + physics.rotation, i);
		if (angle_offsets[i].value > 6.2832f)
		{
			angle_offsets[i].value -= 6.2832f;
		}
	}
	Algorithms::Sorting::sortNamedNodes(&angle_offsets, 3);

	// Calculate Model Matrix
	model = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position, 0.0f)), physics.rotation, glm::vec3(0.0f, 0.0f, 1.0f));

	// Modify Position
	physics.Position += position_offset;
	for (int i = 0; i < 3; i++)
	{
		vertices[i] -= position_offset;
	}

	Vertices::Visualizer::visualizePoint(vertices[0] + physics.Position, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(vertices[1] + physics.Position, 0.3f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(vertices[2] + physics.Position, 0.3f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
}

void Object::Physics::Rigid::RigidTriangle::calcPotentialEnergy()
{
	physics.torque -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION * (physics.Center_of_Mass.x - physics.Rotation_Vertex.x) * Constant::POTENTIAL_ENERGY_CONSTANT;
}

float Object::Physics::Rigid::RigidTriangle::calcMomentOfInertia()
{
	return physics.Mass * (
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[0]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[1]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[2]), 2)
		);
}

void Object::Physics::Rigid::RigidTriangle::findExtremeVertices()
{
	// Find Bottom Vertices
	short index = find_max(vertices[0].y, vertices[1].y, vertices[2].y);
	extreme_vertex_lower[0] = other_values[index][0];
	extreme_vertex_lower[1] = other_values[index][1];

	// Find Upper Vertices
	index = find_min(vertices[0].y, vertices[1].y, vertices[2].y);
	extreme_vertex_upper[0] = other_values[index][0];
	extreme_vertex_upper[1] = other_values[index][1];

	// Find Left Vertices
	index = find_max(vertices[0].x, vertices[1].x, vertices[2].x);
	extreme_vertex_left[0] = other_values[index][0];
	extreme_vertex_left[1] = other_values[index][1];

	// Find Bottom Vertices
	index = find_min(vertices[0].x, vertices[1].x, vertices[2].x);
	extreme_vertex_right[0] = other_values[index][0];
	extreme_vertex_right[1] = other_values[index][1];
}
