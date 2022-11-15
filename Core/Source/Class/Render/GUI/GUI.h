#ifndef GUI_H
#define GUI_H

// This is the Class That Defines a GUI Instance
// This Class is Able to Read GUI Files, Store GUI Elements, and Allow User Interactions

#include "Element.h"

namespace GUI
{
	// Element Section in a GUI Structure
	struct ElementSection
	{
		// Array of Elements
		// Prefered Array Layout : Master, Toggle Groups, Shapes, ScrollBars, ColorWheels, Grids, Boxes, TextObjects
		Element** element_array = nullptr;

		// Size of Array
		int element_array_size = 0;

		// Starting Index of Shapes (Index to Begin Rendering)
		int shape_start = 0;

		// Starting Index of Boxes (Index to Begin Rendering Text)
		int boxes_start = 0;
	};

	// The Main GUI Class
	class GUI
	{
		// The Path to the GUI File
		std::string gui_path = "";

		// The Array of GUI Sections
		// First Section Gets Updated First, and Rendered Last
		ElementSection* element_sections = nullptr;

		// The Number of GUI Sections
		int section_count = 0;
	};
}

#endif
