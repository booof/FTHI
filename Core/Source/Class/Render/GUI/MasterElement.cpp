#include "MasterElement.h"
#include "Render\Struct\DataClasses.h"

#include "Globals.h"

#include "HorizontalScrollBar.h"
#include "VerticalScrollBar.h"

#include "Vertices/Visualizer/Visualizer.h"
#include "Render\Editor\ObjectInfo.h"

#include "Source\Algorithms\Transformations\Transformations.h"

#include "Render\GUI\GUI.h"

// IDEA: Add a Stack for Clipping Regions
// Each Time a Master Element / Other Element is Being Used, Add Clipping to the Stack
// Remove the Clipping Region From Stack When Finished
// If a Region Extends Past Region Already in Stack, Prevent That From Happinging
// Will Potentially Fix the Problem With the Drop Down Menu Clipping Regions

void Render::GUI::MasterElement::updatePointers()
{
	// If Horizontal Scroll Bar Exists, Get Horizontal Percent Pointer
	if (defaults->horizontal_bar != nullptr)
		horizontal_offset = defaults->horizontal_bar->getOffsetPointer();

	// If Horizontal Scroll Bar Exists, Get Horizontal Percent Pointer
	if (defaults->vertical_bar != nullptr)
		vertical_offset = defaults->vertical_bar->getOffsetPointer();
}

Render::GUI::MasterElement::MasterElement(glm::vec2 position, float width, float height, DefaultElements* defaults_)
{
	// Store Values
	gui_position = position;
	gui_width = width;
	gui_height = height;
	defaults = defaults_;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Get Pointers to Objects
	updatePointers();
}

Render::GUI::MasterElement::MasterElement(glm::vec2 position, float width, float height)
{
	// Store Values
	gui_position = position;
	gui_width = width;
	gui_height = height;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}
}

Render::GUI::MasterElement::MasterElement(ElementData& element_data_, MasterData& data_)
{
	// Store Values
	element_data = element_data_;
	data = data_;

	// Store Initial GUI Position
	gui_position = element_data.position;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Set Initial Veiwport Dimensions
	viewport_instance.width = (GLuint)Source::Algorithms::Transformations::transformRelativeScreenWidth(data.width * 2);
	viewport_instance.height = (GLuint)Source::Algorithms::Transformations::transformRelativeScreenHeight(data.height * 2);
	viewport_instance.matrix_ptr = glm::value_ptr(element_model);

	// Generate Initial Default Elements
	defaults = new DefaultElements;
}

Render::GUI::MasterElement::~MasterElement()
{
	// Delete Default Structure
	//if (defaults != nullptr)
	//	delete defaults;
}

glm::vec2* Render::GUI::MasterElement::pointerToPositionOverride()
{
	return nullptr;
}

void Render::GUI::MasterElement::storeElements()
{
	/*
	// If Child List is Empty, Do Nothing
	if (children_size == 0)
		return;

	// Count the Number of Elements in Child List
	element_count = 0;
	for (int i = 0; i < children_size; i++)
	{
		using namespace Object;
		if (children[i]->data_object->getObjectIdentifier()[0] == ObjectList::ELEMENT)
			element_count++;
	}

	// If There Are No Elements, Do Nothing
	if (element_count == 0)
		return;

	// Allocate the Element Array
	element_ptr = new Element
	*/
}

void Render::GUI::MasterElement::storeElements(Element** elements, int count)
{
	element_ptr = elements;
	element_count = count;
}

