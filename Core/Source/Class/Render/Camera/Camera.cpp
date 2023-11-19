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

Render::Camera::Camera::Camera(float initialX, float initialY, bool stationary, bool wrap_, glm::vec4 scene_bounderies_)
{
	Position = glm::vec3(initialX, initialY, 0.0f);
	Stationary = stationary;
	wrap = wrap_;
	scene_bounderies = scene_bounderies_;
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

	// If Wrapping is Enabled, Test if Camera Should be Wrapped
	if (wrap)
	{
		// Reset Wrapping Flag
		wrapped = false;

		// Test Horizontal Wrap
		if (Position.x < scene_bounderies.x) {
			Position.x += scene_bounderies.y - scene_bounderies.x;
			wrapped = true;
		} else if (Position.x > scene_bounderies.y) {
			Position.x += scene_bounderies.x - scene_bounderies.y;
			wrapped = true;
		}

		// Test Vertical Wrap
		if (Position.y < scene_bounderies.z) {
			Position.y += scene_bounderies.w - scene_bounderies.z;
			wrapped = true;
		} else if (Position.y > scene_bounderies.w) {
			Position.y += scene_bounderies.z - scene_bounderies.w;
			wrapped = true;
		}
	}

	// If Wrapping is Dissabled, Prevent Camera from Observing Any Levels That Are Outside Range
	else
	{
		// If Scene Was Zoomed, Update Radius
		if (Global::zoom)
		{
			camera_radius.x = Global::halfScalarX * Global::zoom_scale;
			camera_radius.y = 50.0f * Global::zoom_scale;
		}

		// If Camera X-Position is Offscreen, Set Camera Position to be Onscreen
		if (Position.x - camera_radius.x < scene_bounderies.x) {
			Position.x = scene_bounderies.x + camera_radius.x;
			Global::Keys[GLFW_KEY_A] = false;
		}
		else if (Position.x + camera_radius.x > scene_bounderies.y) {
			Position.x = scene_bounderies.y - camera_radius.x;
			Global::Keys[GLFW_KEY_D] = false;
		}

		// If Camera X-Position is Offscreen, Set Camera Position to be Onscreen
		if (Position.y - camera_radius.y < scene_bounderies.z) {
			Position.y = scene_bounderies.z + camera_radius.y;
			Global::Keys[GLFW_KEY_S] = false;
		}
		else if (Position.y + camera_radius.y > scene_bounderies.y) {
			Position.y = scene_bounderies.w - camera_radius.y;
			Global::Keys[GLFW_KEY_W] = false;
		}
	}

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
