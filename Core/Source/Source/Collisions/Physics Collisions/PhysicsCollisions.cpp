#include "PhysicsCollisions.h"

#include "Globals.h"
#include "Constants.h"

#include "Class/Object/Physics/RigidBody/RigidBody.h"
#include "Class/Object/Physics/RigidBody/RigidRectangle.h"
#include "Class/Object/Physics/RigidBody/RigidTrapezoid.h"
#include "Class/Object/Physics/RigidBody/RigidTriangle.h"
#include "Class/Object/Physics/RigidBody/RigidCircle.h"
#include "Class/Object/Physics/RigidBody/RigidPolygon.h"
#include "Class/Object/Physics/SoftBody/SoftBody.h"
#include "Class/Object/Physics/SoftBody/SpringMass.h"
#include "Class/Object/Physics/SoftBody/Wire.h"
#include "Class/Object/Physics/Hinge/Hinge.h"
#include "Class/Render/Shape/Shape.h"

#include "Class/Object/Entity/EntityBase.h"

//#include "Source/Algorithms/Common/Common.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"

#include "Source/Vertices/Visualizer/Visualizer.h"

// Temporary Collision Values
glm::vec2 temp_collision_loc1;
glm::vec2 temp_collision_loc2;
float temp_collision_angle1;
float temp_collision_angle2;
float temp_effectivness1;
float temp_collision_slope1;
float temp_collision_slope2;
float temp_possible_vertex2;

bool Source::Collisions::Physics::primaryCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, short object1_vertex_count, short object2_vertex_count)
{
	// Initial Object Angles
	static float theta_object1, theta_object2;

	// Object Positions
	static glm::vec2 object_position1, object_position2;

	// Perform Circle Collision Detection
	if (initialCircleCollisionDetection(object1, object2, theta_object1, theta_object2))
		return false;

	// Collision Detection Variables
	Collision_Loc collision_loc;

	// Object Positions
	object_position1 = object1->physics.Position;
	object_position2 = object2->physics.Position;

	// Collision Detection Using Vertices of Object1
	setupRayMethod(object1, object2, collision_loc, object_position1, object_position2, theta_object1, theta_object2, object1_vertex_count, object2_vertex_count, false);

	// Collision Deterction Using Vertices of Object2
	setupRayMethod(object2, object1, collision_loc, object_position2, object_position1, theta_object2, theta_object1, object2_vertex_count, object1_vertex_count, true);

	// Collision Resolution
	return finalizeCollisionDetection(collision_loc);
}

