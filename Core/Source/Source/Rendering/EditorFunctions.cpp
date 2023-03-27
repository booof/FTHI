#include "EditorFunctions.h"

#include "Globals.h"
#include "Class/Render/Objects/Level.h"
#include "Class/Render/Editor/Selector.h"
#include "Source/Events/EventListeners.h"
#include "Class/Render/Shader/Shader.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Class/Render/Editor/Debugger.h"
#include "Class/Render/Editor/ObjectInfo.h"

void Source::Rendering::Editing::edit(Render::Objects::Level* level, Editor::Selector* selector, Editor::ObjectInfo* object_info)
{
	// Update Debugger
	debugger->update();

	// Memory for Acceleration Timer
	static glm::i16vec4 accelerationTimer = glm::i16vec4(0, 0, 0, 0);

	// Set Selector to Disable Highlighting
	selector->highlighting = false;

	// Test if Engine is Paused
	if (Global::paused)
		Source::Listeners::smoothKeyCallbackEditorSimplified(*level->camera, nullptr, accelerationTimer);

	else if (!Global::editor_options->Active)
	{
		// Perform Editor Key Callback
		if (!Global::texting)
			Source::Listeners::SmoothKeyCallback_Editor(*level->camera, *selector, *level, accelerationTimer);

		// If Selector is Inactive, Perform Selecting
		if ((!selector->active && !selector->editing) || 
			( (Global::Keys[GLFW_KEY_LEFT_CONTROL] || Global::Keys[GLFW_KEY_RIGHT_CONTROL])
			^ (selector->selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])) ) 
			)
			level->testSelector(*selector, *object_info);

		// If Selecting and Right Click, Activate Editor Window
		else if (Global::RightClick)
		{
			Global::RightClick = false;
			selector->active_window = true;
			selector->genObjectEditorWindow();
			glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);
		}

		// If Window is Active, Edit Window
		if (selector->active_window)
			selector->updateWindow();

		// Else if Selector is Active, Edit Selector
		else if (selector->active || selector->force_selector_initialization)
			selector->updateSelector();
	}
}

void Source::Rendering::Editing::renderEditor(Render::Objects::Level* level, Editor::Selector* selector, Editor::ObjectInfo* object_info)
{
	// Render Level Visualizers
	level->drawVisualizers();

	// Test if Editor Options Should be Displayed
	if (Global::editor_options->Active)
	{
		glDisable(GL_DEPTH_TEST);
		Global::editor_options->Blitz();
		glEnable(GL_DEPTH_TEST);
		Global::colorShaderStatic.Use();
		glUniform1i(Global::staticLocColor, 0);
	}

	else
	{
		// Test if Selector Should be Displayed Instead
		if (selector->active)
		{
			selector->blitzSelector();
		}

		// Test if Selector Highlighter Should be Displayed
		if (selector->highlighting)
		{
			selector->blitzHighlighter();
		}

		// Test if Editor Window Should be Displayed
		if (selector->active_window)
		{
			selector->drawWindow();
		}

		// Draw Header
		debugger->drawCompilerStatus();
		object_info->drawInfo();
	}
}

void Source::Rendering::Editing::drawLevelBoarders(Render::Camera::Camera* camera)
{
	// If Level Border Visualization is Disabled, Do Nothing
	if (!Global::level_border)
		return;

	// Bind Level Boarder Shader
	Global::borderShader.Use();

	// Bind Empty Vertex Object
	glBindVertexArray(Global::level_border_VAO);

	// Store Camera Position
	glUniform2f(Global::level_border_camera_pos_loc, camera->Position.x, camera->Position.y);

	// Draw Borders
	glDrawArrays(GL_POINTS, 0, 1);

	// Unbind Empty Vertex Object
	glBindVertexArray(0);
}

