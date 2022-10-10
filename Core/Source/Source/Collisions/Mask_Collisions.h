// Object Collisions With Collision Masks

// Floor Value Collision Detection
bool Floor_Value_Collisions(glm::vec2 value, float& returned_value, float& angle, int& material_index)
{
	for (int level = 0; level < 9; level++)
	{
		for (int i = 0; i < levels_list[level].floor_objects_size; i++)
		{
			if (levels_list[level].floor_objects[i].TestCollisions(value.x, value.y, 2.0f, false))
			{
				returned_value = levels_list[level].floor_objects[i].returnedValue;
				angle = levels_list[level].floor_objects[i].current_angle;
				material_index = levels_list[level].floor_objects[i].material.Index;
				return true;
			}
		}
	}

	return false;
}

// Floor Value Edge Collision Detection
bool Floor_Value_Edge_Collisions(glm::vec2 left_vertex, glm::vec2 right_vertex, float& returned_value, glm::vec2& returned_vertex, int& material_index)
{
	for (int level = 0; level < 9; level++)
	{
		for (int i = 0; i < levels_list[level].floor_objects_size; i++)
		{
			if (levels_list[level].floor_objects[i].Test_Collisions_Edge(left_vertex, right_vertex, 1.0f, returned_vertex))
			{
				returned_value = levels_list[level].floor_objects[i].returnedValue;
				material_index = levels_list[level].floor_objects[i].material.Index;
				return true;
			}
		}
	}

	return false;

}

// Wall Value Collisions
bool Wall_Value_Collisions(glm::vec2 value, float& returned_value, float& angle, int& material_index, bool right)
{
	for (int level = 0; level < 9; level++)
	{
		for (int i = 0; i < levels_list[level].wall_objects_size; i++)
		{
			if (!(right ^ levels_list[level].wall_objects[i].right) && levels_list[level].wall_objects[i].TestCollisions(value.x, value.y))
			{
				returned_value = levels_list[level].wall_objects[i].returnedValue;
				angle = levels_list[level].wall_objects[i].current_angle;
				material_index = levels_list[level].wall_objects[i].material.Index;
				return true;
			}
		}
	}

	return false;
}

// Floor Value Collision Detection
bool Reverse_Floor_Value_Collisions(glm::vec2 value, float& returned_value, float& angle, int& material_index)
{
	for (int level = 0; level < 9; level++)
	{
		for (int i = 0; i < levels_list[level].ceiling_objects_size; i++)
		{
			if (levels_list[level].ceiling_objects[i].TestCollisions(value.x, value.y))
			{
				returned_value = levels_list[level].ceiling_objects[i].returnedValue;
				angle = levels_list[level].ceiling_objects[i].current_angle;
				material_index = levels_list[level].ceiling_objects[i].material.Index;
				return true;
			}
		}
	}

	return false;
}

// Mask Collisions for Objects
template <class Type> void Mask_Collisions(Type& object)
{
	static bool first_vertex_collision, second_vertex_collision;
	static float first_returned_value, second_returned_value, first_angle, second_angle;
	static short first_vertex_index, second_vertex_index; 
	static int first_material, second_material;

	// Floor Object Collisions
	first_vertex_index = object.extreme_vertex_lower[0];
	second_vertex_index = object.extreme_vertex_lower[1];
	first_vertex_collision = Floor_Value_Collisions(object.vertices[first_vertex_index] + object.physics.Position, first_returned_value, first_angle, first_material);
	second_vertex_collision = Floor_Value_Collisions(object.vertices[second_vertex_index] + object.physics.Position, second_returned_value, second_angle, second_material);
	Mask_Helper_Horizontal(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);
	Mask_Helper_Floor_Edge_Collisions(object, first_vertex_collision, second_vertex_collision, first_vertex_index, second_vertex_index);

	// Left Wall Object Collisions
	first_vertex_index = object.extreme_vertex_left[0];
	second_vertex_index = object.extreme_vertex_left[1];
	first_vertex_collision = Wall_Value_Collisions(object.vertices[first_vertex_index] + object.physics.Position, first_returned_value, first_angle, first_material, false);
	second_vertex_collision = Wall_Value_Collisions(object.vertices[second_vertex_index] + object.physics.Position, second_returned_value, second_angle, second_material, false);
	Mask_Helper_Vertical(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);

	// Right Wall Object Collisions
	first_vertex_index = object.extreme_vertex_right[0];
	second_vertex_index = object.extreme_vertex_right[1];
	first_vertex_collision = Wall_Value_Collisions(object.vertices[first_vertex_index] + object.physics.Position, first_returned_value, first_angle, first_material, true);
	second_vertex_collision = Wall_Value_Collisions(object.vertices[second_vertex_index] + object.physics.Position, second_returned_value, second_angle, second_material, true);
	Mask_Helper_Vertical(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);

	// Reverse Floor Object Collisions
	first_vertex_index = object.extreme_vertex_upper[0];
	second_vertex_index = object.extreme_vertex_upper[1];
	first_vertex_collision = Reverse_Floor_Value_Collisions(object.vertices[first_vertex_index] + object.physics.Position, first_returned_value, first_angle, first_material);
	second_vertex_collision = Reverse_Floor_Value_Collisions(object.vertices[second_vertex_index] + object.physics.Position, second_returned_value, second_angle, second_material);
	Mask_Helper_Horizontal(object, first_vertex_collision, second_vertex_collision, first_returned_value, second_returned_value, first_angle, second_angle, first_vertex_index, second_vertex_index, first_material, second_material);
}