bool Source::Collisions::Physics::secondaryCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidCircle& object2, short object1_vertex_count, bool inverse)
{
	// Initial Object Angles
	static float theta_object1, theta_object2;

	// Object Positions
	static glm::vec2 object_position;

	// Quadrant
	static int quadrant;

	// Vertices
	static glm::vec2 vertex_left, vertex_right, position_offset;

	// Initial Circle Collision Detection
	if (initialCircleCollisionDetection(object1, &object2, theta_object1, theta_object2))
		return false;

	// Determine Quadrant and Vertices of Object
	quadrant = Algorithms::Math::findClosestWithoutGoingOver(theta_object1, object1->angle_offsets, object1_vertex_count);
	vertex_left = object1->vertices[object1->angle_to_vertex_left[quadrant]];
	vertex_right = object1->vertices[object1->angle_to_vertex_right[quadrant]];

	Vertices::Visualizer::visualizePoint(vertex_left + object1->physics.Position, 0.3f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(vertex_right + object1->physics.Position, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	// Parameterize Vertices
	float mx = vertex_right.x - vertex_left.x;
	float bx = vertex_left.x;
	float my = vertex_right.y - vertex_left.y;
	float by = vertex_left.y;

	// Find Angle Perpendicular to Edge
	float theta = atan(my / mx) + 1.5708f * Algorithms::Math::getSign(mx);
	float sine = sin(theta);
	if (sine == 0)
		sine = 0.001f;
	float cosine = cos(theta);
	if (cosine == 0)
		cosine = 0.001f;

	// Determine if Position of Circle Exists Between Edge
	float t = (bx / cosine + (object2.physics.Position.y - object1->physics.Position.y) / sine - by / sine - (object2.physics.Position.x - object1->physics.Position.x) / cosine) / (my / sine - mx / cosine);

	// Circle Exists Before Left Vertex
	if (t < 0)
	{
		if (glm::distance(object1->physics.Position + vertex_left, object2.physics.Position) < object2.radius)
		{
			// Get Angle Between Circle and Vertex
			glm::vec2 delta_pos = -object2.physics.Position + (object1->physics.Position + vertex_left);
			theta = atan(delta_pos.y / delta_pos.x);
			if (delta_pos.x < 0)
				theta += 3.14159f;

			// Get Collision Locs
			temp_collision_loc2 = object2.physics.Position + object2.radius * Algorithms::Math::parameterizeAngle(theta);
			temp_collision_loc1 = object1->physics.Position + vertex_left;

			return true;
		}
	}

	// Circle Exists After Right Vertex
	else if (t > 1)
	{
		if (glm::distance(object1->physics.Position + vertex_right, object2.physics.Position) < object2.radius)
		{
			// Get Angle Between Circle and Vertex
			glm::vec2 delta_pos = -object2.physics.Position + (object1->physics.Position + vertex_right);
			theta = atan(delta_pos.y / delta_pos.x);
			if (delta_pos.x < 0)
				theta += 3.14159f;

			// Get Collision Locs
			temp_collision_loc2 = object2.physics.Position + object2.radius * Algorithms::Math::parameterizeAngle(theta);
			temp_collision_loc1 = object1->physics.Position + vertex_right;

			return true;
		}
	}

	// Circle is Between Edge
	else
	{
		// Get Vertex Positions
		glm::vec2 vertex_pos = glm::vec2(mx * t + bx, my * t + by) + object1->physics.Position;
		glm::vec2 edge = object2.physics.Position + object2.radius * Algorithms::Math::parameterizeAngle(theta);
		object_position = object2.physics.Position;

		Vertices::Visualizer::visualizePoint(edge, 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		Vertices::Visualizer::visualizePoint(vertex_pos, 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		// Squeeze Method
		if (((edge.x < vertex_pos.x && vertex_pos.x < object_position.x) || (edge.x > vertex_pos.x && vertex_pos.x > object_position.x))
			&& ((edge.y < vertex_pos.y && vertex_pos.y < object_position.y) || (edge.y > vertex_pos.y && vertex_pos.y > object_position.y)))
		{


			// Get Collision Locs
			temp_collision_loc2 = edge;
			temp_collision_loc1 = vertex_pos;

			return true;
		}
	}

	return false;
}

bool Source::Collisions::Physics::trinaryCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, short object_vertex_count, bool inverse)
{
	// Initial Object Angles
	static float theta_object, theta_node;

	// Object Positions
	static glm::vec2 object_position;

	// Quadrant
	static int quadrant;

	// Vertices
	static glm::vec2 vertex_left, vertex_right;

	// Initial Circle Collision Detection
	if (initialCircleCollisionDetectionNode(object, node, theta_object, theta_node))
		return false;

	// Determine Quadrant and Vertices of Object
	quadrant = Algorithms::Math::findClosestWithoutGoingOver(theta_object, object->angle_offsets, object_vertex_count);

	//findClosestWithoutGoingOver(float test_value, Object::Physics::Rigid::Named_Node* list, int list_size)

	vertex_left = object->vertices[object->angle_to_vertex_left[quadrant]];
	vertex_right = object->vertices[object->angle_to_vertex_right[quadrant]];

	// Parameterize Vertices
	float mx = vertex_right.x - vertex_left.x;
	float bx = vertex_left.x;
	float my = vertex_right.y - vertex_left.y;
	float by = vertex_left.y;

	// Find Angle Perpendicular to Edge
	float theta = atan(my / mx) + 1.5708f * Algorithms::Math::getSign(mx);
	float sine = sin(theta);
	if (sine == 0)
		sine = 0.001f;
	float cosine = cos(theta);
	if (cosine == 0)
		cosine = 0.001f;

	// Determine if Position of Circle Exists Between Edge
	float t = (bx / cosine + (node.Position.y - object->physics.Position.y) / sine - by / sine - (node.Position.x - object->physics.Position.x) / cosine) / (my / sine - mx / cosine);
	if (t > 1.0f || t < 0.0f)
		return false;

	// Get Vertex Positions
	glm::vec2 vertex_pos = glm::vec2(mx * t + bx, my * t + by) + object->physics.Position;
	glm::vec2 edge = node.Position + node.Radius * Algorithms::Math::parameterizeAngle(theta);
	object_position = node.Position;

	// Squeeze Method
	if (((edge.x < vertex_pos.x && vertex_pos.x < object_position.x) || (edge.x > vertex_pos.x && vertex_pos.x > object_position.x))
		&& ((edge.y < vertex_pos.y && vertex_pos.y < object_position.y) || (edge.y > vertex_pos.y && vertex_pos.y > object_position.y)))
	{
		// Get Collision Locs
		temp_collision_loc2 = edge;
		temp_collision_loc1 = vertex_pos;

		return true;
	}

	return false;
}

bool Source::Collisions::Physics::collisionDetectionCircle(Object::Physics::Rigid::RigidCircle& object1, Object::Physics::Rigid::RigidCircle& object2)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = object2.physics.Position - object1.physics.Position;
	theta1 = Algorithms::Math::angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = object1.physics.Position + object1.radius * Algorithms::Math::parameterizeAngle(theta1);
	temp_collision_loc2 = object2.physics.Position + object2.radius * Algorithms::Math::parameterizeAngle(theta2);

	Vertices::Visualizer::visualizePoint(object1.physics.Position + glm::vec2(0.0f, 3.0f), 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(temp_collision_loc1, 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(temp_collision_loc2, 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	//Visualize_Line(object1.physics.Position, object1.physics.Position + 100.0f * Parameterize_Angle(theta1), 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(object1.physics.Position, object2.physics.Position);
	if (distance > object1.radius + object2.radius)
		return false;

	return true;
}

bool Source::Collisions::Physics::collisionDetectionCircleNode(Object::Physics::Rigid::RigidCircle& object, Object::Physics::Soft::Node& node)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = node.Position - object.physics.Position;
	theta1 = Algorithms::Math::angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = object.physics.Position + object.radius * Algorithms::Math::parameterizeAngle(theta1);
	temp_collision_loc2 = node.Position + node.Radius * Algorithms::Math::parameterizeAngle(theta2);

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(object.physics.Position, node.Position);
	if (distance > object.radius + node.Radius)
		return false;

	return true;
}

bool Source::Collisions::Physics::collisionDetectionNode(Object::Physics::Soft::Node& node1, Object::Physics::Soft::Node& node2)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = node2.Position - node1.Position;
	theta1 = Algorithms::Math::angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = node1.Position + node1.Radius * Algorithms::Math::parameterizeAngle(theta1);
	temp_collision_loc2 = node2.Position + node2.Radius * Algorithms::Math::parameterizeAngle(theta2);

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(node1.Position, node2.Position);
	if (distance > node1.Radius + node2.Radius)
		return false;

	return true;
}

bool Source::Collisions::Physics::initialCircleCollisionDetection(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, float& theta_object1, float& theta_object2)
{
	static float delta_w, delta_h, r;

	// Perform Circle Collision by Testing if Distance is Less Than Longest Possible Edges of Objects Combined
	delta_w = object2->physics.Position.x - object1->physics.Position.x;
	delta_h = object2->physics.Position.y - object1->physics.Position.y;
	r = (float)glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (object1->longest_edge + object2->longest_edge < r - 20.0f)
		return true;

	// Calculate Angle Between Objects if Circle Collision Passed
	theta_object1 = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_object1 += 3.14159f;
	if (theta_object1 < 0)
		theta_object1 += 6.28318f;
	theta_object2 = theta_object1 + 3.14159f;
	if (theta_object2 > 6.28318f)
		theta_object2 -= 6.28318f;

	return false;
}

bool Source::Collisions::Physics::initialCircleCollisionDetectionNode(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, float& theta_object, float& theta_node)
{
	static float delta_w, delta_h, r;

	// Perform Circle Collision by Testing if Distance is Less Than Longest Possible Edges of Objects Combined
	delta_w = node.Position.x - object->physics.Position.x;
	delta_h = node.Position.y - object->physics.Position.y;
	r = (float)glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (object->longest_edge + node.Radius < r - 20.0f)
		return true;

	// Calculate Angle Between Objects if Circle Collision Passed
	theta_object = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_object += 3.14159f;
	theta_node = theta_object + 3.14159f;
	if (theta_node > 6.28318f)
		theta_node -= 6.28318f;

	return false;
}

template<class Type1, class Type2>
void Source::Collisions::Physics::setupRayMethod(Type1* object1, Type2* object2, Collision_Loc& collision_loc, glm::vec2& object1_position, glm::vec2& object2_position, float& object_theta1, float& object_theta2, short object1_vertex_count, short object2_vertex_count, bool enable_swap)
{
	static int angle_index, vertex_quadrant;
	static float delta_w, delta_h, theta1, theta2;
	static glm::vec2 pos_offset, vertex_left, vertex_right;
	glm::vec2 position_offsets[2];

	// Find and Store Verties
	angle_index = Algorithms::Math::findClosestWithoutGoingOver(object_theta1, object1->angle_offsets, object1_vertex_count);
	Object::Physics::Rigid::Named_Node* test = 0;
	position_offsets[0] = object1->vertices[object1->angle_to_vertex_left[angle_index]];
	position_offsets[1] = object1->vertices[object1->angle_to_vertex_right[angle_index]];

	for (int i = 0; i < 2; i++)
	{
		// Calculate Object Thetas
		delta_w = position_offsets[i].x + object1_position.x - object2_position.x;
		delta_h = position_offsets[i].y + object1_position.y - object2_position.y;
		pos_offset = glm::vec2(delta_w, delta_h);
		theta1 = atan(delta_h / delta_w);
		theta2 = theta1;
		if (delta_w > 0) { theta1 += 3.14159f; }
		else { theta2 += 3.14159f; }
		if (theta2 < 0) { theta2 += 6.28318f; }

		// Determine Which Vertices to Use
		vertex_quadrant = Algorithms::Math::findClosestWithoutGoingOver(theta2, object2->angle_offsets, object2_vertex_count);
		vertex_left = object2->vertices[object2->angle_to_vertex_left[vertex_quadrant]];
		vertex_right = object2->vertices[object2->angle_to_vertex_right[vertex_quadrant]];

		//Vertices::Visualizer::visualizePoint(vertex_left + object2_position, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		//Vertices::Visualizer::visualizePoint(vertex_right + object2_position, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		// Perform Ray Method
		// IDEA: Possibly Use object1_position Instead of object2_position in Ray Method Calculation. Might Give More Accurate Results. Testing Required
		collision_loc.collision |= performRayMethod(collision_loc, object2_position, vertex_left, vertex_right, pos_offset, theta1);
		collision_loc.swap |= (collision_loc.greatest && enable_swap);
		collision_loc.greatest = false;

		if (enable_swap)
		{
			uint8_t loc_index = collision_loc.temp_collision_index - 1;
			glm::vec2 temp = collision_loc.temp_collision_locs1[loc_index];
			collision_loc.temp_collision_locs1[loc_index] = collision_loc.temp_collision_locs2[loc_index];
			collision_loc.temp_collision_locs2[loc_index] = temp;
		}
	}
}

bool Source::Collisions::Physics::performRayMethod(Collision_Loc& collision_loc, glm::vec2 object_position, glm::vec2& object_vertex_left, glm::vec2& object_vertex_right, glm::vec2& pos_offset, float& theta)
{
	static float mx, bx, my, by, sine, cosine, t, temp_distance;
	static glm::vec2 edge, vertex_pos;

	// Parameterize Object Edge
	mx = object_vertex_right.x - object_vertex_left.x;
	bx = object_vertex_left.x;
	my = object_vertex_right.y - object_vertex_left.y;
	by = object_vertex_left.y;

	// Get Trig Results of Object Edge
	sine = sin(theta);
	if (sine == 0)
		sine = 0.001f;
	cosine = cos(theta);
	if (cosine == 0)
		cosine = 0.001f;

	// Find the Parameter of Object Edge
	t = (bx / cosine + pos_offset.y / sine - by / sine - pos_offset.x / cosine) / (my / sine - mx / cosine);

	// Calculate Global Position of Edge
	edge = glm::vec2(mx * t + bx, my * t + by) + object_position;

	// Calculate Global Position of Object Position Offset
	vertex_pos = pos_offset + object_position;

	//Visualize_Line(vertex_pos, vertex_pos + 100.0f * Parameterize_Angle(theta), 0.1f, glm::vec3(1.0f, 0.0f, 1.0f));
	//Visualize_Point(edge, 0.25f, glm::vec3(0.0f, 0.0f, 0.0f));
	//Visualize_Point(vertex_pos, 0.35f, glm::vec3(1.0f, 0.0f, 0.0f));

	//Visualize_Point(object_position + object_vertex_left, 0.4f, glm::vec3(0.0f, 1.0f, 0.0f));
	//Visualize_Point(object_position + object_vertex_right, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));


	if (t > 1 || t < 0) { return false; }

	// Perform Squeeze Method
	if (((edge.x < vertex_pos.x && vertex_pos.x < object_position.x) || (edge.x > vertex_pos.x && vertex_pos.x > object_position.x))
		&& ((edge.y < vertex_pos.y && vertex_pos.y < object_position.y) || (edge.y > vertex_pos.y && vertex_pos.y > object_position.y)))
	{
		// IDEA: Use Average Values in Collision Loc Instead of Greatest Value
		// Average is Only Used If Both Vertices in a Single Object Collide
		// Greatest Will Still be Used to Enable Location Swap
		// This Will Allow Boxes to Stay Ontop of Eachother'

		// Another IDEA:
		// Swapping Will be Applied Directly After Ray Method Instead of After All Rays are Finialized
		// This Will Allow Averages to be Applied for Both Objects
		// This Also Means We No Longer Need to Calculate Greatest Distance

		uint8_t& loc_index = collision_loc.temp_collision_index;
		collision_loc.temp_collision_locs2[loc_index] = edge;
		collision_loc.temp_collision_locs1[loc_index] = vertex_pos;
		loc_index++;

		// Determine if Collision Resolution Has Greater Distance
		temp_distance = glm::distance(edge, vertex_pos);
		if (temp_distance > collision_loc.greatest_distance)
		{
			//std::cout << temp_distance << "    ddd\n";
			collision_loc.greatest_distance = temp_distance;
			collision_loc.object_edge = edge;
			collision_loc.object_vertex = vertex_pos;
			collision_loc.greatest = true;
			collision_loc.mx = mx;
			collision_loc.my = my;
		}

		return true;
	}

	return false;
}

bool Source::Collisions::Physics::finalizeCollisionDetection(Collision_Loc& collision_loc)
{
	// Perform Collision Resolution
	if (collision_loc.collision)
	{
		// Store Collision Loc
		if (collision_loc.swap)
		{
			temp_collision_loc1 = collision_loc.object_edge;
			temp_collision_loc2 = collision_loc.object_vertex;
		}

		else
		{
			temp_collision_loc2 = collision_loc.object_edge;
			temp_collision_loc1 = collision_loc.object_vertex;
		}

		//temp_collision_loc1 = glm::vec2(0.0f, 0.0f);
		//temp_collision_loc2 = glm::vec2(0.0f, 0.0f);
		//for (int i = 0; i < collision_loc.temp_collision_index; i++)
		//{
		//	temp_collision_loc1 += collision_loc.temp_collision_locs1[i];
		//	temp_collision_loc2 += collision_loc.temp_collision_locs2[i];
		//}
		//temp_collision_loc1 /= collision_loc.temp_collision_index;
		//temp_collision_loc2 /= collision_loc.temp_collision_index;

		//Vertices::Visualizer::visualizePoint(temp_collision_loc1, 1.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		//Vertices::Visualizer::visualizePoint(temp_collision_loc2, 1.2f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		return true;
	}

	return false;
}

bool Source::Collisions::Physics::physicsEntityRigidCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, short object_vertex_count)
{
	// Initial Object angles
	static float theta_object, theta_entity;

	// Positions
	static glm::vec2 object_position, entity_position;

	// Perform Basic Circle Collision
	if (physicsEntityBasicCircleCollisionDetection(object, entity, theta_object, theta_entity))
		return false;

	// Collision Detection Variables
	Collision_Loc collision_loc;

	// Positions
	object_position = object->physics.Position;
	entity_position = entity.state.position;

	// Collision Using Vertices of Object
	//setupRayMethod(object, entity, collision_loc, object_position, entity_position, theta_object, theta_entity, object_vertex_count, 4, false);
	setupRayMethod(object, &entity, collision_loc, object_position, entity_position, theta_object, theta_entity, object_vertex_count, 4, false);

	// Collision Using Vertices of Player
	setupRayMethod(&entity, object, collision_loc, entity_position, object_position, theta_entity, theta_object, 4, object_vertex_count, true);

	// Collision Resolution
	return physicsEntityRigidResolutionHelper(object, entity, collision_loc, theta_entity, theta_object, object_vertex_count);
}

bool Source::Collisions::Physics::physicsEntityBasicCircleCollisionDetection(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, float& theta_object, float& theta_player)
{
	static float delta_w, delta_h, r;

	// Perform circle collision detection
	delta_w = -entity.state.position.x + object->physics.Position.x;
	delta_h = -entity.state.position.y + object->physics.Position.y;
	r = (float)glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (entity.entity.half_collision_height + object->longest_edge < r - 20.0f)
		return true;

	// Calculate Angle between objects
	theta_player = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_player += 3.14159f;
	theta_object = theta_player + 3.14159f;
	if (theta_object > 6.28318f)
		theta_object -= 6.28318f;

	return false;
}

bool Source::Collisions::Physics::physicsEntityRigidResolutionHelper(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, Collision_Loc& collision_loc, float& theta_player, float& theta_object, short& object_vertex_count)
{
	if (finalizeCollisionDetection(collision_loc))
	{
		static int number_to_quadrant[4] = { 1, 2, 3, 0 };
		static int cross_section, quadrant, index_left, index_right;
		static float theta_edge, delta_h, delta_w, theta_velocity;

		// Determine Cross Section
		cross_section = number_to_quadrant[Algorithms::Math::findClosestWithoutGoingOverNamedIndex(theta_object, object->angle_offsets, object_vertex_count)];
		//cross_section = Closest_Without_Going_Over_Named_Index(theta_object, object.angle_offsets, object_vertex_count) + 1;
		//if (cross_section >= object_vertex_count)
		//	cross_section = 0;

		// Get Vertices of Object
		quadrant = Algorithms::Math::findClosestWithoutGoingOver(theta_object, object->angle_offsets, object_vertex_count);
		index_left = object->angle_to_vertex_left[quadrant];
		index_right = object->angle_to_vertex_right[quadrant];

		//Visualize_Point(object.physics.Position + object.vertices[index_left], 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//Visualize_Point(object.physics.Position + object.vertices[index_right], 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		// Calculate Angle of Collision
		delta_h = object->vertices[index_right].y - object->vertices[index_left].y;
		delta_w = object->vertices[index_right].x - object->vertices[index_left].x;
		theta_edge = Algorithms::Math::angle_from_vector(delta_w, delta_h);
		temp_collision_angle1 = theta_edge + 1.5708f;
		glm::vec2 player_position = entity.state.position;

		// Calculate Angle of Collision
		if (object_vertex_count > 4)
		{
			temp_collision_angle2 = object->angle_between_offsets * index_left;
		}

		else
		{
			temp_collision_angle2 = object->angle_between_offsets * index_left + object->physics.rotation;
		}

		// Determine Effectiveness of Forces Applied by Player
		theta_velocity = Algorithms::Math::angle_from_vector(entity.state.velocity);
		temp_effectivness1 = abs(sin(theta_velocity - theta_edge)) * distance(temp_collision_loc1, temp_collision_loc2) * 1.5f;
		//temp_effectivness1 = 1;
		if (entity.state.velocity.x == 0 && entity.state.velocity.y == 0)
			temp_effectivness1 = 0;

		// Calculate Slope of Player
		temp_collision_slope1 = 1.5708f * ((int)floor((theta_player - 0.7854f) / 3.14159f) % 2);

		// Calculate Slope of Rectangle
		temp_collision_slope2 = theta_edge;
		while (temp_collision_slope2 > 1.5708)
			temp_collision_slope2 -= 3.14159f;
		while (temp_collision_slope2 < -1.5708)
			temp_collision_slope2 += 3.14159f;

		// Modify Collision Angle
		if (object_vertex_count > 4)
		{
			if (entity.state.position.x < object->physics.Position.x)
				temp_collision_angle2 += 3.14159f;
		}

		else
		{
			//if ((cross_section % 2))
			//{
			//	if (((sin(theta_object) > 0) ^ (cos(theta_object)) < 0))
			//	{
			//		temp_collision_angle2 += 3.14159f;
			//	}
			//}

			//if ((cross_section % 2 == 0))
			//{
			//	if (((sin(theta_object) > 0) ^ (cos(theta_object)) > 0))
			//	{
			//		temp_collision_angle2 += 3.14159f;
			//	}
			//}

			if ((cross_section % 2))
			{
				if ((sin(theta_object) > 0) ^ (cos(theta_object) < 0))
				{
					temp_collision_angle2 += 3.14159f;
				}
			}

			if ((cross_section % 2 == 0))
			{
				if ((sin(theta_object) > 0) ^ (cos(theta_object) > 0))
				{
					temp_collision_angle2 += 3.14159f;
				}
			}
		}

		// Add collision location as a possible rotation vertex if needed
		int min1 = -1, min2 = -1;

		for (int i = 0; i < object_vertex_count; i++)
		{
			if (min1 == -1 || object->vertices[i].y < object->vertices[min1].y)
			{
				min2 = min1;
				min1 = i;
			}

			else if (min2 == -1 || object->vertices[i].y < object->vertices[min2].y)
			{
				min2 = i;
			}
		}

		if ((index_left == min1 || index_left == min2) && (index_right == min2 || index_right == min1))
		{
			temp_possible_vertex2 = true;
		}


		return true;

	}

	return false;
}

bool Source::Collisions::Physics::physicsEntityRigidCollisionDetectionCircle(Object::Physics::Rigid::RigidCircle& object, Object::Entity::EntityBase& entity)
{
	float delta_w = -entity.state.position.x + object.physics.Position.x;
	float delta_h = -entity.state.position.y + object.physics.Position.y;
	float r = (float)glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if ((entity.entity.half_collision_height + object.longest_edge) < r)
		return false;

	// Calculate Angle between objects
	float theta = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta += 3.14159f;

	// Calculate Edge of Player
	glm::vec2 edge1 = glm::vec2(0.0f);
	edge1.x = entity.state.position.x + entity.entity.half_collision_width * cos(theta);
	edge1.y = entity.state.position.y + entity.entity.half_collision_height * sin(theta);

	// Calculate Edge of Circle
	glm::vec2 edge2(object.physics.Position.x - object.radius * cos(theta), object.physics.Position.y - object.radius * sin(theta));

	//Visualize_Point(edge1, 1.3f, glm::vec3(1.0f, 0.0f, 0.0f));
	//Visualize_Point(edge2, 1.3f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Determine if Collision Occoured
	if (((entity.state.position.x <= edge2.x && edge2.x <= edge1.x && object.physics.Position.x >= edge1.x) || (object.physics.Position.x <= edge1.x && edge1.x <= edge2.x && entity.state.position.x >= edge2.x))
		&& ((entity.state.position.y <= edge2.y && edge2.y <= edge1.y && object.physics.Position.y >= edge1.y) || (object.physics.Position.y <= edge1.y && edge1.y <= edge2.y && entity.state.position.y >= edge2.y)))
	{
		// Get Location of Collision
		temp_collision_loc2 = edge1;
		temp_collision_loc1 = edge2;
		temp_collision_angle2 = -theta;
		temp_collision_angle1 = theta;
		temp_effectivness1 = 1.0f;

		return true;
	}

	return false;
}

bool Source::Collisions::Physics::physicsEntityRigidCollisionDetectionNode(Object::Physics::Soft::Node & node, Object::Entity::EntityBase& entity)
{
	float delta_w = -entity.state.position.x + node.Position.x;
	float delta_h = -entity.state.position.y + node.Position.y;
	float r = (float)glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if ((entity.entity.half_collision_height + node.Radius) < r)
		return false;

	// Calculate Angle between objects
	float theta = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta += 3.14159f;

	// Calculate Edge of Player
	glm::vec2 edge1 = glm::vec2(0.0f);
	edge1.x = entity.state.position.x + entity.entity.half_collision_width * cos(theta);
	edge1.y = entity.state.position.y + entity.entity.half_collision_height * sin(theta);

	// Calculate Edge of Circle
	glm::vec2 edge2(node.Position.x - node.Radius * cos(theta), node.Position.y - node.Radius * sin(theta));

	//Visualize_Point(edge1, 1.3f, glm::vec3(1.0f, 0.0f, 0.0f));
	//Visualize_Point(edge2, 1.3f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Determine if Collision Occoured
	if (((entity.state.position.x <= edge2.x && edge2.x <= edge1.x && node.Position.x >= edge1.x) || (node.Position.x <= edge1.x && edge1.x <= edge2.x && entity.state.position.x >= edge2.x))
		&& ((entity.state.position.y <= edge2.y && edge2.y <= edge1.y && node.Position.y >= edge1.y) || (node.Position.y <= edge1.y && edge1.y <= edge2.y && entity.state.position.y >= edge2.y)))
	{
		// Get Location of Collision
		temp_collision_loc2 = edge1;
		temp_collision_loc1 = edge2;
		temp_collision_angle2 = -theta;
		temp_collision_angle1 = theta;
		temp_effectivness1 = 1.0f;

		return true;
	}

	return false;
}

void Source::Collisions::Physics::objectCollisionDetectionOuter(Object::Physics::PhysicsBase* pointer_to_object1, Struct::List<Object::Physics::PhysicsBase>::Iterator it, Struct::List<Object::Physics::PhysicsBase>& physics_list)
{
	//for (int j = i + 1; j < physics_count; j++)
	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it2 = it + 1; it2 != physics_list.end(); it2++)
	{
		// Test if Object2 is a Hinge
		if ((*it).type == Object::Physics::PHYSICS_TYPES::TYPE_HINGE)
			objectCollisionDetectionInnerHinge(pointer_to_object1, &it2);

		// Object 2 is a Normal Object
		else
			objectCollisionDetectionInner(pointer_to_object1, &it2);
	}
}

void Source::Collisions::Physics::objectCollisionDetectionOuterHinge(Object::Physics::PhysicsBase* pointer_to_hinge, Struct::List<Object::Physics::PhysicsBase>::Iterator it, Struct::List<Object::Physics::PhysicsBase>& physics_list)
{
	// Statically Cast Object
	Object::Physics::Hinge::Hinge* hinge = static_cast<Object::Physics::Hinge::Hinge*>(pointer_to_hinge);

	// Iterate Through Each Object
	for (int k = 0; k < hinge->number_of_objects; k++)
		objectCollisionDetectionOuter(hinge->objects[k], it, physics_list);

	// Iterate Through Each Hinge
	for (int k = 0; k < hinge->number_of_children; k++)
		objectCollisionDetectionOuterHinge(hinge->children[k], it, physics_list);
}

void Source::Collisions::Physics::objectCollisionDetectionInner(Object::Physics::PhysicsBase* pointer_to_object1, Object::Physics::PhysicsBase* pointer_to_object2)
{
	switch (pointer_to_object1->type)
	{

	// Object 1 is a rectangle
	case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
	{
		Object::Physics::Rigid::RigidRectangle* object1 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 3))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object1, *object2, 4, false))
				resolveRigid(object1, object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, object2->number_of_sides))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (trinaryCollisionDetection(object1, object2->nodes[k], 4, false))
					resolveRigidAndSoft(object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a trapezoid
	case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
	{
		Object::Physics::Rigid::RigidTrapezoid* object1 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, 3))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object1, *object2, 4, false))
				resolveRigid(object1, object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 4, object2->number_of_sides))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (trinaryCollisionDetection(object1, object2->nodes[k], 4, false))
					resolveRigidAndSoft(object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a triangle
	case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
	{
		Object::Physics::Rigid::RigidTriangle* object1 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 3, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 3, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 3, 3))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object1, *object2, 3, false))
				resolveRigid(object1, object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, 3, object2->number_of_sides))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (trinaryCollisionDetection(object1, object2->nodes[k], 3, false))
					resolveRigidAndSoft(object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a circle
	case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
	{
		Object::Physics::Rigid::RigidCircle* object1 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object2, *object1, 4, true))
				resolveRigid(object2, object1, true, false);
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			if (secondaryCollisionDetection(object2, *object1, 4, true))
				resolveRigid(object2, object1, true, false);
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object2, *object1, 3, true))
				resolveRigid(object2, object1, true, false);
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			if (collisionDetectionCircle(*object1, *object2))
				resolveRigid(object1, object2, false, false);
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			if (secondaryCollisionDetection(object2, *object1, object2->number_of_sides, true))
				resolveRigid(object2, object1, true, false);
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (collisionDetectionCircleNode(*object1, object2->nodes[k]))
					resolveRigidAndSoft(object1, object2->nodes[k], false);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a polygon
	case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
	{
		Object::Physics::Rigid::RigidPolygon* object1 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, object1->number_of_sides, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, object1->number_of_sides, 4))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, object1->number_of_sides, 3))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			if (secondaryCollisionDetection(object1, *object2, object1->number_of_sides, false))
				resolveRigid(object1, object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			if (primaryCollisionDetection(object1, object2, object1->number_of_sides, object2->number_of_sides))
				resolveRigid(object1, object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (trinaryCollisionDetection(object1, object2->nodes[k], object1->number_of_sides, false))
					resolveRigidAndSoft(object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a Soft Body
	case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
	case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
	{
		Object::Physics::Soft::SoftBody* object1 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object1);
		switch (pointer_to_object2->type)
		{

		// Object 2 is a rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			Object::Physics::Rigid::RigidRectangle* object2 = static_cast<Object::Physics::Rigid::RigidRectangle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (trinaryCollisionDetection(object2, object1->nodes[k], 4, false))
					resolveRigidAndSoft(object2, object1->nodes[k], true);
			}
			break;
		}

		// Object2 is a trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			Object::Physics::Rigid::RigidTrapezoid* object2 = static_cast<Object::Physics::Rigid::RigidTrapezoid*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (trinaryCollisionDetection(object2, object1->nodes[k], 4, false))
					resolveRigidAndSoft(object2, object1->nodes[k], true);
			}
			break;
		}

		// Object2 is a triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			Object::Physics::Rigid::RigidTriangle* object2 = static_cast<Object::Physics::Rigid::RigidTriangle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (trinaryCollisionDetection(object2, object1->nodes[k], 3, false))
					resolveRigidAndSoft(object2, object1->nodes[k], true);
			}
			break;
		}

		// Object 2 is a circle
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			Object::Physics::Rigid::RigidCircle* object2 = static_cast<Object::Physics::Rigid::RigidCircle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (collisionDetectionCircleNode(*object2, object1->nodes[k]))
					resolveRigidAndSoft(object2, object1->nodes[k], false);
			}
			break;
		}

		// Object 2 is a polygon
		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			Object::Physics::Rigid::RigidPolygon* object2 = static_cast<Object::Physics::Rigid::RigidPolygon*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (trinaryCollisionDetection(object2, object1->nodes[k], object2->number_of_sides, false))
					resolveRigidAndSoft(object2, object1->nodes[k], true);
			}
			break;
		}

		// Object 2 is a Soft Body
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS: { [[fallthrough]]; }
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			// IDEA: Do Not Have Collisions Between Tow Different Wires
			Object::Physics::Soft::SoftBody* object2 = static_cast<Object::Physics::Soft::SoftBody*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				for (int h = 0; h < object2->node_count; h++)
				{
					if (collisionDetectionNode(object1->nodes[k], object2->nodes[h]))
						resolveSoft(object1->nodes[k], object2->nodes[h]);
				}
			}
			break;
		}
		}
		break;
	}
	}
}

