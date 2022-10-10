#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include "ExternalLibs.h"

namespace GUI
{
	// Struct for Initializing Text
	struct TextData
	{
		// Position
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Scale
		float scale = NULL;

		// Color
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Static
		bool static_ = false;

		// Text
		std::string text = "";
	};

	// Object for Rendering Text
	class TextObject
	{
		// Text Data
		TextData data;

	public:

		// Initialize Text
		TextObject(TextData data_);

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
	};
}

#endif