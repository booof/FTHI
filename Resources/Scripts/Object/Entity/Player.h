#ifndef PLAYER_H
#define PLAYER_H

#include "Class/Object/Entity/Controllables.h"

class WeaponBase;

// The Base Player Class
class Player
{
public:

	// Pointer Position of Player
	glm::vec2* position_pointer = nullptr;

	// Coordinates Used for Collisions (Half Width and Half Height)
	float posXmodifier, posYmodifier;

	// Variables Used for Jumping
	double jumptimer = 0;
	double applyGravity = 0.0;

	// Forces
	glm::vec2 forces = glm::vec2(0.0f, 0.0f);

	// Health of Player
	int Health = 100;
	int MaxHealth = 100;

	// Stamina of Player
	int Stamina = 200;
	int MaxStamina = 300;

	// Speed Multiplier Used for Sprinting
	float speed_multiplier = 1;
	bool run = false;
	bool moving = false;

	// Test if Weapon Should Reload
	bool reload = false;

	// Loaded Weapons
	WeaponBase* PrimaryWeapon;
	WeaponBase* SecondaryWeapon;
	WeaponBase* TrinaryWeapon;

	// Currently Selected Weapon
	WeaponBase* CurrentWeapon;
	short unsigned int weaponIndex = 1;

	// Weight of Player
	float weight = 5.0f;

	//  Strength of Player
	float strength = 10.0f;
	float half_strength = 5.0f;

	// Collision Stuff
	glm::vec2 vertices[4];
	//Named_Node angle_offsets[4];
	short angle_to_vertex_left[4] = { 1, 2, 3, 0 };
	short angle_to_vertex_right[4] = { 2, 3, 0, 1 };

	// Initialize Player
	Player(Object::Entity::Controllables& controllable);

	// Draw the Heads Up Display
	void BlitzHUD();

	// Update Player
	void Update(Object::Entity::EntityState & state);

	// Handles Horizontal Movement from Inputs
	void Move(std::string Direction, Object::Entity::EntityState& state);

	// Handles Jumping Animations
	void Jump(Object::Entity::EntityState& state);

	// Allows Player to Fall
	void Fall(Object::Entity::EntityState& state);

	// Updates Player Health When Taking Damage
	void Damage(int damage);

	// Updates Player Stamina When Stamina is Used
	void DrainStamina(int change);

	// Update HUD Vertices
	void UpdateHUDVertices();

	// Swap Equiped Weapons
	void SwapWeapons(unsigned short int index);

	// Kill The Player
	void dead(Object::Entity::EntityState& state);

private:

	// Private Player Variables

	// Player Jump Variables
	float oldY;

	// Vertex Buffer Objects
	GLuint HUDVAO, HUDVBO;
	GLuint HealthVAO, HealthVBO;
	GLuint StaminaVAO, StaminaVBO;

	// HUD Values
	std::string Clip = "";
	std::string Ammo = "";
	float Scale = 0.0f;
	float BarX = 0.0f;
	float BaseBarSize = 0.0f;
	float BarSizeY = 0.0f;
	float HealthY = 0.0f;
	float StaminaY = 0.0f;
	float ClipY = 0.0f;
	float AmmoY = 0.0f;
	float TextX = 0.0f;
};

#endif