void Source::Collisions::Physics::objectCollisionDetectionInnerHinge(Object::Physics::PhysicsBase* pointer_to_object1, Object::Physics::PhysicsBase* pointer_to_hinge)
{
	// Statically Cast Object
	Object::Physics::Hinge::Hinge* hinge = static_cast<Object::Physics::Hinge::Hinge*>(pointer_to_hinge);

	// Iterate Through Each Object
	for (int k = 0; k < hinge->number_of_objects; k++)
		objectCollisionDetectionInner(pointer_to_object1, hinge->objects[k]);

	// Iterate Through Each Hinge
	for (int k = 0; k < hinge->number_of_children; k++)
		objectCollisionDetectionInnerHinge(pointer_to_object1, hinge->children[k]);
}

void Source::Collisions::Physics::resolveRigid(Object::Physics::Rigid::RigidBody* object1, Object::Physics::Rigid::RigidBody* object2, bool enable_first_rotation_vertex, bool enable_second_rotation_vertex)
{
	// IDEA: temp_collision_slope2 is calculated based on the angle of the Slopes of an Object in Collision
	// This is Done by Using Calculating the Change in Angle Between the Two Slopes Resulting From the Two Vertices Used to Test Collisions In Each Object

	// Separate Objects
	//glm::vec2 collision_offset = ((temp_collision_loc1 - temp_collision_loc2) * glm::vec2(abs(sin(temp_collision_slope2)), abs(cos(temp_collision_slope2))));
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	object1->physics.Position.x -= collision_offset.x / 2.0f;
	object2->physics.Position.x += collision_offset.x / 2.0f;

	if (object1->physics.grounded && !object2->physics.grounded)
	{
		object2->physics.Position.y += collision_offset.y;
		object2->physics.grounded = true;
		if (enable_second_rotation_vertex || true)
			object2->physics.Rotation_Vertex = temp_collision_loc2 - object2->physics.Position;
	}

	else if (object2->physics.grounded && !object1->physics.grounded)
	{
		object1->physics.Position.y -= collision_offset.y;
		object1->physics.grounded = true;
		if (enable_first_rotation_vertex || true)
			object1->physics.Rotation_Vertex = temp_collision_loc1 - object1->physics.Position;
	}

	else
	{
		object1->physics.Position.y -= collision_offset.y / 2.0f;
		object2->physics.Position.y += collision_offset.y / 2.0f;
	}

	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(object1->physics.Velocity, glm::vec2(0, 0)) * object1->physics.Mass;
	float work_2 = glm::distance(object2->physics.Velocity, glm::vec2(0, 0)) * object2->physics.Mass;

	// Collision is Inelastic
	if ((work_1 > work_2 * Constant::INELASTIC || work_2 > work_1 * Constant::INELASTIC))
	{
		glm::vec2 new_velocity = (object1->physics.Mass * object1->physics.Velocity + object2->physics.Mass * object1->physics.Velocity) / (object1->physics.Mass + object2->physics.Mass);
		object1->physics.Velocity = new_velocity;
		object2->physics.Velocity = new_velocity;
	}

	// Collision is elastic
	else
	{
		float mass_sum = object1->physics.Mass + object2->physics.Mass;
		glm::vec2 new_velocity_1 = ((object1->physics.Mass - object2->physics.Mass) / mass_sum) * object1->physics.Velocity + ((2 * object2->physics.Mass) / mass_sum) * object2->physics.Velocity;
		glm::vec2 new_velocity_2 = ((object2->physics.Mass - object1->physics.Mass) / mass_sum) * object2->physics.Velocity + ((2 * object1->physics.Mass) / mass_sum) * object1->physics.Velocity;
		object1->physics.Velocity = new_velocity_1;
		object2->physics.Velocity = new_velocity_2;
	}
}

