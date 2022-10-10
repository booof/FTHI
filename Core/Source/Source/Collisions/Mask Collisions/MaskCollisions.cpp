#include "MaskCollisions.h"

#include "Globals.h"
#include "Constants.h"

#include "Class/Object/Collision/Horizontal/FloorMask.h"
#include "Class/Object/Collision/Vertical/LeftMask.h"
#include "Class/Object/Collision/Vertical/RightMask.h"
#include "Class/Object/Collision/Horizontal/CeilingMask.h"
#include "Class/Object/Collision/Trigger/TriggerMask.h"

#include "Class/Object/Physics/RigidBody/RigidBody.h"
#include "Class/Object/Physics/RigidBody/RigidCircle.h"
#include "Class/Object/Physics/Softody/SoftBody.h"
#include "Class/Object/Physics/Hinge/Hinge.h"
#include "Class/Render/Shape/Shape.h"

#include "Class/Object/Entity/EntityBase.h"

#include "Source/Vertices/Visualizer/Visualizer.h"

bool Source::Collisions::Mask::collisionsFloorMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int floor_mask_count, Object::Mask::Floor::FloorMask** floor_masks)
{
	for (int i = 0; i < floor_mask_count; i++)
	{
		if (floor_masks[i]->testCollisions(value, 2.0f, false))
		{
			returned_value = floor_masks[i]->returned_value;
			angle = floor_masks[i]->returned_angle;
			material_index = floor_masks[i]->material;
			return true;
		}
	}

	return false;
}

bool Source::Collisions::Mask::collisionsFloorMaskEdge(glm::vec2 left_vertex, glm::vec2 right_vertex, float& returned_value, glm::vec2& returned_vertex, int& material_index, int floor_mask_count, Object::Mask::Floor::FloorMask ** floor_masks)
{
	//Vertices::Visualizer::visualizePoint(left_vertex, 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//Vertices::Visualizer::visualizePoint(right_vertex, 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	for (int i = 0; i < floor_mask_count; i++)
	{
		if (floor_masks[i]->testEdgeCollisions(left_vertex, right_vertex, 1.0f, returned_vertex))
		{
			returned_value = floor_masks[i]->returned_value;
			material_index = floor_masks[i]->material;
			return true;
		}
	}

	return false;
}

bool Source::Collisions::Mask::collisionsLeftWallMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int left_mask_count, Object::Mask::Left::LeftMask** left_masks)
{
	for (int i = 0; i < left_mask_count; i++)
	{
		if (left_masks[i]->testCollisions(value, 2.0f))
		{
			returned_value = left_masks[i]->returned_value;
			angle = left_masks[i]->returned_angle;
			material_index = left_masks[i]->material;
			return true;
		}
	}

	return false;
}

bool Source::Collisions::Mask::collisionsRightWallMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int right_mask_count, Object::Mask::Right::RightMask** right_masks)
{
	for (int i = 0; i < right_mask_count; i++)
	{
		if (right_masks[i]->testCollisions(value, 2.0f))
		{
			returned_value = right_masks[i]->returned_value;
			angle = right_masks[i]->returned_angle;
			material_index = right_masks[i]->material;
			return true;
		}
	}

	return false;
}

bool Source::Collisions::Mask::collisionsCeilingMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int ceiling_mask_count, Object::Mask::Ceiling::CeilingMask** ceiling_masks)
{
	for (int i = 0; i < ceiling_mask_count; i++)
	{
		if (ceiling_masks[i]->testCollisions(value, 2.0f))
		{
			returned_value = ceiling_masks[i]->returned_value;
			angle = ceiling_masks[i]->returned_angle;
			material_index = ceiling_masks[i]->material;
			return true;
		}
	}

	return false;
}

