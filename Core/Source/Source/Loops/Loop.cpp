#include "Loop.h"
#include "Globals.h"
#include "Class/Render/Objects/Level.h"
#include "Macros.h"
#include "Source/Rendering/PostRender.h"
#include "Source/Rendering/PreRender.h"
#include "Source/Rendering/EditorFunctions.h"
#include "ExternalLibs.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Camera/Camera.h"
#include "Class/Render/Editor/ProjectSelector.h"
#include "Source/Rendering/Exit.h"
#include "Class/Render/Objects/ChangeController.h"
#include "Class/Render/Editor/ObjectInfo.h"

// Use This Function to Quickly Empty Error Log
//while (glGetError() != GL_NO_ERROR) {}

//#define FFMPEG

void Loop::loop()
{
	// Loop Condition
	bool enable_loop = true;

	// Level Object
	Render::Objects::Level* level = new Render::Objects::Level(std::string(" "), std::string(" "));

	// Selector Object
	Editor::Selector* selector = new Editor::Selector();
	selector->level = level;

	// Object Info
	Editor::ObjectInfo* object_info = new Editor::ObjectInfo();
	selector->info = object_info;


#ifdef FFMPEG

	// start ffmpeg telling it to expect raw rgba 720p-60hz frames
	// -i - tells it to read frames from stdin
	const char* cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1280x720 -i - "
		"-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip output.mp4";

	// open pipe to ffmpeg's stdin in binary write mode
	FILE* ffmpeg = _popen(cmd, "wb");

	// Buffer to Hold Pixel Data
	int* buffer = new int[Global::screenWidth * Global::screenHeight];

#endif

	// Allow Processes to Finish
	//Sleep(1000);

	// Main Loop
	do
	{
		// Show That Program is Looping
	//	std::cout << "looping\n";

		// Pre-Rendering
		Source::Rendering::Pre::preRender();

		//level->camera->updatePosition();

		// Update Camera
		level->updateCamera();

#ifdef EDITOR

		// Test if Level Should Reload
		if (Global::reload_all)
			level->reloadAll();

		// Draw Level Boarders
		Source::Rendering::Editing::drawLevelBoarders(level->camera);

#endif


#ifdef EDITOR

		// Editor Specific Updates
		if (Global::editing || Global::paused)
		{
			// Perform Editing Features
			Source::Rendering::Editing::edit(level, selector, object_info);
		}

		// Normal Object Updating
		else
		{
			// Global Scripting
			//script_handler->updateGlobalScripts();
			Global::updateGlobalScripts();

			// Update Objects
			level->updateContainer();
		}

#else

		// Global Scripting
		script_handler->updateGlobalScripts();

		// Update Objects
		level->updateContainer();

#endif

		// Draw Objects
		level->drawContainer();

		// Draw Editor Objects
		Source::Rendering::Editing::renderEditor(level, selector, object_info);

		// Post Rendering
		Source::Rendering::Post::postRender();


#ifdef FFMPEG

		// Store Video Output to File, If Enabled
		glReadPixels(0, 0, Global::screenWidth, Global::screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		fwrite(buffer, sizeof(int) * Global::screenWidth * Global::screenHeight, 1, ffmpeg);

#endif

		//glfwSwapBuffers(Global::window);
		glFinish();

		// Test if the Elusive Breakpoint Has Been Activated
		if (Global::activate_elusive_breakpoint)
		{
			std::cout << "The Elusive Breakpoint Has Been Activate\n";
			Global::activate_elusive_breakpoint = false;
		}

		// Determine if Safe to Exit
		if (glfwWindowShouldClose(Global::window))
		{
			if (Source::Render::Exit::determineSafeToExit())
				enable_loop = false;
			else
				glfwSetWindowShouldClose(Global::window, false);
		}
	}
	while (enable_loop);

#ifdef FFMPEG

	// Close Video Output File, If Enabled
	_pclose(ffmpeg);

#endif

	// Delete Level Container
	delete level;

	// Delete Selector
	delete selector;
}