void Source::Collisions::Physics::resolveSoft(Object::Physics::Soft::Node& node1, Object::Physics::Soft::Node& node2)
{
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	node1.Position -= collision_offset / 2.0f;
	node2.Position += collision_offset / 2.0f;

	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(node1.Velocity, glm::vec2(0, 0)) * node1.Mass;
	float work_2 = glm::distance(node2.Velocity, glm::vec2(0, 0)) * node2.Mass;

	// Collision is Inelastic
	if (work_1 > work_2 * Constant::INELASTIC || work_2 > work_1 * Constant::INELASTIC)
	{
		glm::vec2 new_velocity = (node1.Mass * node1.Velocity + node2.Mass * node1.Velocity) / (node1.Mass + node2.Mass);
		node1.Velocity = new_velocity;
		node2.Velocity = new_velocity;
	}

	// Collision is elastic
	else
	{
		float mass_sum = node1.Mass + node2.Mass;
		glm::vec2 new_velocity_1 = ((node1.Mass - node2.Mass) / mass_sum) * node1.Velocity + ((2 * node2.Mass) / mass_sum) * node2.Velocity;
		glm::vec2 new_velocity_2 = ((node2.Mass - node1.Mass) / mass_sum) * node2.Velocity + ((2 * node1.Mass) / mass_sum) * node1.Velocity;
		node1.Velocity = new_velocity_1;
		node2.Velocity = new_velocity_2;
	}
}

