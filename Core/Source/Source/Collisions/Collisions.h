// Contains All Collision Detection Algorithms

// Player Rectangle Collision
bool operator==(const Object_Rectangle& object, const Player player)
{
	return Player_Collision_Detection(object, player, 4);
}

// Player Trapezoid Collision
bool operator==(const Object_Trapezoid& object, const Player& player)
{
	return Player_Collision_Detection(object, player, 4);
}

// Player Triangle Collision
bool operator==(const Object_Triangle& object, const Player& player)
{
	return Player_Collision_Detection(object, player, 3);
}

// Player Circle Collision
bool operator==(const Object_Circle& object, const Player& player)
{
	float delta_w = -player.position.x + object.physics.Position.x;
	float delta_h = -player.position.y + object.physics.Position.y;
	float r = glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if ((player.posYmodifier + object.longest_edge) < r)
		return false;

	// Calculate Angle between objects
	float theta = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta += 3.14159;

	// Calculate Edge of Player
	glm::vec2 edge1 = glm::vec2(0.0f);
	edge1.x = player.position.x + player.posXmodifier * cos(theta);
	edge1.y = player.position.y + player.posYmodifier * sin(theta);

	// Calculate Edge of Circle
	glm::vec2 edge2(object.physics.Position.x - object.Radius * cos(theta), object.physics.Position.y - object.Radius * sin(theta));

	//Visualize_Point(edge1, 1.3f, glm::vec3(1.0f, 0.0f, 0.0f));
	//Visualize_Point(edge2, 1.3f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Determine if Collision Occoured
	if (((player.position.x <= edge2.x && edge2.x <= edge1.x && object.physics.Position.x >= edge1.x) || (object.physics.Position.x <= edge1.x && edge1.x <= edge2.x && player.position.x >= edge2.x))
		&& ((player.position.y <= edge2.y && edge2.y <= edge1.y && object.physics.Position.y >= edge1.y) || (object.physics.Position.y <= edge1.y && edge1.y <= edge2.y && player.position.y >= edge2.y)))
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

// Player Polygon Collision
bool operator==(const Object_Polygon& object, const Player& player)
{
	return Player_Collision_Detection(object, player, object.number_of_sides);
}

// Player Node Collision
bool operator==(const Object_Soft_Body::Node& node, const Player& player)
{
	float delta_w = -player.position.x + node.Position.x;
	float delta_h = -player.position.y + node.Position.y;
	float r = glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if ((player.posYmodifier + node.Radius) < r)
		return false;

	// Calculate Angle between objects
	float theta = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta += 3.14159;

	// Calculate Edge of Player
	glm::vec2 edge1 = glm::vec2(0.0f);
	edge1.x = player.position.x + player.posXmodifier * cos(theta);
	edge1.y = player.position.y + player.posYmodifier * sin(theta);

	// Calculate Edge of Circle
	glm::vec2 edge2(node.Position.x - node.Radius * cos(theta), node.Position.y - node.Radius * sin(theta));

	// Determine if Collision Occoured
	if (((node.Position.x <= edge1.x && edge1.x <= edge2.x) || (node.Position.x >= edge1.x && edge1.x >= edge2.x)) &&
		((node.Position.y <= edge1.y && edge1.y <= edge2.y) || (node.Position.y >= edge1.y && edge1.y >= edge2.y)))
	{
		// Get Location of Collision
		temp_collision_loc2 = edge1;
		temp_collision_loc1 = edge2;
		temp_collision_angle2 = theta;
		temp_collision_angle1 = theta;
		return true;
	}

	return false;
}

// Default Collision Detection Algorithm
template <class Type1, class Type2> bool Primary_Collision_Detection(Type1& object1, Type2& object2, short object1_vertex_count, short object2_vertex_count)
{
	// Initial Object Angles
	static float theta_object1, theta_object2;

	// Object Positions
	static glm::vec2 object_position1, object_position2;

	// Perform Circle Collision Detection
	if (Initial_Circle_Collision_Detection(object1, object2, theta_object1, theta_object2))
		return false;

	// Collision Detection Variables
	Collision_Loc collision_loc;

	// Object Positions
	object_position1 = object1.physics.Position;
	object_position2 = object2.physics.Position;

	// Collision Detection Using Vertices of Object1
	Ray_Method_Setup(object1, object2, collision_loc, object_position1, object_position2, theta_object1, theta_object2, object1_vertex_count, object2_vertex_count, false);

	// Collision Deterction Using Vertices of Object2
	Ray_Method_Setup(object2, object1, collision_loc, object_position2, object_position1, theta_object2, theta_object1, object2_vertex_count, object1_vertex_count, true);

	// Collision Resolution
	return Finalize_Collision_Detection(collision_loc);
}

// Collision Detection Algorithm With a Circle
template <class Type> bool Secondary_Collision_Detection(Type& object1, const Object_Circle& object2, short object1_vertex_count, bool inverse)
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
	if (Initial_Circle_Collision_Detection(object1, object2, theta_object1, theta_object2))
		return false;

	// Determine Quadrant and Vertices of Object
	quadrant = Closest_Without_Going_Over(theta_object1, object1.angle_offsets, object1_vertex_count);
	vertex_left = object1.vertices[object1.angle_to_vertex_left[quadrant]];
	vertex_right = object1.vertices[object1.angle_to_vertex_right[quadrant]];
	
	// Parameterize Vertices
	float mx = vertex_right.x - vertex_left.x;
	float bx = vertex_left.x;
	float my = vertex_right.y - vertex_left.y;
	float by = vertex_left.y;

	// Find Angle Perpendicular to Edge
	float theta = atan(my / mx) + 1.5708f * Sign(mx);
	float sine = sin(theta);
	if (sine == 0)
		sine = 0.001f;
	float cosine = cos(theta);
	if (cosine == 0)
		cosine = 0.001f;

	// Determine if Position of Circle Exists Between Edge
	float t = (bx / cosine + (object2.physics.Position.y - object1.physics.Position.y) / sine - by / sine - (object2.physics.Position.x - object1.physics.Position.x) / cosine) / (my / sine - mx / cosine);
	
	// Circle Exists Before Left Vertex
	if (t < 0)
	{
		if (glm::distance(object1.physics.Position + vertex_left, object2.physics.Position) < object2.Radius)
		{
			// Get Angle Between Circle and Vertex
			glm::vec2 delta_pos = -object2.physics.Position + (object1.physics.Position + vertex_left);
			theta = atan(delta_pos.y / delta_pos.x);
			if (delta_pos.x < 0)
				theta += 3.14159f;

			// Get Collision Locs
			temp_collision_loc2 = object2.physics.Position + object2.Radius * Parameterize_Angle(theta);
			temp_collision_loc1 = object1.physics.Position + vertex_left;

			return true;
		}
	}

	// Circle Exists After Right Vertex
	else if (t > 1)
	{
		if (glm::distance(object1.physics.Position + vertex_right, object2.physics.Position) < object2.Radius)
		{
			// Get Angle Between Circle and Vertex
			glm::vec2 delta_pos = -object2.physics.Position + (object1.physics.Position + vertex_right);
			theta = atan(delta_pos.y / delta_pos.x);
			if (delta_pos.x < 0)
				theta += 3.14159f;

			// Get Collision Locs
			temp_collision_loc1 = object2.physics.Position + object2.Radius * Parameterize_Angle(theta);
			temp_collision_loc2 = object1.physics.Position + vertex_right;

			return true;
		}
	}

	// Circle is Between Edge
	else
	{
		// Get Vertex Positions
		glm::vec2 vertex_pos = glm::vec2(mx * t + bx, my * t + by) + object1.physics.Position;
		glm::vec2 edge = object2.physics.Position + object2.Radius * Parameterize_Angle(theta);
		object_position = object2.physics.Position;

		//Visualize_Point(edge, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//Visualize_Point(vertex_pos, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

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

// Collision Detection Algorithm With a Node
template <class Type, class Node> bool Trinary_Collision_Detection(Type& object, Node& node, short object_vertex_count, bool inverse)
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
	if (Initial_Circle_Collision_Detection_Node(object, node, theta_object, theta_node))
		return false;

	// Determine Quadrant and Vertices of Object
	quadrant = Closest_Without_Going_Over(theta_object, object.angle_offsets, object_vertex_count);
	vertex_left = object.vertices[object.angle_to_vertex_left[quadrant]];
	vertex_right = object.vertices[object.angle_to_vertex_right[quadrant]];
	
	// Parameterize Vertices
	float mx = vertex_right.x - vertex_left.x;
	float bx = vertex_left.x;
	float my = vertex_right.y - vertex_left.y;
	float by = vertex_left.y;

	// Find Angle Perpendicular to Edge
	float theta = atan(my / mx) + 1.5708f * Sign(mx);
	float sine = sin(theta);
	if (sine == 0)
		sine = 0.001f;
	float cosine = cos(theta);
	if (cosine == 0)
		cosine = 0.001f;

	// Determine if Position of Circle Exists Between Edge
	float t = (bx / cosine + (node.Position.y - object.physics.Position.y) / sine - by / sine - (node.Position.x - object.physics.Position.x) / cosine) / (my / sine - mx / cosine);
	if (t > 1.0f || t < 0.0f)
		return false;

	// Get Vertex Positions
	glm::vec2 vertex_pos = glm::vec2(mx * t + bx, my * t + by) + object.physics.Position;
	glm::vec2 edge = node.Position + node.Radius * Parameterize_Angle(theta);
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

// Collision Between Circles
bool Circle_Collision_Detection(Object_Circle& object1, Object_Circle& object2)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = object2.physics.Position - object1.physics.Position;
	theta1 = angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = object1.physics.Position + object1.Radius * Parameterize_Angle(theta1);
	temp_collision_loc2 = object2.physics.Position + object2.Radius * Parameterize_Angle(theta2);

	Visualize_Point(object1.physics.Position + glm::vec2(0.0f, 3.0f), 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	Visualize_Point(temp_collision_loc1, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	Visualize_Point(temp_collision_loc2, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

	//Visualize_Line(object1.physics.Position, object1.physics.Position + 100.0f * Parameterize_Angle(theta1), 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(object1.physics.Position, object2.physics.Position);
	if (distance > object1.Radius + object2.Radius)
		return false;

	return true;
}

// Collision Between a Circle and a Node
template <class Node> bool Circle_Node_Collision_Detection(Object_Circle& object, Node& node)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = node.Position - object.physics.Position;
	theta1 = angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = object.physics.Position + object.Radius * Parameterize_Angle(theta1);
	temp_collision_loc2 = node.Position + node.Radius * Parameterize_Angle(theta2);

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(object.physics.Position, node.Position);
	if (distance > object.Radius + node.Radius)
		return false;

	return true;
}

// Collision Detection Algorithm Between Nodes
template <class Node1, class Node2> bool Node_Collision_Detection(Node1& node1, Node2& node2)
{
	static float distance, theta1, theta2;
	static glm::vec2 delta_pos;

	// Get Angles of Collisions
	delta_pos = node2.Position - node1.Position;
	theta1 = angle_from_vector(delta_pos);
	theta2 = theta1 + 3.14159f;
	if (theta2 > 6.28318f)
		theta2 -= 6.28318f;

	// Get Collision Locs
	temp_collision_loc1 = node1.Position + node1.Radius * Parameterize_Angle(theta1);
	temp_collision_loc2 = node2.Position + node2.Radius * Parameterize_Angle(theta2);

	// Test if Radii of Circles Intercept Eachother
	distance = glm::distance(node1.Position, node2.Position);
	if (distance > node1.Radius + node2.Radius)
		return false;

	return true;
}

// Function to Perform Initial Circle Collision Detection
template <class Type1, class Type2> bool Initial_Circle_Collision_Detection(Type1& object1, Type2& object2, float& theta_object1, float& theta_object2)
{
	static float delta_w, delta_h, r;

	// Perform Circle Collision by Testing if Distance is Less Than Longest Possible Edges of Objects Combined
	delta_w = object2.physics.Position.x - object1.physics.Position.x;
	delta_h = object2.physics.Position.y - object1.physics.Position.y;
	r = glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (object1.longest_edge + object2.longest_edge < r - 20.0f)
		return true;

	// Calculate Angle Between Objects if Circle Collision Passed
	theta_object1 = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_object1 += 3.14159;
	theta_object2 = theta_object1 + 3.14159f;
	if (theta_object2 > 6.28318f)
		theta_object2 -= 6.28318f;
	
	return false;
}

// Function to Perform Initial Circle Collision Detection With a Node
template <class Type, class Node> bool Initial_Circle_Collision_Detection_Node(Type& object, Node& node, float& theta_object, float& theta_node)
{
	static float delta_w, delta_h, r;

	// Perform Circle Collision by Testing if Distance is Less Than Longest Possible Edges of Objects Combined
	delta_w = node.Position.x - object.physics.Position.x;
	delta_h = node.Position.y - object.physics.Position.y;
	r = glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (object.longest_edge + node.Radius < r - 20.0f)
		return true;

	// Calculate Angle Between Objects if Circle Collision Passed
	theta_object = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_object += 3.14159;
	theta_node = theta_object + 3.14159f;
	if (theta_node > 6.28318f)
		theta_node -= 6.28318f;
	
	return false;
}

// Function to Set Up the Ray Method
template <class Type1, class Type2> void Ray_Method_Setup(Type1& object1, Type2& object2, Collision_Loc& collision_loc, glm::vec2& object1_position, glm::vec2& object2_position, float& object_theta1, float& object_theta2, short object1_vertex_count, short object2_vertex_count, bool enable_swap)
{
	static int angle_index, vertex_quadrant;
	static float delta_w, delta_h, theta1, theta2;
	static glm::vec2 pos_offset, vertex_left, vertex_right;
	glm::vec2 position_offsets[2];

	// Find and Store Verties
	angle_index = Closest_Without_Going_Over(object_theta1, object1.angle_offsets, object1_vertex_count);
	position_offsets[0] = object1.vertices[object1.angle_to_vertex_left[angle_index]];
	position_offsets[1] = object1.vertices[object1.angle_to_vertex_right[angle_index]];

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
		vertex_quadrant = Closest_Without_Going_Over(theta2, object2.angle_offsets, object2_vertex_count);
		vertex_left = object2.vertices[object2.angle_to_vertex_left[vertex_quadrant]];
		vertex_right = object2.vertices[object2.angle_to_vertex_right[vertex_quadrant]];

		// Perform Ray Method
		collision_loc.collision |= Ray_Method(collision_loc, object2_position, vertex_left, vertex_right, pos_offset, theta1);
		collision_loc.swap |= (collision_loc.greatest && enable_swap);
		collision_loc.greatest = false;
	}
}

// Function to Perform Ray Method
bool Ray_Method(Collision_Loc& collision_loc, glm::vec2 object_position, glm::vec2& object_vertex_left, glm::vec2& object_vertex_right, glm::vec2& pos_offset, float& theta)
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

// Final Step in Collision Detection / Setup for Collision Resolution
bool Finalize_Collision_Detection(Collision_Loc& collision_loc)
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

		return true;
	}

	return false;
}

