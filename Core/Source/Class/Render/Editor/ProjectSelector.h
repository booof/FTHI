#ifndef PROJECT_SELECTOR_H
#define PROJECT_SELECTOR_H

#include "ExternalLibs.h"
#include "Render/GUI/Box.h"
#include "Render/GUI/MasterElement.h"
#include "Render/GUI/VerticalScrollBar.h"

// The Eventual Goal of this Engine is to Allow for the Creation of Video Games, (Preferably More Than One)
// This GUI is Intended to Allow the User to Select Which Game They Currently Want to Work On
// Instead of Having a Single Resources Folder for a Single Project, There Will be a New Folder Outside of the FTHI Engine Called Projects
// Each Folder Inside the Projects Folder Will be an Individual Project With the Same Contents as the Resources Folder
// The Engine Itself Will Still Have Its Own Resources Folder Intended for Use in the Editor, Such as Light Object Textures, Cursors, and Notification Sounds
// The Project Selector GUI Will Allow Users to Select Which Project They Want to Work On
// Loading a Project Will Flush All Currently Initialized Data in the Engine and Reload Everything Needed for the New Project
// Default Settings Will Have This Window Appear Before Initializing the Main Loop and Level, Prompting the User to Select a Project
// This GUI Will Have a Check Mark Box That, When Checked, Will Skip the Prompting From This Window Upon the Next Use of the Engine, Defaulting to the Most Recent Project
// This GUI Can Also be Activated Using a Soon-to-be-Determined Keybind, Allowing the User to Switch Between Projects In the Same Context

// Note: This New Project System Will Have to Redo Some of the Script Framework
// There Will be a Need to Determine How to Dynamically Compile and Load Code From These Scripts As Well as Bind Script Function Pointer to the Script Handler
// The Function Pointers Should be Easy as Each Project Will Have Their Own Main Code File Where the Script Arrays Can be Defined. These Arrays Must be Defined as External Variables in the Script Handler
// I Have No Fucking Idea How to Dynamically Compile Code (Possibly DLLs?)

// This Object Should Exist as a Singleton

// Window Layout: Should be Similar to Visual Studio Project Selector
// Projects Should be Sorted Based on Most Recent Use in Engine, With Most Recent On Top

// File Layout:
// First Line/Byte: Startup Enabled Boolean
// Second Line/Byte: Number of Projects
// Next Lines:
// Project Layout : project_name, project_path, date_last_modified, raw_date_last_modified
// NOTE: date_last_modified is equal to the time at which Project was Last Saved
// ALSO: date_last_modified = mm/dd/yyyy hour:min

namespace Editor
{
	// The Project Selector Class
	class ProjectSelector
	{
		// Struct for Project
		struct ProjectInstance
		{
			// Project Name
			std::string name = "";

			// Project Path
			std::string path = "";

			// Project Access Time
			std::string access_time = "";

			// Project Access Time (Seconds Since 1970)
			time_t raw_access_time = 0;
		};

		// The Singleton Instance
		static ProjectSelector instance;

		// The Current Project Name
		std::string current_project_name;

		// The Current Project Path
		std::string current_project_path;

		// The Index of the Currently Selected Project
		int current_project_index = -1;

		// Window Vertex Object
		GLuint windowVAO = 0, windowVBO = 0;

		// The "Load GUI On Startup" Box
		GUI::Box startup_enabled_box;

		// The Boolean That Determines if GUI Should be Shown on Startup
		bool startup_enabled = true;

		// Determines if a Project is Initialized
		bool project_initialized = false;

		// Determines if 24-Hour Time Should be Used
		bool use_24_hour_clock = false;

		// Box to Search for Project
		GUI::Box box_search;
		std::string search_text = "";

		// Box to Determine if Window Opens on Startup
		GUI::Box box_startup_enabled;

		// Box to Open Project
		GUI::Box box_open_project;

		// Box to Open Project Directory
		GUI::Box box_open_project_directory;

		// Box to Remove Project
		GUI::Box box_remove_project;

		// Box to Create New Project
		GUI::Box box_create_project;

		// Box to Add Project From Directory
		GUI::Box box_add_project;

		// Bar for Project Selection List
		GUI::VerticalScrollBar project_selection_bar;
		bool scrolling = false;
		float scoll_offset = 0.0f;

		// Master Element
		GUI::MasterElement master_element;

		// Initial Constructor
		ProjectSelector() {}

		// Read the Project List File
		void readProjectListFile(ProjectInstance** instances, int& instance_count);

		// Write Changes to Project List to File
		void writeProjectListFile();

		// Load DLL
		void loadDLL();

		// Load Project
		void loadProject();

		// Function to Copy File Contents to Another File
		void copyFileContents(std::string read_path, std::string write_path);

		// Function to Get Date/Time for File
		std::string getDateAndTime();

		// Add Project to File
		void addProjectToFile(ProjectInstance instance);

		// Remove Project From File
		void removeProjectFromFile(int index);

		// Generate a New Project
		bool createProject();

		// Add Project From Directory
		bool addProject(ProjectInstance* instances, int& instance_count);

	public:

		// Remove Copy Constructor
		ProjectSelector(const ProjectSelector&) = delete;

		// Initialize Object
		void initializeProjectSelector();

		// Allow User to Select a Project
		void select_project();

		// Uninitialize Object
		void uninitializeProjectSelector();

		// Recompile the Project
		void recompileProject();

		// Toggle Between Editor and Gameplay
		void toggleEngineMode();

		// Return the Singleton
		static ProjectSelector* get();
	};
}

#endif