void Source::Collisions::Physics::resolveRigidAndSoft(Object::Physics::Rigid::RigidBody* object, Object::Physics::Soft::Node& node, bool enable_object_rotate)
{
	// Separate Objects
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	object->physics.Position.x -= collision_offset.x;
	if (object->physics.grounded)
	{
		node.Position.y += collision_offset.y;
	}

	else
	{
		object->physics.Position.y -= collision_offset.y;
		object->physics.grounded = true;
		if (enable_object_rotate)
			object->physics.Rotation_Vertex = temp_collision_loc1 - object->physics.Position;
	}

	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(object->physics.Velocity, glm::vec2(0, 0)) * object->physics.Mass;
	float work_2 = glm::distance(node.Velocity, glm::vec2(0, 0)) * node.Mass;

	// Collision is Inelastic
	if (work_1 > work_2 * Constant::INELASTIC || work_2 > work_1 * Constant::INELASTIC)
	{
		glm::vec2 new_velocity = (object->physics.Mass * object->physics.Velocity + node.Mass * object->physics.Velocity) / (object->physics.Mass + node.Mass);
		object->physics.Velocity = new_velocity;
		node.Velocity = new_velocity;
	}

	// Collision is elastic
	else
	{
		float mass_sum = object->physics.Mass + node.Mass;
		glm::vec2 new_velocity_1 = ((object->physics.Mass - node.Mass) / mass_sum) * object->physics.Velocity + ((2 * node.Mass) / mass_sum) * node.Velocity;
		glm::vec2 new_velocity_2 = ((node.Mass - object->physics.Mass) / mass_sum) * node.Velocity + ((2 * object->physics.Mass) / mass_sum) * object->physics.Velocity;
		object->physics.Velocity = new_velocity_1;
		node.Velocity = new_velocity_2;
	}
}

