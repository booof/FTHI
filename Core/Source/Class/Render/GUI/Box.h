#pragma once
#ifndef BOX_H
#define BOX_H

#include "Element.h"
#include "AdvancedString.h"
#include "VerticalScrollBar.h"

namespace Render::GUI
{
	class ToggleGroup;

	// Box Modes
	enum BOX_MODES : uint8_t
	{
		NULL_BOX,
		TOGGLE_BOX,
		FUNCTION_BOX,
		DROP_DOWN_BOX,
		GENERAL_TEXT_BOX,
		ALPHABETICAL_TEXT_BOX,
		NUMERICAL_TEXT_BOX,
		ABSOLUTE_NUMERICAL_TEXT_BOX,
		INTEGER_TEXT_BOX,
		ABSOLUTE_INTEGER_TEXT_BOX,
		FILE_PATH_BOX,
		GROUPED_BOX // Should Be Last Since Should Not be Accessed by Any Users (Only Through Toggle Group Object)
	};

	// Struct for Initializing Box
	struct BoxData
	{
		// Type of Box
		// 0 - 5 = Text Box With Corrisponding Text Mode
		// 6 = Opens File Explorer on Click
		// 7 = Toggle Box
		// 8 = Toggle Box With Multiple Boxes 
		// 9 = Function Pointer
		// 10 = Box That Does Not Change
		// 11 = Box With Drop-Down Menu
		uint8_t mode = 0;

		// Z-Position of Box
		float zpos = -1.0f;

		// Width of Box
		float width = 1.0f;

		// Height of Box
		float height = 1.0f;

		// Determines if the Text is Centered
		bool centered = true;

		// Text
		AdvancedString button_text;

		// Background Color
		glm::vec4 background_color = glm::vec4(1.0f);

		// Outline Color
		glm::vec4 outline_color = glm::vec4(1.0f);

		// Highlight Color
		glm::vec4 highlight_color = glm::vec4(1.0f);

		// Text Color
		glm::vec4 text_color = glm::vec4(1.0f);
	};

	// Drop Down Data
	struct DropDownData
	{
		// The Number of Items in The Menu
		uint8_t item_count = 0;

		// The Currently Selected Item
		uint8_t current_index = 0;

		// The Number of Characters in the Menu
		uint16_t character_count = 0;

		// The Height of Each Item and Size of Text
		float item_height = 5.0f;

		// The Max Height of the Menu that Appears when Dropped Down
		float max_menu_height = 30.0f;

		// Scroll Bar Width
		float bar_width = 0.5f;

		// The Color of the Background
		glm::vec4 menu_background_color = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

		// The Color of the Menu Highlight
		glm::vec4 menu_highlight_color = glm::vec4(0.7f, 0.7f, 0.7f, 0.8f);

		// The Color of the Menu Text
		glm::vec4 menu_text_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// The Text Used in the Menu
		char** text = nullptr;
	};

	// Combined Struct for Full Box Data
	struct BoxDataBundle
	{
		ElementData data1;
		BoxData data2;
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

		// Pointer to Modifiable Data (If Mode is Set to Text/Drop-Down)
		void* data_pointer;

		// Vertex Objects
		GLuint VAO, VBO;

		// Model Matrix
		glm::mat4 model;

		// Position of Text
		float text_x = 0;
		float text_y = 0;

		// The Drop Down Data, If Needed
		DropDownData* drop_down_data = nullptr;

		// The Vertex Objects for the Drop Down Menu
		static GLuint DropDownVAO, DropDownVBO;

		// The Scroll Bar for the Drop Down Menu
		static VerticalScrollBar DropDownBar;

		// The List of Advanced Strings to Draw
		static std::vector<AdvancedString> DropDownStrings;

		// The Drop Down Box that is Currently Active
		static Box* DropDownActivePtr;

		// The Currently Moused Over Drop Down Item
		static uint8_t DropDownMousedItem;

		// The 4 Corners of the Clipping Region (At Default Position)
		static glm::ivec4 DropDownClipCorners;

		// The Ratio Between Bar Offsets and Device Coordinates
		static float DropDownBarRatio;

		// Store Position of Object
		void storePositions();

		// Initialize Vertices
		void initializeVertices();

		// Update Text
		void updateText();

		// Access the Drop Down Menu
		void accessDropDown();

		// Helper Function to Draw Drop Down Menu
		void drawDropDownHelper();

		// Helper Function to Draw Drop Down Text
		void drawDropDownTextHelper();

	public:

		// Object Text is Currently Being Edited
		bool texting = false;

		// Initialize Box
		Box(ElementData& data1, BoxData& data2);

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

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Store Drop Down Data
		void storeDropDownData(DropDownData* new_drop_down_data);

		// Draw the Drop Down Menu, If It Exists
		static void drawDropDown();

		// Draw the Text of the Drop Down Menu, If It Exisits
		static void drawDropDownText();
	};
}

#endif
