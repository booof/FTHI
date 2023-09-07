#include "RigidCircle.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Circle
#include "Render/Shape/Circle.h"

#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"

Object::Physics::Rigid::RigidCircle::RigidCircle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset) : RigidBody(uuid_, data_, rigid_, shape_, 1200, offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_CIRCLE;

	// Set Number of Vertices to Draw
	number_of_vertices = 60;

	// Store Radius
	radius = *static_cast<Shape::Circle*>(shape)->pointerToRadius();
	longest_edge = radius;

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

void Object::Physics::Rigid::RigidCircle::prepCollisions()
{
	// Decrease Rotation Speed
	physics.rotation_velocity *= 0.95f;

	physics.Rotation_Vertex = glm::vec2(0.0f, 0.0f);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position, 0.0f));

	//Vertices::Visualizer::visualizeRay(physics.Position, physics.rotation, 0.1f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}

void Object::Physics::Rigid::RigidCircle::calcPotentialEnergy()
{
	physics.torque -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION * (physics.Center_of_Mass.x - physics.Rotation_Vertex.x) * Constant::POTENTIAL_ENERGY_CONSTANT;
}

float Object::Physics::Rigid::RigidCircle::calcMomentOfInertia()
{
	return radius;
}

void Object::Physics::Rigid::RigidCircle::findExtremeVertices()
{
}