void Source::Collisions::Physics::physicsEntityCollisionRigidResolution(Object::Physics::Rigid::RigidBody* object, Object::Entity::EntityBase& entity, bool enable_rotation_vertices)
{
	//return;

	if (temp_possible_vertex2)
	{
		object->possible_rotation_vertices.push_back(temp_collision_loc1 - object->physics.Position);
		temp_possible_vertex2 = false;
	}

	glm::vec2 collision_offset = (temp_collision_loc2 - temp_collision_loc1) * glm::vec2(abs(sin(temp_collision_slope2)), abs(cos(temp_collision_slope2)));
	glm::vec2 pivot = object->physics.Rotation_Vertex;

	// Disable Sprint (Optional)
	//player.run = false;
	//Keys[GLFW_KEY_LEFT_CONTROL] = false;


	// If player is standing, only shift object position
	if (entity.state.grounded)
	{
		object->physics.Position.y += collision_offset.y;
		if (!object->physics.grounded)
		{
			if (enable_rotation_vertices)
			{
				object->physics.Rotation_Vertex = temp_collision_loc2 - object->physics.Position;
			}
			object->physics.grounded = true;
		}
	}

	// Normally, Player is Moved Back
	else
	{
		entity.state.position.y -= collision_offset.y;

		pivot = object->physics.Center_of_Mass;

		// If Surface is Nearly Flat, Allow Player to Jump
		if (abs(temp_collision_slope2) < 0.2f)
			entity.state.grounded = true;

		// If Surface is Nearly Vertical, Allow Player to Walljump
		if (abs(temp_collision_slope2) > 0.9f)
			entity.state.walled = true;
	}

	// Player X is Always Moved
	//collision_offset.x = glm::distance(temp_collision_loc1, temp_collision_loc2) * Sign(collision_offset.x);
	entity.state.position.x -= collision_offset.x;

	// If Player is attempting to use force, apply to object
	//temp_collision_angle1 = 0;
	//temp_effectivness1 = 5.0f;
	object->physics.Forces += glm::vec2(cos(temp_collision_angle1), sin(temp_collision_angle1)) * entity.stats.Force * temp_effectivness1;

	// Calculate Direction of Force for Player
	float player_angle = -1.5708f; // Direction of Force is up if not moving
	float player_velocity = glm::distance(entity.state.velocity, glm::vec2(0.0f));
	if (player_velocity)
	{
		glm::vec2 normalized_player_velocity = glm::normalize(entity.state.velocity);
		player_angle = atan(normalized_player_velocity.y / normalized_player_velocity.x);
		if (normalized_player_velocity.x < 0)
			player_angle += 3.14159f;
	}

	// Calculate the Torque for the Object
	object->physics.torque -= entity.stats.Force * glm::distance(temp_collision_loc2, object->physics.Position + pivot) * sin(player_angle - temp_collision_angle2) * Constant::TORQUE_CONSTANT * temp_effectivness1;
}

