#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constant
{
	// Physics Constants
	const float SPEED = 15.0f; // Universal Speed Constant
	const float TERMINALVELOCITY = 50.0f; // Terminal Velocity
	const float GRAVITATIONAL_ACCELERATION = 16.5f; // Gravitational Acceleration for Calculating Force of Weight
	const float TORQUE_CONSTANT = 7.0f; // Multiplier to torque applied to objects
	const float POTENTIAL_ENERGY_CONSTANT = 3.0f; // Multiplier to torque applied by potential energy
	const float INELASTIC = 50; // Inelastic Constant
	const float OVERPOWER_CONSTANT = 25.0f; // Force Required to Overcome Object

	// Editor Constants
	const float FOREGROUNDZ = -99.9f; // Z Position of Foreground Terrain Objects 
	const float CLAMP_CONSTANT = 0.8f; // How Close Objects Must Be to Clamp
	const float ROTATION_CONSTANT = 0.25f; // How Fast Selector Rotations Are

	// Material Array Constants
	const int MATERIALS_COUNT = 25;
	const int MATERIALS_PERMUTATIONS = 351; // (MATERIALS_COUNT * (MATERIALS_COUNT + 1)) / 2

	// Size of an Object Instance
	const unsigned int INSTANCE_SIZE = 112;
}

#endif