bool Render::GUI::MasterElement::updateElement()
{
	// NOTE FOR MASTER ELEMENT IMPELEMENTATION
	// Width and Height of Element are to be Changed and Modified by Users
	// Width, Height, and Position are Only for Setting the Clipping Window and Interactable Section of Master Element
	// Width and Height Don't Affect Scrolling Function, Only Size of Scroll Region in ScrollBar Does
	// Scroll Bars Simply Move Children of Master Element, Does Not Effect The Actual Master Element Unless Master Element is Nested Inside another Master Element That Was Scrolled
	// TODO: Add a Stack in GUIs That Stores the Clipping and Interactable Regions as Master Elements Are Processed

	// If Mouse was Updated, Update Elements
	if (Global::LeftClick || Global::cursor_Move)
	{
		// Test if Mouse is Inside GUI (Implement When Multiple Master Elements are Added)

		// Set The Modified Element Flags
		modified_by_user = was_modified;
		was_modified = false;

		// Update Mouse Position
		updateElementModel();

		// Iterate Through Elements and Update Them If Modified
		for (int i = 0; i < element_count; i++)
		{
			if (element_ptr[i]->updateElement())
				break;
		}

		// Iterate Through Children and Update Them If Modified
		// Will be Replaced Later
		for (int i = 0; i < children_size; i++)
		{
			using namespace Object;
			if (children[i]->data_object->getObjectIdentifier()[0] == ObjectList::ELEMENT)
			{
				if (static_cast<Render::GUI::Element*>(children[i])->updateElement())
					break;
			}
		}

		// Return True as GUI is Being Updated
		return true;
	}

	return false;
}

void Render::GUI::MasterElement::blitzElement()
{
	// Object Has no Vertices

	// If Using Editing Mode, Draw a Single Point for Visualization
	if (Global::editing)
	{
		Vertices::Visualizer::visualizePoint(element_data.position, 2.0f, glm::vec4(0.3f, 0.3f, 0.3f, 0.5f));
		glm::mat4 temp_matrix = glm::mat4(1.0f);
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, false, glm::value_ptr(temp_matrix));
	}
}

void Render::GUI::MasterElement::linkValue(void* value_ptr)
{
	// Delete Existing Default Struct
	if (defaults != nullptr)
		delete defaults;

	// Reinterpret Pointer as Defualt Elements
	defaults = static_cast<DefaultElements*>(value_ptr);

	// Get Pointers to Objects
	updatePointers();
}

void Render::GUI::MasterElement::moveElement(float newX, float newY)
{
	// Move Offset of Mouse
	gui_position = glm::vec2(newX, newY);
}

Render::GUI::DefaultElements* Render::GUI::MasterElement::getDefaults()
{
	return defaults;
}

glm::mat4& Render::GUI::MasterElement::getModel()
{
	return element_model;
}

void Render::GUI::MasterElement::updateElementModel()
{
	if (Global::editing)
	{
		element_model = glm::mat4(1.0f);
		if (horizontal_offset == nullptr)
			gui_mouse_position.x = Global::mouseX / Global::zoom_scale;
		else
		{
			gui_mouse_position.x = Global::mouseX / Global::zoom_scale - *horizontal_offset;
			element_model = glm::translate(element_model, glm::vec3(*horizontal_offset, 0.0f, 0.0f));
		}
		if (vertical_offset == nullptr)
			gui_mouse_position.y = Global::mouseY / Global::zoom_scale;
		else
		{
			gui_mouse_position.y = Global::mouseY / Global::zoom_scale - *vertical_offset;
			element_model = glm::translate(element_model, glm::vec3(0.0f, *vertical_offset, 0.0f));
		}

		//if (object_index == 14)
		//	std::cout << "Offset: " << *vertical_offset << "\n";

		// Only for GUI Editor: Update Positions of Children
		if (data_object != nullptr)
		{
			glm::vec2 new_offset = glm::vec2(0.0f, 0.0f);
			if (vertical_offset != nullptr)
				new_offset.y = *vertical_offset;
			if (horizontal_offset != nullptr)
				new_offset.x = *horizontal_offset;
			glm::vec2 delta_offset = new_offset - static_cast<DataClass::Data_MasterElement*>(data_object)->getScrollOffsets();
			static_cast<DataClass::Data_MasterElement*>(data_object)->getScrollOffsets() = new_offset;

			if (delta_offset.x != 0.0f || delta_offset.y != 0.0f)
			{
				for (int i = 0; i < children_size; i++)
					children[i]->updateSelectedPosition(delta_offset.x, delta_offset.y);
			}
		}
	}

	else
	{
		element_model = glm::mat4(1.0f);
		if (horizontal_offset == nullptr)
			gui_mouse_position.x = Global::mouseRelativeX;
		else
		{
			gui_mouse_position.x = Global::mouseRelativeX - *horizontal_offset;
			element_model = glm::translate(element_model, glm::vec3(*horizontal_offset, 0.0f, 0.0f));
		}
		if (vertical_offset == nullptr)
			gui_mouse_position.y = Global::mouseRelativeY;
		else
		{
			gui_mouse_position.y = Global::mouseRelativeY - *vertical_offset;
			element_model = glm::translate(element_model, glm::vec3(0.0f, *vertical_offset, 0.0f));
		}
	}

	// Update the Viewport
	updateViewport();
}

