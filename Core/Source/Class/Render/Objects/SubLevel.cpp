#include "Level.h"
#include "SubLevel.h"
#include "Algorithms/Common/Common.h"
#include "Globals.h"

//#include "Class/Render/Objects/Level.h"
#include "Class/Render/Struct/LinkedList.h"
#include "Class/Object/Object.h"
#include "Class/Render/Camera/Camera.h"
#include "TextureHandler.h"
//#include "Class/Render/Objects/Level.h"

// Collision Masks
#include "Class/Object/Collision/Horizontal/Line/FloorMaskLine.h"
#include "Class/Object/Collision/Horizontal/Slant/FloorMaskSlant.h"
#include "Class/Object/Collision/Horizontal/Slope/FloorMaskSlope.h"
#include "Class/Object/Collision/Vertical/Line/LeftMaskLine.h"
#include "Class/Object/Collision/Vertical/Curve/LeftMaskCurve.h"
#include "Class/Object/Collision/Vertical/Line/RightMaskLine.h"
#include "Class/Object/Collision/Vertical/Curve/RightMaskCurve.h"
#include "Class/Object/Collision/Horizontal/Line/CeilingMaskLine.h"
#include "Class/Object/Collision/Horizontal/Slant/CeilingMaskSlant.h"
#include "Class/Object/Collision/Horizontal/Slope/CeilingMaskSlope.h"
#include "Class/Object/Collision/Trigger/TriggerMask.h"

// Lights
#include "Class/Object/Lighting/Directional.h"
#include "Class/Object/Lighting/Point.h"
#include "Class/Object/Lighting/Spot.h"
#include "Class/Object/Lighting/Beam.h"

// Editor Helpers
#include  "Class/Render/Objects/UnsavedLevel.h"
#include "Class/Render/Objects/ChangeController.h"

#ifdef EDITOR

Render::Objects::SubLevel::SubLevel(std::string& level_data_path, glm::i16vec4 pos, glm::vec2& wrapping_offset)
{
	// Store Coordinates of Level
	level_x = pos.x;
	level_y = pos.y;

	// Calculate the Object Offsets
	object_offsets.x = (float)pos.z * wrapping_offset.x;
	object_offsets.y = (float)pos.w * wrapping_offset.y;

#ifdef SHOW_LEVEL_LOADING
	std::cout << "loading level: " << level_x << " " << level_y << "  Offset: " << object_offsets.x << " " << object_offsets.y << "\n";
#endif

	// Get Path of Level
	path = level_data_path;
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(level_x)));
	path.append(",");
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(level_y)));

	// If File Does Not Exist, Create File
	//std::ofstream file(path, std::ios::binary | std::ios::ate);
	//file.close();

	// Read Headers
	readHeaders();
}

Render::Objects::SubLevel::SubLevel(std::string& gui_path)
{
	// Coordinates of Level Are Set to 0,0
	level_x = 0;
	level_y = 0;

	// Get Path of Object Data
	path = gui_path + "Object.dat";

	// Read Headers
	readHeaders();
}

void Render::Objects::SubLevel::readLevel(uint16_t& index_in_object_array)
{
	// Get Pointer to Unsaved Level
	unsaved_level = change_controller->getUnsavedLevel(level_x, level_y, level_version);
	
	// Store Pointer to Active Objects
	unsaved_level->active_objects = &active_objects;

	// Read Unsaved Level
	unsaved_level->buildObjects(index_in_object_array, object_offsets);

	// Level Has Been Initialized
	initialized = true;
}

// Read Headers
void Render::Objects::SubLevel::readHeaders()
{
	// Get Pointer to Unsaved Level
	unsaved_level = change_controller->getUnsavedLevel(level_x, level_y, level_version);

	// Store Pointer to Active Objects
	unsaved_level->active_objects = &active_objects;

	// Get Header From Unsaved Level
	number_of_loaded_objects = unsaved_level->returnObjectHeader();
}

void Render::Objects::SubLevel::drawVisualizer()
{
	unsaved_level->drawVisualizer();
}

void Render::Objects::SubLevel::updateModelMatrix()
{
	unsaved_level->updateModelMatrix();
}

#else

