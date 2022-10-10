#ifndef RIGID_TRIANGLE_H
#define RIGID_TRIANGLE_H

#ifndef DLL_HEADER
#include "RigidBody.h"
#endif

namespace Object::Physics::Rigid
{
	// Rigid Body Physics Object With the Shape of a Triangle
	class RigidTriangle : public RigidBody
	{
		glm::vec2 coords1;
		glm::vec2 coords2;
		glm::vec2 coords3;

		short other_values[3][2] = {
			{1, 2}, {2, 0}, {0, 1}
		};

		// Triangle Data
		float width, height, width_angle, height_angle;

		// Finds the Minimum Value from Three Vertces
		short find_min(float x, float y, float z);

		// Finds the Maximum Value from Three Vertices
		short find_max(float x, float y, float z);

	public:

		// Vertex Angles
		float angle_initials[3];

		glm::vec2 position_offset;
		glm::vec2 relative_position;
		float offset_angle = 0.0f;
		float offset_distance = 0.0f;

		// Initialize Object
		RigidTriangle(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_);

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
