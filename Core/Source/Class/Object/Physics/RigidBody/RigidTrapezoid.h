#pragma once
#ifndef RIGID_TRAPEZOID_H
#define RIGID_TRAPEZOID_H

#ifndef DLL_HEADER
#include "RigidBody.h"
#endif

namespace Object::Physics::Rigid
{
	// Rigid Body Physics Object With the Shape of a Trapezoid
	class RigidTrapezoid : public RigidBody
	{
		// Map to Shift Vertices to the Right in a Trapezoid
		short shift_vertex_right[4] = { 1, 2, 3, 0 };

	public:

		// Shape Values
		float half_height = 0.0f;
		float half_width = 0.0f;
		float size_offset_height = 0.0f;
		float size_offset_width = 0.0f;

		// Vertex Angles
		float angle_initials[4];

		glm::vec2 position_offset;
		glm::vec2 relative_position;
		float offset_angle = 0.0f;
		float offset_distance = 0.0f;

		// Initialize Object
		RigidTrapezoid(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_);

		// Creates Vertices and Lines For Collision Detection
		void prepCollisions();

		// Calculates the Potenital Energy of an Object and Sets the Angular Velocity
		void calcPotentialEnergy();

		// Calculates the Moment of Inertia
		float calcMomentOfInertia();

		// Find Extreme Vertices
		void findExtremeVertices();
	};
}

#endif