// Player Collision Detection
template <class Type> bool Player_Collision_Detection(Type& object, const Player& player, short object_vertex_count)
{
	// Initial Object angles
	static float theta_object, theta_player;

	// Positions
	static glm::vec2 object_position, player_position;

	// Perform Basic Circle Collision
	if (Player_Circle_Collision_Detection(object, player, theta_object, theta_player))
		return false;

	// Collision Detection Variables
	Collision_Loc collision_loc;

	// Positions
	object_position = object.physics.Position;
	player_position = glm::vec2(player.position.x, player.position.y);

	// Collision Using Vertices of Object
	Ray_Method_Setup(object, player, collision_loc, object_position, player_position, theta_object, theta_player, object_vertex_count, 4, false);

	// Collision Using Vertices of Player
	Ray_Method_Setup(player, object, collision_loc, player_position, object_position, theta_player, theta_object, 4, object_vertex_count, true);

	// Collision Resolution
	return Player_Collision_Resolution(object, player, collision_loc, theta_player, theta_object, object_vertex_count);
}

// Basic Circle Collision Detection Between Object and Player
template <class Type> bool Player_Circle_Collision_Detection(Type& object, const Player& player, float& theta_object, float& theta_player)
{
	static float delta_w, delta_h, r;
	
	// Perform circle collision detection
	delta_w = -player.position.x + object.physics.Position.x;
	delta_h = -player.position.y + object.physics.Position.y;
	r = glm::sqrt(glm::pow(delta_w, 2) + glm::pow(delta_h, 2));
	if (player.posYmodifier + object.longest_edge < r - 20.0f)
		return true;

	// Calculate Angle between objects
	theta_player = atan(delta_h / delta_w);
	if (delta_w < 0)
		theta_player += 3.14159;
	theta_object = theta_player + 3.14159f;
	if (theta_object > 6.28318f)
		theta_object -= 6.28318f;

	return false;
}