void Source::Collisions::Mask::maskCollisionsRigid(Object::Physics::Rigid::RigidBody* object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
	Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks)
{
	static bool first_vertex_collision, second_vertex_collision;
	static float first_returned_value, second_returned_value, first_angle, second_angle;
	static short first_vertex_index, second_vertex_index;
	static int first_material, second_material;

	// Test if Rigid Object is a Circle
	if (object->shape->shape == Shape::CIRCLE)
	{
		maskCollisionsRigidCircle(*static_cast<Object::Physics::Rigid::RigidCircle*>(object), floor_mask_count, left_mask_count, right_mask_count, ceiling_mask_count, trigger_mask_count,
			floor_masks, left_masks, right_masks, ceiling_masks, trigger_masks);
		return;
	}

	// Floor Object Collisions
	first_vertex_index = object->extreme_vertex_lower[0];
	second_vertex_index = object->extreme_vertex_lower[1];
	first_vertex_collision = collisionsFloorMask(object->vertices[first_vertex_index] + object->physics.Position, first_returned_value, first_angle, first_material, floor_mask_count, floor_masks);
	second_vertex_collision = collisionsFloorMask(object->vertices[second_vertex_index] + object->physics.Position, second_returned_value, second_angle, second_material, floor_mask_count, floor_masks);
	maskCollisionsRigidHorizontalHelper(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);
	maskCollisionsRigidFloorEdgeHelper(object, first_vertex_collision, second_vertex_collision, first_vertex_index, second_vertex_index, floor_mask_count, floor_masks);

	// Left Wall Object Collisions
	first_vertex_index = object->extreme_vertex_left[0];
	second_vertex_index = object->extreme_vertex_left[1];
	first_vertex_collision = collisionsLeftWallMask(object->vertices[first_vertex_index] + object->physics.Position, first_returned_value, first_angle, first_material, left_mask_count, left_masks);
	second_vertex_collision = collisionsLeftWallMask(object->vertices[second_vertex_index] + object->physics.Position, second_returned_value, second_angle, second_material, left_mask_count, left_masks);
	maskCollisionsRigidVerticalHelper(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);

	// Right Wall Object Collisions
	first_vertex_index = object->extreme_vertex_right[0];
	second_vertex_index = object->extreme_vertex_right[1];
	first_vertex_collision = collisionsRightWallMask(object->vertices[first_vertex_index] + object->physics.Position, first_returned_value, first_angle, first_material, right_mask_count, right_masks);
	second_vertex_collision = collisionsRightWallMask(object->vertices[second_vertex_index] + object->physics.Position, second_returned_value, second_angle, second_material, right_mask_count, right_masks);
	maskCollisionsRigidVerticalHelper(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);

	// Reverse Floor Object Collisions
	first_vertex_index = object->extreme_vertex_upper[0];
	second_vertex_index = object->extreme_vertex_upper[1];
	first_vertex_collision = collisionsCeilingMask(object->vertices[first_vertex_index] + object->physics.Position, first_returned_value, first_angle, first_material, ceiling_mask_count, ceiling_masks);
	second_vertex_collision = collisionsCeilingMask(object->vertices[second_vertex_index] + object->physics.Position, second_returned_value, second_angle, second_material, ceiling_mask_count, ceiling_masks);
	maskCollisionsRigidHorizontalHelper(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);
}

void Source::Collisions::Mask::maskCollisionsRigidHorizontalHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material)
{
	static glm::vec2 normal_force;

	if (first_vertex_collision || second_vertex_collision)
	{
		// Get Normal Force
		maskCollisionsRigidNormalForceHelper(object, normal_force, first_vertex_collision, second_vertex_collision, first_angle, second_angle);

		// Both Vertices Collided
		if (first_vertex_collision && second_vertex_collision)
			maskCollisionsRigidHorizontalBothHelper(object, first_vertex_index, second_vertex_index, first_returned_value, second_returned_value, first_material, second_material, normal_force);

		// Only First Vertex Collided
		else if (first_vertex_collision)
			maskCollisionsRigidHorizontalSingleHelper(object, first_vertex_index, first_returned_value, first_material, normal_force);

		// Only Second Vertex Collided
		else
			maskCollisionsRigidHorizontalSingleHelper(object, second_vertex_index, second_returned_value, second_material, normal_force);
	}
}

void Source::Collisions::Mask::maskCollisionsRigidVerticalHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material)
{
	static glm::vec2 normal_force;

	if (first_vertex_collision || second_vertex_collision)
	{
		// Get Normal Force
		maskCollisionsRigidNormalForceHelper(object, normal_force, first_vertex_collision, second_vertex_collision, first_angle, second_angle);

		// Both Vertices Collided
		if (first_vertex_collision && second_vertex_collision)
			maskCollisionsRigidVerticalBothHelper(object, first_vertex_index, second_vertex_index, first_returned_value, second_returned_value, first_material, second_material, normal_force);

		// Only First Vertex Collided
		else if (first_vertex_collision)
			maskCollisionsRigidVerticalSingleHelper(object, first_vertex_index, first_returned_value, first_material, normal_force);

		// Only Second Vertex Collided
		else
			maskCollisionsRigidVerticalSingleHelper(object, second_vertex_index, second_returned_value, second_material, normal_force);
	}
}

