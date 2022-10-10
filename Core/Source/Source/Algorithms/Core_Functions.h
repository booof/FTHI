// Several Functions Important to Keep Game Working

// Update Level Location
void UpdateLevel(glm::vec2 position, glm::vec2& level)
{
	level.x = floor(position.x / 128);
	level.y = floor(position.y / 64);
}

// Controlls the Creation and Deletion of Game Objects
void levelHandler(glm::vec2 level_old, glm::vec2 level_new, bool save)
{
	// Test if Handler Should do a Complete Map Reset
	if ((abs(level_old.x - level_new.x) > 1 || abs(level_old.y - level_new.y) > 1) || (level_old == level_new))
	{
		// Deconstruct Level Objects
		for (int i = 0; i < 9; i++)
		{
			if (save) { levels_list[i].Save_Level(); }
			levels_list[i].~Level();
		}

		// Create New Objects
		int iterater = 0;
		for (int level_y = 1; level_y > -2; level_y--)
		{
			for (int level_x = -1; level_x < 2; level_x++)
			{
				Level* new_level;
				new_level = new Level(glm::vec2(level_new.x + level_x, level_new.y + level_y));
				levels_list[iterater] = new_level[0];
				iterater++;


			}
		}
	}

	// Load and Unload Selected Levels
	else
	{
		// Move to the Left
		if (level_old.x - level_new.x > 0)
		{
			// Deconstruct Old Levels
			for (int i = 2; i < 9; i += 3)
			{
				levels_list[i].Save_Level();
				levels_list[i].~Level();
			}

			// Shift Loaded Levels to the Right
			int next_level_location = -1;
			for (int i = 8; i > -1; i--)
			{
				// Load New Levels from the Left
				if (!(i % 3))
				{
					Level* new_level;
					new_level = new Level(glm::vec2(level_new.x - 1, level_new.y + next_level_location));
					levels_list[i] = new_level[0];
					next_level_location++;
				}

				// Shift Levels
				else
				{
					levels_list[i] = levels_list[i - 1];
				}
			}
		}

		// Move to the Right
		else if (level_old.x - level_new.x < 0)
		{
			// Deconstruct Old Levels
			for (int i = 0; i < 9; i += 3)
			{
				levels_list[i].Save_Level();
				levels_list[i].~Level();
			}

			// Shift Loaded Levels to the Left
			int next_level_location = 1;
			for (int i = 0; i < 9; i++)
			{
				// Load New Levels from the Right
				if ((i % 3) == 2)
				{
					Level* new_level;
					new_level = new Level(glm::vec2(level_new.x + 1, level_new.y + next_level_location));
					levels_list[i] = new_level[0];
					next_level_location--;
				}

				// Shift Levels
				else
				{
					levels_list[i] = levels_list[i + 1];
				}
			}
		}

		// Move to the North
		if (level_old.y - level_new.y < 0)
		{
			// Deconstruct Old Levels
			for (int i = 6; i < 9; i++)
			{
				levels_list[i].Save_Level();
				levels_list[i].~Level();
			}

			// Shift Loaded Levels to the South
			int next_level_location = 1;
			for (int i = 8; i > -1; i--)
			{
				// Load New Levels from the North
				if (i < 3)
				{
					Level* new_level;
					new_level = new Level(glm::vec2(level_new.x + next_level_location, level_new.y + 1));
					levels_list[i] = new_level[0];
					next_level_location--;
				}

				// Shift Levels
				else
				{
					levels_list[i] = levels_list[i - 3];
				}
			}
		}

		// Move to the South
		else if (level_old.y - level_new.y > 0)
		{
			// Deconstruct Old Levels
			for (int i = 0; i < 3; i++)
			{
				levels_list[i].Save_Level();
				levels_list[i].~Level();
			}

			// Shift Loaded Levels to the North
			int next_level_location = -1;
			for (int i = 0; i < 9; i++)
			{
				// Load New Levels from the South
				if (i > 5)
				{
					Level* new_level;
					new_level = new Level(glm::vec2(level_new.x + next_level_location, level_new.y - 1));
					levels_list[i] = new_level[0];
					next_level_location++;
				}

				// Shift Levels
				else
				{
					levels_list[i] = levels_list[i + 3];
				}
			}
		}
	}

	// Update Lights
	lightingHandler();

	// Load Physics
	delete[] physics;
	physics_count = 0;
	for (int i = 0; i < 9; i++)
		physics_count += levels_list[i].objects_size;
	physics = new Object_Base * [physics_count];
	int physics_index = 0;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < levels_list[i].objects_size; j++)
		{
			physics[physics_index] = levels_list[i].objects[j];
			physics_index++;
		}
	}
}

