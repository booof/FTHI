// Story Game Loop
void Loop_Story(GLFWwindow* window)
{
	// Reset Camera
	camera.accelerationL = 1;
	camera.accelerationR = 1;
	camera.accelerationY = 1;
	camera.constantX = 0;
	camera_speed_multiplier = 1.0f;

	// Generate Collision Mask Lines and Light Indicators
	for (int index = 0; index < 9; index++)
	{
		for (int i = 0; i < levels_list[index].floor_objects_size; i++)
		{
			levels_list[index].floor_objects[i].InitializeLine();
		}

		for (int i = 0; i < levels_list[index].wall_objects_size; i++)
		{
			levels_list[index].wall_objects[i].InitializeLine();
		}

		for (int i = 0; i < levels_list[index].ceiling_objects_size; i++)
		{
			levels_list[index].ceiling_objects[i].InitializeLine();
		}

		for (int i = 0; i < levels_list[index].trigger_objects_size; i++)
		{
			levels_list[index].trigger_objects[i].InitializeLine();
		}
	}

	// The Player 
	Player player(0, 20.0f, -25.0f, Player_Textures.find("egg.jpg")->second);
	camera.Position = player.position;

	enemy = Enemy_Base(camera.Position.x, camera.Position.y + 10.0f, -3.0f);

	// The Actual Loop
	while (looptype == 0 && !glfwWindowShouldClose(window))
	{
		Error_Log();
		//std::cout << "\n";

		// Reset Velocities
		player.velocity = glm::vec2(0.0f, 0.0f);

		// Process Time
		DeltaTime();

		// Handle Inputs
		glfwPollEvents();
		SmoothKeyCallback_Gameplay(camera, player);

		// Frame by Frame
		Frame_By_Frame(window, player, camera);


		// Update Objects

		// Determine if HUD Should Resize
		if (framebufferResize)
		{
			player.UpdateHUDVertices();
			framebufferResize = false;
		}

		// Update Player Position
		player.Update();

		// Update Enemy Positions
		//enemy.Update();

		// Test Player Collisions
		int sign;
		for (int level = 0; level < 9; level++)
		{
			// CollisionMaskFloor
			for (int i = 0; i < levels_list[level].floor_objects_size; i++)
			{
				// Ground Player if Collision Occoured
				if (levels_list[level].floor_objects[i].TestCollisions(player.position.x, player.position.y - player.posYmodifier, SPEED * deltaTime * 1.5f + 1.0f, true))
				{
					player.position.y = levels_list[level].floor_objects[i].returnedValue + player.posYmodifier;
					player.velocity.y = 0;
					player.Grounded = true;

					// Calculate Momentum
					if (player.momentum.y < 0)
					{
						// Bonk if Momentum is High
						if (player.momentum.y < -20)
						{
							std::cout << "Bonked Ground\n";

							// Damage Player
							player.Damage((int)pow(-player.momentum.y - 20, 1.75));
						}

						player.momentum.y = 0;
					}
				}
			}

			// CollisionMaskWall
			for (int i = 0; i < levels_list[level].wall_objects_size; i++)
			{
				// Calculate Sign of Offset
				sign = -1 + 2 * (levels_list[level].wall_objects[i].Type % 2);

				// Stop Player if Collision Occoured
				if (levels_list[level].wall_objects[i].TestCollisions(player.position.x + player.posXmodifier * sign, player.position.y))
				{
					player.position.x = levels_list[level].wall_objects[i].returnedValue - player.posXmodifier * sign;

					// Calculate Momentum

					// Left Collision Test
					if (levels_list[level].wall_objects[i].Type == 0)
					{
						// Test Momentum
						if (player.momentum.x < 0)
						{
							// Bonk if Momentum is High
							if (player.momentum.x < -20)
							{
								std::cout << "Bonked Left\n";

								// Damage Player
								player.Damage((int)pow(-player.momentum.x - 20.4, 1.75));
							}

							Keys[GLFW_KEY_LEFT_CONTROL] = false;
							player.momentum.x = 0;
						}

						// Test Sprint
						else if (Keys[GLFW_KEY_A])
						{
							Keys[GLFW_KEY_LEFT_CONTROL] = false;
						}
					}

					// Right Collision Test
					else if (levels_list[level].wall_objects[i].Type == 1)
					{
						// Test Momentum
						if (player.momentum.x > 0)
						{
							// Bonk if Momentum is High
							if (player.momentum.x > 20)
							{
								std::cout << "Bonked Right\n";

								// Damage Player
								player.Damage((int)pow(player.momentum.x - 20.4, 1.75));
							}

							Keys[GLFW_KEY_LEFT_CONTROL] = false;
							player.momentum.x = 0;
						}

						// Test Sprint
						else if (Keys[GLFW_KEY_D])
						{
							Keys[GLFW_KEY_LEFT_CONTROL] = false;
						}
					}
				}
			}

			// CollisionMaskReverseFloor
			for (int i = 0; i < levels_list[level].ceiling_objects_size; i++)
			{
				// Stop Jump if Collision Occoured
				if (levels_list[level].ceiling_objects[i].TestCollisions(player.position.x, player.position.y + player.posYmodifier))
				{
					player.position.y = levels_list[level].ceiling_objects[i].returnedValue - player.posYmodifier;
					player.jumptimer = 0;

					// Calculate Momentum
					if (player.momentum.y > 0)
					{
						// Bonk if Momentum is High
						if (player.momentum.y > 20)
						{
							std::cout << "Bonked Ceiling\n";

							// Damage Player
							player.Damage((int)pow(player.momentum.y - 20, 1.75));
						}

						player.momentum.y = 0;
					}
				}
			}

			// CollisionMaskTrigger
			for (int i = 0; i < levels_list[level].trigger_objects_size; i++)
			{
				// Test Player Specific Triggers
				if (levels_list[level].trigger_objects[i].Action < 4)
				{
					// Test Collisions
					if (levels_list[level].trigger_objects[i].TestCollisions(player.position.x, player.position.y))
					{
						// Test if Trigger is a Death Trigger
						if (!levels_list[level].trigger_objects[i].Action)
						{
							player.dead();
							levels_list[level].trigger_objects[i].State = true;
							Frequency_Linker(levels_list[level].trigger_objects[i].Frequency, levels_list[level].trigger_objects[i].State, levels_list[level].trigger_objects[i].identifier);
						}

						// Test if Trigger is a Togglable Player Trigger and Toggle if Neccessary
						else if (levels_list[level].trigger_objects[i].Action == 2 && levels_list[level].trigger_objects[i].Inside == false)
						{
							levels_list[level].trigger_objects[i].State = !levels_list[level].trigger_objects[i].State;
							Frequency_Linker(levels_list[level].trigger_objects[i].Frequency, levels_list[level].trigger_objects[i].State, levels_list[level].trigger_objects[i].identifier);
						}

						// Trigger is a One Time Player Trigger or Active Only When Player is Inside
						else if ((levels_list[level].trigger_objects[i].Action == 3 || levels_list[level].trigger_objects[i].Action == 1) && !levels_list[level].trigger_objects[i].State)
						{
							levels_list[level].trigger_objects[i].State = true;
							Frequency_Linker(levels_list[level].trigger_objects[i].Frequency, levels_list[level].trigger_objects[i].State, levels_list[level].trigger_objects[i].identifier);
						}

						// Player is Inside Trigger
						levels_list[level].trigger_objects[i].Inside = true;
					}

					// The Player is Not Inside the Trigger
					else
					{
						levels_list[level].trigger_objects[i].Inside = false;
						if (levels_list[level].trigger_objects[i].Action == 3 && levels_list[level].trigger_objects[i].State)
						{
							levels_list[level].trigger_objects[i].State = false;
							Frequency_Linker(levels_list[level].trigger_objects[i].Frequency, levels_list[level].trigger_objects[i].State, levels_list[level].trigger_objects[i].identifier);
						}
					}
				}
			}
		}

		// Preform Object Collisions
		//for (int level = 0; level < 9; level++)
		//{
		//	for (int i = 0; i < levels_list[level].objects_size; i++)
		//	{
		//		levels_list[level].objects[i]->Update(player);
		//	}
		//}
		//objectHandler();


		// Update Camera
		if (!camera.Stationary)
		{
			// Update Camera when Grouned
			if (player.Grounded)
			{
				// Reset Values
				camera.accelerationY = 0.5f;
				camera.fall = false;

				// Comparison Variables
				float playerTop = player.position.y - 40 * zoom_scale;
				float playerBottom = player.position.y - 20 * zoom_scale;

				// Move Camera Up
				if (camera.Position.y < playerTop)
				{
					camera.Move("NORTH");

					if (camera.Position.y > playerTop)
					{
						camera.Position.y = playerTop;
					}
				}

				// Move Camera Down
				else if (camera.Position.y > playerBottom)
				{
					// Speed Camera if Player is Far Below Camera
					if (player.position.y < camera.Position.y + 5 * zoom_scale)
					{
						camera.accelerationY = 1 + abs(player.position.y - camera.Position.y);
					}

					camera.Move("SOUTH");

					if (camera.Position.y < playerBottom)
					{
						camera.Position.y = playerBottom;
					}
				}
			}

			// Update Camera when Falling
			else if (camera.fall)
			{
				// Calculate Camera Fall Speed
				camera.accelerationY = 4;

				// Move Camera Down
				if (camera.Position.y <= player.position.y - 70 * zoom_scale)
				{
					camera.Position.y = player.position.y - 80 * zoom_scale;
				}

				// Move Camera Up
				else
				{
					camera.Move("SOUTH");
				}

				// Exit Fall Mode when Momentum is Positive
				if (player.momentum.y > 0)
				{
					camera.fall = false;
				}
			}

			// Test if Camera Should Fall
			else if (player.applyGravity <= 0 && player.position.y < camera.Position.y + 5.0f)
			{
				camera.accelerationY = player.momentum.y / 2;
				camera.fall = true;
			}

			// Y Momentum
			else if (player.speed_multiplier == 1)
			{
				// Going Up
				if (player.momentum.y > 5)
				{
					// Move Camera Up
					camera.accelerationY = 0.1f;
					camera.Move("SOUTH");

					// Prevent Camera Y from Going Past Player Y
					if (camera.Position.y < player.position.y - 50 * zoom_scale)
					{
						camera.Position.y = player.position.y - 50 * zoom_scale;
					}
				}

				// Going Down
				if (player.momentum.y < -5)
				{
					camera.fall = true;
					camera.accelerationY = player.momentum.y / 2;
					player.applyGravity = 0;
				}
			}

			// X Momentum
			if (abs(player.momentum.x) > 5 && player.speed_multiplier == 1)
			{
				// Moving to the Right
				if (player.momentum.x > 5)
				{
					// Move Camera Right
					camera.accelerationR = player.momentum.x / 2;
					camera.Move("EAST");

					// Prevent Camera X from Going Greater Than Player X
					if (camera.Position.x > player.position.x)
					{
						camera.Position.x = player.position.x;
					}

					camera.accelerationR = 0;
				}

				// Moving to the Left
				else if (player.momentum.x < -5)
				{
					// Move Camera Left
					camera.accelerationL = -player.momentum.x / 2;
					camera.Move("WEST");

					// Prevent Camera X from Going Less Than Player X
					if (camera.Position.x < player.position.x)
					{
						camera.Position.x = player.position.x;
					}

					camera.accelerationL = 0;
				}
			}

			// Stabilize Camera after Calculations
			if (camera.constantX) { camera.Position.x = player.position.x + camera.constantX; }
			camera.updatePosition();
		}

		// Bind Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Multisample);
		glEnable(GL_DEPTH_TEST);

		// Clear Window
		glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glScissor(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

		// Perform Object Collisions
		temp_collision_angle1 = 0;
		for (int i = 0; i < physics_count; i++)
		{
			physics[i]->Update(player);
		}
		objectCollisionDetection();

		// Draw Static Line Colored Objects
		colorShaderStaticLine.Use();

		// Collision Mask
		for (int level = 0; level < 9; level++)
		{
			// Floor
			for (int i = 0; i < levels_list[level].floor_objects_size; i++)
			{
				levels_list[level].floor_objects[i].DrawLine();
			}

			// Wall
			for (int i = 0; i < levels_list[level].wall_objects_size; i++)
			{
				levels_list[level].wall_objects[i].DrawLine();
			}

			// Reverse Floor
			for (int i = 0; i < levels_list[level].ceiling_objects_size; i++)
			{
				levels_list[level].ceiling_objects[i].DrawLine();
			}
		}

		// Draw Colored Objects
		colorShader.Use();
		glUniform4f(glGetUniformLocation(colorShader.Program, "material.viewPos"), camera.Position.x, camera.Position.y, 0.0f, 0.0f);

		// Draw Colored Terrain
		for (int level = 0; level < 9; level++)
		{
			// Backdrop
			for (int i = 0; i < levels_list[level].backdrop_objects_texture_start; i++)
			{
				levels_list[level].backdrop_objects[i].Draw_Color();
			}

			// Background
			for (int i = 0; i < levels_list[level].background_objects_texture_start; i++)
			{
				levels_list[level].background_objects[i].Draw_Color();
			}

			// Foreground
			for (int i = 0; i < levels_list[level].foreground_objects_texture_start; i++)
			{
				levels_list[level].foreground_objects[i].Draw_Color();
			}

			// Formerground
			for (int i = 0; i < levels_list[level].formerground_objects_texture_start; i++)
			{
				levels_list[level].formerground_objects[i].Draw_Color();
			}

			// Objects
			for (int i = 0; i < levels_list[level].objects_texture_start; i++)
			{
				levels_list[level].objects[i]->Draw_Color();
			}
		}

		// Draw Textured Objects
		texShader.Use();
		glUniform4f(glGetUniformLocation(texShader.Program, "material.viewPos"), camera.Position.x, camera.Position.y, 0.0f, 0.0f);

		// Draw Textured Terrain
		for (int level = 0; level < 9; level++)
		{
			// Backdrop
			for (int i = levels_list[level].backdrop_objects_texture_start; i < levels_list[level].backdrop_objects_size; i++)
			{
				levels_list[level].backdrop_objects[i].Draw_Texture();
			}

			// Background
			for (int i = levels_list[level].background_objects_texture_start; i < levels_list[level].background_objects_size; i++)
			{
				levels_list[level].background_objects[i].Draw_Texture();
			}

			// Foreground
			for (int i = levels_list[level].foreground_objects_texture_start; i < levels_list[level].foreground_objects_size; i++)
			{
				levels_list[level].foreground_objects[i].Draw_Texture();
			}

			// Formerground
			for (int i = levels_list[level].formerground_objects_texture_start; i < levels_list[level].formerground_objects_size; i++)
			{
				levels_list[level].formerground_objects[i].Draw_Texture();
			}

			// Objects
			for (int i = levels_list[level].objects_texture_start; i < levels_list[level].objects_size; i++)
			{
				levels_list[level].objects[i]->Draw_Texture();
			}
		}

		// Static Textures
		texShaderStatic.Use();
		glUniform1i(staticLocTexture, 0);

		// Draw Player
		player.Blitz();

		// Draw Enemies
		//enemy.Blitz();

		// Draw Particles
		particleHandler();
		for (int level = 0; level < 9; level++)
		{
			for (int i = 0; i < levels_list[level].particle_objects_size; i++)
			{
				levels_list[level].particle_objects[i]->Update();
				levels_list[level].particle_objects[i]->Draw_Particles();
			}
		}

		// Blitz Multisampled Framebuffer to Framebuffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, Frame_Buffer_Object_Multisample);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Frame_Buffer_Object_HDR);
		glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// Bind Core Framebuffer
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

		// Draw the Heads Up Display
		texShaderStatic.Use();
		glUniform1i(staticLocTexture, 1);
		player.BlitzHUD();

		// Draw FPS
		display_FPS();

		// Apply Bloom
		Bloom(Frame_Buffer_Texture_Bloom, 6);

		// Draw Core Texture
		FramebufferShader_Core.Use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(screenVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_Core);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Unbind Objects
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		// Update Window
		glfwSwapBuffers(window);
	}
}