void Source::Collisions::Mask::maskCollisionsRigidNormalForceHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, bool& first_vertex_collision, bool& second_vertex_collision, float& first_vertex_angle, float& second_vertex_angle)
{
	static float surface_angle, object_angle;

	surface_angle = 0.0f;

	// Angle is Average of Both Surfaces if Both Vertices Collided
	if (first_vertex_collision && second_vertex_collision)
	{
		surface_angle = (first_vertex_angle + second_vertex_angle) / 2;
	}

	else if (first_vertex_collision)
	{
		surface_angle = first_vertex_angle;
	}

	else if (second_vertex_collision)
	{
		surface_angle = second_vertex_angle;
	}

	// Calculate Normal Force
	object_angle = 1.5708f + surface_angle;
	normal_force = -glm::vec2(cos(object_angle), sin(object_angle)) * sin(1.5708f - surface_angle) * object->physics.Forces.y;
	if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
	object->physics.Forces += normal_force;
}

void Source::Collisions::Mask::maskCollisionsRigidHorizontalBothHelper(Object::Physics::Rigid::RigidBody* object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Move box up the max value
	if (glm::distance(object->vertices[first_vertex_index].y + object->physics.Position.y, first_returned_value) > glm::distance(object->vertices[second_vertex_index].y + object->physics.Position.y, second_returned_value))
	{
		object->physics.Position.y = first_returned_value - object->vertices[first_vertex_index].y;
	}

	else
	{
		object->physics.Position.y = second_returned_value - object->vertices[second_vertex_index].y;
	}

	// Insert Results as Possible Rotation Vertex
	object->possible_rotation_vertices.push_back(object->vertices[first_vertex_index]);
	object->possible_rotation_vertices.push_back(object->vertices[second_vertex_index]);

	// Apply Physics Changes
	object->physics.Velocity.y = 0;
	object->physics.rotation_velocity /= (glm::distance(object->vertices[first_vertex_index], object->vertices[second_vertex_index]) + 1);

	object->physics.torque = 0;
	//object->physics.Rotation_Vertex = object->physics.Center_of_Mass;
	object->physics.Rotation_Vertex = (object->vertices[first_vertex_index] + object->vertices[second_vertex_index]) * 0.5f;
	object->physics.grounded = true;

	// Get Friction Coefficient
	friction_coefficient = (getFrictionCoefficient(object->rigid.material, first_object_material, true) + getFrictionCoefficient(object->rigid.material, second_object_material, true)) / 2;

	// Apply Friction
	maskCollisionsHorizontalFrictionHelper(object, normal_force, friction_coefficient);
}

void Source::Collisions::Mask::maskCollisionsRigidHorizontalSingleHelper(Object::Physics::Rigid::RigidBody* object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Insert Results as Possible Rotation Vertex
	object->possible_rotation_vertices.push_back(object->vertices[vertex_index]);

	// Shift box up
	object->physics.Position.y = returned_value - object->vertices[vertex_index].y;
	object->physics.Velocity.y = 0;
	object->physics.grounded = true;

	// Set Rotation Vertex to Vertex
	object->physics.Rotation_Vertex = object->vertices[vertex_index];

	// Get Friction Coefficient
	friction_coefficient = getFrictionCoefficient(object->rigid.material, object_material, true);

	// Apply Friction
	maskCollisionsHorizontalFrictionHelper(object, normal_force, friction_coefficient);
}

void Source::Collisions::Mask::maskCollisionsHorizontalFrictionHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, float& friction_coefficient)
{
	static float max_friction, velocity_angle;

	if (object->physics.Velocity.x)
	{
		max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y);
		velocity_angle = atan(object->physics.Velocity.y / object->physics.Velocity.x);
		if (object->physics.Velocity.x < 0) { velocity_angle += 3.14159f; }
		object->physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
	}
}

