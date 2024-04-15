#pragma once
#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include "ExternalLibs.h"
#include "Element.h"

namespace Render::GUI
{
	// Struct for Initializing Text
	struct TextData
	{
		// Scale of Text
		float scale = NULL;

		// Number of Characters in Initial Text
		int text_size = 0;

		// Color
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Text
		std::string text = "";
	};

	// Combined Struct for Full Box Data
	struct TextDataBundle
	{
		ElementData data1;
		TextData data2;
	};

	// Object for Rendering Text
	class TextObject : public Element
	{
		// Text Data
		TextData data;

	public:

		// Initialize Text
		TextObject(ElementData& data1, TextData& data2);

		// Default Constructor
		TextObject() {}

		// Render Text
		void blitzText();

		// Render Offset Text
		void blitzOffsetText();

		// Render Global Text
		void blitzGlobalText();

		// Swap Text
		void swapText(std::string new_text);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Update Function
		bool updateElement();

		// Blitz Function
		void blitzElement();

		// Link Value Through a Pointer
		void linkValue(void* value_ptr);

		// Move the GUI Element
		void moveElement(float newX, float newY);
	};
}

#endif