// Controlls the Loading and Unloading of Light Objects
void lightingHandler()
{
	// Size of Array in Shaders
	int lights_array_size = 0;

	// Data Offset for Buffer
	int data_offset = 16;

	// Size of Buffer Data
	int directional_data_size = 16;
	int point_data_size = 16;
	int spot_data_size = 16;

	// Calculate Size of Data
	for (int i = 0; i < 9; i++)
	{
		// Update Directional Size
		directional_data_size += 96 * levels_list[i].directional_objects_size;

		// Update Point Size
		point_data_size += 80 * levels_list[i].point_objects_size;

		// Update Spot Size
		spot_data_size += 96 * levels_list[i].spot_objects_size;
	}

	// Directional Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, DirectionalBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, directional_data_size, NULL, GL_DYNAMIC_DRAW);

	// Iterate Through Directional Lights in Currently Loaded Levels and Load them in Shaders
	for (int i = 0; i < 9; i++)
	{
		// Update Size of Lights Array
		lights_array_size += levels_list[i].directional_objects_size;

		// Iterate Through All Directional Lights and Send Data to Shaders
		for (int j = 0; j < levels_list[i].directional_objects_size; j++)
		{
			// Add Light and Line Direction Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset, 16, glm::value_ptr(levels_list[i].directional_objects[j].light_direction));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 16, 16, glm::value_ptr(levels_list[i].directional_objects[j].line_direction));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 32, 16, glm::value_ptr(levels_list[i].directional_objects[j].Ambient));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 48, 16, glm::value_ptr(levels_list[i].directional_objects[j].Diffuse));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 64, 16, glm::value_ptr(levels_list[i].directional_objects[j].Specular));

			// Add Endpoint Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 80, 8, glm::value_ptr(glm::vec2(levels_list[i].directional_objects[j].xPos, levels_list[i].directional_objects[j].yPos)));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 88, 8, glm::value_ptr(glm::vec2(levels_list[i].directional_objects[j].xPos2, levels_list[i].directional_objects[j].yPos2)));

			// Increase Data Offset
			data_offset += 96;
		}
	}

	// Update Buffer
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &lights_array_size);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, DirectionalBuffer);

	// Point Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, point_data_size, NULL, GL_DYNAMIC_DRAW);
	lights_array_size = 0;
	data_offset = 16;

	// Iterate Through Point Lights in Currently Loaded Levels and Load them in Shaders
	for (int i = 0; i < 9; i++)
	{
		// Update Size of Lights Array
		lights_array_size += levels_list[i].point_objects_size;

		// Iterate Through All Point Lights and Send Data to Shaders
		for (int j = 0; j < levels_list[i].point_objects_size; j++)
		{
			// Add Position Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset, 16, glm::value_ptr(glm::vec4(levels_list[i].point_objects[j].xPos, levels_list[i].point_objects[j].yPos, 0.0f, 0.0f)));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 16, 16, glm::value_ptr(levels_list[i].point_objects[j].Ambient));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 32, 16, glm::value_ptr(levels_list[i].point_objects[j].Diffuse));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 48, 16, glm::value_ptr(levels_list[i].point_objects[j].Specular));

			// Add Attenuation Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 64, 4, &levels_list[i].point_objects[j].Linear);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 68, 4, &levels_list[i].point_objects[j].Quadratic);

			// Increase Data Offset
			data_offset += 80;
		}
	}

	// Update Buffer
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 16, &lights_array_size);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, PointBuffer);

	// Spot Lights
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SpotBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, spot_data_size, NULL, GL_DYNAMIC_DRAW);
	lights_array_size = 0;
	data_offset = 16;

	// Iterate Through Spot Lights in Currently Loaded Levels and Load them in Shaders
	for (int i = 0; i < 9; i++)
	{
		// Update Size of Lights Array
		lights_array_size += levels_list[i].spot_objects_size;

		// Iterate Through All Spot Lights and Send Data to Shaders
		for (int j = 0; j < levels_list[i].spot_objects_size; j++)
		{
			// Add Position Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset, 16, glm::value_ptr(glm::vec4(levels_list[i].spot_objects[j].xPos, levels_list[i].spot_objects[j].yPos, 0.0f, 0.0f)));

			// Add Direction Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 16, 16, glm::value_ptr(levels_list[i].spot_objects[j].Direction));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 32, 16, glm::value_ptr(levels_list[i].spot_objects[j].Ambient));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 48, 16, glm::value_ptr(levels_list[i].spot_objects[j].Diffuse));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 64, 16, glm::value_ptr(levels_list[i].spot_objects[j].Specular));

			// Add Attenuation Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 80, 4, &levels_list[i].spot_objects[j].Linear);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 84, 4, &levels_list[i].spot_objects[j].Quadratic);

			// Add Angle Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 88, 4, &levels_list[i].spot_objects[j].InsideCutoff);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, data_offset + 92, 4, &levels_list[i].spot_objects[j].OutSideCutoff);

			// Increase Data Offset
			data_offset += 96;
		}
	}

	// Update Buffer
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4, &lights_array_size);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SpotBuffer);

	// Unbind Storage Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// Controlls the Storage Buffer for Particles
