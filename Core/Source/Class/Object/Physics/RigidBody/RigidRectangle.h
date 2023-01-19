#pragma once
#ifndef RIGID_RECTANGLE_H
#define RIGID_RECTANGLE_H

#ifndef DLL_HEADER
#include "RigidBody.h"
#include "Render/Shape/Rectangle.h"
#endif

namespace Object::Physics::Rigid
{
	// Rigid Body Physics Object With the Shape of a Rectangle
	class RigidRectangle : public RigidBody
	{
		// Map to Shift Vertices to the Right in a Rectangle
		short shift_vertex_right[4] = { 1, 2, 3, 0 };

	public:

		// Vertex Angles
		float angle_initials[4];

		// Width and Height Divided in Half
		float half_width = 0, half_height = 0;
		
		// Initialize Object
		RigidRectangle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_);

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
