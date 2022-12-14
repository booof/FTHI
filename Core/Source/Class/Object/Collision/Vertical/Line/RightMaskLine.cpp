#include "RightMaskLine.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Mask::Right::RightMaskLine::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(RIGHT_WALL);
	object_file.put(VERTICAL_LINE);

	// Write Data
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

void Object::Mask::Right::RightMaskLine::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = MASK;
	selector.object_identifier[1] = RIGHT_WALL;
	selector.object_identifier[2] = VERTICAL_LINE;

	// Store Editor Values
	selector.vertical_line_data = data;
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

glm::vec2 Object::Mask::Right::RightMaskLine::returnPosition()
{
	return data.position;
}

void Object::Mask::Right::RightMaskLine::info(Editor::ObjectInfo& object_info, std::string& name, VerticalLineData& data)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Right Wall Mask Vertical Line", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addSingleValue("Height: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &data.height, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), false);
}

#endif

bool Object::Mask::Right::RightMaskLine::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between Y Values
	if (data.position.y < test_position.y && y_top > test_position.y)
	{
		// Check if Object is Between X Values
		if (data.position.x + error > data.position.x && data.position.x - 0.01f < test_position.x)
		{
			return true;
		}
	}

	return false;
}

void Object::Mask::Right::RightMaskLine::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Right::RightMaskLine::RightMaskLine(VerticalLineData& data_) : VerticalLine(data_)
{
	// Store Type
	type = VERTICAL_LINE;
	returned_value = data.position.x;
	returned_angle = 1.5708f;

	// Store Storage Type
	storage_type = RIGHT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Right::RightMaskLine::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Right::RightMaskLine::getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom)
{
	top = glm::vec2(data.position.x, y_top);
	bottom = data.position;
}