void Source::Collisions::Physics::physicsEntityCollisionSoftResolution(Object::Physics::Soft::Node& node, Object::Entity::EntityBase& entity)
{
	// Resolve Collision
	glm::vec2 collision_offset = temp_collision_loc2 - temp_collision_loc1;
	collision_offset *= 1.0f;
	entity.state.position.x -= collision_offset.x;
	if (entity.state.grounded)
	{
		node.Position.y += collision_offset.y * 2.0f;
	}

	else
	{
		entity.state.position.y -= collision_offset.y;

		// Test if Player Can Jump
		if (abs(temp_collision_angle2) < 0.2f)
			entity.state.grounded = true;

		// Test if Player Can Walljump
		if (abs(temp_collision_angle2) > 0.8f)
			entity.state.walled = true;
	}

	// Apply Forces
	node.Forces += glm::vec2(cos(temp_collision_angle1), sin(temp_collision_angle1)) * entity.stats.Force * 5.0f;
}

void Source::Collisions::Physics::physicsEntityHingeCollisionHelper(Object::Physics::Hinge::Hinge& hinge, Struct::List<Object::Entity::EntityBase>& entity_list)
{
	// Iterate Through Object List
	//physicsEntityCollisionDetection(hinge.objects, entity_list);

	// Remember to Add Similar List in Hinge

	// Iterate Through Children
	for (int i = 0; i < hinge.number_of_children; i++)
		physicsEntityHingeCollisionHelper(*hinge.children[i], entity_list);
}