Render::Objects::SubLevel::SubLevel(int x, int y)
{
	// Store Coordinates of Level
	level_x = x;
	level_y = y;

	// Get Path of Level
	path = Global::level_data_path;
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(x)));
	path.append(",");
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(y)));

	// If File Does Not Exist, Create File
	std::ofstream file(path, std::ios::binary | std::ios::ate);
	file.close();

	// Read Headers
	readHeaders();
}

void Render::Objects::SubLevel::readLevel(Object::Object** objects)
{
	// Object Identifiers
	char object_identifier;

	// Index of Objects
	int index = 0;

	// File of Level
	std::ifstream file;

// Enable to Allow Compiler to Read Through Array of Lambdas
#define ENABLE_LAG

#ifdef ENABLE_LAG

	// Lambda Declaration

	// Construct FloorMaskLine Lambda
	static auto readFloorMaskLine = [&]
	{
		Object::Mask::HorizontalLineData data;
		bool platform;
		file.read((char*)&data, sizeof(data));
		file.read((char*)&platform, sizeof(bool));
		objects[index] = new Object::Mask::Floor::FloorMaskLine(data, platform);
	};

	// Construct FloorMaskSlant Lambda
	static auto readFloorMaskSlant = [&]
	{
		Object::Mask::SlantData data;
		bool platform;
		file.read((char*)&data, sizeof(data));
		file.read((char*)&platform, sizeof(bool));
		objects[index] = new Object::Mask::Floor::FloorMaskSlant(data, platform);
	};

	// Construct FloorMaskSlope Lambda
	static auto readFloorMaskSlope = [&]
	{
		Object::Mask::SlopeData data;
		bool platform;
		file.read((char*)&data, sizeof(data));
		file.read((char*)&platform, sizeof(bool));
		objects[index] = new Object::Mask::Floor::FloorMaskSlope(data, platform);
	};

	// Read FloorMasks Lambda
	static auto readFloorMasks = [&]
	{
		static std::function<void()> floor_masks[3] = { readFloorMaskLine, readFloorMaskSlant, readFloorMaskSlope };
		file.read(&object_identifier, sizeof(char));
		floor_masks[object_identifier]();
	};

	// Construct LeftMaskLine Lambda
	static auto readLeftMaskLine = [&]
	{
		Object::Mask::VerticalLineData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Left::LeftMaskLine(data);
	};

	// Construct LeftMaskCurve Lambda
	static auto readLeftMaskCurve = [&]
	{
		Object::Mask::CurveData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Left::LeftMaskCurve(data);
	};

	// Read LeftMasks Lambda
	static auto readLeftMasks = [&]
	{
		static std::function<void()> left_masks[2] = { readLeftMaskLine, readLeftMaskCurve };
		file.read(&object_identifier, sizeof(char));
		left_masks[object_identifier]();
	};

	// Construct RightMaskLine Lambda
	static auto readRightMaskLine = [&]
	{
		Object::Mask::VerticalLineData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Right::RightMaskLine(data);
	};

	// Construct RightMaskCurve Lambda
	static auto readRightMaskCurve = [&]
	{
		Object::Mask::CurveData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Right::RightMaskCurve(data);
	};

	// Read RightMasks Lambda
	static auto readRightMasks = [&]
	{
		static std::function<void()> right_masks[2] = { readRightMaskLine, readRightMaskCurve };
		file.read(&object_identifier, sizeof(char));
		right_masks[object_identifier]();
	};

	// Construct CeilingMaskLine Lambda
	static auto readCeilingMaskLine = [&]
	{
		Object::Mask::HorizontalLineData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Ceiling::CeilingMaskLine(data);
	};

	// Construct CeilingMaskSlant Lambda
	static auto readCeilingMaskSlant = [&]
	{
		Object::Mask::SlantData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Ceiling::CeilingMaskSlant(data);
	};

	// Construct CeilingMaskSlope Lambda
	static auto readCeilingMaskSlope = [&]
	{
		Object::Mask::SlopeData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Ceiling::CeilingMaskSlope(data);
	};

	// Read CeilingMasks Lambda
	static auto readCeilingMasks = [&]
	{
		static std::function<void()> ceiling_masks[3] = { readCeilingMaskLine, readCeilingMaskSlant, readCeilingMaskSlope };
		file.read(&object_identifier, sizeof(char));
		ceiling_masks[object_identifier]();
	};
	
	// Construct TriggerMask Lambda
	static auto readTriggerMask = [&]
	{
		Object::Mask::Trigger::TriggerData data;
		file.read((char*)&data, sizeof(data));
		objects[index] = new Object::Mask::Trigger::TriggerMask(data);
	};

	// Read Masks Lambda
	static auto readMasks = [&]
	{
		static std::function<void()> masks[5] = { readFloorMasks, readLeftMasks, readRightMasks, readCeilingMasks, readTriggerMask };
		file.read((char*)&object_identifier, sizeof(char));
		masks[object_identifier]();
	};

	// Object Lambda Array
	static std::function<void()> lambdas[1] = { readMasks };

#endif

	// Read File

	// Open File
	file.open(path, std::ios::binary);

	// Read Header
	file.read((char*)&header, sizeof(header));

	// Parse File
	while (!file.eof())
	{
		file.read((char*)&object_identifier, sizeof(char));
		lambdas[object_identifier]();
		index++;
	}

	// Level Has Been Initialized
	initialized = true;
}

