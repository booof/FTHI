#pragma once
#ifndef ADVANCED_STRING_H
#define ADVANCED_STRING_H

#include "ExternalLibs.h"

// This is a Wrapper Class for a String With Extra Features

// This Object Will Act Similar to a String, But Will Have Features
// To Make it Compatable With Advanced Text Rendering and Text Editing

// The Primary Goal is to Reduce the Rendering Overhead of the Advanced
// Render Text Function by Making the Size Calculation and Centering a
// One-And-Done Operation, Unless the Text is Edited or the Font is Changed

namespace Render::GUI
{
	// The Advanced String Wrapper Class
	class AdvancedString
	{
		// The String Data of the Object
		std::string data = "";

		// The String That Will be Rendered
		std::string render_string = "";

		// The Maximum Length of the String
		float max_length = 0.0f;

		// The Scale of the String
		float scale = 0.1f;

		// The Offset of the String
		float offset = 0.0f;

		// Determines if the Text is Centered
		bool centered = false;

	public:

		// Default Constructor
		AdvancedString() {}

		// Constructor With an Initial String
		AdvancedString(std::string string);

		// Constructor With an Initial String and Length
		AdvancedString(std::string string, GLfloat max_length_, GLfloat scale_, bool centered_);

		// Update the Render Text
		void updateRenderText();

		// Update the Render Text Selected
		float updateRenderTextSelected(float unused_space);

		// Calculates the Max Size of the Text
		float calculateTextSize();

		// Calculates the Size of a Character based on Size
		float calculateCharacterWidth(char c);

		// Calculates the Size of a Chatacter in the Given Index
		float calculateCharacterWidth(int index);

		// Calculates the Size Leading Up to An Index
		float calculateSizeTillIndex(int index);

		// Returns the String
		std::string& getString();

		// Sets the String
		void setString(std::string string);

		// Returns the Render String
		std::string& getRenderString();

		// Clear the Rendering Data
		void clearRenderingData();

		// Sets the Advanced String Values
		void setAdvancedValues(GLfloat max_length_, GLfloat scale_, bool centered_);

		// Set the Scale of the Object
		void setScale(GLfloat scale_);

		// Gets the Offset of the Render String
		float getOffset();

		// Gets the Width of the Box
		float getBoxSize();

		// Get the Scale of the Object
		float getScale();

		// Get the Number of Characters in String
		int getCharacterCount();

		// Insert a Character at the Given Index
		void insertCharacter(char c, int index);

		// Remove a Character at the Given Index
		void removeCharacter(int index);
	};
}

#endif
