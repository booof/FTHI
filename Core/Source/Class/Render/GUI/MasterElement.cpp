#include "MasterElement.h"

#include "Globals.h"

#include "HorizontalScrollBar.h"
#include "VerticalScrollBar.h"

void GUI::MasterElement::updatePointers()
{
	// If Horizontal Scroll Bar Exists, Get Horizontal Percent Pointer
	if (defaults->horizontal_bar != nullptr)
		horizontal_offset = &defaults->horizontal_bar->BarOffset;

	// If Horizontal Scroll Bar Exists, Get Horizontal Percent Pointer
	if (defaults->vertical_bar != nullptr)
		vertical_offset = &defaults->vertical_bar->BarOffset;
}

GUI::MasterElement::MasterElement(glm::vec2 position, float width, float height, DefaultElements* defaults_)
{
	// Store Values
	gui_position = position;
	gui_width = width;
	gui_height = height;
	defaults = defaults_;

	// Get Pointers to Objects
	updatePointers();
}

GUI::MasterElement::MasterElement(glm::vec2 position, float width, float height)
{
	// Store Values
	gui_position = position;
	gui_width = width;
	gui_height = height;
}

GUI::MasterElement::~MasterElement()
{
	// Delete Default Structure
	//if (defaults != nullptr)
	//	delete defaults;
}

bool GUI::MasterElement::updateElement()
{
	// Test if Mouse is Inside GUI

	// Set The Modified Element Flags
	modified_by_user = was_modified;
	was_modified = false;

	// Set Default ScrollBar

	// Update Mouse Position
	updateElementModel();

	// Return True as GUI is Being Updated
	return true;
}

void GUI::MasterElement::blitzElement()
{
	// Object Has no Vertices
}

void GUI::MasterElement::linkValue(void* value_ptr)
{
	// Delete Existing Default Struct
	if (defaults != nullptr)
		delete defaults;

	// Reinterpret Pointer as Defualt Elements
	defaults = static_cast<DefaultElements*>(value_ptr);

	// Get Pointers to Objects
	updatePointers();
}

void GUI::MasterElement::moveElement(float newX, float newY)
{
	// Move Offset of Mouse
	gui_position = glm::vec2(newX, newY);
}

GUI::DefaultElements* GUI::MasterElement::getDefaults()
{
	return defaults;
}

glm::mat4& GUI::MasterElement::getModel()
{
	return element_model;
}

void GUI::MasterElement::updateElementModel()
{
	element_model = glm::mat4(1.0f);
	if (horizontal_offset == nullptr)
		gui_mouse_position.x = Global::mouseX / Global::zoom_scale + gui_position.x;
	else
	{
		gui_mouse_position.x = Global::mouseX / Global::zoom_scale - *horizontal_offset + gui_position.x;
		element_model = glm::translate(element_model, glm::vec3(*horizontal_offset, 0.0f, 0.0f));
	}
	if (vertical_offset == nullptr)
		gui_mouse_position.y = Global::mouseY / Global::zoom_scale + gui_position.y;
	else
	{
		gui_mouse_position.y = Global::mouseY / Global::zoom_scale - *vertical_offset + gui_position.y;
		element_model = glm::translate(element_model, glm::vec3(0.0f, *vertical_offset, 0.0f));
	}
}

float GUI::MasterElement::getHorizontalOffset()
{
	// If Horizontal Offset is Nullptr, Return 0
	if (horizontal_offset == nullptr)
		return 0;

	// Else, Get Value
	return *horizontal_offset;
}

float GUI::MasterElement::getVerticalOffset()
{
	// If Vertical Offset is Nullptr, Return 0
	if (vertical_offset == nullptr)
		return 0;

	// Else, Get Value
	return *vertical_offset;
}
