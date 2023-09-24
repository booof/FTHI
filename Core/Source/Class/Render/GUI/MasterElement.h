#pragma once
#ifndef MASTER_ELEMENT_H
#define MASTER_ELEMENT_H

// This Class is Meant to Controll Interactions Between GUI Elements
// Handles Events if an Element is Being Modified
// There Should Only be 1 Master Element at a Time
// Should be Called at the Beginning or End of the "Update Elements" Loop (Preferably Beginning)
// IDEA: There Should be a Test to See if Mouse is Inside GUI. If True, All Other Elements in GUI Will be Tested 

#include "Element.h"

namespace GUI
{
	class ScrollBar;
	class HorizontalScrollBar;
	class VerticalScrollBar;

	// Struct Containing Indices of Default Objects
	struct DefaultElements
	{
		// Default Bar
		ScrollBar* default_bar = nullptr;

		// Horizontal ScrollBar
		HorizontalScrollBar* horizontal_bar = nullptr;

		// Vertical ScrollBar
		VerticalScrollBar* vertical_bar = nullptr;
	};

	// Master Element Class
	class MasterElement : public Element
	{
		// Default Elements
		DefaultElements* defaults = nullptr;

		// Position of GUI Relative to Center of Screen
		glm::vec2 gui_position = glm::vec2(0.0f, 0.0f);

		// Width of the GUI
		float gui_width = 0.0f;

		// Height of he GUI
		float gui_height = 0.0f;

		// Pointer to Horizontal Offset
		float* horizontal_offset = nullptr;

		// Pointer to Vertical Percent
		float* vertical_offset = nullptr;

		// Model Matrix for Elements
		glm::mat4 element_model = glm::mat4(1.0f);
		
		// Update Pointers of Object
		void updatePointers();

	public:

		// Constructor
		MasterElement(glm::vec2 position, float width, float height, DefaultElements* defaults_);

		// Constructor Minus Defaults
		MasterElement(glm::vec2 position, float width, float height);

		// Default Constructor
		MasterElement() {};

		// Deconstructor
		~MasterElement();

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
	};
}

#endif