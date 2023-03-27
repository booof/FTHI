#pragma once
#ifndef EVENT_LISTENERS_H
#define EVENT_LISTENERS_H

#include "ExternalLibs.h"
#include "Macros.h"

#include "Class/Render/Camera/Camera.h"

#include "Class/Render/Objects/Level.h"
#include "Class/Render/Editor/Selector.h"

// I know I shouldn't use "Using Namespace" in a header file but
// this retard of a compiler would throw and error otherwise
using namespace Render::Camera;
using namespace Editor;
using namespace Render::Objects;

namespace Source::Listeners
{

	// Executes Whenever the Window Changes Size. Changes Screen Variables
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	// Handles Individual Keyboard Inputs and Key Events
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

	// Handles Keyboard Events for Typing Text
	void TypeCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

	// Retrieves Cursor Position
	void CursorCallback(GLFWwindow* window, double xPos, double yPos);

	// Handles Events Involving Mouse Scroll Wheel
	void ScrollCallback(GLFWwindow* window, double offsetX, double offsetY);

	// Handles Events Involving a ScrollBar
	void ScrollBarCallback(GLFWwindow* window, double offsetX, double offsetY);

	// Handles Individual Mouse Inputs
	void MouseCallback(GLFWwindow* window, int button, int action, int mods);

//#ifdef EDITOR

	// Handles Keyboard Events for Frame by Frame
	void FrameByFrameCallback(GLFWwindow* window, int key, int scancode, int action, int mode);

	void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	// Handles Inputs that Update Every Frame for Editor Loop
	//void SmoothKeyCallback_Editor(Render::Camera::Camera& camera, Editor::Selector& selector, Render::Objects::Level& level, glm::vec4& acceleration_timer);
	void SmoothKeyCallback_Editor(Camera& camera, Selector& selector, Level& level, glm::i16vec4& acceleration_timer);

	// Simplified Smooth Key Callback for Editor (Minimal Inputs)
	void smoothKeyCallbackEditorSimplified(Camera& camera, Selector* selector, glm::i16vec4& acceleration_timer);

//#endif

	//void SmoothKeyBack(Render::Camera::Camera& camera, int h);

	// Update the Direction Camera is Moving in the Editor
	void updateEditorCameraMovement(Camera& camera, Selector* selector, unsigned char direction, int16_t& acceleration_timer, int16_t& inverse_acceleration_timer, float& camera_acceleration);
}

#endif