void particleHandler()
{
	// Bind Buffer and Store Data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticleBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle_Positions), Particle_Positions, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ParticleBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// Object Collision Handler
void objectCollisionDetection()
{
	// Iterate Through Each Permutation Between Two Objects
	for (int i = 0; i < physics_count; i++)
	{
		// Test if Object1 is a Hinge
		if (physics[i]->shape == 9)
			objectCollisionDetectionOuterHinge(physics[i], i);

		// Object 1 is a Normal Object
		else
			objectCollisionDetectionOuter(physics[i], i);
	}
}

// Object Collision Handler
void objectCollisionDetectionOuter(Object_Base* pointer_to_object1, int& i)
{
	for (int j = i + 1; j < physics_count; j++)
	{
		// Test if Object2 is a Hinge
		if (physics[j]->shape == 9)
			objectCollisionDetectionInnerHinge(pointer_to_object1, physics[j]);

		// Object 2 is a Normal Object
		else
			objectCollisionDetectionInner(pointer_to_object1, physics[j]);
	}
}

// Object Collision Handler
void objectCollisionDetectionOuterHinge(Object_Base* pointer_to_hinge, int& i)
{
	// Statically Cast Object
	Object_Hinge* hinge = static_cast<Object_Hinge*>(pointer_to_hinge);

	// Iterate Through Each Object
	for (int k = 0; k < hinge->number_of_objects; k++)
		objectCollisionDetectionOuter(hinge->objects[k], i);

	// Iterate Through Each Hinge
	for (int k = 0; k < hinge->number_of_children; k++)
		objectCollisionDetectionOuterHinge(hinge->children[k], i);
}

// Object Collision Handler
void objectCollisionDetectionInner(Object_Base* pointer_to_object1, Object_Base* pointer_to_object2)
{
	switch (pointer_to_object1->shape)
	{

	// Object 1 is a rectangle
	case 1:
	{
		Object_Rectangle* object1 = static_cast<Object_Rectangle*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{

			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 3))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object1, *object2, 4, false))
				Resolve_Rigid(*object1, *object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, object2->number_of_sides))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case 6: { [[fallthrough]]; }
		case 7:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object1, object2->nodes[k], 4, false))
					Resolve_Soft_And_Rigid(*object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a trapezoid
	case 2:
	{
		Object_Trapezoid* object1 = static_cast<Object_Trapezoid*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{
			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, 3))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object1, *object2, 4, false))
				Resolve_Rigid(*object1, *object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 4, object2->number_of_sides))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case 6: { [[fallthrough]]; }
		case 7:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object1, object2->nodes[k], 4, false))
					Resolve_Soft_And_Rigid(*object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a triangle
	case 3:
	{
		Object_Triangle* object1 = static_cast<Object_Triangle*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{
			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 3, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 3, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 3, 3))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object1, *object2, 3, false))
				Resolve_Rigid(*object1, *object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, 3, object2->number_of_sides))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case 6: { [[fallthrough]]; }
		case 7:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object1, object2->nodes[k], 3, false))
					Resolve_Soft_And_Rigid(*object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a circle
	case 4:
	{
		Object_Circle* object1 = static_cast<Object_Circle*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{
			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object2, *object1, 4, true))
				Resolve_Rigid(*object1, *object2, false, true);
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object2, *object1, 4, true))
				Resolve_Rigid(*object1, *object2, false, true);
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object2, *object1, 3, true))
				Resolve_Rigid(*object1, *object2, false, false);
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			if (Circle_Collision_Detection(*object1, *object2))
				Resolve_Rigid(*object1, *object2, false, false);
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object2, *object1, object2->number_of_sides, true))
				Resolve_Rigid(*object1, *object2, false, true);
			break;
		}

		// Object 2 is a Soft Body
		case 6: { [[fallthrough]]; }
		case 7:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (Circle_Node_Collision_Detection(*object1, object2->nodes[k]))
					Resolve_Soft_And_Rigid(*object1, object2->nodes[k], false);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a polygon
	case 5:
	{
		Object_Polygon* object1 = static_cast<Object_Polygon*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{
			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, object1->number_of_sides, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, object1->number_of_sides, 4))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, object1->number_of_sides, 3))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			if (Secondary_Collision_Detection(*object1, *object2, object1->number_of_sides, false))
				Resolve_Rigid(*object1, *object2, true, false);
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			if (Primary_Collision_Detection(*object1, *object2, object1->number_of_sides, object2->number_of_sides))
				Resolve_Rigid(*object1, *object2, true, true);
			break;
		}

		// Object 2 is a Soft Body
		case 6: { [[fallthrough]]; }
		case 7:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object2->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object1, object2->nodes[k], object1->number_of_sides, false))
					Resolve_Soft_And_Rigid(*object1, object2->nodes[k], true);
			}
			break;
		}
		}
		break;
	}

	// Object 1 is a Soft Body
	case 6: { [[fallthrough]]; }
	case 7:
	{
		Object_Soft_Body* object1 = static_cast<Object_Soft_Body*>(pointer_to_object1);
		switch (pointer_to_object2->shape)
		{
			// Object 2 is a rectangle
		case 1:
		{
			Object_Rectangle* object2 = static_cast<Object_Rectangle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object2, object1->nodes[k], 4, false))
					Resolve_Soft_And_Rigid(*object2, object1->nodes[k], true);
			}
			break;
		}

		// Object2 is a trapezoid
		case 2:
		{
			Object_Trapezoid* object2 = static_cast<Object_Trapezoid*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object2, object1->nodes[k], 4, false))
					Resolve_Soft_And_Rigid(*object2, object1->nodes[k], true);
			}
			break;
		}

		// Object2 is a triangle
		case 3:
		{
			Object_Triangle* object2 = static_cast<Object_Triangle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object2, object1->nodes[k], 3, false))
					Resolve_Soft_And_Rigid(*object2, object1->nodes[k], true);
			}
			break;
		}

		// Object 2 is a circle
		case 4:
		{
			Object_Circle* object2 = static_cast<Object_Circle*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (Circle_Node_Collision_Detection(*object2, object1->nodes[k]))
					Resolve_Soft_And_Rigid(*object2, object1->nodes[k], false);
			}
			break;
		}

		// Object 2 is a polygon
		case 5:
		{
			Object_Polygon* object2 = static_cast<Object_Polygon*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				if (Trinary_Collision_Detection(*object2, object1->nodes[k], object2->number_of_sides, false))
					Resolve_Soft_And_Rigid(*object2, object1->nodes[k], true);
			}
			break;
		}

		// Object 2 is a Soft Body
		case 6:
		{
			Object_Soft_Body* object2 = static_cast<Object_Soft_Body*>(pointer_to_object2);
			for (int k = 0; k < object1->node_count; k++)
			{
				for (int h = 0; h < object2->node_count; h++)
				{
					if (Node_Collision_Detection(object1->nodes[k], object2->nodes[h]))
						Resolve_Soft(object1->nodes[k], object2->nodes[h]);
				}
			}
			break;
		}
		}
		break;
	}
	}
}

