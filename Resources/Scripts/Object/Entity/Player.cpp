#include "Player.h"
#include "Class/Object/Entity/Controllables.h"
#include "Class/Render/Camera/Camera.h"
#include "Globals.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Class/Render/Shader/Shader.h"

// Enable Gameplay Inputs Declaration
void enableGamplayInputs(Player* player_pointer, Object::Entity::EntityState* state_);

// Initialize Player Function
void initialize_player(Object::Object* object_pointer)
{
	Object::Entity::Controllables& object = *static_cast<Object::Entity::Controllables*>(object_pointer);

	// Generate The Base Player Object
	object.external_data = new Player(object);
	
	// Store Pointer to Player in Gameplay Inputs
	enableGamplayInputs(static_cast<Player*>(object.external_data), &object.state);
}

// Update Player Function
void update_player(Object::Object* object_pointer)
{
	Object::Entity::Controllables& object = *static_cast<Object::Entity::Controllables*>(object_pointer);

	// Update Player Data
	Player& player = *static_cast<Player*>(object.external_data);
	//static_cast<Player*>(object.external_data)->Update();
	player.Update(object.state);

	//if (player.position_pointer->y < 0)
	//{
	//	player.position_pointer->y = 0;
	//	player.velocity.y = 0;
	//	player.Grounded = true;

	//	// Calculate Momentum
	//	if (player.momentum.y < 0)
	//	{
	//		// Bonk if Momentum is High
	//		if (player.momentum.y < -20)
	//		{
	//			std::cout << "Bonked Ground\n";

	//			// Damage Player
	//			player.Damage((int)pow(-player.momentum.y - 20, 1.75));
	//		}

	//		player.momentum.y = 0;
	//	}
	//}



	// Update Model Matrix


	
	//Global::texShaderStatic.Use();
	//static_cast<Player*>(object.external_data)->Blitz();
}

// Delete Player Function
void delete_player(Object::Object* object_pointer)
{

}