// Helper Function for Horizontal Collision Resolution
template <class Type> void Mask_Helper_Horizontal(Type& object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material)
{
	static glm::vec2 normal_force;

	if (first_vertex_collision || second_vertex_collision)
	{
		// Get Normal Force
		Mask_Helper_Normal_Force(object, normal_force, first_vertex_collision, second_vertex_collision, first_angle, second_angle);

		// Both Vertices Collided
		if (first_vertex_collision && second_vertex_collision)
			Mask_Helper_Horizantal_Both(object, first_vertex_index, second_vertex_index, first_returned_value, second_returned_value, first_material, second_material, normal_force);

		// Only First Vertex Collided
		else if (first_vertex_collision)
			Mask_Helper_Horizontal_Single(object, first_vertex_index, first_returned_value, first_material, normal_force);

		// Only Second Vertex Collided
		else
			Mask_Helper_Horizontal_Single(object, second_vertex_index, second_returned_value, second_material, normal_force);
	}
}

// Helper Function for Vertical Collision Resolution
template <class Type> void Mask_Helper_Vertical(Type& object, bool& first_vertex_collision, bool& second_vertex_collision, float& first_returned_value, float& second_returned_value, float& first_angle, float& second_angle, short& first_vertex_index, short& second_vertex_index, int& first_material, int& second_material)
{
	static glm::vec2 normal_force;

	if (first_vertex_collision || second_vertex_collision)
	{
		// Get Normal Force
		Mask_Helper_Normal_Force(object, normal_force, first_vertex_collision, second_vertex_collision, first_angle, second_angle);

		// Both Vertices Collided
		if (first_vertex_collision && second_vertex_collision)
			Mask_Helper_Vertical_Both(object, first_vertex_index, second_vertex_index, first_returned_value, second_returned_value, first_material, second_material, normal_force);

		// Only First Vertex Collided
		else if (first_vertex_collision)
			Mask_Helper_Vertical_Single(object, first_vertex_index, first_returned_value, first_material, normal_force);

		// Only Second Vertex Collided
		else
			Mask_Helper_Vertical_Single(object, second_vertex_index, second_returned_value, second_material, normal_force);
	}

}

// Helper Function to Calculate Normal Force
template <class Type> void Mask_Helper_Normal_Force(Type& object, glm::vec2& normal_force, bool& first_vertex_collision, bool& second_vertex_collision, float& first_vertex_angle, float& second_vertex_angle)
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
	normal_force = -glm::vec2(cos(object_angle), sin(object_angle)) * sin(1.5708f - surface_angle) * object.physics.Forces.y;
	if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
		object.physics.Forces += normal_force;
}

// Helper Function to Resolve Horizontal Collisions With Both Vertices
template <class Type> void Mask_Helper_Horizantal_Both(Type& object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Move box up the max value
	if (glm::distance(object.vertices[first_vertex_index].y + object.physics.Position.y, first_returned_value) > glm::distance(object.vertices[second_vertex_index].y + object.physics.Position.y, second_returned_value))
	{
		object.physics.Position.y = first_returned_value - object.vertices[first_vertex_index].y;
	}

	else
	{
		object.physics.Position.y = second_returned_value - object.vertices[second_vertex_index].y;
	}

	// Insert Results as Possible Rotation Vertex
	object.possible_rotation_vertices.push_back(object.vertices[first_vertex_index]);
	object.possible_rotation_vertices.push_back(object.vertices[second_vertex_index]);
	
	// Apply Physics Changes
	object.physics.Velocity.y = 0;
	object.physics.rotation_velocity /= (glm::distance(object.vertices[first_vertex_index], object.vertices[second_vertex_index]) + 1);
	object.physics.torque = 0;
	object.physics.Rotation_Vertex = object.physics.Center_of_Mass;
	object.physics.grounded = true;

	// Get Friction Coefficient
	friction_coefficient = (get_friction_coefficient(object.material.Index, first_object_material, true) + get_friction_coefficient(object.material.Index, second_object_material, true)) / 2;

	// Apply Friction
	Mask_Helper_Horizontal_Friction(object, normal_force, friction_coefficient);
}