// Object Collision Handler
void objectCollisionDetectionInnerHinge(Object_Base* pointer_to_object1, Object_Base* pointer_to_hinge)
{
	// Statically Cast Object
	Object_Hinge* hinge = static_cast<Object_Hinge*>(pointer_to_hinge);

	// Iterate Through Each Object
	for (int k = 0; k < hinge->number_of_objects; k++)
		objectCollisionDetectionInner(pointer_to_object1, hinge->objects[k]);

	// Iterate Through Each Hinge
	for (int k = 0; k < hinge->number_of_children; k++)
		objectCollisionDetectionInnerHinge(pointer_to_object1, hinge->children[k]);
}

// Collision Resolution Between Rigid Bodies
template <class Object1, class Object2> void Resolve_Rigid(Object1& object1, Object2& object2, bool enable_first_rotation_vertex, bool enable_second_rotation_vertex)
{
	// Separate Objects
	//glm::vec2 collision_offset = ((temp_collision_loc1 - temp_collision_loc2) * glm::vec2(abs(sin(temp_collision_slope2)), abs(cos(temp_collision_slope2))));
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	object1.physics.Position.x -= collision_offset.x / 2.0f;
	object2.physics.Position.x += collision_offset.x / 2.0f;

	if (object1.physics.grounded && !object2.physics.grounded)
	{
		object2.physics.Position.y += collision_offset.y;
		object2.physics.grounded = true;
		if (enable_second_rotation_vertex)
			object2.physics.Rotation_Vertex = temp_collision_loc2 - object2.physics.Position;
	}

	else if (object2.physics.grounded && !object1.physics.grounded)
	{
		object1.physics.Position.y -= collision_offset.y;
		object1.physics.grounded = true;
		if (enable_first_rotation_vertex)
			object1.physics.Rotation_Vertex = temp_collision_loc1 - object1.physics.Position;
	}

	else
	{
		object1.physics.Position.y -= collision_offset.y / 2.0f;
		object2.physics.Position.y += collision_offset.y / 2.0f;
	}
	
	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(object1.physics.Velocity, glm::vec2(0, 0)) * object1.physics.Mass;
	float work_2 = glm::distance(object2.physics.Velocity, glm::vec2(0, 0)) * object2.physics.Mass;

	// Collision is Inelastic
	if (work_1 > work_2 * INELASTIC || work_2 > work_1 * INELASTIC)
	{
		glm::vec2 new_velocity = (object1.physics.Mass * object1.physics.Velocity + object2.physics.Mass * object1.physics.Velocity) / (object1.physics.Mass + object2.physics.Mass);
		object1.physics.Velocity = new_velocity;
		object2.physics.Velocity = new_velocity;
	}

	// Collision is elastic
	else
	{
		float mass_sum = object1.physics.Mass + object2.physics.Mass;
		glm::vec2 new_velocity_1 = ((object1.physics.Mass - object2.physics.Mass) / mass_sum) * object1.physics.Velocity + ((2 * object2.physics.Mass) / mass_sum) * object2.physics.Velocity;
		glm::vec2 new_velocity_2 = ((object2.physics.Mass - object1.physics.Mass) / mass_sum) * object2.physics.Velocity + ((2 * object1.physics.Mass) / mass_sum) * object1.physics.Velocity;
		object1.physics.Velocity = new_velocity_1;
		object2.physics.Velocity = new_velocity_2;
	}
}

