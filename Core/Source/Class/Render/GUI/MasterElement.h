#pragma once
#ifndef MASTER_ELEMENT_H
#define MASTER_ELEMENT_H

// This Class is Meant to Controll Interactions Between GUI Elements
// Handles Events if an Element is Being Modified
// Should be Called at the Beginning or End of the "Update Elements" Loop (Preferably Beginning)
// IDEA: There Should be a Test to See if Mouse is Inside GUI. If True, All Other Elements in GUI Will be Tested 

#include "Element.h"

namespace Render::GUI
{
	class ScrollBar;
	class HorizontalScrollBar;
	class VerticalScrollBar;
	class ViewportStack;

	// Struct Containing Data for Master Element
	// NOTE: If No Selected Bar, Value is -1
	struct MasterData
	{
		// The Width of the Interactable Section of Master Element
		float width = 2.0f;

		// The Height of the Interactable Section of Master Element
		float height = 2.0f;

		// The Index of the Initial Vertical Scroll Bar
		int16_t initial_vertical_bar = -1;

		// The Index of the Initial Horizontal Scroll Bar
		int16_t initial_horizontal_bar = -1;

		// Determines if the Object is Initially Active
		bool initially_active = true;

		// Determines Which Bar is the Default Bar
		bool vertical_is_default = true;
	};

	// Struct Containing Indices of Default Objects
	struct DefaultElements
	{
		// Default Bar
		ScrollBar* default_bar = nullptr;

		// Horizontal ScrollBar
		ScrollBar* horizontal_bar = nullptr;

		// Vertical ScrollBar
		ScrollBar* vertical_bar = nullptr;
	};

	// Master Element Class
	class MasterElement : public Element
	{
		// Master Element Data
		MasterData data;

		// Pointer to List of Elements
		Element** element_ptr = nullptr;

		// Default Elements
		DefaultElements* defaults = nullptr;

		// Model Matrix for Elements
		glm::mat4 element_model = glm::mat4(1.0f);

		// Position of GUI Relative to Center of Screen
		glm::vec2 gui_position = glm::vec2(0.0f, 0.0f);

		// Determines if the Object is Active
		bool active = true;

		// The Number of Elements This Object Contains
		int element_count = 0;

		// Width of the GUI
		float gui_width = 0.0f;

		// Height of he GUI
		float gui_height = 0.0f;

		// The Offset in the Static Object Vertices
		// TODO: Make Boxes and Similar Elements Static Objects to be Rendered With Terrain Objects Simultainiously
		int static_vertex_offset = 0;

		// The Offset in the Dynamic Object Vertices
		int dynamic_vertex_offset = 0;

		// The Number of Static Object Vertices
		int static_vertex_count = 0;

		// The Number of Dynamic Object Vertices
		int dynamic_vertex_count = 0;

		// Pointer to Horizontal Offset
		float* horizontal_offset = nullptr;

		// Pointer to Vertical Percent
		float* vertical_offset = nullptr;

		// The Pointer to the Viewport Stack
		ViewportStack* viewport_stack = nullptr;
		
		// The Viewport Data
		ViewportInstance viewport_instance;

		// Update Pointers of Object
		void updatePointers();

	public:

		// Constructor
		MasterElement(glm::vec2 position, float width, float height, DefaultElements* defaults_);

		// Constructor Minus Defaults
		MasterElement(glm::vec2 position, float width, float height);

		// Preferable Constructor That Will Later Replace the Above Two Constructors
		MasterElement(ElementData& element_data_, MasterData& data_);

		// Default Constructor
		MasterElement() {};

		// Deconstructor
		~MasterElement();

		// Return Pointer to Position
		glm::vec2* pointerToPositionOverride();

		// Generate the Element Pointer 
		void storeElements();

		// Store the Element Pointer
		void storeElements(Element** elements, int count);

		// Update Function
		bool updateElement();

		// Blitz Function
		void blitzElement();

		// Link Value Through a Pointer
		void linkValue(void* value_ptr);

		// Move the GUI Element
		void moveElement(float newX, float newY);

		// Return the Default Elements Struct
		DefaultElements* getDefaults();

		// Return Reference to the Element Model
		glm::mat4& getModel();

		// Update the Element Model
		void updateElementModel();

		// Get the Horizontal Offset
		float getHorizontalOffset();

		// Get the Vertical Offset
		float getVerticalOffset();

		// Enable the Master Element and All Child Elements
		void activateElements();

		// Deactivate the Master Element and All Child Elements
		void deactivateElements();

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Store the Pointer to the Viewport Stack
		void storeViewportStack(ViewportStack* stack);

		// Update the Viewport of Master Elements
		void updateViewport();

		// Render Static Objects
		void renderMasterStaticObjects();

		// Render Dynamic Objects
		void renderMasterDynamicObjects();

		// Render Text
		void renderMasterText();

		// Override of Recursive Terrain Generation
		void genTerrainRecursively(int& offset_static, int& offset_dynamic, int& instance, int& instance_index);

		// Generate Terrain From Children of This Object
		void genTerrain(int& offset_static, int& offset_dynamic, int& instance, int& instance_index);

		// Store Scroll Bar
		void storeScrollBar(Render::GUI::ScrollBar& bar);
	};
}

#endif