float Render::GUI::MasterElement::getHorizontalOffset()
{
	// If Horizontal Offset is Nullptr, Return 0
	if (horizontal_offset == nullptr)
		return 0;

	// Else, Get Value
	return *horizontal_offset;
}

float Render::GUI::MasterElement::getVerticalOffset()
{
	// If Vertical Offset is Nullptr, Return 0
	if (vertical_offset == nullptr)
		return 0;

	// Else, Get Value
	return *vertical_offset;
}

void Render::GUI::MasterElement::activateElements()
{
	// If Already Active, Early Return
	if (active)
		return;

	// Set to Active
	active = true;

	// Set Terrain and Lighting Children to be Active
}

void Render::GUI::MasterElement::deactivateElements()
{
	// If Already Deactivated, Early Return
	if (!active)
		return;

	// Set to Inactive
	active = false;

	// Set Terrain and Lighting Children to be Inactive
}

bool Render::GUI::MasterElement::testMouseCollisions(float x, float y)
{
	// Test Mouse X
	if (x > element_data.position.x - 1.0f && x < element_data.position.x + 1.0f)
	{
		// Test Mouse Y
		if (y > element_data.position.y - 1.0f && y < element_data.position.y + 1.0f)
			return true;
	}

	return false;
}

void Render::GUI::MasterElement::storeViewportStack(ViewportStack* stack)
{
	viewport_stack = stack;
}

void Render::GUI::MasterElement::updateViewport()
{
	// Determine Engine Coordinates of Lower Left Corner
	float x = gui_position.x - data.width * 0.5f;
	float y = gui_position.y - data.height * 0.5f;

	// Convert Positions to Screen Coordinates
	viewport_instance.x = (GLuint)Source::Algorithms::Transformations::transformRelativeScreenCoordsX(x);
	viewport_instance.y = (GLuint)Source::Algorithms::Transformations::transformRelativeScreenCoordsY(y);
}

void Render::GUI::MasterElement::renderMasterStaticObjects()
{
	// Push Viewport
	viewport_stack->pushInstance(viewport_instance);

	// Render Static Objects
	glDrawArrays(GL_TRIANGLES, static_vertex_offset, static_vertex_count);

	// Render Child Master Elements
	for (int i = 0; i < element_count; i++) {
		if (element_ptr[i]->data_object->getObjectIdentifier()[1] == MASTER)
			static_cast<MasterElement*>(element_ptr[i])->renderMasterStaticObjects();
	}

	// Render Static Elements (Change When Boxes and Similar Elements Share Static Object Vertices)
	for (int i = 0; i < element_count; i++) {
		element_ptr[i]->blitzElement();
	}

	// Pop Viewport
	viewport_stack->popInstance();
}

void Render::GUI::MasterElement::renderMasterDynamicObjects()
{
	// Push Viewport
	viewport_stack->pushInstance(viewport_instance);

	// Render Dynamic Objects and Child Master Elements
	glDrawArrays(GL_TRIANGLES, dynamic_vertex_offset, dynamic_vertex_count);

	// Render Child Master Elements
	for (int i = 0; i < element_count; i++) {
		if (element_ptr[i]->data_object->getObjectIdentifier()[1] == MASTER)
			static_cast<MasterElement*>(element_ptr[i])->renderMasterDynamicObjects();
	}

	// Pop Viewport
	viewport_stack->popInstance();
}

