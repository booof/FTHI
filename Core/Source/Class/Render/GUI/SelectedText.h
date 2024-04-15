#pragma once
#ifndef SELECTED_TEXT_H
#define SELECTED_TEXT_H

// This is A Singleton Class With the Single Purpose of Allowing
// A User to Easily Type Through a GUI

// To Activate the Object, Give it a Pointer to an Advanced String.
// The Object Will Continue to Edit That String Until a New Advanced
// String Pointer is Given or a Function is Called to Stop Selecting

// This Object Will Include a Small VAO That Will Render a Text Icon
// On the String Currently Being Edited. It Will Dynamically Fit the
// Scale of the Text Being Edited

// Might Include Some Animations, Such as Blinking When Idleing (Not
// Adding Text for 0.5 seconds), or When an Invalid Operation is Given,
// Such as Pressing Right Arrow At the End of Text or Attempting to Add
// A Number to an Alphabetial String

// This Object Needs to be Initialized In Order for the Text Icon
// To be Renderable

#include "ExternalLibs.h"

#define CURSOR_BLINK_TIMER 0.5
#define CURSOR_FAST_BLINK_TIMER 0.1
#define CURSOR_IDLE_TIMER 1.0
#define CURSOR_FAST_LENGTH 0.6

#define CURSOR_WIDTH 0.2f

#define USE_BACKSLASH true

namespace Render::GUI
{
	// Declaration for Advanced String
	class AdvancedString;

	// Declaration for Box
	class Box;

	// Enums for The Current Text Mode
	enum TEXTMODES : uint8_t
	{
		GENERAL_TEXT = 0,
		ALPHABETICAL_TEXT,
		NUMERICAL_TEXT,
		ABSOLUTE_NUMERICAL_TEXT,
		INTEGER_TEXT,
		ABSOLUTE_INTEGER_TEXT,
		PATH_TEXT
	};

	// The Selected Text Singleton
	class SelectedText
	{
		// The Singleton Instance
		static SelectedText instance;

		// The Pointer to the Advanced String Object Being Edited
		AdvancedString* text = nullptr;

		// Determines if the Object is Being Used
		bool active = false;

		// The Mode of the Object
		uint8_t mode = 0;

		// The Number of Characters in the String
		int character_count = 0;

		// The Current Index To Place a Character
		int text_index = 0;

		// The Width of the String Based on Scale
		float string_width = 0.0f;

		// The Width of the Text Object
		float box_width = 0.0f;

		// The X-Position of the box
		float box_x = 0.0f;

		// Vertex Object for Cursor
		GLuint VAO = 0;
		GLuint VBO = 0;

		// Model Matrix for Cursor
		glm::mat4 model = glm::mat4(1.0f);

		// Position of Text in Box
		glm::vec2 text_pos = glm::vec2(0.0f, 0.0f);

		// A Function Pointer to the Closing Function
		std::function<void()> closing_func = nullptr;

		// A Pointer to the Box Whos Text is Selected
		Box* box = nullptr;

		// The Backup String of the Text's Initial Value
		std::string backup = "";

		// Cursor Timer Values
		double cursor_idle_timer  = 0.0f;
		bool   cursor_idle_toggle = false;
		double cursor_timer       = 0.0f;
		bool   cursor_toggle      = false;
		double fast_cursor_timer  = 0.0f;
		bool   fast_cursor_toggle = false;

		// Audio Source
		ALuint exclaimation_buffer = NULL, exclaimation_source = NULL;

		// Initialize Object
		SelectedText() {}

		// Call This Function Whenver an Invalid Input Occours
		void handleInvalidInput();

		// Update the Rendering of the Selected Text Object
		void updateSelectedText();

		// Parse the String to Test if it Satisfies The Current Mode
		bool parseCharacter(char c);

		// Toggles the Negative Sign of a String
		void toggleNegative();

		// Set the Height of Cursor
		void setCursorHeight(float new_scale);

	public:

		// Remove the Copy Constructor
		SelectedText(const SelectedText&) = delete;

		// Initialize Object
		void initializeSelectedText();

		// Return the Singleton
		static SelectedText* get();

		// Delete the Singleton
		void deleteSelectedText();

		// Assign a Text Object to Edit
		void assignText(AdvancedString* text_, glm::vec2 text_pos_, uint8_t mode_, Box* box_);

		// Force Update Text of Object
		void forceUpdateText(std::string& new_string);

		// Stop Editing Text
		void stopSelecting();

		// Insert a Character Into the Object
		void insertCharacter(char character);

		// Delete a Character From the Object
		void removeCharacter();

		// Move Cursor Left
		void moveLeft();

		// Move Cursor Right
		void moveRight();

		// Set Cursor to the Top of the Text
		void moveToTop();

		// Set Cursor to the Bottom of the Text
		void moveToBottom();

		// Render the Cursor
		void renderCursor();

		// Assign the Closing Function
		void assignCloser(std::function<void()> closer);

		// Get the Current String
		std::string& getString();

		// Returns True if Active
		bool isActive();

		// Clears the Text
		void clearText();

		// Sets the Text to the Backup Value
		void setBackup();

		// Returns the Backup String
		std::string& getBackup();

		// Returns the Box Pointer
		Box* getBox();
	};
}

#endif
