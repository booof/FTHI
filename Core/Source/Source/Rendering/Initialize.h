#pragma once
#ifndef INITIALIZE_H
#define INITIALIZE_H

#include "ExternalLibs.h"

// GUI Data for Construction
#include "Class/Render/GUI/Box.h"
#include "Class/Render/GUI/ToggleGroup.h"
#include "Class/Render/GUI/TextObject.h"

namespace Source::Rendering::Initialize
{
	// Initialize Program
	bool initialize();

	// WGL Extension Support Finder
	bool WGLExtensionSupported(const char* extension_name);

	// Construct R-Value Box
	Render::GUI::BoxDataBundle constrtuctBox(uint8_t mode, float xPos, float yPos, float zPos, float width, float height, bool centered, std::string text, glm::vec4 background_color, glm::vec4 outline_color, glm::vec4 highlight_color, glm::vec4 text_color);;

	// Construct R-Value Grouper
	Render::GUI::ToggleGroupData constructGrouper(uint8_t group_count, uint8_t initial_value);

	// Construct Text Object
	Render::GUI::TextDataBundle constructText(float xPos, float yPos, float scale, glm::vec4 color, bool static_, std::string text);
}

#endif
