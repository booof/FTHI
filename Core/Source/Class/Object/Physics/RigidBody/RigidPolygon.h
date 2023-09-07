#pragma once
#ifndef RIGID_POLYGON_H
#define RIGID_POLYGON_H

#ifndef DLL_HEADER
#include "RigidBody.h"
#endif

namespace Object::Physics::Rigid
{
	// Rigid Body Object With the Shape of a Polygon
	class RigidPolygon : public RigidBody
	{
		// Object Rotation Offset
		float rotation_offset = 0.0f;

		// Radius
		float radius = 0;

	public:

		// Number of Sides of Polygon
		int number_of_sides = 0;

		// Initialize Object
		RigidPolygon(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, glm::vec2& offset);

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