void Render::Objects::SubLevel::readHeaders()
{
	// File of Level
	std::ifstream file;

	// Open File
	file.open(path, std::ios::binary);

	// Read Header
	file.read((char*)&header, sizeof(header));

	// Return to beginning of file
	file.clear();
	file.seekg(0);

	// Close File
	file.close();
}

void Render::Objects::SubLevel::writeLevel()
{

}

#endif

void Render::Objects::SubLevel::addHeader(uint32_t& object_count)
{
	object_count += number_of_loaded_objects;
}

void Render::Objects::SubLevel::subtractHeader(uint32_t& object_count)
{
	object_count -= number_of_loaded_objects;
}

void* Render::Objects::SubLevel::deactivateObjects()
{
	// Deactivate All Static Objects Part of This Level
	for (uint32_t i = 0; i < number_of_loaded_objects; i++)
		active_objects[i].active = false;

	// Return the Active Array
	if (number_of_loaded_objects)
		return active_objects;
	return 0;
}

void Render::Objects::SubLevel::includeNewActives(Object::Active* new_actives, int new_active_count, Level* level)
{
	// If There are Objects in Level, Rebalance the Array
	if (new_active_objects == 0 && removed_count == 0)
		return;

	// Store the Old Object Count
	int old_object_count = number_of_loaded_objects;

	// Determine The New Object Count
	number_of_loaded_objects += new_active_objects - removed_count;

	// Generate the New Active Array
	Object::Active* new_active_array = nullptr;
	if (number_of_loaded_objects)
		new_active_array = new Object::Active[number_of_loaded_objects];

	// The Iterator to Insert Objects Into
	int insertion_index = 0;

	// Copy All Currently Alive Active Objects Into New Array
	for (int i = 0; i < old_object_count; i++)
	{
		// Don't Add Dead Objects
		if (active_objects[i].alive)
		{
			new_active_array[insertion_index] = active_objects[i];
			active_objects[i].object_ptr->active_ptr = &new_active_array[insertion_index++];
		}
	}

	// Copy New Active Objects Into New Array
	for (int i = 0; i < new_active_count; i++)
	{
		if (new_actives[i].level_pos.x == level_x && new_actives[i].level_pos.y == level_y)
		{
			new_active_array[insertion_index] = new_actives[i];
			new_actives[i].object_ptr->active_ptr = &new_active_array[insertion_index++];
		}
	}

	// Delete the Old Active Array
	if (old_object_count)
		delete[] active_objects;

	// Store the New Active Array
	active_objects = new_active_array;
	unsaved_level->active_objects = &active_objects;

	// Reset the New Actives Count
	new_active_objects = 0;
	removed_count = 0;
}

void Render::Objects::SubLevel::resetCounts()
{
	// Reset the New Actives Count
	new_active_objects = 0;
	removed_count = 0;
}

void Render::Objects::SubLevel::deleteSubLevel()
{
#ifdef SHOW_LEVEL_LOADING
	std::cout << "deleting level: " << level_x << " " << level_y << "   " << path << "\n";
#endif

	// Delete Active Objects Array
	if (number_of_loaded_objects)
		delete[] active_objects;
}

void Render::Objects::SubLevel::reloadActivePointer()
{
	// Get the Unsaved Level
	UnsavedLevel* unsaved_level = change_controller->getUnsavedLevel(level_x, level_y, 0);

	// Update the Active Pointer to the Unsaved Object
	unsaved_level->active_objects = &active_objects;
}
