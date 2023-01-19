#pragma once
#ifndef TOGGLE_GROUP_H
#define TOGGLE_GROUP_H

#include "ExternalLibs.h"
#include "Box.h"

namespace GUI
{
	// Struct for Grouping Multiple Toggle Boxes
	struct ToggleGroupData
	{
		// Number of Boxes To Be Grouped Proceding This Object
		uint8_t group_count;

		// Initial Enemuerated Value
		uint8_t initial_value;
	};

	// Toggle Group Object
	class ToggleGroup
	{
		// Object Data
		ToggleGroupData data;

		// Toggle Pointer
		uint8_t* toggle_pointer;

		// Array of Grouped Boxes
		Box** grouped_boxes;

		// Default Pointer Target
		uint8_t dummy_var = 0;

	public:
		
		// Initialize Object From File
		ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, std::ifstream& file);

		// Initialize Object With Box Data
		ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, BoxData* datas);

		// Default Constructor
		ToggleGroup();

		// Set Pointer
		void setDataPointer(uint8_t* var);

		// Get Change State from Grouped Boxes
		void changeState(uint8_t index);
	};
}

#endif
