#include "FloorMaskSlope.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Mask::Floor::FloorMaskSlope::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(FLOOR);
	object_file.put(HORIZONTAL_SLOPE);

	// Write Data
	object_file.write((char*)&data, sizeof(data));
	object_file.write((char*)&platform, sizeof(bool));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

void Object::Mask::Floor::FloorMaskSlope::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = MASK;
	selector.object_identifier[1] = FLOOR;
	selector.object_identifier[2] = HORIZONTAL_SLOPE;

	// Store Editor Values
	selector.slope_data = data;
	selector.floor_mask_platform = platform;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data, platform);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

glm::vec2 Object::Mask::Floor::FloorMaskSlope::returnPosition()
{
	return data.position;
}

void Object::Mask::Floor::FloorMaskSlope::info(Editor::ObjectInfo& object_info, std::string& name, SlopeData& data, bool platform)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Floor Mask Slope", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Size: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "W: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " H: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.width, &data.height, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addBooleanValue("Platform? ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &platform, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
}

#endif

bool Object::Mask::Floor::FloorMaskSlope::testCollisions(glm::vec2 test_position, float error, bool fallthrough)
{
	// Test if Fallthough Occoured
	if (fallthrough && platform)
		return false;

	Vertices::Visualizer::visualizePoint(glm::vec2(x_left, data.position.y), 0.4f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(glm::vec2(x_right, data.position.y), 0.4f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	Vertices::Visualizer::visualizePoint(data.position, 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	// Check if Object is Between X-Values
	if (x_left < test_position.x && test_position.x < x_right)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = half_height * atan(slope * (test_position.x - data.position.x)) + data.position.y;

		Vertices::Visualizer::visualizePoint(glm::vec2(test_position.x, localY), 0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - Angle / 2.0f * HeightSign) < posY && localY > posY)
		if ((localY - error - data.height * 0.25f * height_sign) < test_position.y && localY + 0.01 > test_position.y)
		{
			// Calculate Derivative
			float derivative = (half_height * slope) / (1 + (float)glm::pow(slope * (test_position.x - data.position.x), 2));
			returned_angle = atan(derivative);

			returned_value = localY;
			return true;
		}
	}

	return false;
}

void Object::Mask::Floor::FloorMaskSlope::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = glm::vec2(data.position.x - abs(data.width), data.position.y - data.height);
	right = glm::vec2(data.position.x + abs(data.width), data.position.y + data.height);
}

void Object::Mask::Floor::FloorMaskSlope::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Floor::FloorMaskSlope::FloorMaskSlope(SlopeData& data_, bool& platform_) : Slope(data_)
{
	// Store Type
	type = HORIZONTAL_SLOPE;
	platform = std::move(platform_);

	// Store Storage Type
	storage_type = FLOOR_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(0.04f, 0.24f, 1.0f, 1.0f));
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

glm::vec2* Object::Mask::Floor::FloorMaskSlope::pointerToPosition()
{
	return &data.position;
}
