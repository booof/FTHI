#pragma once
#ifndef SCENE_CONTROLLER_H
#define SCENE_CONTROLLER_H

#include "ExternalLibs.h"
#include "Render/GUI/Box.h"
#include "Render/GUI/TextObject.h"
#include "Render/GUI/MasterElement.h"
#include "Render/GUI/VerticalScrollBar.h"

// This is the Scene Controller Object
// This Object Allows The User to Select Which Level
// or GUI to Edit. In Addition, it Allows the User to
// Set the Size of a Sublevel, the Number of Sublevels
// in a Level in Each Direction, If Sublevels Wrap, as 
// Well as Level/GUI Specific Variables, Such as Gravity,
// Maximum Framerate, and deltaTime

// Upon First Creation of a Project, This GUI Will be Shown
// A Default Level Configuration Will No Longer be Provided
// After That, If The User Wants to Use This GUI Again, They
// Need to Press f8. From There, They Can Edit Global Variables
// and/or Create/Load a New Level/GUI. The Sublevel Size is
// Constant Upon Creation of a Level and Cannot be Changed. 
// Sublevel Count Can be Increased Freely, However, Decreasing
// Will be Forbidden if there is a Sublevel That Will be Culled.
// Wrap is the Only Characteristic That May be Changed Freely.
// Loading Into a Different GUI/Level Will Trigger the Same Response
// as Loading Into a Different Project in the Project Selector

// The Project Level Directory System Will Need to be Changed to 
// Incorporate These Changes.

// This Object Should Exist as a Singleton

// Declaration for Level
namespace Render::Objects
{
	struct SceneData;
	class Level;
}

// Declaration for GUI
namespace GUI
{
	struct GUIData;
	class GUI;
}

namespace Editor
{
	class SceneController
	{

	public:

		// List of Possible Modes
		enum class SCENE_MODES : uint8_t
		{
			NONE = 0,
			LEVEL,
			GUI,
			EDITING
		};

	private:

		// List of SubModes During Editing
		enum class EDITING_MODES : uint8_t
		{
			NONE = 0,
			VARIABLES,
			SCRITPS,
			LEVELS,
			GUIS
		};

		// List of Instance Types During Editing
		enum class INSTANCE_TYPES : uint8_t
		{
			NONE = 0,
			LEVEL,
			GUI
		};

		// The Struct for the Scene List
		struct SceneList
		{
			// The Identifier
			char identifier[4] = { 0, 0, 42, 125 };

			// The Number of Scenes
			uint16_t scene_count = 0;

			// The Number of GUIs
			uint16_t gui_count = 0;

			// Default Level Index
			uint16_t defualt_level_index = 0;
		};

		// Struct for a Scene / GUI Object
		struct Instance
		{
			// The Index of the Object
			uint16_t index = 0;

			// The Name Size of the Object
			uint16_t name_size = 0;

			// The Name of the Object
			std::string name = "";
		};

		// The Singleton Instance
		static SceneController instance;

		// Current Mode of the Object
		SCENE_MODES current_mode = SCENE_MODES::NONE;

		// Current Editing Submode of the Object
		EDITING_MODES editing_mode = EDITING_MODES::NONE;

		// Currently Loaded Instance Type
		INSTANCE_TYPES current_instance_type = INSTANCE_TYPES::NONE;

		// Window Vertex Objects
		GLuint windowVAO = 0, windowVBO = 0;

		// Vertex Objects for Level/GUI Selection
		GLuint selectionVAO = 0, selectionVBO = 0;

		// Number of Verticies for Selection
		// Only Includes the Vertices Involved in Dynamic Scrolling
		unsigned int selection_vertex_count = 0;

		// The Currently Selected Object in a List
		// Used to Carry List Index Into Functions
		int selected_selection = -1;

		// Verticle Scroll Bar
		GUI::VerticalScrollBar bar;

		// Vertex Array for Boxes
		std::vector<GUI::Box> boxes;

		// Vertex Array for Text
		std::vector<GUI::TextObject> text;

		// Master Elements
		GUI::MasterElement master_element_static;
		GUI::MasterElement master_element_dynamic;

		// Location Where the Dynamic Elements Start
		int dynamic_text_start = 0;
		int dynamic_box_start = 0;

		// Pointer to the Name of the Current Instance
		std::string* instance_name = nullptr;

		// Pointer to the Scene Data Object for Levels
		Render::Objects::SceneData* level_data = nullptr;

		// Pointer to the GUI Data Object for GUIs
		GUI::GUIData* gui_data = nullptr;

		// Scene List Object
		SceneList list;

		// Vector of Level Instances
		std::vector<Instance> levels;

		// Vector of GUI Instances
		std::vector<Instance> guis;

		// The Path to the Currently Loaded Instance
		std::string instance_path = "";