void Source::Collisions::Physics::physicsCollisionDetection(Struct::List<Object::Physics::PhysicsBase>& physics_list)
{
	// Iterate Through Each Permutation Between Two Objects
	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it = physics_list.begin(); it != physics_list.end(); it++)
	{
		// Test if Object1 is a Hinge
		if ((*it).type == Object::Physics::PHYSICS_TYPES::TYPE_HINGE)
			objectCollisionDetectionOuterHinge(&it, it, physics_list);

		// Object 1 is a Normal Object
		else
			objectCollisionDetectionOuter(&it, it, physics_list);
	}
}

void Source::Collisions::Physics::physicsEntityCollisionDetection(Struct::List<Object::Physics::PhysicsBase>& physics_list, Struct::List<Object::Entity::EntityBase>& entity_list)
{
	// Iterate Through Each Physics Object
	for (physics_list.it = physics_list.begin(); physics_list.it != physics_list.end(); physics_list.it++)
	{
		switch ((*physics_list.it).type)
		{

		// Rigid Rectangle
		case Object::Physics::PHYSICS_TYPES::TYPE_RECTANGLE:
		{
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				if (physicsEntityRigidCollisionDetection(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, 4))
				{
					physicsEntityCollisionRigidResolution(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, true);
				}
			}

			break;
		}

		// Rigid Trapezoid
		case Object::Physics::PHYSICS_TYPES::TYPE_TRAPEZOID:
		{
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				if (physicsEntityRigidCollisionDetection(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, 4))
				{
					physicsEntityCollisionRigidResolution(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, true);
				}
			}

			break;
		}

		// Rigid Triangle
		case Object::Physics::PHYSICS_TYPES::TYPE_TRIANGLE:
		{
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				if (physicsEntityRigidCollisionDetection(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, 3))
				{
					physicsEntityCollisionRigidResolution(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, true);
				}
			}

			break;
		}

		// Circle Rigid Bodies
		case Object::Physics::PHYSICS_TYPES::TYPE_CIRCLE:
		{
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				if (physicsEntityRigidCollisionDetectionCircle(*static_cast<Object::Physics::Rigid::RigidCircle*>(&physics_list.it), *entity_list.it))
				{
					physicsEntityCollisionRigidResolution(static_cast<Object::Physics::Rigid::RigidBody*>(&physics_list.it), *entity_list.it, false);
				}
			}

			break;
		}

		case Object::Physics::PHYSICS_TYPES::TYPE_POLYGON:
		{
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				Object::Physics::Rigid::RigidPolygon& temp = *static_cast<Object::Physics::Rigid::RigidPolygon*>(&physics_list.it);
				if (physicsEntityRigidCollisionDetection(&temp, *entity_list.it, temp.number_of_sides))
				{
					physicsEntityCollisionRigidResolution(&temp, *entity_list.it, true);
				}
			}

			break;
		}

		// Soft Bodies
		case Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS:
		case Object::Physics::PHYSICS_TYPES::TYPE_WIRE:
		{
			Object::Physics::Soft::SoftBody* temp = static_cast<Object::Physics::Soft::SoftBody*>(&physics_list.it);
			for (entity_list.it = entity_list.begin(); entity_list.it != entity_list.end(); entity_list.it++)
			{
				for (int k = 0; k < temp->node_count; k++)
				{
					if (physicsEntityRigidCollisionDetectionNode(temp->nodes[k], *entity_list.it))
					{
						physicsEntityCollisionSoftResolution(temp->nodes[k], *entity_list.it);
					}
				}
			}

			break;
		}

		// Hinge
		case Object::Physics::PHYSICS_TYPES::TYPE_HINGE:
		{
			physicsEntityHingeCollisionHelper(*static_cast<Object::Physics::Hinge::Hinge*>(&physics_list.it), entity_list);
			break;
		}

		}
	}
}
