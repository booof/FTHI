#pragma once
#ifndef BOX_H
#define BOX_H

#include "Element.h"

namespace GUI
{
	class ToggleGroup;

	// Box Modes
	enum BOX_MODES : uint8_t
	{
		GENERAL_TEXT_BOX,
		ALPHABETICAL_TEXT_BOX,
		NUMERICAL_TEXT_BOX,
		ABSOLUTE_NUMERICAL_TEXT_BOX,
		INTEGER_TEXT_BOX,
		ABSOLUTE_INTEGER_TEXT_BOX,
		TOGGLE_BOX,
		GROUPED_BOX,
		FUNCTION_BOX,
		NULL_BOX
	};

	// Struct for Initializing Box
	struct BoxData
	{
		// Type of Box
		// 0 - 5 = Text Box With Corrisponding Text Mode
		// 6 = Toggle Box
		// 7 = Toggle Box With Multiple Boxes 
		// 8 = Function Pointer
		// 9 = Box That Does Not Change
		uint8_t mode = 0;

		// Midpoint Position of Box
		glm::vec2 position = glm::vec2(0.0f);

		// Z-Position of Box
		float zpos = -1.0f;

		// Width of Box
		float width = 1.0f;

		// Height of Box
		float height = 1.0f;

		// Determines if the Text is Centered
		bool centered = true;

		// Text
		std::string button_text = "";

		// Background Color
		glm::vec4 background_color = glm::vec4(1.0f);

		// Outline Color
		glm::vec4 outline_color = glm::vec4(1.0f);

		// Highlight Color
		glm::vec4 highlight_color = glm::vec4(1.0f);

		// Text Color
		glm::vec4 text_color = glm::vec4(1.0f);
	};

	// Box Class for GUIs
	class Box : public Element
	{
		// Data
		BoxData data;

		// Lower Left Corner of Box
		glm::vec2 lower_left;

		// State of Object
		bool state = false;

		// Determines if Box is Highlighted
		bool highlighted = false;

		// Pointer to Grouper (If Mode is Set to Grouped)
		ToggleGroup* grouper;
		uint8_t group_index = 0;

		// Pointer to a Function (If Mode is Set to Function)
		std::function<void()> funct;

		// Pointer to Modifiable Data (If Mode is Set to Text)
		void* data_pointer;

		// Vertex Objects
		GLuint VAO, VBO;

		// Model Matrix
		glm::mat4 model;

		// Position of Text
		float text_x = 0;
		float text_y = 0;

		// Store Position of Object
		void storePositions();

		// Initialize Vertices
		void initializeVertices();

		// Update Text
		void updateText();

	public:

		// Object Text is Currently Being Edited
		bool texting = false;

		// Initialize Box
		Box(BoxData& data_);

		// Defualt Constructor
		Box();

		// Move the Box
		void moveElement(float new_x, float new_y);

		// Link Value Through a Pointer
		void linkValue(void* value);

		// Set Data Pointer
		void setDataPointer(void* data_pointer_);

		// Set Function Pointer
		void setFunctionPointer(std::function<void()> funct_);

		// Group Box
		void groupBox(ToggleGroup* grouper_, uint8_t index);

		// Set Group Box to False
		void setGroupFalse();

		// Set State
		bool updateElement();

		// Set State to True
		void setTrue();

		// Set State to False
		void setFalse();

		// Get State
		bool getState();

		// Get Text
		std::string getText();

		// Blitz Box
		void blitzElement();

		// Blitz Text
		void blitzText(float offset = 0.0f);

		// Blitz Offset Text
		void blitzOffsetText();

		// Blitz Global Text
		void blitzGlobalText();
	};
}

#endif
