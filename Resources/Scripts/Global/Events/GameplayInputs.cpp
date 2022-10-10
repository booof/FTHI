#include "ExternalLibs.h"
#include "Globals.h"
#include "Class/Object/Entity/Controllables.h"
#include "Class/Render/Camera/Camera.h"
#include "../../Resources/Scripts/Object/Entity/Player.h"

// Pointer to The Player That is to be Updated
Player* player_;

// Pointer to the Player's State
Object::Entity::EntityState* state;

// Pointer to Camera That is to be Updated
Render::Camera::Camera* camera_pointer;

// If True, Exit Function
bool quit_function = true;

// Handles Inputs That Update Every Frame for Story Loop
void SmoothKeyCallback_Gameplay()
{
	//std::cout << "no\n";

	// If True, Exit Function
	if (quit_function)
		return;

	Render::Camera::Camera& camera = *Global::camera_pointer;
	Player& player = *player_;

	// Reset Player Velocity
	state->velocity = glm::vec2(0.0f, 0.0f);

	//std::cout << "gey\n";

	float camera_offset = 45 * Global::zoom_scale;

	// Move Side to Side if Momentum is Zero
	if (player.applyGravity != 0 || state->grounded)
	{
		// Move Player RIGHT if D is Held
		if (Global::Keys[GLFW_KEY_D])
		{
			player.Move("RIGHT", *state);

			// Test if Camera Should Move
			if (!camera.Stationary)
			{
				camera.accelerationL = 1;

				// Camera is on Player
				if (camera.Position.x - player.position_pointer->x >= camera_offset - 1.0f)
				{
					camera.accelerationR = 1;
					camera.constantX = camera_offset;
				}

				// Camera is Behind Player
				else
				{
					camera.constantX = 0;
					if (camera.accelerationR < 1.5) { camera.accelerationR += 0.02f; }
					camera.moveCamera(Render::Camera::Directions::EAST);
				}
			}
		}

		// Move Player LEFT if A is Held
		if (Global::Keys[GLFW_KEY_A])
		{
			player.Move("LEFT", *state);

			// Test if Camera Should Move
			if (!camera.Stationary)
			{
				camera.accelerationR = 1;

				// Camera is on Player
				if (player.position_pointer->x - camera.Position.x >= camera_offset - 1.0f)
				{
					camera.accelerationL = 1;
					camera.constantX = -camera_offset;
				}

				// Camera is Behind Player
				else
				{
					camera.constantX = 0;
					if (camera.accelerationL < 1.5) { camera.accelerationL += 0.02f; }
					camera.moveCamera(Render::Camera::Directions::WEST);
				}
			}
		}
	}

	// Begin Jump if Space is Held
	if (Global::Keys[GLFW_KEY_SPACE])
	{
		if (player.jumptimer == 0 && state->grounded)
		{
			player.jumptimer = 0.3;
			player.applyGravity = 1;

			// Increase Momentum if Sprinting
			if (player.speed_multiplier > 1)
			{
				if (Global::Keys[GLFW_KEY_A])
				{
					state->momentum.x = -22.5f;
				}

				else if (Global::Keys[GLFW_KEY_D])
				{
					state->momentum.x = 22.5f;
				}
			}
		}
	}

	// Reload if R is Pressed
	if (Global::Keys[GLFW_KEY_R])
	{
		player.reload = true;
	}

	// Sprint if CRTL is Held
	player.run = false;
	player.speed_multiplier = 1;
	if (Global::Keys[GLFW_KEY_LEFT_CONTROL])
	{
		player.run = true;
		player.speed_multiplier = 1.5;
	}

	// Swap Weapons
	if (Global::Keys[GLFW_KEY_1]) { player.SwapWeapons(1); Global::Keys[GLFW_KEY_1] = false; }
	if (Global::Keys[GLFW_KEY_2]) { player.SwapWeapons(2); Global::Keys[GLFW_KEY_2] = false; }
	if (Global::Keys[GLFW_KEY_3]) { player.SwapWeapons(3); Global::Keys[GLFW_KEY_3] = false; }

	if (Global::Keys[GLFW_KEY_ENTER]) { std::cout << "entered\n"; }

	// Update Camera

	if (!camera.Stationary)
	{
		// Update Camera when Grouned
		if (state->grounded)
		{
			// Reset Values
			camera.accelerationY = 0.5f;
			camera.fall = false;

			// Comparison Variables
			float playerTop = player.position_pointer->y + 20 * Global::zoom_scale;
			float playerBottom = player.position_pointer->y + 40 * Global::zoom_scale;

			// Move Camera Up
			if (camera.Position.y < playerTop)
			{
				camera.moveCamera(Render::Camera::NORTH);
				camera.moveCamera(Render::Camera::NORTH);

				if (camera.Position.y > playerTop)
				{
					camera.Position.y = playerTop;
				}
			}

			// Move Camera Down
			else if (camera.Position.y > playerBottom)
			{
				// Speed Camera if Player is Far Below Camera
				if (player.position_pointer->y < camera.Position.y + 5 * Global::zoom_scale)
				{
					camera.accelerationY = 1 + abs(player.position_pointer->y - camera.Position.y);
				}

				camera.moveCamera(Render::Camera::Directions::SOUTH);

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
			if (camera.Position.y <= player.position_pointer->y - 70 * Global::zoom_scale)
			{
				camera.Position.y = player.position_pointer->y - 80 * Global::zoom_scale;
			}

			// Move Camera Up
			else
			{
				camera.moveCamera(Render::Camera::Directions::SOUTH);
			}

			// Exit Fall Mode when Momentum is Positive
			if (state->momentum.y > 0)
			{
				camera.fall = false;
			}
		}

		// Test if Camera Should Fall
		else if (player.applyGravity <= 0 && player.position_pointer->y < camera.Position.y + 5.0f)
		{
			camera.accelerationY = state->momentum.y * 0.5f;
			camera.fall = true;
		}

		// Y Momentum
		else if (player.speed_multiplier == 1)
		{
			// Going Up
			if (state->momentum.y > 5)
			{
				// Move Camera Up
				camera.accelerationY = 0.1f;
				camera.moveCamera(Render::Camera::Directions::SOUTH);

				// Prevent Camera Y from Going Past Player Y
				if (camera.Position.y < player.position_pointer->y - 50 * Global::zoom_scale)
				{
					camera.Position.y = player.position_pointer->y - 50 * Global::zoom_scale;
				}
			}

			// Going Down
			if (state->momentum.y < -5)
			{
				camera.fall = true;
				camera.accelerationY = state->momentum.y * 0.5f;
				player.applyGravity = 0;
			}
		}

		// X Momentum
		if (abs(state->momentum.x) > 5 && player.speed_multiplier == 1)
		{
			// Moving to the Right
			if (state->momentum.x > 5)
			{
				// Move Camera Right
				camera.accelerationR = state->momentum.x * 0.5f;
				camera.moveCamera(Render::Camera::Directions::EAST);

				// Prevent Camera X from Going Greater Than Player X
				if (camera.Position.x > player.position_pointer->x)
				{
					camera.Position.x = player.position_pointer->x;
				}

				camera.accelerationR = 0;
			}

			// Moving to the Left
			else if (state->momentum.x < -5)
			{
				// Move Camera Left
				camera.accelerationL = -state->momentum.x * 0.5f;
				camera.moveCamera(Render::Camera::Directions::WEST);

				// Prevent Camera X from Going Less Than Player X
				if (camera.Position.x < player.position_pointer->x)
				{
					camera.Position.x = player.position_pointer->x;
				}

				camera.accelerationL = 0;
			}
		}

		// Stabilize Camera after Calculations
		if (camera.constantX) { camera.Position.x = player.position_pointer->x + camera.constantX; }
		camera.updatePosition();
	}
}

// Enable Gameplay Inputs
void enableGamplayInputs(Player* player_pointer, Object::Entity::EntityState* state_)
{
	// Store Pointer to Player
	player_ = player_pointer;

	// Store Pointer to State
	state = state_;

	// Store Pointer to Camera
	camera_pointer = Global::camera_pointer;

	// Enable Global Inputs
	quit_function = false;
}