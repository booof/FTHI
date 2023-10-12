#include "Camera.h"

#ifdef DLL_HEADER
namespace Global
{
	extern float* deltaTime;
}
extern "C" {
#endif

#ifndef DLL_HEADER

// Globals
#include "Globals.h"
#include "Class/Render/Shader/Shader.h"
#include "Class\Render\Editor\Debugger.h"

Render::Camera::Camera::Camera(float initialX, float initialY, bool stationary)
{
	Position = glm::vec3(initialX, initialY, 0.0f);
	Stationary = stationary;
	zoomScale = 1;
	accelerationL = 1;
	accelerationR = 1;
	accelerationY = 1;
	constantX = 0;
	fall = false;
	view = glm::translate(glm::mat4(1.0f), glm::vec3(-initialX, -initialY, 0.0));

	Global::objectShader.Use();
	glUniform4f(glGetUniformLocation(Global::objectShader.Program, "material.viewPos"), -Position.x, -Position.y, 0.0f, 0.0f);

	Global::camera_pointer = this;
}

#endif

void Render::Camera::Camera::moveCamera(unsigned char direction)
{

#ifndef DLL_HEADER

	// The Distance the Camera Moves
	float distance = Constant::SPEED * Global::deltaTime * Global::camera_speed_multiplier;
	//distance = 0;

#else

	float distance = 15.0f * *Global::deltaTime;

#endif

	// Process Movements
	if (direction == NORTH) { Position.y += distance * accelerationY; }
	if (direction == SOUTH) { Position.y -= distance * accelerationY; }
	if (direction == EAST)  { Position.x += distance * accelerationR; }
	if (direction == WEST)  { Position.x -= distance * accelerationL; }

	//updatePosition();

	// State that the Camera Moved
	moved = true;
}

#ifndef DLL_HEADER

void Render::Camera::Camera::updatePosition()
{
	// This Should Only be Executed Once Per Frame At the Start of Rendering

	// TODO: If Wrapping is Dissabled, Prevent Camera from Observing Any Levels That Are Outside Range

	// Calculate View Matrix
	view = glm::translate(glm::mat4(1.0f), glm::vec3(-Position.x, -Position.y, Position.z));

	// Save View Matrix
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(Global::projectionStatic));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Calculate Relative World Mouse Coordinates
	Global::mouseRelativeX = (float)Global::mouseX + Position.x;
	Global::mouseRelativeY = (float)Global::mouseY + Position.y;

	//Global::objectShader.Use();
	//glUniform4f(glGetUniformLocation(Global::objectShader.Program, "material.viewPos"), -Position.x, -Position.y, 0.0f, 0.0f);
}

void Render::Camera::Camera::testForCringe()
{
	// If Cringe, Program Will Crash Here
	int x = Position.x;
}

void Render::Camera::Camera::updateDebugPositions(bool override_move)
{
	// Test if Camera and Mouse Should Update
	if (override_move || moved)
	{
		debugger->setCameraPosition(glm::vec2(Position.x, Position.y));
		debugger->setMousePosition(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));
	}

	// Else, Test if Mouse Should Update
	else if (Global::cursor_Move)
		debugger->setMousePosition(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));

	// Disable the Camera Moved Flag
	moved = false;
}

#endif

void Render::Camera::Camera::setPosition(glm::vec3 new_pos)
{
	// Store Position
	Position = new_pos;

#ifndef DLL_HEADER

	// Update Values
	updatePosition();

#endif

}

#ifdef DLL_HEADER
}
#endif
