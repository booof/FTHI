#pragma once
#ifndef SCRIPT_WIZARD_H
#define SCRIPT_WIZARD_H

#include "ExternalLibs.h"
#include "Render/GUI/MasterElement.h"
#include "Render/GUI/Box.h"
#include "Render/GUI/VerticalScrollBar.h"

// Welcome to the Script Wizard!
// This Wizard Will Show All Currently Used Scripts and Their Index
// Each Script Will Have a Path That Leads to The Respecive Script C++ File
// Both Global, Function, and Object Scripts Will Have an Identifier Next to Them as Well as the Object Type The Script is Intended to be Used For
// Object Scripts Cannot be Missmatched Between Object Types, Function Scripts Are Indended if There is a Need to Reuse Code Between Objects
// Scripts Can be Appended to the Wizard, Moved Around in the Wizard, and Removed From the Wizard
// Moving and Removing a Script From the Wizard Will Prompt the User if They Want to Search the Levels and Modify the Script
// This Process may be Slow, and Not Doing it May Cause Unintended Consequences
// The Primary Use of this Wizard is to Store Scripts in an Enumerated Index, Allowing For Mapping to Function Pointers
// The Only Thing This Wizard Can Modify is the Script Path
// This Wizard is the Only Way to Select Scripts in the Editor Window

// Script Types:
// Global: These Scripts Are Executed Every Frame by the Main Loop and Allow for the Use of Global Variables Between Scripts
// Object: These Scripts Are Executed Every Frame and Each Variable Defined is Unique Per Object. Private Object Variables and Functions Can be Accessed

// This GUI Is Also Used to Organize the Files Used in a Project

// IDEA: The Changes Made Should Only be Saved When a Project is Saved.
// This Means That, Once Level Objects Are Saved, This Object is Free to Change the Script Indicies in Each Object Directly From the Level Files
// This Allows the Program to Safely Prevent Any Script Conflicts If Scripts are Changed and Effciently Allow Saving of Objects to a Single Operation
// The Changes Made in Level Data Files Are to Be Carried Out by an Index Map To Map Old Incidies to New Indicies
// This Map Will Have an Initial Size Equal to the Initial Script Size, Can Grow If Scripts are Added, But Will Not Change if Scripts are Removed
// If A Script is Removed, The Corrisponding Index Will Become 0;

namespace Editor
{
	// The Script Wizard Object
	class ScriptWizard
	{
		// Types of Loops
		enum LOOP_TYPES : uint8_t
		{
			FILES,
			GLOBALS,
			OBJECTS
		};

		// Structure for Files
		struct FileInstances
		{
			// Name of File
			std::string name = "";

			// Path of File
			std::string path = "";
		};

		// Structure for Scripts
		struct ScriptInstance
		{
			// Name of Function
			std::string function = "";

			// Index to File That Defines Function
			int index = 0;
		};

		// Structure for Object Scripts
		struct ObjectScript
		{
			// The Name of the Script Group
			std::string name = "";

			// The Initializer Script
			std::string initializer = "";

			// The Update Script
			std::string update = "";

			// The Delete Script
			std::string uninitializer = "";
		};

		// The Singleton Instance
		static ScriptWizard instance;

		// Window Vertex Object
		GLuint windowVAO = NULL, windowVBO = NULL;

		// Window Body Vertex Object
		GLuint bodyVAO = NULL, bodyVBO = NULL;

		// Highlight Vertex Object
		GLuint highlightVAO = NULL, highlightVBO = NULL;

		// Seleted Row and Column of the Object
		int selected_row = -1;
		int selected_column = -1;

		// Master Element
		Render::GUI::MasterElement master;

		// ScrollBar
		Render::GUI::VerticalScrollBar bar;
		bool scrolling = false;
		float scoll_offset = 0.0f;
		
		// Search Box

		// Add Instance Box
		Render::GUI::Box box_add_instance;

		// Remove Instance Box
		Render::GUI::Box box_remove_instance;

		// Load Instance Box
		Render::GUI::Box box_load_instance;

		// Modify Instance Box
		Render::GUI::Box box_modify_instance;

		// Modify Instance Box Has Been Replaced With a New Box
		// To Modify a Value, Double Click on the Value You Want to Change
		// For Objects and Globls, This Box is Now A Box That Allows The Creation of Function Definitions

		// Move Instance Box
		Render::GUI::Box box_move_instance;

		// Open Instance Box
		Render::GUI::Box box_open_instance;

		// Exit Box
		Render::GUI::Box box_exit;

		// Edit Value Box
		Render::GUI::Box box_edit_value;

		// The Current Loop to Interact With
		uint8_t current_loop = 0;

		// The Object Script Map
		int* object_script_map = nullptr;
		bool object_scripts_moved = false;

		// The Array of Files
		FileInstances* files = nullptr;
		int files_size = 0;

		// The Array of Global Scripts
		std::string* globals = nullptr;
		int globals_size = 0;

		// The Array of Object Scripts
		ObjectScript* objects = nullptr;
		int objects_size = 0;

		// Determines if Object is Initialized
		bool initialized = false;

		// Initialize Object
		ScriptWizard() {}

		// This Function Essentially Creates a Sorted Set of Function Names
		void sortScriptHeader(std::string* raw_array, int& raw_array_size);

		// Resize File
		void resizeFile(int new_size);

		// Add File
		bool addFile();

		// Remove File
		void removeFile(int index);

		// Load File
		bool loadFile();

		// Open File
		void openFile(int index);

		// Resize Global
		void resizeGlobal(int new_size);

		// Add Global
		void addGlobal();

		// Remove Global
		void removeGlobal(int index);

		// Open Global
		void openGlobal(int index);

		// Resize Object
		void resizeObject(int new_size);

		// Add Object
		void addObject();

		// Remove Object
		void removeObject(int index);

		// Open Object
		void openObject(int index, int index2);

	public:

		// Remove Copy Constructor
		ScriptWizard(const ScriptWizard&) = delete;

		// Initialize Object
		void initializeScriptWizard();

		// Update Object
		void updateScriptWizard();

		// Load Script Data
		void loadScriptData();

		// Write Script Data
		void writeScriptData();

		// Generate CMakeList
		void genCMakeList();

		// Return the Singleton
		static ScriptWizard* get();

		// The Closer Function for Selected Text
		void textCloser();

		// Determines if Object was Modified
		bool modified = false;
	};
}


#endif