// Collision Resolution Between Object and Player
template <class Type> bool Player_Collision_Resolution(Type& object, const Player& player, Collision_Loc& collision_loc, float& theta_player, float& theta_object, short& object_vertex_count)
{
	if (Finalize_Collision_Detection(collision_loc))
	{
		static int number_to_quadrant[4] = { 1, 2, 3, 0 };
		static int cross_section, quadrant, index_left, index_right;
		static float theta_edge, delta_h, delta_w, theta_velocity;

		// Determine Cross Section
		cross_section = number_to_quadrant[Closest_Without_Going_Over_Named_Index(theta_object, object.angle_offsets, object_vertex_count)];
		//cross_section = Closest_Without_Going_Over_Named_Index(theta_object, object.angle_offsets, object_vertex_count) + 1;
		//if (cross_section >= object_vertex_count)
		//	cross_section = 0;

		// Get Vertices of Object
		quadrant = Closest_Without_Going_Over(theta_object, object.angle_offsets, object_vertex_count);
		index_left = object.angle_to_vertex_left[quadrant];
		index_right = object.angle_to_vertex_right[quadrant];

		//Visualize_Point(object.physics.Position + object.vertices[index_left], 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//Visualize_Point(object.physics.Position + object.vertices[index_right], 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		// Calculate Angle of Collision
		delta_h = object.vertices[index_right].y - object.vertices[index_left].y;
		delta_w = object.vertices[index_right].x - object.vertices[index_left].x;
		theta_edge = angle_from_vector(delta_w, delta_h);
		temp_collision_angle1 = theta_edge + 1.5708f;
		glm::vec2 player_position = glm::vec2(player.position.x, player.position.y);

		// Calculate Angle of Collision
		if (object_vertex_count > 4)
		{
			temp_collision_angle2 = object.angle_between_offsets * index_left;
		}

		else
		{
			temp_collision_angle2 = object.angle_between_offsets * index_left + object.physics.rotation;
		}

		// Determine Effectiveness of Forces Applied by Player
		theta_velocity = angle_from_vector(player.velocity);
		temp_effectivness1 = abs(sin(theta_velocity - theta_edge)) * distance(temp_collision_loc1, temp_collision_loc2) * 1.5f;
		//temp_effectivness1 = 1;
		if (player.velocity.x == 0 && player.velocity.y == 0)
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
			if (player.position.x < object.physics.Position.x)
				temp_collision_angle2 += 3.14159f;
		}

		else
		{
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
			if (min1 == -1 || object.vertices[i].y < object.vertices[min1].y)
			{
				min2 = min1;
				min1 = i;
			}

			else if (min2 == -1 || object.vertices[i].y < object.vertices[min2].y)
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