void Render::GUI::MasterElement::renderMasterText()
{
	// Push Viewport
	viewport_stack->pushInstance(viewport_instance);

	// Render Text of Elements and Child Master Elements
	for (int i = 0; i < element_count; i++)
	{
		Element* element = element_ptr[i];
		switch (element->data_object->getObjectIdentifier()[1])
		{
			
		// Master Elements 
		case (Render::GUI::MASTER):
		{
			static_cast<MasterElement*>(element)->renderMasterText();
			break;
		}

		// Box Text
		case (Render::GUI::BOX):
		{
			static_cast<Box*>(element)->blitzOffsetText();
			break;
		}


		// Actual Text
		case (Render::GUI::TEXT):
		{
			static_cast<TextObject*>(element)->blitzOffsetText();
			break;
		}

		}
	}

	// Pop Viewport
	viewport_stack->popInstance();
}

void Render::GUI::MasterElement::genTerrainRecursively(int& offset_static, int& offset_dynamic, int& instance, int& instance_index)
{
	// Do Absolutly Nothing, Master Elements are a Dead End
}

void Render::GUI::MasterElement::genTerrain(int& offset_static, int& offset_dynamic, int& instance, int& instance_index)
{
	// Store Current Offsets of Vertices
	static_vertex_offset = offset_static * 0.05;
	dynamic_vertex_offset = offset_dynamic * 0.05;

	// Recursively Generate Terrain from Children
	Object::genTerrainRecursively(offset_static, offset_dynamic, instance, instance_index);

	// Store How Many Vertices Were Added by This Object
	static_vertex_count = offset_static - static_vertex_offset * 0.05;
	dynamic_vertex_count = offset_dynamic - dynamic_vertex_offset * 0.05;
}

void Render::GUI::MasterElement::storeScrollBar(Render::GUI::ScrollBar& bar)
{
	// Test Vertical Scroll Bar
	if (bar.getIdentifier() == data.initial_vertical_bar) {
		defaults->vertical_bar = &bar;
		vertical_offset = bar.getOffsetPointer();
		static_cast<DataClass::Data_MasterElement*>(data_object)->getScrollOffsets().y = *vertical_offset;
		if (data.vertical_is_default)
			defaults->default_bar = &bar;
	}

	// Test Horizontal Scroll Bar
	if (bar.getIdentifier() == data.initial_horizontal_bar) {
		defaults->horizontal_bar = &bar;
		horizontal_offset = bar.getOffsetPointer();
		static_cast<DataClass::Data_MasterElement*>(data_object)->getScrollOffsets().x = *horizontal_offset;
		if (!data.vertical_is_default)
			defaults->default_bar = &bar;
	}
}

// Function to Read Data and Create an Object
Object::Object* DataClass::Data_MasterElement::genObject(glm::vec2& offset)
{
	return new Render::GUI::MasterElement(element_data, data);
}

void DataClass::Data_MasterElement::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.write((char*)&data, sizeof(Render::GUI::MasterData));
}

void DataClass::Data_MasterElement::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&element_data, sizeof(Render::GUI::ElementData));
	object_file.read((char*)&data, sizeof(Render::GUI::MasterData));
}

Render::GUI::MasterData& DataClass::Data_MasterElement::getMasterData()
{
	return data;
}

DataClass::Data_MasterElement::Data_MasterElement(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::ELEMENT;
	object_identifier[1] = Render::GUI::MASTER;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
	element_data.element_type = Render::GUI::MASTER;
}

void DataClass::Data_MasterElement::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Master Element", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &element_data.position, false);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_MasterElement::makeCopy()
{
	return new DataClass::Data_MasterElement(*this);
}

glm::vec2& DataClass::Data_MasterElement::getScrollOffsets()
{
	return scroll_offsets;
}
