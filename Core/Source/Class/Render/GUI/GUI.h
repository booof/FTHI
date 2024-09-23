#pragma once
#ifndef GUI_H
#define GUI_H

#include "Render\Container\Container.h"
#include "Render\Objects\UnsavedBase.h"
#include "Object\Terrain\Foreground.h"
#include "Element.h"
#include "MasterElement.h"
#include "Render\Objects\SubLevel.h"

// This is the Class That Defines a GUI Instance
// This Class is Able to Read GUI Files, Store GUI Elements, and Allow User Interactions


namespace Render::GUI
{
	// Data for GUIs
	struct GUIData
	{
		// The File Identifier
		// [0] = Data Type: 0 = SceneList, 1 = SceneData, 2 = Level Data, 3 = Editor Data, 5 = Vars
		// [1] = Instance Type: 0 = Levels, 1 = GUIs. Similar Thing for Var Files
		// [2] = Identifier #1: Static Number
		// [3] = Identifier #2: Static Number
		char identifier[4] = { 1, 1, 42, 125 };

		// Width of the GUI
		float gui_width = 64.0f;

		// Height of the GUI
		float gui_height = 64.0f;

		// The Size of the GUI Name
		uint16_t name_size = 0;

		// GUI Script
		uint16_t script = 0;

		// Set to True if the GUI Should Move With Camera
		bool stationary = true;
	};

	// GUI Container Subclass
	struct ObjectContainer_GUI : public ObjectContainer
	{
		// Seperators for Level Specific Objects
		Element** element_start = nullptr;
		Object::Terrain::Foreground** static_start = nullptr;
		Object::Terrain::Foreground** dynamic_start = nullptr;

		// Counts for Level Specific Objects
		uint16_t element_count = 0;
		uint16_t terrain_count = 0;

		// Counts for The Different Terrain Objects
		uint16_t static_object_count = 0;
		uint16_t dynamic_object_count = 0;
	};

	// The Viewport Stack Class
	class ViewportStack
	{
		// Note: Size of Array Should be the Maximum Master Element Depth
		// Current Implementation is Number of Master Elements + 1, Change Later

		// Array of Viewport Instances
		// First Instance is Always the Full Screen Size
		ViewportInstance* instances = nullptr;

		// The Current Index in the Stack
		int instance_index = 0;

	public:

		// Generate the Object
		ViewportStack(int max_instances_count);

		// Default Constructor
		ViewportStack() {}

		// Delete the Object
		~ViewportStack();

		// Add a Viewport to the Stack and Switch to It
		void pushInstance(ViewportInstance& instance);

		// Remove a Viewport From the Stack and Switch to the Previous Viewport
		void popInstance();
	};

	// The Main GUI Class
	class GUI : public Container
	{
		// GUI Data Object
		GUIData gui_data;

		// Path to Both Object And Editor Data of GUI
		std::string gui_path = "";

		// The Array of Master Elements in GUI
		// First Section Gets Updated First, and Rendered Last
		// Currently Unloaded Master Elements Will be in Last Section of Array
		MasterElement** element_sections = nullptr;

		// The Number of GUI Sections
		int section_count = 0;

		// The Array of Base Master Elements with No Master Element Parent
		MasterElement** base_element_sections = nullptr;

		// The Number of Base GUI Sections
		int base_section_count = 0;

		// The Singular Sublevel to Read Objects
		Objects::SubLevel sublevel;

		// Number of Terrain Vertices
		GLuint number_of_vertices;

		// Projection Matrix
		glm::mat4 projection;

		// The Object Container
		ObjectContainer_GUI container;

		// The Viewport Stack
		ViewportStack viewport_stack;

		// The Offset in the Static Object Vertices
		int static_vertex_offset = 0;

		// The Offset in the Dynamic Object Vertices
		int dynamic_vertex_offset = 0;

		// The Number of Static Object Vertices
		int static_vertex_count = 0;

		// The Number of Dynamic Object Vertices
		int dynamic_vertex_count = 0;

		// Only to be Used When Reallocating Objects Array
		ObjectContainer& getContainer();

		// Reload the Terrain Objects of Container
		void constructTerrain();

		// Construct Terrain Using the Correct Algorithm
		void constructTerrainAlgorithm(int& instance, int& instance_index);

		// Reload the Lighting Objects of Container
		void loadLights();

		// Generate the Elements of the Container
		void genElements();

		// Determine the Starting Pointers the Different Objects in Container Through Sorting
		void segregateObjects();
			
		// Test Selector Object
		uint8_t testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Remove Marked Object From List With Unkown Container Type
		void removeMarkedChildrenFromList(DataClass::Data_Object* parent);

		// Incorporate a List of Objects Into Loaded Level With Unkown Container Type
		void incorperatNewObjects(Object::Object** new_objects, int new_objects_size);

		// Reset the Object Pass Over Flag
		void resetObjectPassOver();

		// Store Level of Origin
		void storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, Object::Object* real_object);

		// Reallocate All Memory
		void reallocateAll(bool del, uint32_t size);

	public:

		// Initialize New Level Container
		GUI(std::string& gui_path, bool editing);

		// Deconstructor to Avoid Memory Leaks
		~GUI();

		// Returns the Type of Container Object
		CONTAINER_TYPES getContainerType();

		// Add Object and Decendants Into Container
		void genObjectIntoContainer(DataClass::Data_Object* new_object, Object::Object* real_parent, uint16_t& index, int16_t delta_size);

		// Generate Objects Into This Container
		void buildObjectsGenerator(std::vector<DataClass::Data_Object*>& data_object_array, uint16_t& index, Object::Object* parent, glm::vec2 position_offset, uint16_t& active_index, Objects::UnsavedLevel& unsaved_level);

		// Update Level Location
		void updateLevelPos(glm::vec2 position, glm::i16vec2& level);

		// Update Camera
		void updateCamera();

		// Update Level Container
		void updateContainer();

		// Draw Level Objects
		void drawContainer();

		// Draw Static Level Objects if Visualization is Enabled
		void drawVisualizers();

		// Draw Level Objects
		void loadObjects();

		// Reload All Objects
		void reloadAll();

		// Get the Level Size
		void getSublevelSize(glm::vec2& sizes);

		// Get Pointers to the GUI Information
		void getGUIInfo(GUIData** data, std::string** name);

		// Get Path to GUI Data
		std::string getGUIPath();
	};
}

#endif
