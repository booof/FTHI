#pragma once
#ifndef MASK_COLLISIONS_H
#define MASK_COLLISIONS_H

#include "ExternalLibs.h"

namespace Object::Mask
{
	namespace Floor
	{ 
		class FloorMask; 
	}

	namespace Left 
	{
		class LeftMask;
	}

	namespace Right
	{
		class RightMask;
	}

	namespace Ceiling
	{
		class CeilingMask;
	}

	namespace Trigger
	{
		class TriggerMask;
	}
}

namespace Object::Physics
{
	class PhysicsBase;

	namespace Rigid
	{
		class RigidBody;
		class RigidCircle;
	}

	namespace Soft
	{
		class SoftBody;
	}
}

namespace Object::Entity
{
	class EntityBase;
};

namespace Source::Collisions::Mask
{
	// Floor Value Collision Detection
	bool collisionsFloorMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int floor_mask_count, Object::Mask::Floor::FloorMask** floor_masks);

	// Floor Value Edge Collision Detection
	bool collisionsFloorMaskEdge(glm::vec2 left_vertex, glm::vec2 right_vertex, float& returned_value, glm::vec2& returned_vertex, int& material_index, int floor_mask_count, Object::Mask::Floor::FloorMask ** floor_masks);

	// Wall Value Collisions
	bool collisionsLeftWallMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int left_mask_count, Object::Mask::Left::LeftMask** left_masks);

	// Wall Value Collisions
	bool collisionsRightWallMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int right_mask_count, Object::Mask::Right::RightMask** right_masks);

	// Ceiling Value Collision Detection
	bool collisionsCeilingMask(glm::vec2 value, float& returned_value, float& angle, int& material_index, int ceiling_mask_count, Object::Mask::Ceiling::CeilingMask** ceiling_masks);

	// Mask Collisions for Rigid Body Objects
	void maskCollisionsRigid(Object::Physics::Rigid::RigidBody* object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
		Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks);

	// Helper Function for Horizontal Collision Resolution With Rigid Bodies
	void maskCollisionsRigidHorizontalHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material);

	// Helper Function for Vertical Collision Resolution With Rigid Bodies
	void maskCollisionsRigidVerticalHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material);

	// Helper Function to Calculate Normal Force With Rigid Bodied
	void maskCollisionsRigidNormalForceHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, bool& first_vertex_collision, bool& second_vertex_collision, float& first_vertex_angle, float& second_vertex_angle);

	// Helper Function to Resolve Horizontal Collisions With Both Vertices With Rigid Bodies
	void maskCollisionsRigidHorizontalBothHelper(Object::Physics::Rigid::RigidBody* object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force);

	// Helper Function to Resolve Horizontal Collisions With a Single Vertex With Rigid Bodies
	void maskCollisionsRigidHorizontalSingleHelper(Object::Physics::Rigid::RigidBody* object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force);

	// Helper Function to Apply Horizontal Friction
	void maskCollisionsHorizontalFrictionHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, float& friction_coefficient);

	// Helper Function to Resolve Vertical Collisions With Both Vertices With Rigid Bodies
	void maskCollisionsRigidVerticalBothHelper(Object::Physics::Rigid::RigidBody* object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force);

	// Helper Function to Resolve Vertical Collisions With a Single Vertex With Rigid Bodies
	void maskCollisionsRigidVerticalSingleHelper(Object::Physics::Rigid::RigidBody* object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force);

	// Helper Function to Apply Vertical Friction
	void maskCollisionsVerticalFrictionHelper(Object::Physics::Rigid::RigidBody* object, glm::vec2& normal_force, float& friction_coefficient);

	// Helper Function to Resolve Floor Edge Collisions With Rigid Bodies
	void maskCollisionsRigidFloorEdgeHelper(Object::Physics::Rigid::RigidBody* object, bool& first_vertex_collision, bool& second_vertex_collision, short& first_vertex_index, short& second_vertex_index, short floor_mask_count, Object::Mask::Floor::FloorMask** floor_masks);

	// Rigid Body Circle - Mask Collisions
	void maskCollisionsRigidCircle(Object::Physics::Rigid::RigidCircle& object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count,
		Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks);

	// Soft Body - Mask Collisions
	void maskCollisionsSoft(Object::Physics::Soft::SoftBody * object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count,
		Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks);

	// Retrieves the correct friction coefficient
	float getFrictionCoefficient(int index1, int index2, bool kinetic);

	// Mask Collisions for Physics Objects
	void maskCollisionsPhysics(Object::Physics::PhysicsBase* object, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count,
		Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks);

	// Mask Collisions for Entities
	void maskCollisionsEntity(Object::Entity::EntityBase& entity, int floor_mask_count, int left_mask_count, int right_mask_count, int ceiling_mask_count, int trigger_mask_count, 
		Object::Mask::Floor::FloorMask** floor_masks, Object::Mask::Left::LeftMask** left_masks, Object::Mask::Right::RightMask** right_masks, Object::Mask::Ceiling::CeilingMask** ceiling_masks, Object::Mask::Trigger::TriggerMask** trigger_masks);
}

#endif