// Helper Function to Resolve Horizontal Collisions With a Single Vertex
template <class Type> void Mask_Helper_Horizontal_Single(Type& object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Insert Results as Possible Rotation Vertex
	object.possible_rotation_vertices.push_back(object.vertices[vertex_index]);

	// Shift box up
	object.physics.Position.y = returned_value - object.vertices[vertex_index].y;
	object.physics.Velocity.y = 0;
	object.physics.grounded = true;

	// Set Rotation Vertex to Vertex
	object.physics.Rotation_Vertex = object.vertices[vertex_index];

	// Get Friction Coefficient
	friction_coefficient = get_friction_coefficient(object.material.Index, object_material, true);

	// Apply Friction
	Mask_Helper_Horizontal_Friction(object, normal_force, friction_coefficient);
}

// Helper Function to Apply Horizontal Friction
template <class Type> void Mask_Helper_Horizontal_Friction(Type& object, glm::vec2& normal_force, float& friction_coefficient)
{
	static float max_friction, velocity_angle;

	if (object.physics.Velocity.x)
	{
		max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y);
		velocity_angle = atan(object.physics.Velocity.y / object.physics.Velocity.x);
		if (object.physics.Velocity.x < 0) { velocity_angle += 3.14159f; }
			object.physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
	}
}

// Helper Function to Resolve Vertical Collisions With Both Vertices
template <class Type> void Mask_Helper_Vertical_Both(Type& object, short& first_vertex_index, short& second_vertex_index, float& first_returned_value, float& second_returned_value, int& first_object_material, int& second_object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Move box up the max value
	if (glm::distance(object.vertices[first_vertex_index].x + object.physics.Position.x, first_returned_value) > glm::distance(object.vertices[second_vertex_index].x + object.physics.Position.x, second_returned_value))
	{
		object.physics.Position.x = first_returned_value - object.vertices[first_vertex_index].x;
	}

	else
	{
		object.physics.Position.x = second_returned_value - object.vertices[second_vertex_index].x;
	}

	// Insert Results as Possible Rotation Vertex
	object.possible_rotation_vertices.push_back(object.vertices[first_vertex_index]);
	object.possible_rotation_vertices.push_back(object.vertices[second_vertex_index]);
	
	// Apply Physics Changes
	object.physics.Velocity.x = 0;
	object.physics.rotation_velocity /= (glm::distance(object.vertices[first_vertex_index], object.vertices[second_vertex_index]) + 1);
	object.physics.torque = 0;
	object.physics.Rotation_Vertex = object.physics.Center_of_Mass;

	// Get Friction Coefficient
	friction_coefficient = (get_friction_coefficient(object.material.Index, first_object_material, true) + get_friction_coefficient(object.material.Index, second_object_material, true)) / 2;

	// Apply Friction
	Mask_Helper_Horizontal_Friction(object, normal_force, friction_coefficient);
}

// Helper Function to Resolve Vertical Collisions With a Single Vertex
template <class Type> void Mask_Helper_Vertical_Single(Type& object, short& vertex_index, float& returned_value, int& object_material, glm::vec2& normal_force)
{
	static float friction_coefficient;

	// Insert Results as Possible Rotation Vertex
	object.possible_rotation_vertices.push_back(object.vertices[vertex_index]);

	// Shift box up
	object.physics.Position.x = returned_value - object.vertices[vertex_index].x;
	object.physics.Velocity.x = 0;

	// Set Rotation Vertex to Vertex
	object.physics.Rotation_Vertex = object.vertices[vertex_index];

	// Get Friction Coefficient
	friction_coefficient = get_friction_coefficient(object.material.Index, object_material, true);

	// Apply Friction
	Mask_Helper_Horizontal_Friction(object, normal_force, friction_coefficient);
}

// Helper Function to Apply Vertical Friction
template <class Type> void Mask_Helper_Vertical_Friction(Type& object, glm::vec2& normal_force, float& friction_coefficient)
{
	static float max_friction, velocity_angle;

	if (object.physics.Velocity.y)
	{
		max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y);
		velocity_angle = atan(object.physics.Velocity.y / object.physics.Velocity.x);
		if (object.physics.Velocity.y < 0) { velocity_angle += 3.14159f; }
			object.physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
	}
}