// Collision Resolution Between Soft Bodies
template <class Node1, class Node2> void Resolve_Soft(Node1& node1, Node2& node2)
{
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	node1.Position -= collision_offset / 2.0f;
	node2.Position += collision_offset / 2.0f;

	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(node1.Velocity, glm::vec2(0, 0)) * node1.Mass;
	float work_2 = glm::distance(node2.Velocity, glm::vec2(0, 0)) * node2.Mass;

	// Collision is Inelastic
	if (work_1 > work_2 * INELASTIC || work_2 > work_1 * INELASTIC)
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

// Collision Resolution Between Rigid and Soft Bodies
template <class Object, class Node> void Resolve_Soft_And_Rigid(Object& object, Node& node, bool enable_object_rotate)
{
	// Separate Objects
	glm::vec2 collision_offset = (temp_collision_loc1 - temp_collision_loc2);
	object.physics.Position.x -= collision_offset.x;
	if (object.physics.grounded)
	{
		node.Position.y += collision_offset.y;
	}

	else
	{
		object.physics.Position.y -= collision_offset.y;
		object.physics.grounded = true;
		if (enable_object_rotate)
			object.physics.Rotation_Vertex = temp_collision_loc1 - object.physics.Position;
	}
	
	// Calculate Momentum Changes

	// Calculate Work of Both Objects
	float work_1 = glm::distance(object.physics.Velocity, glm::vec2(0, 0)) * object.physics.Mass;
	float work_2 = glm::distance(node.Velocity, glm::vec2(0, 0)) * node.Mass;

	// Collision is Inelastic
	if (work_1 > work_2 * INELASTIC || work_2 > work_1 * INELASTIC)
	{
		glm::vec2 new_velocity = (object.physics.Mass * object.physics.Velocity + node.Mass * object.physics.Velocity) / (object.physics.Mass + node.Mass);
		object.physics.Velocity = new_velocity;
		node.Velocity = new_velocity;
	}

	// Collision is elastic
	else
	{
		float mass_sum = object.physics.Mass + node.Mass;
		glm::vec2 new_velocity_1 = ((object.physics.Mass - node.Mass) / mass_sum) * object.physics.Velocity + ((2 * node.Mass) / mass_sum) * node.Velocity;
		glm::vec2 new_velocity_2 = ((node.Mass - object.physics.Mass) / mass_sum) * node.Velocity + ((2 * object.physics.Mass) / mass_sum) * object.physics.Velocity;
		object.physics.Velocity = new_velocity_1;
		node.Velocity = new_velocity_2;
	}
}

// Handles Rectangle Objects - Player Collisions
void Rectangle_Collisions(Object_Rectangle& object, Player& player)
{
	if (object == player)
	{
		Player_Collisions_Helper(object, player, true);
	}
}

// Handles Rectangle Objects - Player Collisions
void Trapezoid_Collisions(Object_Trapezoid& object, Player& player)
{
	if (object == player)
	{
		Player_Collisions_Helper(object, player, true);
	}
}

// Handles Triangle Objects - Player Collisions
void Triangle_Collisions(Object_Triangle& object, Player& player)
{
	if (object == player)
	{
		Player_Collisions_Helper(object, player, true);
	}
}

// Handles Circle Objects - Player Collisions
void Circle_Collisions(Object_Circle& object, Player& player)
{
	if (object == player)
	{
		Player_Collisions_Helper(object, player, false);
	}
}

// Handles Polygon Objects - Player Collisions
void Polygon_Collisions(Object_Polygon& object, Player& player)
{
	if (object == player)
	{
		Player_Collisions_Helper(object, player, true);
	}
}

// Handles Node - Player Collisions
template <class Node> void Node_Collisions(Node& node, Player& player)
{
	if (node == player)
	{
		// Resolve Collision
		glm::vec2 collision_offset = temp_collision_loc2 - temp_collision_loc1;
		collision_offset *= 1.0f;
		player.position.x -= collision_offset.x;
		if (player.Grounded)
		{
			node.Position.y += collision_offset.y * 2.0f;
		}

		else
		{
			player.position.y -= collision_offset.y;
			
			// Test if Player Can Jump
			if (abs(temp_collision_angle2) < 0.2f)
				player.Grounded = true;

			// Test if Player Can Walljump
			if (abs(temp_collision_angle2) > 0.8f)
				player.stats.Walled = true;
		}

		// Apply Forces
		node.Forces += glm::vec2(cos(temp_collision_angle1), sin(temp_collision_angle1)) * player.stats.Force * 5.0f;
	}
}

// Helper function for object - player collisions
template <class Object> void Player_Collisions_Helper(Object& object, Player& player, bool enable_rotation_vertices)
{
	//return;

	if (temp_possible_vertex2)
	{
		object.possible_rotation_vertices.push_back(temp_collision_loc1 - object.physics.Position);
		temp_possible_vertex2 = false;
	}

	glm::vec2 collision_offset = (temp_collision_loc2 - temp_collision_loc1) * glm::vec2(abs(sin(temp_collision_slope2)), abs(cos(temp_collision_slope2)));
	glm::vec2 pivot = object.physics.Rotation_Vertex;

	// Disable Sprint (Optional)
	//player.run = false;
	//Keys[GLFW_KEY_LEFT_CONTROL] = false;


	// If player is standing, only shift object position
	if (player.Grounded)
	{
		object.physics.Position.y += collision_offset.y;
		if (!object.physics.grounded)
		{
			if (enable_rotation_vertices)
			{
				object.physics.Rotation_Vertex = temp_collision_loc2 - object.physics.Position;
			}
			object.physics.grounded = true;
		}
	}
	
	// Normally, Player is Moved Back
	else
	{
		player.position.y -= collision_offset.y;

		pivot = object.physics.Center_of_Mass;

		// If Surface is Nearly Flat, Allow Player to Jump
		if (abs(temp_collision_slope2) < 0.2f)
			player.Grounded = true;

		// If Surface is Nearly Vertical, Allow Player to Walljump
		if (abs(temp_collision_slope2) > 0.9f)
			player.stats.Walled = true;
	}

	// Player X is Always Moved
	//collision_offset.x = glm::distance(temp_collision_loc1, temp_collision_loc2) * Sign(collision_offset.x);
	player.position.x -= collision_offset.x;
	
	// If Player is attempting to use force, apply to object
	//temp_collision_angle1 = 0;
	//temp_effectivness1 = 5.0f;
	object.physics.Forces += glm::vec2(cos(temp_collision_angle1), sin(temp_collision_angle1)) * player.stats.Force * temp_effectivness1;

	// Calculate Direction of Force for Player
	float player_angle = -1.5708f; // Direction of Force is up if not moving
	float player_velocity = glm::distance(player.velocity, glm::vec2(0.0f));
	if (player_velocity)
	{
		glm::vec2 normalized_player_velocity = glm::normalize(player.velocity);
		player_angle = atan(normalized_player_velocity.y / normalized_player_velocity.x);
		if (normalized_player_velocity.x < 0)
			player_angle += 3.14159f;
	}

	// Calculate the Torque for the Object
	object.physics.torque -= player.stats.Force * glm::distance(temp_collision_loc2, object.physics.Position + pivot) * sin(player_angle - temp_collision_angle2) * TORQUE_CONSTANT * temp_effectivness1;
}

// Animates Textures
template<class T> void Animate(float& animation_counter, T& texture_layer, Texture texture)
{
	// Increase Counter
	animation_counter += deltaTime;

	// If Counter is Above Data, Increase Layer and Reset Counter
	if (animation_counter > texture.data[(int)texture_layer - 1])
	{
		texture_layer += 1;
		animation_counter = 0;

		if (texture_layer > texture.size)
		{
			texture_layer = 1;
		}
	}
}

// Calculate Bloom
void Bloom(GLuint Texture, int iterations)
{
	// Activate Bloom Shader
	FramebufferShader_Bloom.Use();

	// Which Framebuffer to Use
	bool current_framebuffer = false;
	bool first_iteration = true;

	for (int i = 0; i < iterations; i++)
	{
		// Bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Bloom[current_framebuffer]);

		// Send Iteration to Shader
		glUniform1i(bloomLoc, i);

		// Bind Texture
		glBindTexture(GL_TEXTURE_2D, first_iteration ? Texture : Frame_Buffer_Texture_PingPong[!current_framebuffer]);

		// Apply Blur
		FramebufferShader_Bloom.Use();

		// Update Variables
		current_framebuffer = !current_framebuffer;
		first_iteration = false;
	}

	// Bind Texture to Core Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Core);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_PingPong[current_framebuffer]);
	glActiveTexture(GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Test to Select Physics Objects
void Test_Selector_Physics_Objects(bool& collide, int level, int i, bool from_hinge, bool from_same_hinge, bool child_hinge, GLFWwindow* window, Selector& selector, Object_Base* object, Object_Hinge* pointer_to_hinge)
{
	// Test Collisions
	if (object->Test_Mouse_Collisions())
	{
		// Test if Lock Should be Toggled
		if (lock_object)
		{
			object->Lock = !object->Lock;
			lock_object = false;
			if (selector.editing) { selector.Return(true, true); }
			selector = Selector();
			if (!object->Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
		}

		// Don't Select Object if It is Being Locked
		if (object->Lock)
		{
			glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
		}

		// Select Object For Hinge
		else if (LeftClick && !selector.moving && selector.type_major == 12 && selector.type_minor == 9 && !from_same_hinge)
		{
			// Disable Left Click
			LeftClick = false;

			// Find Hinge
			Object_Hinge* hinge = static_cast<Object_Hinge*>(selector.file_visualizer);

			// Append Object
			Object_Hinge::Limb limb_null = Object_Hinge::Limb();
			hinge->append(*object, false, limb_null);

			// Pop Object from Level
			levels_list[level].pop(i, 12);
		}

		// Deselect Object For Hinge
		else if (LeftClick && !selector.moving && selector.type_major == 12 && selector.type_minor == 9 && from_same_hinge)
		{
			// Disable Left Click
			LeftClick = false;

			// Get Hinge
			Object_Hinge* hinge = static_cast<Object_Hinge*>(selector.file_visualizer);

			// Get Object and Pop From List
			Object_Hinge::Limb limb_null = Object_Hinge::Limb();
			Object_Base* retrieved_object = hinge->pop(i, child_hinge, limb_null);

			// Add Object to Levels
			glm::vec2 level_index;
			UpdateLevel(glm::vec2(retrieved_object->Initial_Position.x, retrieved_object->Initial_Position.y), level_index);
			int index = level_to_index(level_index);
			levels_list[index].objects[levels_list[index].objects_size] = retrieved_object;
			levels_list[index].objects_size++;
			levels_list[index].Sort();
		}

		// Select Object For Editing
		else if (!selector.editing)
		{
			// Select Rigid Body
			if (object->shape < 6)
			{
				Object_Rigid_Body* object_rigid_body = static_cast<Object_Rigid_Body*>(object);
				std::vector<float> new_vector;
				new_vector.push_back(object_rigid_body->physics.Mass);
				new_vector.push_back(object_rigid_body->physics.Max_Health);
				new_vector.push_back(object_rigid_body->physics.Center_of_Gravity.x);
				new_vector.push_back(object_rigid_body->physics.Center_of_Gravity.y);
				new_vector.push_back(object_rigid_body->physics.fluid);
				new_vector.push_back(object_rigid_body->number_of_vertices / 3);
				new_vector.push_back(object_rigid_body->Radius);
				new_vector.push_back(object_rigid_body->Radius_inner);
				selector = Selector(object_rigid_body->physics.Position.x, object_rigid_body->physics.Position.y, object_rigid_body->Initial_Position.z, object_rigid_body->Width, object_rigid_body->Height, object_rigid_body->Angle1, object_rigid_body->Angle2, object_rigid_body->Sizeoffset1, object_rigid_body->Sizeoffset2, new_vector, object_rigid_body->Frequency);
			}

			// Select Soft Bofy
			else if (object->shape == 6)
			{
				Object_SpringMass* object_spring_mass = static_cast<Object_SpringMass*>(object);
				std::vector<float> new_vector;
				selector = Selector(object_spring_mass->Initial_Position.x, object_spring_mass->Initial_Position.y, object_spring_mass->Initial_Position.z, 2.0f, 2.0f, -1, -1, -1, -1, new_vector, object_spring_mass->Frequency);
				selector.File_Name = object_spring_mass->File_Name;
			}

			// Select Wire
			else if (object->shape == 7)
			{
				Object_Wire* object_wire = static_cast<Object_Wire*>(object);
				std::vector<float> new_vector;
				new_vector.push_back(object_wire->node_count);
				new_vector.push_back(object_wire->Mass);
				new_vector.push_back(object_wire->Health);
				new_vector.push_back(object_wire->Radius);
				new_vector.push_back(object_wire->Stiffness);
				new_vector.push_back(object_wire->Dampening);
				new_vector.push_back(object_wire->total_rest_length);
				new_vector.push_back(object_wire->break_distance);
				selector = Selector(object_wire->Initial_Position.x, object_wire->Initial_Position.y, object_wire->Initial_Position.z, object_wire->Size, 1.0f, object_wire->Angle, -1, -1, -1, new_vector, object_wire->Frequency);
			}

			// Select Anchor
			else if (object->shape == 8)
			{
				Object_Anchor* object_anchor = static_cast<Object_Anchor*>(object);
				std::vector<float> new_vector;
				selector = Selector(object_anchor->Initial_Position.x, object_anchor->Initial_Position.y, object_anchor->Initial_Position.z, 1.1f, 1.1f, -1, -1, -1, -1, new_vector, object_anchor->Frequency);
			}

			// Select Hinge
			else if (object->shape == 9)
			{
				Object_Hinge* object_hinge = static_cast<Object_Hinge*>(object);
				std::vector<float> new_vector;
				selector = Selector(object_hinge->Initial_Position.x, object_hinge->Initial_Position.y, object_hinge->Initial_Position.z, 1.1f, 1.1f, -1, -1, -1, -1, new_vector, object_hinge->Frequency);
				selector.File_Name = object_hinge->File_Name;
			}

			// If Left Mouse Button is Being Held, Select the Object
			if (LeftClick)
			{
				// Return Selector if Already Editing An Object
				if (selector.editing)
				{
					selector.Return(true, true);
				}

				// Object is From a Hinge
				if (from_hinge)
				{
					selector.Replace_From_Hinge(i, object->shape == 9, pointer_to_hinge);
				}

				// Object is From Levels
				else
				{
					selector.Replace(i, object->object_type, level);
					levels_list[level].Sort();
				}
			}
		}
	
		collide = true;
	}

	if (collide) { return; }

	// If Object is a Hinge, Iterate Through All Objects and Children
	if (object->shape == 9)
	{
		// Get Hinge
		Object_Hinge* hinge = static_cast<Object_Hinge*>(object);

		// Iterate Through All Objects
		for (int j = 0; j < hinge->number_of_objects; j++)
		{
			Test_Selector_Physics_Objects(collide, 0, j, true, false, false, window, selector, hinge->objects[j], hinge);
			if (collide) { return; }
		}

		// Iterate Through All Children
		for (int j = 0; j < hinge->number_of_children; j++)
		{
			Test_Selector_Physics_Objects(collide, 0, j, true, false, true, window, selector, hinge->children[j], hinge);
			if (collide) { return; }
		}
	}
}

