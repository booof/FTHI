#include "ToggleGroup.h"

GUI::ToggleGroup::ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, std::ifstream& file)
{
	// Store Data
	data = std::move(data_);

	// Store Pointer to Dummy Var
	toggle_pointer = &dummy_var;

	// Generate Array of Group Boxes
	grouped_boxes = new Box*[data.group_count];

	// Create Grouped Boxes
	uint8_t array_index;
	for (uint8_t i = 0, j = first_box_index; i < data.group_count; i++, j++)
	{
		// Calculate Index in Box Array
		array_index = i + first_box_index;

		// Read Box
		BoxData box_data;
		file.read((char*)&box_data, sizeof(box_data));

		// Store Box in Box Array
		box_array[j] = Box(box_data);

		// Store Pointer to Box in Group Array
		grouped_boxes[i] = &(box_array[j]);

		// Group Box
		box_array[j].groupBox(this, i);
	}
}

GUI::ToggleGroup::ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, BoxData* datas)
{
	// Store Data
	data = data_;

	// Store Pointer to Dummy Var
	toggle_pointer = &dummy_var;

	// Generate Array of Group Boxes
	grouped_boxes = new Box*[data.group_count];

	// Create Grouped Boxes
	uint8_t array_index;
	for (uint8_t i = 0, j = first_box_index; i < data.group_count; i++, j++)
	{
		// Calculate Index in Box Array
		array_index = i + first_box_index;

		// Store Box in Box Array
		box_array[j] = Box(datas[i]);

		// Store Pointer to Box in Group Array
		grouped_boxes[i] = &(box_array[j]);

		// Group Box
		box_array[j].groupBox(this, i);
	}
}

GUI::ToggleGroup::ToggleGroup() { toggle_pointer = &dummy_var; }

void GUI::ToggleGroup::setDataPointer(uint8_t* var)
{
	// Store Pointer
	toggle_pointer = var;

	// Group Boxes Again Because C++ Decided to be Gay
	for (int i = 0; i < data.group_count; i++)
		grouped_boxes[i]->groupBox(this, i);

	// Toggle Currently Active Box
	grouped_boxes[*var]->setTrue();
}

void GUI::ToggleGroup::changeState(uint8_t index)
{
	// Dont Do Anythin if Index Did Not Change
	if (*toggle_pointer - data.initial_value == index)
		return;

	// Set All Boxes to False
	for (int i = 0; i < data.group_count; i++)
		grouped_boxes[i]->setGroupFalse();

	// Update Value Pointer
	*toggle_pointer = index + data.initial_value;

	// Set Index Box to True
	grouped_boxes[index]->setTrue();
} 


