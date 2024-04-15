#include "ToggleGroup.h"
#include "Globals.h"
#include "Source\Vertices\Visualizer\Visualizer.h"

Render::GUI::ToggleGroup::ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, std::ifstream& file)
{
	// Store Data
	data = std::move(data_);

	// Store Pointer to Dummy Var
	toggle_pointer = &dummy_var;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Generate Array of Group Boxes
	grouped_boxes = new Box*[data.group_count];

	// Create Grouped Boxes
	uint8_t array_index;
	for (uint8_t i = 0, j = first_box_index; i < data.group_count; i++, j++)
	{
		// Calculate Index in Box Array
		array_index = i + first_box_index;

		// Read Box
		BoxDataBundle box_data;
		file.read((char*)&box_data, sizeof(box_data));

		// Store Box in Box Array
		box_array[j] = Box(box_data.data1, box_data.data2);

		// Store Pointer to Box in Group Array
		grouped_boxes[i] = &(box_array[j]);

		// Group Box
		box_array[j].groupBox(this, i);
	}
}

Render::GUI::ToggleGroup::ToggleGroup(ToggleGroupData& data_, Box* box_array, uint8_t first_box_index, BoxDataBundle* datas)
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
		box_array[j] = Box(datas[i].data1, datas[i].data2);

		// Store Pointer to Box in Group Array
		grouped_boxes[i] = &(box_array[j]);

		// Group Box
		box_array[j].groupBox(this, i);
	}
}

Render::GUI::ToggleGroup::ToggleGroup() { toggle_pointer = &dummy_var; }

void Render::GUI::ToggleGroup::setDataPointer(uint8_t* var)
{
	// Store Pointer
	toggle_pointer = var;

	// Group Boxes Again Because C++ Decided to be Gay
	for (int i = 0; i < data.group_count; i++)
		grouped_boxes[i]->groupBox(this, i);

	// Toggle Currently Active Box
	grouped_boxes[*var]->setTrue();
}

void Render::GUI::ToggleGroup::changeState(uint8_t index)
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
bool Render::GUI::ToggleGroup::testMouseCollisions(float x, float y)
{
	return false;
}

bool Render::GUI::ToggleGroup::updateElement()
{
	// Object Does Not Interract With Anything, Do Nothing
	return false;
}

void Render::GUI::ToggleGroup::blitzElement()
{
	// Draw Visualizer Only if In Editing Mode
	if (Global::editing)
		Vertices::Visualizer::visualizePoint(element_data.position, 1.0f, glm::vec4(0.8f, 0.0f, 0.0f, 0.8f));
}

void Render::GUI::ToggleGroup::linkValue(void* value_ptr)
{
	// TODO: Link Value to Match With Toggled Box Index
}

void Render::GUI::ToggleGroup::moveElement(float newX, float newY)
{
	// No Need to do Anything Here, Object is a Visualizer
}

// IDEA FOR SELECTING / EDITING TOGGLE GROUP:
// Starts out as Unsizable Square, Similar to Group Object, and Will be Stuck at 0,0
// Will Grow and Change Size to Encompass All Boxes it Covers
// Will Change Position so In Center of Covered Boxes
// Size Will be Slightly Larger than Distance Between Furthest Boxes
// WILL NOT HAVE AN MODIFIABLE SIZE NOR POSITION
// SIZE AND POSITION ARE ONLY FOR SELECTING, SIZE STORED IN SEPERATE DATA STRUCT, WON'T BE USED IN ENGINE MODE (EXCLUDED THROUGH MACROS)

// ANOTHER IDEA:
// Toggle Group Boxes Can Only be Created Through the Toggle Group Object
// Editor Window for Toggle Group Object Allows Creation and Deletion of Toggle Boxes
// Toggle Group Mode for Boxes Should be Inaccessable in Editor Window