void Source::Collisions::Mask::maskCollisionsRigidVerticalBothHelper(Object::Physics::Rigid::RigidBody* object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Move box up the max value
	if (glm::distance(object->vertices[first_vertex_index].x + object->physics.Position.x, first_returned_value) > glm::distance(object->vertices[second_vertex_index].x + object->physics.Position.x, second_returned_value))
	{
		object->physics.Position.x = first_returned_value - object->vertices[first_vertex_index].x;
	}

	else
	{
		object->physics.Position.x = second_returned_value - object->vertices[second_vertex_index].x;
	}

	// Insert Results as Possible Rotation Vertex
	object->possible_rotation_vertices.push_back(object->vertices[first_vertex_index]);
	object->possible_rotation_vertices.push_back(object->vertices[second_vertex_index]);

	// Apply Physics Changes
	object->physics.Velocity.x = 0;
	object->physics.rotation_velocity /= (glm::distance(object->vertices[first_vertex_index], object->vertices[second_vertex_index]) + 1);
	object->physics.torque = 0;
	object->physics.Rotation_Vertex = object->physics.Center_of_Mass;

	// Get Friction Coefficient
	friction_coefficient = (getFrictionCoefficient(object->rigid.material, first_object_material, true) + getFrictionCoefficient(object->rigid.material, second_object_material, true)) / 2;

	// Apply Friction
	maskCollisionsVerticalFrictionHelper(object, normal_force, friction_coefficient);
}

void Source::Collisions::Mask::maskCollisionsRigidVerticalSingleHelper(Object::Physics::Rigid::RigidBody* object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Insert Results as Possible Rotation Vertex
	object->possible_rotation_vertices.push_back(object->vertices[vertex_index]);

	// Shift box up
	object->physics.Position.x = returned_value - object->vertices[vertex_index].x;
	object->physics.Velocity.x = 0;

	// Set Rotation Vertex to Vertex
	object->physics.Rotation_Vertex = object->vertices[vertex_index];

	// Get Friction Coefficient
	friction_coefficient = getFrictionCoefficient(object->rigid.material, object_material, true);

	// Apply Friction
	maskCollisionsVerticalFrictionHelper(object, normal_force, friction_coefficient);
}

void Source::Collisions::Mask::maskCollisionsVerticalFrictionHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, float& friction_coefficient)
{
	static float max_friction, velocity_angle;

	if (object->physics.Velocity.y)
	{
		max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y);
		velocity_angle = atan(object->physics.Velocity.y / object->physics.Velocity.x);
		if (object->physics.Velocity.y < 0) { velocity_angle += 3.14159f; }
		object->physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
	}
}

void Source::Collisions::Mask::maskCollisionsRigidFloorEdgeHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, short& first_vertex_index, short& second_vertex_index, short floor_mask_count, Object::Mask::Floor::FloorMask** floor_masks)
{
	static float returned_edge_value, friction_coefficient;
	static int returned_edge_material;
	static glm::vec2 returned_edge_vertex, edge_normal_force;

	// Only Test Edge of Masks Only if Both Vertices Don't Touch A Mask
	if (!(first_vertex_collision && second_vertex_collision))
	{
		// Test if Object Intersects Edge of Collision Mask
		if (collisionsFloorMaskEdge(object->vertices[first_vertex_index] + object->physics.Position, object->vertices[second_vertex_index] + object->physics.Position, returned_edge_value, returned_edge_vertex, returned_edge_material, floor_mask_count, floor_masks))
		{
			// Localize Vertex for Object
			returned_edge_vertex -= object->physics.Position;

			// Calculate Normal Force
			edge_normal_force = -glm::vec2(cos(1.5708f), sin(1.5708f)) * sin(1.5708f) * object->physics.Forces.y;
			if (abs(edge_normal_force.x) < 0.05f) { edge_normal_force.x = 0; }
			object->physics.Forces += edge_normal_force;

			// Insert Results as Possible Rotation Vertex
			object->possible_rotation_vertices.push_back(returned_edge_vertex);

			// Shift box up
			object->physics.Position.y = returned_edge_value - returned_edge_vertex.y;
			object->physics.Velocity.y = 0;
			object->physics.grounded = true;

			// Set Rotation Vertex to Vertex
			object->physics.Rotation_Vertex = returned_edge_vertex;

			// Get Friction Coefficient
			friction_coefficient = getFrictionCoefficient(object->rigid.material, returned_edge_material, true);

			// Apply Friction
			maskCollisionsHorizontalFrictionHelper(object, edge_normal_force, friction_coefficient);
		}
	}
}