		// The Pointer to the Currently Loaded Instance
		void* currently_loaded_instance = nullptr;

		// The Pointer to the Current Instance Struct
		Instance* current_instance = nullptr;

		// Determines if the Scene List Vertex Objects Should be Force Reloaded
		bool force_list_reload = false;

		// Initial Constructor
		SceneController() {}

		// Prepare Window Objects for Specified Mode
		void prepareForMode(SCENE_MODES mode, bool force_reload);

		// Function to Only Generate Background Vertices
		void genBackgroundOnly();

		// Generate Background Vertices
		void genBackground();

		// Generate the Title and Exit Box
		void genTitle();

		// Generate the Chunk Layout Elements
		void genChunkLayoutElements(float initial_y, int initial_box, GUI::TextData& text_data, GUI::BoxData& box_data);

		// Generate the Selection List for Levels and GUIs
		void genSelectionList(std::vector<Instance>& vec);

		// Prep Object for Level Creation Mode
		void prepareLevelCreation();

		// Prep Object for GUI Creation Mode
		void prepareGUICreation();

		// Prep Object for Editing Mode
		void prepareEditing();

		// Prepare for Editing Submode
		void prepareEditingSubMode(EDITING_MODES mode, bool force_reload);

		// Generate Variable Mode Elements
		void genVariableMode();

		// Generate Script Mode Elements
		void genScriptMode();

		// Generate Level Mode Elements
		void genLevelMode();

		// Generate GUI Mode Elements
		void genGUIMode();

		// Generate Tab Matrix
		glm::mat4 genTabMatrix(int index);

		// Read Instance Vector
		void readInstanceVector(std::ifstream& file, std::vector<Instance>& vec, int size);

		// Write Instance Vector
		void writeInstanceVector(std::ofstream& file, std::vector<Instance>& vec, int size);

		// Generate the Level Path
		void genLevelPath(int index);

		// Pre-Loop Initialization
		void initializeLoop(SCENE_MODES mode, bool& looping, std::function<void()>& exit_function);

		// Initial Loop Rendering of Drawing Background
		void renderInitial(float& modified_mouse_x, float& modified_mouse_y, glm::mat4& temp, bool& looping);

		// Final Loop Rendering of Drawing Elements
		void renderFinal(glm::mat4& temp);

		// Test if an Instance Name is Valid
		bool testValidName();

		// Returns True if Level is Valid
		bool testValidLevel(void* backup);

		// Returns True if GUI is Valid
		bool testValidGUI(void* backup);

		// Test Boxes in Loop
		void testBoxes(int& highlighted_box);

		// The Creation Loop for Creating Levels and GUIs
		void loopCreation(bool& looping, bool& successfull, void* backup);

		// Generate the Backup Data When Editing Variables
		void* genBackupData();

		// Test for Invalid User Inputs
		bool testInvalidInputs(void* backup);

		// Write Instance Data
		void writeInstanceData();

		// Write the Scene Data
		void writeSceneData();

		// Write the GUI Data
		void writeGUIData();

		// Write the Scene List
		void writeSceneList();

		// Write Globals
		void writeGlobals();

		// Get Index From Mouse Position in List
		int getIndexFromMouse(float relative_mouse_y);

		// Get Y-Position From Index in List
		float getPositionFromIndex(int index);

		// Set the Default Level
		void setDefault();

		// Load an Instance From a List
		void loadInstance();

		// Load a Level
		void loadLevel(int index);

		// Load a GUI
		void loadGUI(int index);

		// Add an Instance From Folder
		void addInstance();

		// Add an Instance to a Vector
		void addInstanceToVector(std::string& new_name, std::vector<Instance>& vec, uint16_t& list_count);

		// Test if an Instance Name Matches
		bool testMatchingName(std::string& test_name, std::vector<Instance>& vec);

		// Helper Helper Function to Remove an Instance
		void removeInstanceHelperHelper(std::vector<Instance>& vec, uint16_t& count);

		// Helper Function to Remove an Instance
		bool removeInstanceHelper();

		// Remove an Instance From the Scenes List
		void removeInstance();

		// Delete an Instance From the Scenes List and Folder
		void deleteInstance();

	public:

		// Remove Copy Constructor
		SceneController(const SceneController&) = delete;

		// Initialize the Object
		void initializeSceneController();

		// Enter Editing Mode
		void startEditingMode();

		// Generate a New Level Mode
		// Will Return True if a Level Was Generated (For First Level Creation)
		bool generateNewLevel();

		// Generate a New GUI Mode
		void generateNewGUI();

		// Load Scenes When Loading Into a Project
		void loadScenes();

		// Generate the First Level When Creating a New Project
		bool generateFirstLevel();

		// Load the Default Level
		void loadDefault();

		// Return the Singleton
		static SceneController* get();

		// Get the Current Instance
		void* getCurrentInstance();
	};
}

#endif
