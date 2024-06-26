#pragma once
#ifndef DEBUGGER_H
#define DEBUGGER_H

// This GUI is Dedicated to Parsing the CMake Output File and Report Any Errors to the User
// This GUI is Activated Once After Each Recompilation of Scripts and Before Each Switch Into Gameplay Mode
// This GUI Will be Activated if a Flag Reporting a New Compilation is True and This Flag Will Only Turn Off If There Are No Errors
// Each Error is Neatly Organized in a Window Similar to the Error List in Visual Studio
// The Size of Each Row in the GUI is Determined by the Size of the Error Message and/or File Path
// Clicking On an Error Will Open the Perpetrating File in the Default Text Editor at the Specified Line and Character of the Error
// This, However, Only Works for Errors and Linker Errors and Only Esoteric Errors Thrown by Microsoft Cannot be Easily Fixed
// Linker Errors Still Open up the Offending File
// If This GUI Detects an Error in the Code, A Notification Will Pop-Up Telling the User About Errors in the Code, Similar to Visual Studio

#include "ExternalLibs.h"
#include "Render/GUI/MasterElement.h"
#include "Render/GUI/VerticalScrollBar.h"
#include "Render/GUI/Box.h"

namespace Render
{
	class Container;
}

namespace Editor
{
	// Debugger Object
	class Debugger
	{
		// Type of Error
		enum ErrorType : uint8_t
		{
			TYPE_ERROR = 0,
			TYPE_LINKER,
			TYPE_WARNING
		};

		// Structure of Debug Error
		struct DebugError
		{
			// Error Type
			ErrorType error_type;

			// The Error Name
			std::string error_name;

			// Error Message
			std::string error_message;

			// Name of File
			std::string file_name;

			// Path of File
			std::string file_path;

			// Line of Error
			int line;

			// Column of Error
			int column;
		};

		// The Singleton Instance
		static Debugger instance;

		// The Flag to Look At Build Output
		bool look_at_build = false;

		// The Flag that Determines if the Compilation Was Looked At At Least Once
		bool looked_at_least_once = false;

		// Window Object
		GLuint windowVAO = 0, windowVBO = 0;

		// Window Body Object
		GLuint bodyVAO = 0, bodyVBO = 0;

		// Highlighter Object
		GLuint highlightVAO = 0, highlightVBO = 0;

		// Array of Errors
		std::vector<DebugError> errors;
		int errors_size = 0;

		// Master Element
		Render::GUI::MasterElement master;

		// ScrollBar
		Render::GUI::VerticalScrollBar bar;
		bool scrolling = false;
		float scoll_offset = 0.0f;

		// Box to Open File of Error
		Render::GUI::Box box_open_file;

		// Box to Recompile Project
		Render::GUI::Box box_recompile_project;

		// Box to Exit GUI
		Render::GUI::Box box_exit;

		// Strings for Header
		std::string header_cam_pos = "";
		std::string header_mouse_pos = "";
		std::string header_object_count = "";

		// The Current Cycle of the Header
		int header_state = 0;

		// Level Width and Height for Positions
		float level_width = 0.0f;
		float level_height = 0.0f;

		// Pointer to the Level
		Render::Container* main_level = nullptr;

		// Initial Constructor
		Debugger() {};

		// Read the Error Log
		void readErrorLog();

		// Draw Compiler Status
		void drawCompilerStatus();

		// Get the Position for Text
		void getTextPosition(std::string& header_string, glm::vec2& position);

	public:

		// Remove the Copy Constructor
		Debugger(const Debugger&) = delete;

		// Initialize Object
		void initializeDebugger();

		// Return the Singleton
		static Debugger* get();

		// Delete the Singleton
		void deleteDebugger();

		// Update the Debugger
		void update();

		// Enable the Flag
		void enableFlag();

		// Test if Project is Runable
		bool testIfCanRun();

		// Update Window
		void updateWindow();

		// Draw the Header of the Debugger
		void drawHeader();

		// Set the Position of the Camera
		void setCameraPosition(glm::vec2 position);

		// Set the Position of the Mouse
		void setMousePosition(glm::vec2 position);

		// Cycle the Header State
		void cycleHeaderState();

		// Store Level Positions
		void storeLevelPositions(float width, float height);

		// Store the Current Object Count
		void storeObjectCount(int count);

		// Returns True if Project Has Been Looked Over
		bool returnIfLookedOver();

		// Returns True if Project Has No Errors
		bool returnNoErrors();

		// Store the Level Object Currently Being Debugged
		void storeCurrentLevel(Render::Container* level);
	};
}

#endif