// Helper Function to Resolve Floor Edge Collisions
template <class Type> void Mask_Helper_Floor_Edge_Collisions(Type& object, bool& first_vertex_collision, bool& second_vertex_collision, short& first_vertex_index, short& second_vertex_index)
{
	static float returned_edge_value, friction_coefficient;
	static int returned_edge_material;
	static glm::vec2 returned_edge_vertex, edge_normal_force;

	// Only Test Edge of Masks Only if Both Vertices Don't Touch A Mask
	if (!(first_vertex_collision && second_vertex_collision))
	{
		// Test if Object Intersects Edge of Collision Mask
		if (Floor_Value_Edge_Collisions(object.vertices[first_vertex_index] + object.physics.Position, object.vertices[second_vertex_index] + object.physics.Position, returned_edge_value, returned_edge_vertex, returned_edge_material))
		{
			// Localize Vertex for Object
			returned_edge_vertex -= object.physics.Position;

			// Calculate Normal Force
			edge_normal_force = -glm::vec2(cos(1.5708f), sin(1.5708f)) * sin(1.5708f) * object.physics.Forces.y;
			if (abs(edge_normal_force.x) < 0.05f) { edge_normal_force.x = 0; }
				object.physics.Forces += edge_normal_force;
			
			// Insert Results as Possible Rotation Vertex
			object.possible_rotation_vertices.push_back(returned_edge_vertex);

			// Shift box up
			object.physics.Position.y = returned_edge_value - returned_edge_vertex.y;
			object.physics.Velocity.y = 0;
			object.physics.grounded = true;

			// Set Rotation Vertex to Vertex
			object.physics.Rotation_Vertex = returned_edge_vertex;

			// Get Friction Coefficient
			friction_coefficient = get_friction_coefficient(object.material.Index, returned_edge_material, true);

			// Apply Friction
			Mask_Helper_Horizontal_Friction(object, edge_normal_force, friction_coefficient);
		}
	}
}

// Circle - Mask Collisions
void Circle_Mask_Collisions(Object_Circle& object)
{
	// Test CollisionMaskFloor
	float returned_value, angle;
	int material_index;
	if (Floor_Value_Collisions(object.physics.Position - glm::vec2(0.0f, object.Radius), returned_value, angle, material_index))
	{
		// Apply Normal Force
		float temp_angle = 3.14159 - angle;
		temp_angle = 1.5708f + angle;
		glm::vec2 normal_force = -glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * object.physics.Forces.y;
		//normal_force = glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * physics.Mass * GRAVITATIONAL_ACCELERATION;
		if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
			object.physics.Forces += normal_force;

		// Apply Friction
		if (object.physics.Velocity.x)
		{
			//std::cout << "lol\n";

			float friction_coefficient = get_friction_coefficient(object.material.Index, material_index, true);
			float max_friction = friction_coefficient * glm::distance(normal_force.x, normal_force.y) * .2f;
			float velocity_angle = atan(object.physics.Velocity.y / object.physics.Velocity.x);
			if (object.physics.Velocity.x < 0) { velocity_angle += 3.14159f; }
				object.physics.Forces -= glm::vec2(cos(velocity_angle), sin(velocity_angle)) * max_friction;
		}

		// Perform Collision Resolution
		object.physics.Position.y = returned_value + object.Radius;
		object.physics.Velocity.y = 0;
		object.physics.grounded = true;
	}
}

// Node - Mask Collisions
//void Node_Mask_Collisions(Object_Soft_Body::Node& node, Material material)
template <class Node> void Node_Mask_Collisions(Node& node, Material material)
{		
	// Test CollisionMaskFloor
	float returned_value, angle;
	int material_index;

	if (Floor_Value_Collisions(node.Position - glm::vec2(0.0f, node.Radius), returned_value, angle, material_index))
	{
		// Apply Normal Force
		float temp_angle = 3.14159 - angle;
		temp_angle = 1.5708f + angle;
		glm::vec2 normal_force = -glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * node.Forces.y;
		//normal_force = glm::vec2(cos(temp_angle), sin(temp_angle)) * sin(1.5708f - angle) * physics.Mass * GRAVITATIONAL_ACCELERATION;
		if (abs(normal_force.x) < 0.05f) { normal_force.x = 0; }
			node.Forces += normal_force;

		// Apply Friction
		if (node.Velocity.x)
		{
			//std::cout << "lol\n";

			float friction_coefficient = get_friction_coefficient(material.Index, material_index, true);
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

// Retrieves the correct friction coefficient
float get_friction_coefficient(int index1, int index2, bool kinetic)
{
	int min_friction, max_friction = 0, friction_index = 0;
	if (index1 > index2) { min_friction = index2; max_friction = index1; }
	else {min_friction = index1; max_friction = index2; }
	friction_index = MATERIALS_COUNT * min_friction + (max_friction - min_friction);
	if (kinetic) { return kinetic_friction[friction_index]; }
	return static_friction[friction_index];
}

