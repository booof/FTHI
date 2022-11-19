#include "CeilingMaskLine.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Mask::Ceiling::CeilingMaskLine::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(CEILING);
	object_file.put(HORIZONTAL_LINE);

	// Write Data
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

void Object::Mask::Ceiling::CeilingMaskLine::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = MASK;
	selector.object_identifier[1] = CEILING;
	selector.object_identifier[2] = HORIZONTAL_LINE;

	// Store Editing Values
	selector.horizontal_line_data = data;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

glm::vec2 Object::Mask::Ceiling::CeilingMaskLine::returnPosition()
{
	return data.position;
}

void Object::Mask::Ceiling::CeilingMaskLine::info(Editor::ObjectInfo& object_info, std::string& name, HorizontalLineData& data)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Ceiling Mask Horizontal Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Width: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.width, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
}

#endif

bool Object::Mask::Ceiling::CeilingMaskLine::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_right)
	{
		// Check if Object Y Intercepts Floor Y
		if (data.position.y + error > test_position.y && data.position.y - 0.01f < test_position.y)
		{
				return true;
		}
	}

	return false;
}

void Object::Mask::Ceiling::CeilingMaskLine::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Ceiling::CeilingMaskLine::CeilingMaskLine(HorizontalLineData& data_) : HorizontalLine(data_)
{
	// Store Data
	type = HORIZONTAL_LINE;
	returned_value = data.position.y;
	returned_angle = 3.1459f;

	// Store Storage Type
	storage_type = CEILING_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	number_of_vertices = 2;

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif
	
}

glm::vec2* Object::Mask::Ceiling::CeilingMaskLine::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Ceiling::CeilingMaskLine::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(x_left, data.position.y);
	right = glm::vec2(x_right, data.position.y);
}
