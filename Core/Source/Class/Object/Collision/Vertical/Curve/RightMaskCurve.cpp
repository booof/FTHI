#include "RightMaskCurve.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Mask::Right::RightMaskCurve::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(RIGHT_WALL);
	object_file.put(VERTICAL_CURVE);

	// Write Data
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

void Object::Mask::Right::RightMaskCurve::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = MASK;
	selector.object_identifier[1] = RIGHT_WALL;
	selector.object_identifier[2] = VERTICAL_CURVE;

	// Store Editing Values
	selector.curve_data = data;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Right Wall Mask Curve", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

glm::vec2 Object::Mask::Right::RightMaskCurve::returnPosition()
{
	return data.position;
}

#endif

bool Object::Mask::Right::RightMaskCurve::testCollisions(glm::vec2 test_position, float error)
{
	// Check if Object is Between Y Values
	if ((modified_coordinates.y < test_position.y && y_top > test_position.y) || (modified_coordinates.y > test_position.y && y_top < test_position.y))
	{
		// Calculated X-Value
		float localX = (log((test_position.y - modified_coordinates.y) / slope) / log(amplitude) + x_offset - modified_coordinates.x);

		// Check if Object X intercepts Wall X
		if (localX + error > test_position.x && localX - 0.01f < test_position.x)
		{
			returned_value = localX;
			returned_angle = 1.5708f;
			return true;
		}
	}

	return false;
}

void Object::Mask::Right::RightMaskCurve::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Right::RightMaskCurve::RightMaskCurve(CurveData& data_) : Curve(data_)
{
	// Store Type
	type = VERTICAL_CURVE;

	// Store Storage Type
	storage_type = RIGHT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 1);
	number_of_vertices = 22;

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

glm::vec2* Object::Mask::Right::RightMaskCurve::pointerToPosition()
{
	return &data.position;
}

void Object::Mask::Right::RightMaskCurve::getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom)
{
	top = glm::vec2((log((y_top - modified_coordinates.y) / slope) / log(amplitude) + x_offset - modified_coordinates.x), y_top);
	bottom = glm::vec2(data.position.x, modified_coordinates.y);
}