void Source::Collisions::Mask::maskCollisionsRigidCircle(Object::Physics::Rigid::RigidCircle& object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
	Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks)
{
	// Test CollisionMaskFloor
	float returned_value, angle;
	int material_index;
	if (collisionsFloorMask(object.physics.Position - glm::vec2(0.0f, object.radius), returned_value, angle, material_index, floor_mask_count, floor_masks))
	{
		// Apply Normal Force
		float temp_angle = 3.14159f - angle;
		temp_angle = 1.5708f + angle;
		glm::vec2 normal_force = -glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * object.physics.Forces.y;
		//normal_force = glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * physics.Mass * GRAVITATIONAL_ACCELERATION;
		if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
		object.physics.Forces += normal_force;

		// Apply Friction
		if (object.physics.Velocity.x)
		{
			//std::cout << "lol\n";

			float friction_coefficient = getFrictionCoefficient(object.rigid.material, material_index, true);
			float max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y) * .2f;
			float velocity_angle = atan(object.physics.Velocity.y / object.physics.Velocity.x);
			if (object.physics.Velocity.x < 0) { velocity_angle += 3.14159f; }
			object.physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
		}

		// Perform Collision Resolution
		object.physics.Position.y = returned_value + object.radius;
		object.physics.Velocity.y = 0;
		object.physics.grounded = true;
		object.physics.Rotation_Vertex = glm::vec2(0.0f, -object.radius);
	}
}

void Source::Collisions::Mask::maskCollisionsSoft(Object::Physics::Soft::SoftBody * object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
	Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks)
{
	// Test CollisionMaskFloor
	float returned_value, angle;
	int material_index;

	for (int i = 0; i < object->node_count; i++)
	{
		Object::Physics::Soft::Node& node = object->nodes[i];

		if (collisionsFloorMask(node.Position - glm::vec2(0.0f, node.Radius), returned_value, angle, material_index, floor_mask_count, floor_masks))
		{
			// Apply Normal Force
			float temp_angle = 3.14159f - angle;
			temp_angle = 1.5708f + angle;
			glm::vec2 normal_force = -glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * node.Forces.y;
			//normal_force = glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * physics.Mass * GRAVITATIONAL_ACCELERATION;
			if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
			node.Forces += normal_force;

			// Apply Friction
			if (node.Velocity.x)
			{
				//std::cout << "lol\n";

				float friction_coefficient = getFrictionCoefficient(node.material, material_index, true);
				float max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y) * .2f;
				float velocity_angle = atan(node.Velocity.y / node.Velocity.x);
				if (node.Velocity.x < 0) { velocity_angle += 3.14159f; }
				node.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
			}

			// Perform Collision Resolution
			node.Position.y = returned_value + node.Radius;
			node.Velocity.y = 0;
			//node.grounded = true;
		}
	}
}

float Source::Collisions::Mask::getFrictionCoefficient(int index1, int index2, bool kinetic)
{
	int min_friction, max_friction = 0, friction_index = 0;
	if (index1 > index2) { min_friction = index2; max_friction = index1; }
	else { min_friction = index1; max_friction = index2; }
	friction_index = Constant::MATERIALS_COUNT * min_friction + (max_friction - min_friction);
	if (kinetic) { return Global::kinetic_friction[friction_index]; }
	return Global::static_friction[friction_index];
}

void Source::Collisions::Mask::maskCollisionsPhysics(Object::Physics::PhysicsBase* object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks)
{
	switch (object->base)
	{

	// Object is a Rigid Body
	case Object::Physics::PHYSICS_BASES::RIGID_BODY:
	{
		maskCollisionsRigid(static_cast<Object::Physics::Rigid::RigidBody*>(object), floor_mask_count, left_mask_count, right_mask_count, ceiling_mask_count, trigger_mask_count,
			floor_masks, left_masks, right_masks, ceiling_masks, trigger_masks);
		break;
	}

	// Object is a Soft Body
	case Object::Physics::PHYSICS_BASES::SOFT_BODY:
	{
		maskCollisionsSoft(static_cast<Object::Physics::Soft::SoftBody*>(object), floor_mask_count, left_mask_count, right_mask_count, ceiling_mask_count, trigger_mask_count,
			floor_masks, left_masks, right_masks, ceiling_masks, trigger_masks);
		break;
	}

	// Object is a Hinge
	case Object::Physics::PHYSICS_BASES::HINGE_BASE:
	{
		if (object->type == Object::Physics::PHYSICS_TYPES::TYPE_HINGE)
		{
			Object::Physics::Hinge::Hinge* hinge = static_cast<Object::Physics::Hinge::Hinge*>(object);
			for (int i = 0; i < hinge->number_of_objects; i++)
				maskCollisionsPhysics(hinge->objects[i], floor_mask_count, left_mask_count, right_mask_count, ceiling_mask_count, trigger_mask_count,
					floor_masks, left_masks, right_masks, ceiling_masks, trigger_masks);
			for (int i = 0; i < hinge->number_of_children; i++)
				maskCollisionsPhysics(hinge->children[i], floor_mask_count, left_mask_count, right_mask_count, ceiling_mask_count, trigger_mask_count,
					floor_masks, left_masks, right_masks, ceiling_masks, trigger_masks);
		}

		break;
	}

	}
}

