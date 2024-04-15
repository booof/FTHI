#pragma once
#ifndef ELEMENT_H
#define ELEMENT_H

// This is the Master Class for GUI Objects
// Used for Polymorphism of GUI Elements to be Used in a GUI Window Instance
// Types of GUI Elements Include
// Boxes
// Horizontal and Vertical Scroll Bars
// Text Objects
// Color Wheels
// Toggle Groups
// Other Things Yet to be Implemented

#include "ExternalLibs.h"
#include "Object\Object.h"

// IDEA: Move all Inline Variables to Public Fields of the Master Element
// So there can be Multiple GUIs Being Used Simultaniously

// IDEA: Add New Box Type Called List
// Clicking the Box Creates a Drop Down List of Items to Select

// IDEA: Each GUI Can Have Multiple Master Elements
// Used, For Example, To Have a Section of the GUI That Scroll and Another Part That Doesn't Such as for the Project Selector
// The First Master Element Created Will be the One That is Selected First, Then The Master Elements That Follow After


namespace Render::GUI
{
	// Declaration of Master Element
	class MasterElement;

	// Element Type List
	enum ElementList : uint8_t
	{
		MASTER,
		TEXT,
		BOX,
		TOGGLE_GROUP,
		SCROLL_BAR,
		GRID,
		COLOR_WHEEL
	};

	// General Data for All Elements
	struct ElementData
	{
		// Constant Position of Object
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Script
		int script = 0;

		// Determines if Object is Static (Does Not Scroll)
		bool is_static = false;

		// Helpful Data to Determine What Type of Element an Object is
		uint8_t element_type;
	};

	// Structure for Viewport Data
	struct ViewportInstance
	{
		GLuint x = 0;
		GLuint y = 0;
		GLuint width = 0;
		GLuint height = 0;
		GLfloat* matrix_ptr = nullptr;
	};

	// GUI Element Class
	class Element : public Object::Object
	{
	protected:

		// The Element Data 
		ElementData element_data;

		// The Pointer to the Element's Master Element
		// All Elements, Except the Primary Master Element, Has a Master Element
		MasterElement* master = nullptr;

	public:

		// Update Function
		virtual bool updateElement() = 0;

		// Blitz Function
		virtual void blitzElement() = 0;

		// Link Value Through a Pointer
		virtual void linkValue(void* value_ptr) = 0;

		// Move the GUI Element
		virtual void moveElement(float newX, float newY) = 0;

		// Update the Object
		void updateObject();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();

		// Return Position of Object
		glm::vec2 returnPosition();

		// Update Selected Position of Object
		void updateSelectedPosition(float deltaX, float deltaY);

		// Returns the Type of Element
		uint8_t getElementType();
	};

	// If True, an Element is Being Modified by User
	inline bool modified_by_user = false;

	// Set to True if an Element was Modified
	inline bool was_modified = false;

	// The Horizontal Scroll Offset
	inline float horizontal_scroll_offset = 0.0f;

	// The Vertical Scroll Offset
	inline float vertical_scroll_offset = 0.0f;

	// The Position of the Mouse
	inline glm::vec2 gui_mouse_position = glm::vec2(0.0f, 0.0f);
}

#endif