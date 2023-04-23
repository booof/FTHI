#include "TextObject.h"

// Functions
#include "Source/Loaders/Fonts.h"

GUI::TextObject::TextObject(TextData data_)
{
	// Store Text Data
	data = data_;
}

void GUI::TextObject::blitzText()
{
	// Draw Text
	Source::Fonts::renderText(data.text, data.position.x, data.position.y, data.scale, data.color, data.static_);
}

void GUI::TextObject::blitzOffsetText()
{
	// Draw Text
	Source::Fonts::renderTextOffsetAdvanced(data.text, data.position.x, data.position.y, data.scale, data.color, 0, false);
}

void GUI::TextObject::blitzGlobalText()
{
	// Draw Text
	Source::Fonts::renderTextGlobal(data.text, data.position.x, data.position.y, data.scale, data.color);
}

void GUI::TextObject::swapText(std::string new_text)
{
	data.text = new_text;
}