Player::Player(Object::Entity::Controllables& controllable)
{
	// Store Pointer to Position
	position_pointer = &controllable.state.position;

	//stats.Force = 35.0f;

	//position = glm::vec3(initialX, initialY, initialZ);

	posXmodifier = 1;
	posYmodifier = 1;

	oldY = position_pointer->y;
	controllable.state.grounded = false;

	controllable.state.direction = 1;

	// Collision Stuff
	vertices[0] = glm::vec2(posXmodifier, posYmodifier);
	vertices[1] = glm::vec2(-posXmodifier, posYmodifier);
	vertices[2] = -vertices[0];
	vertices[3] = -vertices[1];
	//angle_offsets[0] = Named_Node(0.7854f, 2);
	//angle_offsets[1] = Named_Node(2.3562f, 3);
	//angle_offsets[2] = Named_Node(3.927f, 0);
	//angle_offsets[3] = Named_Node(5.4978f, 1);


	// HUD Vertices


	// Initialize Vertex Objects
	glGenVertexArrays(1, &HUDVAO);
	glGenBuffers(1, &HUDVBO);

	// Bind Vertex Objects
	glBindVertexArray(HUDVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HUDVBO);

	// Reserve Buffer Data
	glBufferData(GL_ARRAY_BUFFER, 180 * sizeof(GL_FLOAT), NULL, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Vertices
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Health Vertices


	// Initialize Vertex Objects
	glGenVertexArrays(1, &HealthVAO);
	glGenBuffers(1, &HealthVBO);

	// Bind Vertex Objects
	glBindVertexArray(HealthVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HealthVBO);

	// Bind Vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Stamina Vertices


	// Initialize Vertex Objects
	glGenVertexArrays(1, &StaminaVAO);
	glGenBuffers(1, &StaminaVBO);

	// Bind Vertex Objects
	glBindVertexArray(StaminaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, StaminaVBO);

	// Bind Vertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate HUD Vertices
	UpdateHUDVertices();

	// Initialize Weapons
	//PrimaryWeapon = new Glock(0, -25.0f);
	//SecondaryWeapon = new Enhanced_Glock(0, -25.0f);
	//SecondaryWeapon = new Shotgun(0, -25.0f);
	//TrinaryWeapon = new The_One(0, -25.0f);
	CurrentWeapon = PrimaryWeapon;
}

void Player::BlitzHUD()
{
	// Calculate Model Matrix
	glm::mat4 model = glm::mat4(1.0f);

	// Send Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Send Direction to Shaders
	glUniform1i(Global::directionLoc, 1);

	//// Enable Texture
	//glActiveTexture(HUD_Textures.find("Health.png")->second.index + 3);
	//glUniform1i(Global::textureLayerLocTextureStatic, 0);

	//// Bind Vertex Object
	//glBindVertexArray(HUDVAO);

	//// Draw Health Object
	//glBindTexture(HUD_Textures.find("Health.png")->second.type, HUD_Textures.find("Health.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	//// Draw Stamina Object
	//glBindTexture(HUD_Textures.find("Stamina.png")->second.type, HUD_Textures.find("Stamina.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 6, 6);

	//// Draw Equipment Object
	//glBindTexture(HUD_Textures.find("Equipment.png")->second.type, HUD_Textures.find("Equipment.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 12, 6);

	//// Draw Clip Object
	//glBindTexture(HUD_Textures.find("Clip.png")->second.type, HUD_Textures.find("Clip.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 18, 6);

	//// Draw Ammo Object
	//glBindTexture(HUD_Textures.find("Ammo.png")->second.type, HUD_Textures.find("Ammo.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 24, 6);

	//// Draw MinimapBackground Object
	//glBindTexture(HUD_Textures.find("MinimapBackground.png")->second.type, HUD_Textures.find("MinimapBackground.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 30, 6);

	//// Draw HealthBar Object
	//glBindVertexArray(HealthVAO);
	//glBindTexture(HUD_Textures.find("HealthBar.png")->second.type, HUD_Textures.find("HealthBar.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	//// Draw StaminaBar Object
	//glBindVertexArray(StaminaVAO);
	//glBindTexture(HUD_Textures.find("StaminaBar.png")->second.type, HUD_Textures.find("StaminaBar.png")->second.texture);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	//// Unbind Vertex Objects
	//glBindVertexArray(0);

	//// Disable Texture
	//glBindTexture(HUD_Textures.find("StaminaBar.png")->second.type, 0);

	// Display Text
	//fontShader.Use();
	//RenderText((std::to_string(CurrentWeapon->displayClip) + " / " + std::to_string(CurrentWeapon->displayClipMax)), TextX, ClipY, 0.15f * Scale, glm::vec3(1.0f, 1.0f, 1.0f), true);
	//RenderText((std::to_string(CurrentWeapon->displayAmmo) + " / " + std::to_string(CurrentWeapon->displayAmmoMax)), TextX, AmmoY, 0.15f * Scale, glm::vec3(1.0f, 1.0f, 1.0f), true);
}

void Player::Update(Object::Entity::EntityState & state)
{
	// Calculate Jumps
	if (jumptimer > 0) { Jump(state); }
	else { Fall(state); }

	// Calculate Player Direction
	if (Global::mouseRelativeX < position_pointer->x) { state.direction = -1; }
	else { state.direction = 1; }

	// Run if CTRL is Held
	if (run && moving)
	{
		// Change to only drain when player is moving
		DrainStamina(-1);
		moving = false;
	}

	// Regain Stamina if no Longer Running
	else if (Stamina < MaxStamina)
	{
		DrainStamina(1);
	}

	// Apply Forces
	forces /= weight;
	*position_pointer += forces;
	forces = glm::vec2(0.0f, 0.0f);

	// Update Position Through Momentum
	*position_pointer += state.momentum * Global::deltaTime;
	//position.x += momentum.x * deltaTime;
	//position.y += momentum.y * deltaTime;
	//position += glm::vec3(player_velocity_offset, 0.0f);
	//std::cout << position.x << " " << position.y << " " << player_velocity_offset.x << " " << player_velocity_offset.y << "\n";
	//player_velocity_offset = glm::vec2(0.0f);

	// Apply X Friction
	if (state.momentum.x != 0)
	{
		float multiplier;

		// Apply Ground Friction
		if (state.grounded)
		{
			multiplier = 50;
		}

		// Apply Air Friction
		else
		{
			multiplier = 5;
		}

		// Calculate New X Momentum
		int sign = (int)(abs(state.momentum.x) / state.momentum.x);
		state.momentum.x = sign * (abs(state.momentum.x) - multiplier * Global::deltaTime);

		// If Momentum Switches Signs, Set it to Zero
		if (sign != (int)(abs(state.momentum.x) / state.momentum.x))
		{
			state.momentum.x = 0;
		}
	}

	// Apply Gravity
	if (!state.grounded && !applyGravity && state.momentum.y > -Constant::TERMINALVELOCITY)
	{
		// Signifigantly Slow Down Player if Momentum is Positive
		if (state.momentum.y > 0)
		{
			state.momentum.y -= 50 * Global::deltaTime;
		}

		// Apply Normnal Gravity if Momentum is Negative
		else
		{
			state.momentum.y -= 30 * Global::deltaTime;
		}

		// Stop Gravity if Terminal Velicity Has Been Reached
		if (state.momentum.y <= -Constant::TERMINALVELOCITY)
		{
			state.momentum.y = -Constant::TERMINALVELOCITY;
		}
	}

	// Disable Grounding
	state.grounded = false;

	// Finalize Velocity
	state.velocity += state.momentum * Global::deltaTime;
}

void Player::Move(std::string Direction, Object::Entity::EntityState& state)
{
	if (!(speed_multiplier > 1 && !state.grounded))
	//if (true)
	{
		// Calculate Movements
		if (Direction == "RIGHT") { state.velocity.x += Constant::SPEED * Global::deltaTime * speed_multiplier; state.direction = -1; }
		if (Direction == "LEFT") { state.velocity.x -= Constant::SPEED * Global::deltaTime * speed_multiplier; state.direction = 1; }
		//position.x += velocity.x;
		position_pointer->x += state.velocity.x;
		//player_direction = -direction;
	}

	// Set Moving to True
	moving = true;
}

void Player::Jump(Object::Entity::EntityState& state)
{
	jumptimer -= Global::deltaTime;
	state.velocity.y += .5f * Constant::SPEED * Global::deltaTime;
	//position.y += velocity.y;
	position_pointer->y += state.velocity.y;
}

void Player::Fall(Object::Entity::EntityState& state)
{
	jumptimer = 0;
	state.velocity.y -= .75f * Constant::SPEED * Global::deltaTime;
	//position.y += velocity.y;
	position_pointer->y += state.velocity.y;

	// Test if Player Should Bend To The Laws of Gravity
	if (applyGravity > 0)
	{
		applyGravity -= Global::deltaTime;

		// Set Gravity To True If Needed
		if (applyGravity <= 0)
		{
			applyGravity = 0;
		}
	}

	// Test if Player Left the World
	if (position_pointer->y < -100)
	{
		dead(state);
	}
}

void Player::Damage(int damage)
{
	// Change Health Through Damage
	Health -= damage;

	// Prevent Health From Going Past MaxHealth
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}

	// Kill Player if Health is Gone
	else if (Health <= 0)
	{
		//dead(state);
	}

	// Bind Health Vertex Object
	glBindVertexArray(HealthVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HealthVBO);

	// Calculate Vertices
	float vertices[30];
	float Scale = BaseBarSize * ((float)Health / (float)MaxHealth);
	Vertices::Rectangle::genRectTexture(BarX + Scale / 2, HealthY, -0.1f, Scale, BarSizeY, vertices);

	// Bind Buffer Sub Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Health Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Player::DrainStamina(int change)
{
	// Change Stamina Value
	Stamina += change;

	// Prevent Stamina From Going Past MaxStamina
	if (Stamina > MaxStamina)
	{
		Stamina = MaxStamina;
	}

	// Stop Running if Stamina is Gone
	else if (Stamina <= 0)
	{
		run = false;
		Global::Keys[GLFW_KEY_LEFT_CONTROL] = 0;
		speed_multiplier = 1.0f;
	}

	// Bind Stamina Vertex Object
	glBindVertexArray(StaminaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, StaminaVBO);

	// Calculate Vertices
	float vertices[30];
	float Scale = BaseBarSize * ((float)Stamina / (float)MaxStamina);
	Vertices::Rectangle::genRectTexture(BarX + Scale / 2, StaminaY, -0.1f, Scale, BarSizeY, vertices);

	// Bind Buffer Sub Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Stamina Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Player::UpdateHUDVertices()
{
	// Calculate New Vertices
	Scale = pow(((float)Global::screenWidth / 1280.0f), 0.2f);

	// Bar Background Values
	float BarBackgroundSizeX = 50 * Scale;
	float BarBackgroundX = -Global::halfScalarX + BarBackgroundSizeX / 1.8f;
	float BarBackgroundSizeY = 6 * Scale;

	// Ammo Values
	float EquipmentSizeX = 10.0f * Scale;
	float AmmoSizeX = 30 * Scale;
	float AmmoX = -Global::halfScalarX + AmmoSizeX - 5 * Scale;
	AmmoY = BarBackgroundSizeY - Scale * 2;
	ClipY = 2.3f * BarBackgroundSizeY - Scale * 2;
	TextX = AmmoX;

	// Bar Values
	BarX = BarBackgroundX - BarBackgroundSizeX / 3.15f;
	BaseBarSize = ((float)BarBackgroundSizeX * (4.0f / 5.0f));
	BarSizeY = 5 * Scale;
	HealthY = 100 - BarBackgroundSizeY * 0.8f;
	StaminaY = 100 - 2 * BarBackgroundSizeY;

	// Bind Object
	glBindVertexArray(HUDVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HUDVBO);

	// Health Vertices
	float vertices[30];
	Vertices::Rectangle::genRectTexture(BarBackgroundX, HealthY, -1.0f, BarBackgroundSizeX, BarBackgroundSizeY, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Stamina Vertices
	Vertices::Rectangle::genRectTexture(BarBackgroundX, StaminaY, -1.0f, BarBackgroundSizeX, BarBackgroundSizeY, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 30 * sizeof(GL_FLOAT), sizeof(vertices), vertices);

	// Equipment Vertices
	Vertices::Rectangle::genRectTexture(-Global::halfScalarX + EquipmentSizeX / 1.5f, 12 * Scale, -1.0f, EquipmentSizeX, 2 * EquipmentSizeX, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 60 * sizeof(GL_FLOAT), sizeof(vertices), vertices);

	// Clip Vertices
	Vertices::Rectangle::genRectTexture(AmmoX + 5 * Scale, ClipY + Scale * 1.5f, -1.0f, AmmoSizeX, BarBackgroundSizeY, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 90 * sizeof(GL_FLOAT), sizeof(vertices), vertices);

	// Ammo Vertices
	Vertices::Rectangle::genRectTexture(AmmoX + 5 * Scale, AmmoY + Scale * 1.5f, -1.0f, AmmoSizeX, BarBackgroundSizeY, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 120 * sizeof(GL_FLOAT), sizeof(vertices), vertices);

	// MinimapBackground Vertices
	Vertices::Rectangle::genRectTexture(Global::halfScalarX - Scale * 20, 100 - 15 * Scale, -1.0f, 40.0f * Scale, 30.0f * Scale, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 150 * sizeof(GL_FLOAT), sizeof(vertices), vertices);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Health Object

	// Bind Health Vertex Object
	glBindVertexArray(HealthVAO);
	glBindBuffer(GL_ARRAY_BUFFER, HealthVBO);

	// Calculate Vertices
	float HealthScale = BaseBarSize * ((float)Health / (float)MaxHealth);
	Vertices::Rectangle::genRectTexture(BarX + HealthScale / 2, HealthY, -0.1f, HealthScale, BarSizeY, vertices);

	// Bind Buffer Sub Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Health Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Stamina Object

	// Bind Stamina Vertex Object
	glBindVertexArray(StaminaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, StaminaVBO);

	// Calculate Vertices
	float StaminaScale = BaseBarSize * ((float)Stamina / (float)MaxStamina);
	Vertices::Rectangle::genRectTexture(BarX + StaminaScale / 2, StaminaY, -0.1f, StaminaScale, BarSizeY, vertices);

	// Bind Buffer Sub Data
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Stamina Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Player::SwapWeapons(unsigned short int index)
{
	// Execute only if Index != Current Weapon
	if (index != weaponIndex)
	{
		// Assign New Weapon to Current Weapon
		if (index == 1) { CurrentWeapon = PrimaryWeapon; }
		else if (index == 2) { CurrentWeapon = SecondaryWeapon; }
		else { CurrentWeapon = TrinaryWeapon; }

		// Update Weapon Index
		weaponIndex = index;
	}
}

void Player::dead(Object::Entity::EntityState& state)
{
	Health = MaxHealth;
	*position_pointer = glm::vec2(0.0f, 20.0f);
	state.momentum = glm::vec2(0.0f, 0.0f);
	Global::camera_pointer->Position = glm::vec3(0.0f, 20.0f, 0.0f);
	//glm::vec2 new_level;
	//UpdateLevel(glm::vec2(position.x, position.y), new_level);
	//levelHandler(camera.level, new_level, false);
}
