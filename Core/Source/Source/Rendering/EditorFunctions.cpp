#include "EditorFunctions.h"

#include "Globals.h"
#include "Class/Render/Objects/Level.h"
#include "Class/Render/Editor/Selector.h"
#include "Source/Events/EventListeners.h"
#include "Class/Render/Shader/Shader.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Class/Render/Editor/Debugger.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Class/Render/GUI/SelectedText.h"

void Source::Rendering::Editing::edit(Render::Container* container, Editor::Selector* selector, Editor::ObjectInfo* object_info)
{
	// Update Debugger
	debugger->update();

	// Memory for Acceleration Timer
	static glm::i16vec4 accelerationTimer = glm::i16vec4(0, 0, 0, 0);

	// Set Selector to Disable Highlighting
	selector->highlighting = false;

	// Test if Engine is Paused
	if (Global::paused)
		Source::Listeners::smoothKeyCallbackEditorSimplified(*container->camera, nullptr, accelerationTimer);

	else if (!Global::editor_options->Active)
	{
		// Perform Editor Key Callback
		if (!selected_text->isActive())
			Source::Listeners::SmoothKeyCallback_Editor(*container->camera, *selector, *container, accelerationTimer);

		// If Selector is Inactive, Perform Selecting
		bool complex_selection = ((Global::Keys[GLFW_KEY_LEFT_CONTROL] || Global::Keys[GLFW_KEY_RIGHT_CONTROL])
			^ (selector->selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])));
		if ((!selector->active && !selector->editing) || complex_selection)
		{
			// Test Selection
			uint8_t result = container->testSelector(*selector, *object_info);

			// If Selection Was Invalid and Complex Selection, Disable Left Click
			if (result == 1 && complex_selection)
				Global::LeftClick = false;
		}

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

void Source::Rendering::Editing::renderEditor(Render::Container* container, Editor::Selector* selector, Editor::ObjectInfo* object_info)
{
	// Render Level Visualizers
	container->drawVisualizers();

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
			selector->blitzSelector();

		// Test if Selector Highlighter Should be Displayed
		if (selector->highlighting)
			selector->blitzHighlighter();

		// Test if Editor Window Should be Displayed
		if (selector->active_window)
			selector->drawWindow();

		// If Editor Window is Not Displayed, Display Object Info
		else
			object_info->drawInfo();

		// Draw Header
		debugger->drawHeader();
	}

	// Draw Selected Text Icon, If it is Being Used
	selected_text->renderCursor();
}

