#ifndef PHYSICS_COLLISIONS_H
#define PHYSICS_COLLISIONS_H

#include "ExternalLibs.h"
#include "Render/Struct/List.h"

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
		struct Node;
	}

	namespace Hinge
	{
		class Hinge;
	}
}

namespace Object::Entity
{
	class EntityBase;
};

namespace Source::Collisions::Physics
{
	// Collision Loc Structure
	struct Collision_Loc
	{
		bool collision = false; // Collision Flag
		bool greatest = false; // Greater Distance Flag
		bool swap = false; // Swap Collision Locs Flag
		float greatest_distance = 0.0f; // Current Greatest Collision Loc Distance
		float mx = 0.0f; // Part of Edge Slope
		float my = 0.0f; // Part of Edge Slope
		glm::vec2 object_edge = glm::vec2(0.0f); // Location of Object Edge in Collision
		glm::vec2 object_vertex = glm::vec2(0.0f); // Location of Vertex in Collision

		glm::vec2 temp_collision_locs1[4] = { glm::vec2(0.0f) };
		glm::vec2 temp_collision_locs2[4] = { glm::vec2(0.0f) };
		uint8_t temp_collision_index = 0;
	};

	// Default Collision Detection Algorithm
	bool primaryCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, short object1_vertex_count, short object2_vertex_count);

	// Collision Detection Algorithm With a Circle
	bool secondaryCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidCircle& object2, short object1_vertex_count, bool inverse);

	// Collision Detection Algorithm With a Node
	bool trinaryCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, short object_vertex_count, bool inverse);

	// Collision Between Circles
	bool collisionDetectionCircle(Object::Physics::Rigid::RigidCircle& object1, Object::Physics::Rigid::RigidCircle& object2);

	// Collision Between a Circle and a Node
	bool collisionDetectionCircleNode(Object::Physics::Rigid::RigidCircle& object, Object::Physics::Soft::Node& node);

	// Collision Detection Algorithm Between Nodes
	bool collisionDetectionNode(Object::Physics::Soft::Node& node1, Object::Physics::Soft::Node& node2);

	// Function to Perform Initial Circle Collision Detection
	bool initialCircleCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, float& theta_object1, float& theta_object2);

	// Function to Perform Initial Circle Collision Detection With a Node
	bool initialCircleCollisionDetectionNode(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, float& theta_object, float& theta_node);

	// Function to Set Up the Ray Method
	template <class Type1, class Type2> void setupRayMethod(Type1* object1, Type2* object2, Collision_Loc& collision_loc, glm::vec2& object1_position, glm::vec2& object2_position, float& object_theta1, float& object_theta2, short object1_vertex_count, short object2_vertex_count, bool enable_swap);

	// Function to Perform Ray Method
	bool performRayMethod(Collision_Loc& collision_loc, glm::vec2 object_position, glm::vec2& object_vertex_left, glm::vec2& object_vertex_right, glm::vec2& pos_offset, float& theta);

	// Final Step in Collision Detection / Setup for Collision Resolution
	bool finalizeCollisionDetection(Collision_Loc& collision_loc);

	// Rigid Physics - Entity Collision Detection
	bool physicsEntityRigidCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, short object_vertex_count);

	// Basic Circle Collision Detection Between Rigid Object and Entity
	bool physicsEntityBasicCircleCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, float& theta_object, float& theta_player);

	// Collision Resolution Between Rigid Body Object and Entity
	bool physicsEntityRigidResolutionHelper(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, Collision_Loc& collision_loc, float& theta_player, float& theta_object, short& object_vertex_count);

	// Entity Circle Collision
	bool physicsEntityRigidCollisionDetectionCircle(Object::Physics::Rigid::RigidCircle& object, Object::Entity::EntityBase& entity);

	// Entity Node Collision
	bool physicsEntityRigidCollisionDetectionNode(Object::Physics::Soft::Node & node, Object::Entity::EntityBase& entity);

	// Object Collision Handler
	void objectCollisionDetectionOuter(Object::Physics::PhysicsBase* pointer_to_object1, Struct::List<Object::Physics::PhysicsBase>::Iterator it, Struct::List<Object::Physics::PhysicsBase>& physics_list);

	// Object Collision Handler
	void objectCollisionDetectionOuterHinge(Object::Physics::PhysicsBase* pointer_to_hinge, Struct::List<Object::Physics::PhysicsBase>::Iterator it, Struct::List<Object::Physics::PhysicsBase>& physics_list);

	// Object Collision Handler
	void objectCollisionDetectionInner(Object::Physics::PhysicsBase* pointer_to_object1, Object::Physics::PhysicsBase* pointer_to_object2);

	// Object Collision Handler
	void objectCollisionDetectionInnerHinge(Object::Physics::PhysicsBase* pointer_to_object1, Object::Physics::PhysicsBase* pointer_to_hinge);

	// Collision Resolution Between Rigid Bodies
	void resolveRigid(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, bool enable_first_rotation_vertex, bool enable_second_rotation_vertex);

	// Collision Resolution Between Soft Bodies
	void resolveSoft(Object::Physics::Soft::Node& node1, Object::Physics::Soft::Node& node2);

	// Collision Resolution Between Rigid and Soft Bodies
	void resolveRigidAndSoft(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, bool enable_object_rotate);

	// Rigid Body Physics - Entity Collision Resolver
	void physicsEntityCollisionRigidResolution(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, bool enable_rotation_vertices);

	// Soft Body Physics - Entity Collision Resolver
	void physicsEntityCollisionSoftResolution(Object::Physics::Soft::Node& node, Object::Entity::EntityBase& entity);

	// Hinge - Entity Collisions Helper
	void physicsEntityHingeCollisionHelper(Object::Physics::Hinge::Hinge& hinge, Struct::List<Object::Entity::EntityBase>& entity_list);

	// Physics Collision Handler
	void physicsCollisionDetection(Struct::List<Object::Physics::PhysicsBase>& physics_list);

	// Physics - Entity Collision Handler
	void physicsEntityCollisionDetection(Struct::List<Object::Physics::PhysicsBase>& physics_list, Struct::List<Object::Entity::EntityBase>& entity_list);
}

#endif
