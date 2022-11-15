#include "FloorMaskSlant.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Editor/ObjectInfo.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Mask::Floor::FloorMaskSlant::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(MASK);
	object_file.put(FLOOR);
	object_file.put(HORIZONTAL_SLANT);

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

void Object::Mask::Floor::FloorMaskSlant::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = MASK;
	selector.object_identifier[1] = FLOOR;
	selector.object_identifier[2] = HORIZONTAL_SLANT;

	// Store Editing Values
	selector.slant_data = data;
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

glm::vec2 Object::Mask::Floor::FloorMaskSlant::returnPosition()
{
	return data.position;
}

void Object::Mask::Floor::FloorMaskSlant::info(Editor::ObjectInfo& object_info, std::string& name, SlantData& data, bool platform)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Floor Mask Slant", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position2.x, &data.position2.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addBooleanValue("Platform? ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &platform, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f));
}

#endif

bool Object::Mask::Floor::FloorMaskSlant::testCollisions(glm::vec2 test_position, float error, bool fallthrough)
{
	// Test if Fallthough Occoured
	if (fallthrough && platform)
		return false;

	// Check if Object is Between X-Values
	if (data.position.x < test_position.x && test_position.x < data.position2.x)
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = slope * test_position.x + intercept;

		// Check if Object Y Intercepts Floor Y
		//if ((localY - SPEED * 1.5f * deltaTime - 1.0f) < posY && localY > posY)
		if (localY - error < test_position.y && localY + 0.01 > test_position.y)
		{
			returned_value = localY;
			return true;
		}
	}

	return false;
}

void Object::Mask::Floor::FloorMaskSlant::getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right)
{
	left = data.position;
	right = data.position2;
}

void Object::Mask::Floor::FloorMaskSlant::returnMaterial(int& material)
{
	material = data.material;
}

Object::Mask::Floor::FloorMaskSlant::FloorMaskSlant(SlantData& data_, bool& platform_) : Slant(data_)
{
	// Store Type
	type = HORIZONTAL_SLANT;
	returned_angle = Algorithms::Math::angle_from_vector(data.position2 - data.position);
	platform = std::move(platform_);

	// Store Storage Type
	storage_type = FLOOR_COUNT;

#ifdef EDITOR

	// Create Visualizer
	initializeVisualizer(VAO, VBO, model, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
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

glm::vec2* Object::Mask::Floor::FloorMaskSlant::pointerToPosition()
{
	return &data.position;
}

