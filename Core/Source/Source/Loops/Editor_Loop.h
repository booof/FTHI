// Editor Game Loop8
void Loop_Editor(GLFWwindow* window)
{
	// Reset Camera
	camera.accelerationL = 1;
	camera.accelerationR = 1;
	camera.accelerationY = 1;
	camera.constantX = 0;
	glm::vec4 accelerationTimer = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	camera_speed_multiplier = editor_options.camera_speed;
	shift_speed_multiplier = editor_options.shift_speed;

	// Empty Vector
	std::vector<float> empty_vector;
	empty_vector.push_back(0.0f);
	empty_vector.push_back(0.0f);
	empty_vector.push_back(0.0f);
	empty_vector.push_back(0.0f);
	empty_vector.push_back(0.0f);
	empty_vector.push_back(0.0f);

	// Reset Levels
	levelHandler(camera.level, camera.level, true);

	// Initialize Editor Config
	editor_options.initialize();

	// Generate Collision Mask Lines and Light Indicators
	for (int level = 0; level < 9; level++)
	{
		// Floors
		for (int i = 0; i < levels_list[level].floor_objects_size; i++)
		{
			levels_list[level].floor_objects[i].InitializeLine();
		}

		// Walls
		for (int i = 0; i < levels_list[level].wall_objects_size; i++)
		{
			levels_list[level].wall_objects[i].InitializeLine();
		}

		// Reverse Floors
		for (int i = 0; i < levels_list[level].ceiling_objects_size; i++)
		{
			levels_list[level].ceiling_objects[i].InitializeLine();
		}

		// Death Object
		for (int i = 0; i < levels_list[level].trigger_objects_size; i++)
		{
			levels_list[level].trigger_objects[i].InitializeLine();
		}

		// Directonal Objects
		for (int i = 0; i < levels_list[level].directional_objects_size; i++)
		{
			levels_list[level].directional_objects[i].Initialize_Identifier();
		}

		// Point Objects
		for (int i = 0; i < levels_list[level].point_objects_size; i++)
		{
			levels_list[level].point_objects[i].Initialize_Identifier();
		}

		// Spot Objects
		for (int i = 0; i < levels_list[level].spot_objects_size; i++)
		{
			levels_list[level].spot_objects[i].Initialize_Identifier();
		}
	}

	// Generate Level Borders
	float vertices[] =
	{
		// Position             // Colors
		-128.0f, 64.0f,  1.0f,  0.0f, 0.0f, 0.0f,  // Top Horizontal Left
		256.0f,  64.0f,  1.0f,  0.0f, 0.0f, 0.0f,  // Top Horizotnal Right

		-128.0f, 0.0f,   1.0f,  0.0f, 0.0f, 0.0f,  // Bottom Horizontal Left
		256.0f,  0.0f,   1.0f,  0.0f, 0.0f, 0.0f,  // Bottom Horizontal Right

		0.0f,    128.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // Left Vertical Top
		0.0f,    -64.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // Left Vertical Bottom

		128.0f,  128.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // Right Vertical Top
		128.0f,  -64.0f, 1.0f,  0.0f, 0.0f, 0.0f   // Right Vertical Bottom
	};
	glm::mat4 border_model = glm::mat4(1.0f);
	GLuint borderVAO, borderVBO;
	glGenVertexArrays(1, &borderVAO);
	glGenBuffers(1, &borderVBO);
	glBindVertexArray(borderVAO);
	glBindBuffer(GL_ARRAY_BUFFER, borderVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Selector is Needed to Highlight and Select Objects to Edit
	// Secondary Selector is to Highlight Other Objects while Editing

	// Ojects Used for Editing
	Selector selector;
	Selector selectorBackup;

	// Another Object Used for Editing
	EditorWindow editorWindow(selector);

	while (looptype == 1 && !glfwWindowShouldClose(window))
	{
		// Process Errors
		//Error_Log();

		// Proccess Time
		DeltaTime();

		//std::cout << "\n";

		// Handle Inputs
		cursor_Move = false;
		glfwPollEvents();
		if (!editorWindow.text_edit) { SmoothKeyCallback_Editor(camera, selector, editorWindow, accelerationTimer); }

		// Check Collisions if Mouse Moved
		if ((cursor_Move || LeftClick || lock_object) && !editorWindow.active && !editor_options.Active)
		{
			// Reset Mouse Cursor Image
			if (!selector.editing) { glfwSetCursor(window, Mouse_Textures.find("Cursor")->second); }

			// Determines if a Collision Occoured
			bool collide = false;

			// Test Collisions of Objects of Currently Loaded Levels

			// Physics Object Collisions
			for (int level = 0; level < 9; level++)
			{
				// Test Object Collisions
				for (int i = 0; i < levels_list[level].objects_size * (editor_options.interactable[12] == 0); i++)
				{
					Test_Selector_Physics_Objects(collide, level, i, false, false, false, window, selector, levels_list[level].objects[i], NULL);
					if (collide) { break; }
				}
				if (collide) { break; }
			}

			// Test Physics Objects from Selected Hinge
			if (selector.type_major == 12 && selector.type_minor == 9)
			{
				Object_Hinge* hinge = static_cast<Object_Hinge*>(selector.file_visualizer);
				for (int i = 0; i < hinge->number_of_objects; i++)
				{
					Test_Selector_Physics_Objects(collide, 0, i, true, true, false, window, selector, hinge->objects[i], NULL);
					if (collide) { break; }
				}
				for (int i = 0; i < hinge->number_of_children; i++)
				{
					Test_Selector_Physics_Objects(collide, 0, i, true, true, true, window, selector, hinge->children[i], NULL);
					if (collide) { break; }
				}
			}

			// Light Collisions
			for (int level = 0; level < 9; level++)
			{
				// Test Collisions of Point Objects
				for (int i = 0; i < levels_list[level].point_objects_size * (editor_options.interactable[9] == 0); i++)
				{
					// Test Collisons
					if (levels_list[level].point_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY - 0.75f))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].point_objects[i].Lock = !levels_list[level].point_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].point_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].point_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].point_objects[i].xPos, levels_list[level].point_objects[i].yPos, 0.0f, 2.0f, 3.0f, -1, -1, -1, -1, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 9.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}

				}

				if (collide) { break; }

				// Test Collisions of Spot Objects
				for (int i = 0; i < levels_list[level].spot_objects_size * (editor_options.interactable[10] == 0); i++)
				{
					// Test Collisions
					if (levels_list[level].spot_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY - 0.75f))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].spot_objects[i].Lock = !levels_list[level].spot_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].spot_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].spot_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].spot_objects[i].xPos, levels_list[level].spot_objects[i].yPos, 0.0f, 2.0f, 4.0f, -1, -1, -1, -1, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 10.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }

				// Test Collisions of Direcional Objects
				for (int i = 0; i < levels_list[level].directional_objects_size * (editor_options.interactable[8] == 0); i++)
				{
					// Test Collisions
					if (levels_list[level].directional_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY - 0.75f))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].directional_objects[i].Lock = !levels_list[level].directional_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].directional_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].directional_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].directional_objects[i].xPos, levels_list[level].directional_objects[i].yPos, 0.0f, levels_list[level].directional_objects[i].xPos2 - levels_list[level].directional_objects[i].xPos, 1.0f, atan((levels_list[level].directional_objects[i].yPos2 - levels_list[level].directional_objects[i].yPos) / (levels_list[level].directional_objects[i].xPos2 - levels_list[level].directional_objects[i].xPos)), -1, -1, -1, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 8.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }
				
				// Test Collisions of Particles
				for (int i = 0; i < levels_list[level].particle_objects_size * (editor_options.interactable[11] == 0); i++)
				{
					// Test Collisions
					if (levels_list[level].particle_objects[i]->TestCollisions())
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].particle_objects[i]->Lock = !levels_list[level].particle_objects[i]->Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].particle_objects[i]->Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].particle_objects[i]->Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							std::vector<float> new_vector;
							new_vector.push_back(levels_list[level].particle_objects[i]->MaxLife);
							new_vector.push_back(levels_list[level].particle_objects[i]->Speed);
							new_vector.push_back(levels_list[level].particle_objects[i]->Spread);
							new_vector.push_back(levels_list[level].particle_objects[i]->Size);
							new_vector.push_back(levels_list[level].particle_objects[i]->Maximum_Particles);
							new_vector.push_back(levels_list[level].particle_objects[i]->SpawnWeight);
							selector = Selector(levels_list[level].particle_objects[i]->xPos, levels_list[level].particle_objects[i]->yPos, 0.2f, levels_list[level].particle_objects[i]->Width, levels_list[level].particle_objects[i]->Height, -1, -1, -1, -1, new_vector, levels_list[level].particle_objects[i]->Frequency);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 11.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }
			}

			// Collision Detection
			for (int level = 0; level < 9; level++)
			{
				// Test Collisions of CollisionMaskFloor Objects
				for (int i = 0; i < levels_list[level].floor_objects_size * (editor_options.interactable[0] == 0); i++)
				{
					// Collisions Occour when Y-Positions Differ
					if (levels_list[level].floor_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY - 0.75f, SPEED * deltaTime * 1.5f + 1.0f, false))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].floor_objects[i].Lock = !levels_list[level].floor_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].floor_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].floor_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							// Create Different Object Depending on Object Type
							switch (levels_list[level].floor_objects[i].Type)
							{

							// Line
							case 0:
							{
								selector = Selector(levels_list[level].floor_objects[i].PosX + levels_list[level].floor_objects[i].Size / 2, levels_list[level].floor_objects[i].PosY, 0.0f, levels_list[level].floor_objects[i].Size, zoom_scale, -1, -1, -1, -1, empty_vector, -1);
								break;
							}

							// Slant
							case 1:
							{
								selector = Selector(levels_list[level].floor_objects[i].PosX, levels_list[level].floor_objects[i].PosY, 0.0f, levels_list[level].floor_objects[i].Size, 1.0f, levels_list[level].floor_objects[i].Angle, -1, -1, -1, empty_vector, -1);
								break;
							}

							// Curve
							case 2:
							{
								selector = Selector(levels_list[level].floor_objects[i].PosX, levels_list[level].floor_objects[i].PosY, 0.0f, levels_list[level].floor_objects[i].Size * 2, levels_list[level].floor_objects[i].Angle * 2, -1, -1, -1, -1, empty_vector, -1);
								break;
							}

							}

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, (float)(levels_list[level].floor_objects[i].Type) / 10.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }

				int sign = 0;
				// Test Collisions of CollisionMaskWall Objects
				for (int i = 0; i < levels_list[level].wall_objects_size * (editor_options.interactable[1] == 0); i++)
				{
					sign = -1 + 2 * (levels_list[level].wall_objects[i].Type % 2);

					// Collisions Occour when X-Positions Differ
					if (levels_list[level].wall_objects[i].TestCollisions(mouseRelativeX + sign * 0.75f, mouseRelativeY))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].wall_objects[i].Lock = !levels_list[level].wall_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].wall_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].wall_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							if (levels_list[level].wall_objects[i].Type == 0 || levels_list[level].wall_objects[i].Type == 1)
							{
								selector = Selector(levels_list[level].wall_objects[i].PosX, levels_list[level].wall_objects[i].PosY + levels_list[level].wall_objects[i].Size / 2, 0.0f, zoom_scale, levels_list[level].wall_objects[i].Size, -1, -1, -1, -1, empty_vector, -1);

							}

							else
							{
								selector = Selector(levels_list[level].wall_objects[i].PosX, levels_list[level].wall_objects[i].PosY, 0.0f, levels_list[level].wall_objects[i].Size, levels_list[level].wall_objects[i].Angle, -1, -1, -1, -1, empty_vector, -1);
							}

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, (float)(levels_list[level].wall_objects[i].Type) / 10.0f + 1, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }

				// Test Collisions of CollisionMaskReverseFloor Objects
				for (int i = 0; i < levels_list[level].ceiling_objects_size * (editor_options.interactable[2] == 0); i++)
				{
					// Collisions Occour when Y-Positions Differ
					if (levels_list[level].ceiling_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY + 0.75f))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].ceiling_objects[i].Lock = !levels_list[level].ceiling_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].ceiling_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].ceiling_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							if (levels_list[level].ceiling_objects[i].Type == 0)
							{
								selector = Selector(levels_list[level].ceiling_objects[i].PosX + levels_list[level].ceiling_objects[i].Size / 2, levels_list[level].ceiling_objects[i].PosY, 0.0f, levels_list[level].ceiling_objects[i].Size, zoom_scale, -1, -1, -1, -1, empty_vector, -1);
							}

							else
							{
								selector = Selector(levels_list[level].ceiling_objects[i].PosX, levels_list[level].ceiling_objects[i].PosY, 0.0f, levels_list[level].ceiling_objects[i].Size, 1.0f, levels_list[level].ceiling_objects[i].Angle, -1, -1, -1, empty_vector, -1);
							}

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, (float)(levels_list[level].ceiling_objects[i].Type) / 10.0f + 2, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }

				// Test Collisions of Trigger Object
				for (int i = 0; i < levels_list[level].trigger_objects_size * (editor_options.interactable[3] == 0); i++)
				{
					// Test Collision
					if (levels_list[level].trigger_objects[i].TestCollisions(mouseRelativeX, mouseRelativeY))
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].trigger_objects[i].Lock = !levels_list[level].trigger_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].trigger_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].trigger_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							// Highlight Object
							selector = Selector(levels_list[level].trigger_objects[i].PosX, levels_list[level].trigger_objects[i].PosY, 0.0f, levels_list[level].trigger_objects[i].Width, levels_list[level].trigger_objects[i].Height, -1, -1, -1, -1, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 3.0f, -1);
								levels_list[level].Sort();
							}
						}

						collide = true;
						break;
					}
				}

				if (collide) { break; }
			}

			for (int level = 0; level < 9; level++)
			{
				// Formerground Objects
				for (int i = levels_list[level].formerground_objects_size * (editor_options.interactable[5] == 0) - 1; i > -1 * (editor_options.interactable[5] == 0); i--)
				{
					// Colisions Occour if Test Returns True
					if (levels_list[level].formerground_objects[i].TestCollisions())
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].formerground_objects[i].Lock = !levels_list[level].formerground_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].formerground_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].formerground_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].formerground_objects[i].xPos, levels_list[level].formerground_objects[i].yPos, levels_list[level].formerground_objects[i].zPercent, levels_list[level].formerground_objects[i].size1, levels_list[level].formerground_objects[i].size2, levels_list[level].formerground_objects[i].angle1, levels_list[level].formerground_objects[i].angle2, levels_list[level].formerground_objects[i].sizeOffset1, levels_list[level].formerground_objects[i].sizeOffset2, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								// Retrieve Type of Object
								float type = 5.3f;
								if (levels_list[level].formerground_objects[i].rectangular)
								{
									type = 5.1f;

									if (levels_list[level].formerground_objects[i].sizeOffset1 != -1 && levels_list[level].formerground_objects[i].sizeOffset2 != -1)
									{
										type = 5.2f;
									}
								}

								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, type, -1);
								levels_list[level].Sort();
							}
						}
						collide = true;
						break;
					}

					if (collide) { break; }
				}

				if (collide) { break; }

				// Foreground Objects
				for (int i = levels_list[level].foreground_objects_size * (editor_options.interactable[4] == 0) - 1; i > -1 * (editor_options.interactable[4] == 0); i--)
				{
					// Colisions Occour if Test Returns True
					if (levels_list[level].foreground_objects[i].TestCollisions())
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].foreground_objects[i].Lock = !levels_list[level].foreground_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].foreground_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].foreground_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].foreground_objects[i].xPos, levels_list[level].foreground_objects[i].yPos, levels_list[level].foreground_objects[i].zPercent, levels_list[level].foreground_objects[i].size1, levels_list[level].foreground_objects[i].size2, levels_list[level].foreground_objects[i].angle1, levels_list[level].foreground_objects[i].angle2, levels_list[level].foreground_objects[i].sizeOffset1, levels_list[level].foreground_objects[i].sizeOffset2, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								// Retrieve Type of Object
								float type = 4.3f;
								if (levels_list[level].foreground_objects[i].rectangular)
								{
									type = 4.1f;

									if (levels_list[level].foreground_objects[i].sizeOffset1 != -1 && levels_list[level].foreground_objects[i].sizeOffset2 != -1)
									{
										type = 4.2f;
									}
								}

								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, type, -1);
								levels_list[level].Sort();
							}
						}
						collide = true;
						break;
					}

					if (collide) { break; }
				}

				if (collide) { break; }

				// Background Objects
				for (int i = levels_list[level].background_objects_size * (editor_options.interactable[6] == 0) - 1; i > -1 * (editor_options.interactable[6] == 0); i--)
				{
					// Colisions Occour if Test Returns True
					if (levels_list[level].background_objects[i].TestCollisions())
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].background_objects[i].Lock = !levels_list[level].background_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].background_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].background_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].background_objects[i].xPos, levels_list[level].background_objects[i].yPos, levels_list[level].background_objects[i].zPercent, levels_list[level].background_objects[i].size1, levels_list[level].background_objects[i].size2, levels_list[level].background_objects[i].angle1, levels_list[level].background_objects[i].angle2, levels_list[level].background_objects[i].sizeOffset1, levels_list[level].background_objects[i].sizeOffset2, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								// Retrieve Type of Object
								float type = 6.3f;
								if (levels_list[level].background_objects[i].rectangular)
								{
									type = 6.1f;

									if (levels_list[level].background_objects[i].sizeOffset1 != -1 && levels_list[level].background_objects[i].sizeOffset2 != -1)
									{
										type = 6.2f;
									}
								}

								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, type, -1);
								levels_list[level].Sort();
							}
						}
						collide = true;
						break;
					}

					if (collide) { break; }
				}

				if (collide) { break; }

				// Backdrop Objects
				for (int i = levels_list[level].backdrop_objects_size * (editor_options.interactable[7] == 0) - 1; i > -1 * (editor_options.interactable[7] == 0); i--)
				{
					// Colisions Occour if Test Returns True
					if (levels_list[level].backdrop_objects[i].TestCollisions())
					{
						// Test if Lock Should be Toggled
						if (lock_object)
						{
							levels_list[level].backdrop_objects[i].Lock = !levels_list[level].backdrop_objects[i].Lock;
							lock_object = false;
							if (selector.editing) { selector.Return(true, true); }
							selector = Selector();
							if (!levels_list[level].backdrop_objects[i].Lock) { glfwSetCursor(window, Mouse_Textures.find("Hand")->second); }
						}

						// Don't Select Object if It is Being Locked
						if (levels_list[level].backdrop_objects[i].Lock)
						{
							glfwSetCursor(window, Mouse_Textures.find("Lock")->second);
						}

						// Select Object For Editing
						else if (!selector.editing)
						{
							selector = Selector(levels_list[level].backdrop_objects[i].xPos, levels_list[level].backdrop_objects[i].yPos, levels_list[level].backdrop_objects[i].zPercent, levels_list[level].backdrop_objects[i].size1, levels_list[level].backdrop_objects[i].size2, -1, -1, -1, -1, empty_vector, -1);

							// If Left Mouse Button is Being Held, Select the Object
							if (LeftClick)
							{
								if (selector.editing)
								{
									selector.Return(true, true);
								}

								selector.Replace(i, 7.1, -1);
								levels_list[level].Sort();
							}
						}
						collide = true;
						break;
					}
				}

				if (collide) { break; }
			}

			// Return a Nullified Selector Object if no Collisions Occoured
			if (!collide && !selector.editing)
			{
				selector = Selector();
				Selected_Cursor = 0;
			}

			else
			{
				Selected_Cursor = 3;
			}
		}
	
		// Disable Some Flags
		lock_object = false;

		// Bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Multisample);
		glEnable(GL_DEPTH_TEST);

		// Clear Window
		glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glScissor(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

		// Draw Static Line Colored Objects
		colorShaderStaticLine.Use();

		// Draw Collision Mask
		for (int level = 0; level < 9 * editor_options.visualize_masks; level++)
		{
			// Floor
			for (int i = 0; i < levels_list[level].floor_objects_size * (editor_options.interactable[0] < 2); i++)
			{
				levels_list[level].floor_objects[i].DrawLine();
			}

			// Wall
			for (int i = 0; i < levels_list[level].wall_objects_size * (editor_options.interactable[1] < 2); i++)
			{
				levels_list[level].wall_objects[i].DrawLine();
			}

			// Reverse Floor
			for (int i = 0; i < levels_list[level].ceiling_objects_size * (editor_options.interactable[2] < 2); i++)
			{
				levels_list[level].ceiling_objects[i].DrawLine();
			}

			// Trigger
			for (int i = 0; i < levels_list[level].trigger_objects_size * (editor_options.interactable[3] < 2); i++)
			{
				levels_list[level].trigger_objects[i].Blitz();
			}
		}

		// Draw Non-Static Objects
		if (editor_options.EnableLighting)
		{
			// Draw Colored Objects
			colorShader.Use();
			glUniform4f(glGetUniformLocation(colorShader.Program, "material.viewPos"), camera.Position.x, camera.Position.y, 0.0f, 0.0f);

			// Draw Colored Terrain
			for (int level = 0; level < 9; level++)
			{
				// Backdrop
				for (int i = 0; i < levels_list[level].backdrop_objects_texture_start * (editor_options.interactable[7] < 2); i++)
				{
					levels_list[level].backdrop_objects[i].Draw_Color();
				}

				// Background
				for (int i = 0; i < levels_list[level].background_objects_texture_start * (editor_options.interactable[6] < 2); i++)
				{
					levels_list[level].background_objects[i].Draw_Color();
				}

				// Foreground
				for (int i = 0; i < levels_list[level].foreground_objects_texture_start * (editor_options.interactable[4] < 2); i++)
				{
					levels_list[level].foreground_objects[i].Draw_Color();
				}

				// Formerground
				for (int i = 0; i < levels_list[level].formerground_objects_texture_start * (editor_options.interactable[5] < 2); i++)
				{
					levels_list[level].formerground_objects[i].Draw_Color();
				}

				// Objects
				for (int i = 0; i < levels_list[level].objects_texture_start * (editor_options.interactable[12] < 2); i++)
				{
					levels_list[level].objects[i]->Draw_Color();
				}

				// Particles
				for (int i = 0; i < levels_list[level].particle_objects_size * (editor_options.interactable[11] < 2); i++)
				{
					levels_list[level].particle_objects[i]->Draw_Visualizer();
				}
			}

			// Draw Textured Objects
			texShader.Use();
			glUniform4f(glGetUniformLocation(texShader.Program, "material.viewPos"), camera.Position.x, camera.Position.y, 0.0f, 0.0f);

			// Draw Textured Terrain
			for (int level = 0; level < 9; level++)
			{
				// Backdrop
				for (int i = levels_list[level].backdrop_objects_texture_start; i < levels_list[level].backdrop_objects_size * (editor_options.interactable[7] < 2); i++)
				{
					levels_list[level].backdrop_objects[i].Draw_Texture();
				}

				// Background
				for (int i = levels_list[level].background_objects_texture_start; i < levels_list[level].background_objects_size * (editor_options.interactable[6] < 2); i++)
				{
					levels_list[level].background_objects[i].Draw_Texture();
				}

				// Foreground
				for (int i = levels_list[level].foreground_objects_texture_start; i < levels_list[level].foreground_objects_size * (editor_options.interactable[4] < 2); i++)
				{
					levels_list[level].foreground_objects[i].Draw_Texture();
				}

				// Formerground
				for (int i = levels_list[level].formerground_objects_texture_start; i < levels_list[level].formerground_objects_size * (editor_options.interactable[5] < 2); i++)
				{
					levels_list[level].formerground_objects[i].Draw_Texture();
				}

				// Objects
				for (int i = levels_list[level].objects_texture_start; i < levels_list[level].objects_size * (editor_options.interactable[12] < 2); i++)
				{
					levels_list[level].objects[i]->Draw_Texture();
				}
			}
		}

		// Draw Static Objects
		else
		{
			// Draw Colored Objects
			colorShaderStatic.Use();
			glUniform1i(staticLocColor, 0);

			// Draw Colored Terrain
			for (int level = 0; level < 9; level++)
			{
				// Backdrop
				for (int i = 0; i < levels_list[level].backdrop_objects_texture_start * (editor_options.interactable[7] < 2); i++)
				{
					levels_list[level].backdrop_objects[i].Draw_Static_Color();
				}

				// Background
				for (int i = 0; i < levels_list[level].background_objects_texture_start * (editor_options.interactable[6] < 2); i++)
				{
					levels_list[level].background_objects[i].Draw_Static_Color();
				}

				// Foreground
				for (int i = 0; i < levels_list[level].foreground_objects_texture_start * (editor_options.interactable[4] < 2); i++)
				{
					levels_list[level].foreground_objects[i].Draw_Static_Color();
				}

				// Formerground
				for (int i = 0; i < levels_list[level].formerground_objects_texture_start * (editor_options.interactable[5] < 2); i++)
				{
					levels_list[level].formerground_objects[i].Draw_Static_Color();
				}

				// Objects
				for (int i = 0; i < levels_list[level].objects_texture_start * (editor_options.interactable[12] < 2); i++)
				{
					levels_list[level].objects[i]->Draw_Static_Color();
				}

				// Particles
				for (int i = 0; i < levels_list[level].particle_objects_size * (editor_options.interactable[11] < 2); i++)
				{
					levels_list[level].particle_objects[i]->Draw_Static_Visualizer();
				}
			}

			// Draw Textured Objects
			texShaderStatic.Use();
			glUniform1i(staticLocTexture, 0);

			// Draw Textured Terrain
			for (int level = 0; level < 9; level++)
			{
				// Backdrop
				for (int i = levels_list[level].backdrop_objects_texture_start; i < levels_list[level].backdrop_objects_size * (editor_options.interactable[7] < 2); i++)
				{
					levels_list[level].backdrop_objects[i].Draw_Static_Texture();
				}

				// Background
				for (int i = levels_list[level].background_objects_texture_start; i < levels_list[level].background_objects_size * (editor_options.interactable[6] < 2); i++)
				{
					levels_list[level].background_objects[i].Draw_Static_Texture();
				}

				// Foreground
				for (int i = levels_list[level].foreground_objects_texture_start; i < levels_list[level].foreground_objects_size * (editor_options.interactable[4] < 2); i++)
				{
					levels_list[level].foreground_objects[i].Draw_Static_Texture();
				}

				// Formerground
				for (int i = levels_list[level].formerground_objects_texture_start; i < levels_list[level].formerground_objects_size * (editor_options.interactable[5] < 2); i++)
				{
					levels_list[level].formerground_objects[i].Draw_Static_Texture();
				}

				// Objects
				for (int i = levels_list[level].objects_texture_start; i < levels_list[level].objects_size * (editor_options.interactable[12] < 2); i++)
				{
					levels_list[level].objects[i]->Draw_Static_Texture();
				}
			}
		}

		// Draw Borders
		if (level_border)
		{
			colorShaderStaticLine.Use();
			border_model = glm::translate(glm::mat4(1.0f), glm::vec3(camera.level.x * 128, (camera.level.y + 1) * 64, -2.0));
			glUniformMatrix4fv(modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(border_model));
			glBindVertexArray(borderVAO);
			glDrawArrays(GL_LINES, 0, 16);
			glBindVertexArray(0);
		}

		// Blitz Multisampled Framebuffer to Framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, Frame_Buffer_Object_Multisample);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Frame_Buffer_Object_HDR);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		// Bind GUI Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Core);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		// Draw Screen Texture
		FramebufferShader_HDR.Use();
		glBindVertexArray(screenVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_HDR);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Draw Normals
		NormalShader.Use();

		// Draw Normals of Terrain
		if (normals)
		{
			for (int level = 0; level < 9; level++)
			{
				// Backdrop
				for (int i = 0; i < levels_list[level].backdrop_objects_size * (editor_options.interactable[7] < 2); i++)
				{
					levels_list[level].backdrop_objects[i].Draw_Normal();
				}

				// Background
				for (int i = 0; i < levels_list[level].background_objects_size * (editor_options.interactable[6] < 2); i++)
				{
					levels_list[level].background_objects[i].Draw_Normal();
				}

				// Foreground
				for (int i = 0; i < levels_list[level].foreground_objects_size * (editor_options.interactable[4] < 2); i++)
				{
					levels_list[level].foreground_objects[i].Draw_Normal();
				}

				// Formerground
				for (int i = 0; i < levels_list[level].formerground_objects_size * (editor_options.interactable[5] < 2); i++)
				{
					levels_list[level].formerground_objects[i].Draw_Normal();
				}
			}
		}

		// Draw Static Texture Objects
		texShaderStatic.Use();
		glUniform1i(staticLocTexture, 0);

		// Draw Lights
		for (int level = 0; level < 9 * editor_options.visualize_lights; level++)
		{
			// Directional
			for (int i = 0; i < levels_list[level].directional_objects_size * (editor_options.interactable[8] < 2); i++)
			{
				levels_list[level].directional_objects[i].Blitz();
			}

			// Point
			for (int i = 0; i < levels_list[level].point_objects_size * (editor_options.interactable[9] < 2); i++)
			{
				levels_list[level].point_objects[i].Blitz();
			}

			// Spot
			for (int i = 0; i < levels_list[level].spot_objects_size * (editor_options.interactable[10] < 2); i++)
			{
				levels_list[level].spot_objects[i].Blitz();
			}
		}

		// Test if Editor Window Should Activate
		if (RightClick && !editor_options.Active)
		{
			RightClick = false;

			// Initialize Window if Not Currently Active
			if (!editorWindow.active)
			{
				Selected_Cursor = 0;

				// Initialize Object Specified Window if an Object is Currently Selected
				if (selector.initialized)
				{
					// Reset Several Values
					editorWindow.dialationPercent = 1;
					editorWindow.bar1.percent = 0;

					// Copy Object Buffer Object to Window

					// The Number of Vertices to be Copied
					int vertexCount = (selector.texture.texture == NULL) ? 36 : 30;
					if (selector.normalized) { vertexCount += 18; }
					if (selector.Angle1 && selector.Angle2) { vertexCount = vertexCount / 2; }

					// Bind Normal Object
					glBindVertexArray(editorWindow.selectedObjectVAO);
					glBindBuffer(GL_ARRAY_BUFFER, editorWindow.selectedObjectVBO);

					// Assign Normal Color Data to Shader
					if (selector.texture.texture == NULL && selector.normalized)
					{
						// Nullified Color Data
						glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

						// Send Position Data to Shaders
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GL_FLOAT), (void*)(0));
						glEnableVertexAttribArray(0);

						// Send Color Data to Shaders
						glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(1);

						// Send Normal Data to Shaders
						glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(2);
					}

					// Assign Normal Texture Data to Shader
					else if (selector.texture.texture && selector.normalized)
					{
						// Nullified Texture Data
						glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

						// Send Position Data to Shaders
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(0));
						glEnableVertexAttribArray(0);

						// Send Color Data to Shaders
						glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(1);

						// Send Normal Data to Shaders
						glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void*)(5 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(2);
					}

					// Assign Color Data to Shader
					else if (selector.texture.texture == NULL)
					{
						// Nullified Color Data
						glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

						// Send Position Data to Shaders
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
						glEnableVertexAttribArray(0);

						// Send Color Data to Shaders
						glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(1);
					}

					// Assign Texture Data to Shader
					else
					{
						// Nullified Texture Data
						glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

						// Send Position Data to Shaders
						glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
						glEnableVertexAttribArray(0);

						// Send Color Data to Shaders
						glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
						glEnableVertexAttribArray(1);
					}

					// Unbind Normal Object
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);

					// Bind Selector Object for Reading and Copying and Bind Editor Window Object to be Written to
					glBindBuffer(GL_COPY_READ_BUFFER, selector.objectVBO);
					glBindBuffer(GL_COPY_WRITE_BUFFER, editorWindow.selectedObjectVBO);

					// Copy Selector Object to Editor Window Object
					glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vertexCount * sizeof(GLfloat));

					// Unbind Buffer Objects
					glBindBuffer(GL_COPY_READ_BUFFER, 0);
					glBindBuffer(GL_COPY_WRITE_BUFFER, 0);


					// Send Selector Data to Window
					editorWindow.objectX = selector.xPos;
					editorWindow.objectY = selector.yPos;
					editorWindow.objectZPercent = selector.zPercent;
					editorWindow.objectValue1 = (selector.linear) ? selector.linear : selector.Size1;
					editorWindow.objectValue2 = (selector.quadratic) ? selector.quadratic : selector.Size2;
					editorWindow.objectValue3 = (selector.type_major == 10) ? selector.cutoffAngleInside : ((selector.SizeOffset1 != -1) ? selector.SizeOffset1 : selector.Angle1);
					editorWindow.objectValue4 = (selector.type_major == 10) ? selector.cutoffAngleOutside : ((selector.SizeOffset2 != -1) ? selector.SizeOffset2 : selector.Angle2);
					editorWindow.extraValue = selector.ExtraValues;
					editorWindow.frequencyValue = selector.Frequency;
					editorWindow.objectColor = selector.color;
					editorWindow.lightColorAmbient = selector.lightAmbient;
					editorWindow.lightColorDiffuse = selector.lightDiffuse / glm::vec4(selector.lightIntensity, selector.lightIntensity, selector.lightIntensity, 1.0f);
					editorWindow.lightColorSpecular = selector.lightSpecular / glm::vec4(selector.lightIntensity, selector.lightIntensity, selector.lightIntensity, 1.0f);
					editorWindow.lightIntensity = selector.lightIntensity;
					editorWindow.objectNormal = selector.normal;
					editorWindow.objectTexture = selector.texture;
					editorWindow.material = ReverseTextureMap(selector.material, materials);
					editorWindow.Clamp = selector.Clamp;
					editorWindow.Lock = selector.Lock;
					editorWindow.File_Name = selector.File_Name;
					switch (selector.type_major)
					{
					case 4:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Foreground_Textures);
						break;
					}
					case 5:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Formerground_Textures);
						break;
					}
					case 6:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Background_Textures);
						break;
					}
					case 7:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Backdrop_Textures);
						break;
					}
					case 11:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Particle_Textures);
						break;
					}
					case 12:
					{
						editorWindow.texture = ReverseTextureMap(selector.texture, Object_Textures);
						break;
					}
					}
					editorWindow.material = ReverseTextureMap(selector.material, materials);

					// Re-Create Window
					editorWindow.GenEditingScreen(selector.type_major, selector.type_minor);
				}

				// Initialize New Object Window if no Object is Selected
				else
				{
					// Reset Several Variables
					editorWindow.bar1.percent = 0;

					// The Size of the Object Buffer
					GLint bufferSize;

					// Bind Selected Object Buffer
					glBindBuffer(GL_ARRAY_BUFFER, editorWindow.selectedObjectVBO);

					// Get Size of Buffer
					glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

					// Redo Buffer Data and Nullify it
					// There is a Bug that Requires the Object to be Reset at a Value Above its Size to Clear
					glBufferData(GL_ARRAY_BUFFER, bufferSize + 1, NULL, GL_DYNAMIC_DRAW);
					glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);

					// Unbind Buffer
					glBindBuffer(GL_ARRAY_BUFFER, 0);

					// Re-Create Window
					editorWindow.GenEditingScreen(-1, 0);
				}
			}

			// Set Window to Active for Drawing
			editorWindow.active = true;
		}

		// Draw Selector if Active
		if (selector.initialized)
		{
			colorShaderStatic.Use();
			glUniform1i(staticLocColor, 0);
			selector.Blitz(colorShaderStatic, texShaderStatic, editorWindow.active);
		}

		// Draw Editor Window if Active
		if (editorWindow.active)
		{
			// Draw Object
			colorShaderStatic.Use();
			glUniform1i(staticLocColor, 1);
			editorWindow.Blitz(colorShaderStatic, texShaderStatic, selector, window);
		}

		// Draw Editor Options GUI if Active
		if (editor_options.Active)
		{
			// Draw Object
			colorShaderStatic.Use();
			glUniform1i(staticLocColor, 1);
			editor_options.Blitz(window);
			camera_speed_multiplier = editor_options.camera_speed;
			shift_speed_multiplier = editor_options.shift_speed;
		}

		// Draw GUI Texture
		FramebufferShader_Core.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(screenVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_Core);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw FPS
		display_FPS();

		// Unbind Objects
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		// Change Cursor if Necessary
		if (Current_Cursor != Selected_Cursor)
		{
			ChangeCursor(window);
		}

		// Update Window
		glfwSwapBuffers(window);
	}

	if (selector.editing)
	{
		selector.Return(true, true);
	}
}
