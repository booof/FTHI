#pragma once
#ifndef RIGID_CIRCLE_H
#define RIGID_CIRCLE_H

#ifndef DLL_HEADER
#include "RigidBody.h"
#endif

namespace Object::Physics::Rigid
{
	// Rigid Body Object With the Shape of a Circle
	class RigidCircle : public RigidBody
	{
	public:

		// Circle Data
		float radius = 0;

		// Initialize Object
		RigidCircle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset);

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