void Source::Collisions::Mask::maskCollisionsEntity(Object::Entity::EntityBase& entity, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
	Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks)
{
	int iterator = 0;
	glm::vec2 offset_position = glm::vec2(0.0f, 0.0f);
	glm::vec2& position = entity.state.position;

	// The Error is the Amount of Distance To Test Collisions For in Mask
	float error = Constant::SPEED * Global::deltaTime * 1.5f + 1.0f;

	// Floor Masks
	for (iterator = 0; iterator < floor_mask_count; iterator++)
	{
		// Perform Collision Detection
		offset_position = position - glm::vec2(0.0f, entity.entity.half_collision_height);
		if (floor_masks[iterator]->testCollisions(offset_position, error, true))
		{
			// Perform Collision Resolution
			offset_position.y = floor_masks[iterator]->returned_value;
			position.y = offset_position.y + entity.entity.half_collision_height;
			entity.state.velocity.y = 0;
			entity.state.grounded = true;

			// Calculate Momentum
			if (entity.state.momentum.y < 0)
			{
				// Bonk if Momentum is High
				if (entity.state.momentum.y < -20)
				{
					std::cout << "Bonked Ground\n";

					// Damage Player
					//entity.Damage((int)pow(-player.momentum.y - 20, 1.75));
				}

				entity.state.momentum.y = 0;
			}
		}
	}

	// Left Masks
	for (iterator = 0; iterator < left_mask_count; iterator++)
	{
		// Perform Collision Detection
		offset_position = position - glm::vec2(entity.entity.half_collision_width, 0.0f);
		if (left_masks[iterator]->testCollisions(offset_position, error))
		{
			// Perform Collision Resolution
			offset_position.x = left_masks[iterator]->returned_value;
			position.x = offset_position.x + entity.entity.half_collision_width;
			entity.state.velocity.x = 0;

			// Calculate Momentum
			if (entity.state.momentum.x < 0)
			{
				if (entity.state.momentum.x < -20.4)
				{
					//entity.Damage((int)pow(-player.momentum.x - 20.4, 1.75));
				}

				entity.state.momentum.x = 0;
			}

		}
	}

	// Right Masks
	for (iterator = 0; iterator < right_mask_count; iterator++)
	{
		// Perform Collision Detection
		offset_position = position + glm::vec2(entity.entity.half_collision_width, 0.0f);
		if (right_masks[iterator]->testCollisions(offset_position, error))
		{
			// Perform Collision Resolution
			offset_position.x = right_masks[iterator]->returned_value;
			position.x = offset_position.x - entity.entity.half_collision_width;
			entity.state.velocity.x = 0;

			// Calculate Momentum
			if (entity.state.momentum.x > 0)
			{
				if (entity.state.momentum.x > 20.4)
				{
					//entity.Damage((int)pow(player.momentum.x - 20.4, 1.75));
				}

				entity.state.momentum.x = 0;
			}

		}
	}

	// Ceiling Masks
	for (iterator = 0; iterator < ceiling_mask_count; iterator++)
	{
		// Perform Collision Detection
		offset_position = position + glm::vec2(0.0f, entity.entity.half_collision_height);
		if (ceiling_masks[iterator]->testCollisions(offset_position, error))
		{
			// Perform Collision Resolution
			offset_position.y = ceiling_masks[iterator]->returned_value;
			position.y = offset_position.y - entity.entity.half_collision_height;
			entity.state.velocity.y = 0;
			entity.state.grounded = true;

			// Calculate Momentum
			if (entity.state.momentum.y > 0)
			{
				// Bonk if Momentum is High
				if (entity.state.momentum.y > 20)
				{
					// Damage Player
					//entity.Damage((int)pow(player.momentum.y - 20, 1.75));
				}

				entity.state.momentum.y = 0;
			}
		}
	}

	// Trigger Masks
}
