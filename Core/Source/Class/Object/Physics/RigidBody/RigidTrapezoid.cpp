#include "RigidTrapezoid.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Trapezoid
#include "Render/Shape/Trapezoid.h"

// Sorting Algorithms
#include "Source/Algorithms/Sorting/SortingAlgorithms.h"

// Math Functions
#include "Source/Algorithms/Quick Math/QuickMath.h"

Object::Physics::Rigid::RigidTrapezoid::RigidTrapezoid(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset) : RigidBody(uuid_, data_, rigid_, shape_, 120, offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_TRAPEZOID;

	// Store Number of Vertices
	number_of_vertices = 6;

	// Store Longest Edge
	longest_edge = *static_cast<Shape::Trapezoid*>(shape)->pointerToWidth();

	// Allocate Vertex Array
	vertices = new glm::vec2[4];
	angle_offsets = new Named_Node[4];
	angle_to_vertex_left = new uint8_t[4]{ 1, 2, 3, 0 };
	angle_to_vertex_right = new uint8_t[4]{ 2, 3, 0, 1 };

	// Store Shape Values
	Shape::Trapezoid& temp_trap = *static_cast<Shape::Trapezoid*>(shape);
	half_height = *temp_trap.pointerToHeight() * 0.5f;
	half_width = *temp_trap.pointerToWidth() * 0.5f;
	size_offset_height = *temp_trap.pointerToHeightOffset();
	size_offset_width = *temp_trap.pointerToWidthOffset();

	// Center of Mass Initialization
	physics.Center_of_Mass_Distance = glm::distance(physics.Center_of_Mass, glm::vec2(0, 0));
	physics.Center_of_Mass_Angle = atan(physics.Center_of_Gravity.y / physics.Center_of_Gravity.x);
	if (physics.Center_of_Gravity.x < 0)
		physics.Center_of_Mass_Angle += 3.14159f;
	if (physics.Center_of_Gravity.x == 0)
		physics.Center_of_Mass_Angle = 3.14159f;

	// Determine Angles for Quadrants
	glm::vec2 pivot = glm::vec2(half_height * sin(physics.rotation), half_height * sin(physics.rotation - 1.5708f));
	glm::vec2 pivot_offset = glm::vec2(half_width * cos(physics.rotation), half_width * sin(physics.rotation));
	glm::vec2 parameterized_size_offset = glm::vec2(-size_offset_height * sin(physics.rotation), size_offset_height * cos(physics.rotation));
	glm::vec2 parameterized_size_offset2 = glm::vec2(size_offset_width * cos(physics.rotation), size_offset_width * sin(physics.rotation));
	vertices[0] = glm::vec2(pivot - pivot_offset); // Lower Left
	vertices[1] = glm::vec2(pivot + pivot_offset); // Lower Right
	vertices[2] = -vertices[0]; // Top Right
	vertices[3] = -vertices[1]; // Top Left
	vertices[1] += parameterized_size_offset;
	vertices[2] += parameterized_size_offset;
	vertices[2] += parameterized_size_offset2;
	vertices[3] += parameterized_size_offset2;

	glm::vec2 total_offsets = glm::vec2(0.0f);
	for (int i = 0; i < 4; i++)
	{
		total_offsets += vertices[i];
	}
	position_offset = total_offsets / 4.0f;
	short vertex_to_angle_map[4] = { 2, 3, 0, 1 };
	for (int i = 0; i < 4; i++)
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
	//if (!Global::editing)
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

void Object::Physics::Rigid::RigidTrapezoid::prepCollisions()
{
	// Position Offset Calculation
	physics.Position -= position_offset;
	position_offset = offset_distance * Algorithms::Math::parameterizeAngle(offset_angle + physics.rotation);

	// Determine Vertices of Object
	// Vertices Start at Lower-Left Corner and Traverse Counterclockwise
	glm::vec2 pivot = glm::vec2(half_height * sin(physics.rotation), half_height * sin(physics.rotation - 1.5708f));
	glm::vec2 offset = glm::vec2(half_width * cos(physics.rotation), half_width * sin(physics.rotation));

	glm::vec2 parameterized_size_offset = glm::vec2(-size_offset_height * sin(physics.rotation), size_offset_height * cos(physics.rotation));
	glm::vec2 parameterized_size_offset2 = glm::vec2(size_offset_width * cos(physics.rotation), size_offset_width * sin(physics.rotation));

	vertices[0] = glm::vec2(pivot - offset); // Lower Left
	vertices[1] = glm::vec2(pivot + offset); // Lower Right
	vertices[2] = -vertices[0]; // Top Right
	vertices[3] = -vertices[1]; // Top Left

	vertices[1] += parameterized_size_offset;
	vertices[2] += parameterized_size_offset;
	vertices[2] += parameterized_size_offset2;
	vertices[3] += parameterized_size_offset2;

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

	// Calculate Model Matrix
	model = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position, 0.0f)), physics.rotation, glm::vec3(0, 0, 1));

	// Modify Position
	physics.Position += position_offset;
	for (int i = 0; i < 4; i++)
	{
		vertices[i] -= position_offset;
	}
}

void Object::Physics::Rigid::RigidTrapezoid::calcPotentialEnergy()
{
	physics.torque -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION * (physics.Center_of_Mass.x - physics.Rotation_Vertex.x) * Constant::POTENTIAL_ENERGY_CONSTANT;
}

float Object::Physics::Rigid::RigidTrapezoid::calcMomentOfInertia()
{
	return physics.Mass * (
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[0]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[1]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[2]), 2) +
		(float)glm::pow(glm::distance(physics.Rotation_Vertex, vertices[3]), 2)
		);
}

void Object::Physics::Rigid::RigidTrapezoid::findExtremeVertices()
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
