#include "UnsavedLevel.h"
#include "Source/Algorithms/Common/Common.h"
#include "Class/Render/Editor/Selector.h"
#include "Globals.h"

// Verbose Objects
#include "Class/Render/Shape/Shape.h"
#include "Class/Render/Shape/Rectangle.h"
#include "Class/Render/Shape/Trapezoid.h"
#include "Class/Render/Shape/Triangle.h"
#include "Class/Render/Shape/Circle.h"
#include "Class/Render/Shape/Polygon.h"
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
#include "Class/Object/Terrain/Foreground.h"
#include "Class/Object/Terrain/Formerground.h"
#include "Class/Object/Terrain/Background.h"
#include "Class/Object/Terrain/Backdrop.h"

void Render::Objects::UnsavedLevel::allocateForArrays(LevelInstance& instance)
{
	instance.horizontal_line_floor_data      = new HorizontalLineFloorData[instance.editor_header.floor_mask_horizontal_lines];
	instance.slant_floor_data                = new SlantFloorData[instance.editor_header.floor_mask_slants];
	instance.slope_floor_data                = new SlopeFloorData[instance.editor_header.floor_mask_slopes];
	instance.vertical_line_left_wall_data    = new VerticalLineLeftWallData[instance.editor_header.left_mask_vertical_lines];
	instance.curve_left_wall_data            = new CurveLeftWallData[instance.editor_header.left_mask_curves];
	instance.vertical_line_right_wall_data   = new VerticalLineRightWallData[instance.editor_header.right_mask_vertical_lines];
	instance.curve_right_wall_data           = new CurveRightWallData[instance.editor_header.right_mask_curves];
	instance.horizontal_line_ceiling_data    = new HorizontalLineCeilingData[instance.editor_header.ceiling_mask_horizontal_lines];
	instance.slant_ceiling_data              = new SlantCeilingData[instance.editor_header.ceiling_mask_slants];
	instance.slope_ceiling_data              = new SlopeCeilingData[instance.editor_header.ceiling_mask_slopes];
	instance.trigger_data                    = new TriggerMaskData[instance.editor_header.trigger_masks];
	instance.foreground_terrain_data         = new ForegroundTerrainData[instance.editor_header.foreground_terrain];
	instance.formerground_terrain_data       = new FormergroundTerrainData[instance.editor_header.formerground_terrain];
	instance.background_terrain_data         = new BackgroundTerrainData[instance.editor_header.background_terrain];
	instance.backdrop_terrain_data           = new BackdropTerrainData[instance.editor_header.backdrop_terrain];
	instance.directional_light_data          = new DirectionalLightData[instance.editor_header.directional_lights];
	instance.point_light_data                = new PointLightData[instance.editor_header.point_lights];
	instance.spot_light_data                 = new SpotLightData[instance.editor_header.spot_lights];
	instance.beam_light_data                 = new BeamLightData[instance.editor_header.beam_lights];
	instance.rigid_body_physics_data         = new RigidBodyPhysicsData[instance.editor_header.physics_rigid];
	instance.spring_mass_physics_data        = new SpringMassPhysicsData[instance.editor_header.physics_soft_springmass];
	instance.wire_physics_data               = new WirePhysicsData[instance.editor_header.physics_soft_wires];
	instance.anchor_physics_data             = new AnchorPhysicsData[instance.editor_header.physics_anchors];
	instance.hinge_physics_data              = new HingePhysicsData[instance.editor_header.physics_hinges];
	instance.npc_data                        = new NPC_Data[instance.editor_header.entity_npc];
	instance.controllable_data               = new ControllableData[instance.editor_header.entity_controllable];
	instance.interactable_data               = new InteractableData[instance.editor_header.entity_interactable];
	instance.dynamic_data                    = new DynamicData[instance.editor_header.entity_dynamic];
}

void Render::Objects::UnsavedLevel::constructUnmodifiedDataHelper(LevelInstance& instance)
{
	// Temporary Editor Header to Contain All Current Indicies for Each Object Type
	EditorHeader temp_index_holder;

	// Temporary Holder for Object Identifier
	uint8_t temp_identifier = 0;

	// Size of Object File
	uint32_t object_size = 0;

	// Set Unmodified Flag
	instance.unmodified = true;

	// If Editor File is Empty, Load Empty Headers
	std::filesystem::path temp = editor_path;
	std::error_code ec;
	if ((int)std::filesystem::file_size(temp, ec) == 0)
	{
		return;
	}
	if (ec)
	{
		return;
	}

	// Open Object Data File
	std::ifstream object_file;
	object_file.open(object_path, std::ios::binary);

	// Open Editor Data File
	std::ifstream editor_file;
	editor_file.open(editor_path, std::ios::binary);

	// Get Size of Object File
	object_file.seekg(0, std::ios::end);
	object_size = (uint32_t)object_file.tellg();
	object_file.seekg(0, std::ios::beg);
	editor_file.seekg(0, std::ios::beg);

	// Read Object File Header
	object_file.read((char*)&instance.header, sizeof(LevelHeader));

	// Read Editor File Header
	editor_file.read((char*)&instance.editor_header, sizeof(EditorHeader));

	// Allocate Memory for Arrays
	allocateForArrays(instance);

#define ENABLE_LAG2

#ifdef ENABLE_LAG2

	// Lambda for Rectangle Shapes
	auto readRectangles = [&object_file](Shape::Shape** shape_pointer)->void {
		Shape::Rectangle* new_rect = new Shape::Rectangle;
		object_file.read((char*)(new_rect) + 8, sizeof(Shape::Rectangle) - 8);
		*shape_pointer = static_cast<Shape::Shape*>(new_rect);
	};

	// Lambda for Trapezoid Shapes
	auto readTrapezoids = [&object_file](Shape::Shape** shape_pointer)->void {
		Shape::Trapezoid* new_trap = new Shape::Trapezoid;
		object_file.read((char*)(new_trap) + 8, sizeof(Shape::Trapezoid) - 8);
		*shape_pointer = static_cast<Shape::Shape*>(new_trap);
	};

	// Lambda for Triangle Shapes
	auto readTriangles = [&object_file](Shape::Shape** shape_pointer)->void {
		Shape::Triangle* new_tri = new Shape::Triangle;
		object_file.read((char*)(new_tri) + 8, sizeof(Shape::Triangle) - 8);
		*shape_pointer = static_cast<Shape::Shape*>(new_tri);
	};

	// Lambda for Circle Shapes
	auto readCircles = [&object_file](Shape::Shape** shape_pointer)->void {
		Shape::Circle* new_circle = new Shape::Circle;
		object_file.read((char*)(new_circle) + 8, sizeof(Shape::Circle) - 8);
		*shape_pointer = static_cast<Shape::Shape*>(new_circle);
	};

	// Lambda for Polygon Shapes
	auto readPolygons = [&object_file](Shape::Shape** shape_pointer)->void {
		Shape::Polygon* new_poly = new Shape::Polygon;
		object_file.read((char*)(new_poly) + 8, sizeof(Shape::Polygon) - 8);
		*shape_pointer = static_cast<Shape::Shape*>(new_poly);
	};

	// Lambda for Shapes
	auto readShapes = [&readRectangles, &readTrapezoids, &readTriangles, &readCircles, &readPolygons](Shape::Shape** shape_pointer, uint8_t identifier)->void {
		const std::function<void(Shape::Shape** shape_pointer)> shapes[5] = { readRectangles, readTrapezoids, readTriangles, readCircles, readPolygons };
		shapes[identifier](shape_pointer);
	};

	// Lambda for Floor Mask Horizontal Lines
	auto readFloorMaskHorizontalLines = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].horizontal_line_data, sizeof(Object::Mask::HorizontalLineData));
		object_file.read((char*)&instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].platform, sizeof(bool));
		editor_file.read((char*)&instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].editor_data, sizeof(EditorData));
		instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].name.resize(instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].editor_data.name_size);
		editor_file.read((char*)&instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].name[0], instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].editor_data.name_size);
		instance.horizontal_line_floor_data[temp_index_holder.floor_mask_horizontal_lines].object_index = Global::object_index_counter;
		temp_index_holder.floor_mask_horizontal_lines++;
	};

	// Lambda for Floor Mask Slants
	auto readFloorMaskSlants = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.slant_floor_data[temp_index_holder.floor_mask_slants].slant_data, sizeof(Object::Mask::SlantData));
		object_file.read((char*)&instance.slant_floor_data[temp_index_holder.floor_mask_slants].platform, sizeof(bool));
		editor_file.read((char*)&instance.slant_floor_data[temp_index_holder.floor_mask_slants].editor_data, sizeof(EditorData));
		instance.slant_floor_data[temp_index_holder.floor_mask_slants].name.resize(instance.slant_floor_data[temp_index_holder.floor_mask_slants].editor_data.name_size);
		editor_file.read((char*)&instance.slant_floor_data[temp_index_holder.floor_mask_slants].name[0], instance.slant_floor_data[temp_index_holder.floor_mask_slants].editor_data.name_size);
		instance.slant_floor_data[temp_index_holder.floor_mask_slants].object_index = Global::object_index_counter;
		temp_index_holder.floor_mask_slants++;
	};

	// Lambda for Floor Mask Slopes
	auto readFloorMaskSlopes = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.slope_floor_data[temp_index_holder.floor_mask_slopes].slope_data, sizeof(Object::Mask::SlopeData));
		object_file.read((char*)&instance.slope_floor_data[temp_index_holder.floor_mask_slopes].platform, sizeof(bool));
		editor_file.read((char*)&instance.slope_floor_data[temp_index_holder.floor_mask_slopes].editor_data, sizeof(EditorData));
		instance.slope_floor_data[temp_index_holder.floor_mask_slopes].name.resize(instance.slope_floor_data[temp_index_holder.floor_mask_slopes].editor_data.name_size);
		editor_file.read((char*)&instance.slope_floor_data[temp_index_holder.floor_mask_slopes].name[0], instance.slope_floor_data[temp_index_holder.floor_mask_slopes].editor_data.name_size);
		instance.slope_floor_data[temp_index_holder.floor_mask_slopes].object_index = Global::object_index_counter;
		temp_index_holder.floor_mask_slopes++;
	};

	// Lambda for Floor Masks
	auto readFloorMasks = [&object_file, &temp_identifier, &readFloorMaskHorizontalLines, &readFloorMaskSlants, &readFloorMaskSlopes]()->void {
		const std::function<void()> floor_masks[3] = { readFloorMaskHorizontalLines, readFloorMaskSlants, readFloorMaskSlopes };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		floor_masks[temp_identifier]();
	};

	// Lambda for Vertical Line Left Wall Masks
	auto readLeftWallMaskVerticalLine = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].vertical_line_data, sizeof(Object::Mask::VerticalLineData));
		editor_file.read((char*)&instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].editor_data, sizeof(EditorData));
		instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].name.resize(instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].editor_data.name_size);
		editor_file.read((char*)&instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].name[0], instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].editor_data.name_size);
		instance.vertical_line_left_wall_data[temp_index_holder.left_mask_vertical_lines].object_index = Global::object_index_counter;
		temp_index_holder.left_mask_vertical_lines++;
	};

	// Lambda for Curve Left Wall Masks
	auto readLeftWallMaskCurve = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.curve_left_wall_data[temp_index_holder.left_mask_curves].curve_data, sizeof(Object::Mask::CurveData));
		editor_file.read((char*)&instance.curve_left_wall_data[temp_index_holder.left_mask_curves].editor_data, sizeof(EditorData));
		instance.curve_left_wall_data[temp_index_holder.left_mask_curves].name.resize(instance.curve_left_wall_data[temp_index_holder.left_mask_curves].editor_data.name_size);
		editor_file.read((char*)&instance.curve_left_wall_data[temp_index_holder.left_mask_curves].name[0], instance.curve_left_wall_data[temp_index_holder.left_mask_curves].editor_data.name_size);
		instance.curve_left_wall_data[temp_index_holder.left_mask_curves].object_index = Global::object_index_counter;
		temp_index_holder.left_mask_curves++;
	};

	// Lambda for Left Wall Masks
	auto readLeftWallMasks = [&object_file, &temp_identifier, &readLeftWallMaskVerticalLine, &readLeftWallMaskCurve]()->void {
		const std::function<void()> left_wall_masks[2] = { readLeftWallMaskVerticalLine, readLeftWallMaskCurve };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		left_wall_masks[temp_identifier]();
	};

	// Lambda for Vertical Line Right Wall Masks
	auto readRightWallMaskVerticalLine = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].vertical_line_data, sizeof(Object::Mask::VerticalLineData));
		editor_file.read((char*)&instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].editor_data, sizeof(EditorData));
		instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].name.resize(instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].editor_data.name_size);
		editor_file.read((char*)&instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].name[0], instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].editor_data.name_size);
		instance.vertical_line_right_wall_data[temp_index_holder.right_mask_vertical_lines].object_index = Global::object_index_counter;
		temp_index_holder.right_mask_vertical_lines++;
	};

	// Lambda for Curve Right Wall Masks
	auto readRightWallMaskCurve = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.curve_right_wall_data[temp_index_holder.right_mask_curves].curve_data, sizeof(Object::Mask::CurveData));
		editor_file.read((char*)&instance.curve_right_wall_data[temp_index_holder.right_mask_curves].editor_data, sizeof(EditorData));
		instance.curve_right_wall_data[temp_index_holder.right_mask_curves].name.resize(instance.curve_right_wall_data[temp_index_holder.right_mask_curves].editor_data.name_size);
		editor_file.read((char*)&instance.curve_right_wall_data[temp_index_holder.right_mask_curves].name[0], instance.curve_right_wall_data[temp_index_holder.right_mask_curves].editor_data.name_size);
		instance.curve_right_wall_data[temp_index_holder.right_mask_curves].object_index = Global::object_index_counter;
		temp_index_holder.right_mask_curves++;
	};

	// Lambda for Right Wall Masks
	auto readRightWallMasks = [&object_file, &temp_identifier, &readRightWallMaskVerticalLine, &readRightWallMaskCurve]()->void {
		const std::function<void()> right_wall_masks[2] = { readRightWallMaskVerticalLine, readRightWallMaskCurve };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		right_wall_masks[temp_identifier]();
	};

	// Lambda for Ceiling Mask Horizontal Lines
	auto readCeilingMaskHorizontalLines = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].horizontal_line_data, sizeof(Object::Mask::HorizontalLineData));
		editor_file.read((char*)&instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].editor_data, sizeof(EditorData));
		instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].name.resize(instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].editor_data.name_size);
		editor_file.read((char*)&instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].name[0], instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].editor_data.name_size);
		instance.horizontal_line_ceiling_data[temp_index_holder.ceiling_mask_horizontal_lines].object_index = Global::object_index_counter;
		temp_index_holder.ceiling_mask_horizontal_lines++;
	};

	// Lambda for Ceiling Mask Slants
	auto readCeilingMaskSlants = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].slant_data, sizeof(Object::Mask::SlantData));
		editor_file.read((char*)&instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].editor_data, sizeof(EditorData));
		instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].name.resize(instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].editor_data.name_size);
		editor_file.read((char*)&instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].name[0], instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].editor_data.name_size);
		instance.slant_ceiling_data[temp_index_holder.ceiling_mask_slants].object_index = Global::object_index_counter;
		temp_index_holder.ceiling_mask_slants++;
	};

	// Lambda for Ceiling Mask Slopes
	auto readCeilingMaskSlopes = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].slope_data, sizeof(Object::Mask::SlopeData));
		editor_file.read((char*)&instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].editor_data, sizeof(EditorData));
		instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].name.resize(instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].editor_data.name_size);
		editor_file.read((char*)&instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].name[0], instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].editor_data.name_size);
		instance.slope_ceiling_data[temp_index_holder.ceiling_mask_slopes].object_index = Global::object_index_counter;
		temp_index_holder.ceiling_mask_slopes++;
	};

	// Lambda for Ceiling Masks
	auto readCeilingMasks = [&object_file, &temp_identifier, &readCeilingMaskHorizontalLines, &readCeilingMaskSlants, &readCeilingMaskSlopes]()->void {
		const std::function<void()> ceiling_masks[3] = { readCeilingMaskHorizontalLines, readCeilingMaskSlants, readCeilingMaskSlopes };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		ceiling_masks[temp_identifier]();
	};

	// Lambda for Trigger Masks
	auto readTriggerMasks = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.trigger_data[temp_index_holder.trigger_masks].trigger_data, sizeof(Object::Mask::Trigger::TriggerData));
		editor_file.read((char*)&instance.trigger_data[temp_index_holder.trigger_masks].editor_data, sizeof(EditorData));
		instance.trigger_data[temp_index_holder.trigger_masks].name.resize(instance.trigger_data[temp_index_holder.trigger_masks].editor_data.name_size);
		editor_file.read((char*)&instance.trigger_data[temp_index_holder.trigger_masks].name[0], instance.trigger_data[temp_index_holder.trigger_masks].editor_data.name_size);
		instance.trigger_data[temp_index_holder.trigger_masks].object_index = Global::object_index_counter;
		temp_index_holder.trigger_masks++;
	};

	// Lambda for Collision Masks
	auto readMasks = [&object_file, &temp_identifier, &readFloorMasks, &readLeftWallMasks, &readRightWallMasks, &readCeilingMasks, &readTriggerMasks]()->void {
		const std::function<void()> masks[5] = { readFloorMasks, readLeftWallMasks, readRightWallMasks, readCeilingMasks, readTriggerMasks };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		masks[temp_identifier]();
	};

	// Lambda for Foreground Terrain
	auto readForegroundTerrain = [&object_file, &editor_file, &temp_index_holder, &instance, &readShapes](uint8_t identifier)->void {
		readShapes(&instance.foreground_terrain_data[temp_index_holder.foreground_terrain].shape_data, identifier);
		object_file.read((char*)&instance.foreground_terrain_data[temp_index_holder.foreground_terrain].object_data, sizeof(Object::ObjectData));
		editor_file.read((char*)&instance.foreground_terrain_data[temp_index_holder.foreground_terrain].editor_data, sizeof(EditorData));
		instance.foreground_terrain_data[temp_index_holder.foreground_terrain].name.resize(instance.foreground_terrain_data[temp_index_holder.foreground_terrain].editor_data.name_size);
		editor_file.read((char*)&instance.foreground_terrain_data[temp_index_holder.foreground_terrain].name[0], instance.foreground_terrain_data[temp_index_holder.foreground_terrain].editor_data.name_size);
		instance.foreground_terrain_data[temp_index_holder.foreground_terrain].object_index = Global::object_index_counter;
		temp_index_holder.foreground_terrain++;
	};

	// Lambda for Formerground Terrain
	auto readFormergroundTerrain = [&object_file, &editor_file, &temp_index_holder, &instance, &readShapes](uint8_t identifier)->void {
		readShapes(&instance.formerground_terrain_data[temp_index_holder.formerground_terrain].shape_data, identifier);
		object_file.read((char*)&instance.formerground_terrain_data[temp_index_holder.formerground_terrain].object_data, sizeof(Object::ObjectData));
		editor_file.read((char*)&instance.formerground_terrain_data[temp_index_holder.formerground_terrain].editor_data, sizeof(EditorData));
		instance.formerground_terrain_data[temp_index_holder.formerground_terrain].name.resize(instance.formerground_terrain_data[temp_index_holder.formerground_terrain].editor_data.name_size);
		editor_file.read((char*)&instance.formerground_terrain_data[temp_index_holder.formerground_terrain].name[0], instance.formerground_terrain_data[temp_index_holder.formerground_terrain].editor_data.name_size);
		instance.formerground_terrain_data[temp_index_holder.formerground_terrain].object_index = Global::object_index_counter;
		temp_index_holder.formerground_terrain++;
	};

	// Lambda for Background Terrain
	auto readBackgroundTerrain = [&object_file, &editor_file, &temp_index_holder, &instance, &readShapes](uint8_t identifier)->void {
		readShapes(&instance.background_terrain_data[temp_index_holder.background_terrain].shape_data, identifier);
		object_file.read((char*)&instance.background_terrain_data[temp_index_holder.background_terrain].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.background_terrain_data[temp_index_holder.background_terrain].layer, sizeof(uint8_t));
		editor_file.read((char*)&instance.background_terrain_data[temp_index_holder.background_terrain].editor_data, sizeof(EditorData));
		instance.background_terrain_data[temp_index_holder.background_terrain].name.resize(instance.background_terrain_data[temp_index_holder.background_terrain].editor_data.name_size);
		editor_file.read((char*)&instance.background_terrain_data[temp_index_holder.background_terrain].name[0], instance.background_terrain_data[temp_index_holder.background_terrain].editor_data.name_size);
		instance.background_terrain_data[temp_index_holder.background_terrain].object_index = Global::object_index_counter;
		temp_index_holder.background_terrain++;
	};

	// Lambda for Backdrop Terrain
	auto readBackdropTerrain = [&object_file, &editor_file, &temp_index_holder, &instance, &readShapes](uint8_t identifier)->void {
		readShapes(&instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].shape_data, identifier);
		object_file.read((char*)&instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].object_data, sizeof(Object::ObjectData));
		editor_file.read((char*)&instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].editor_data, sizeof(EditorData));
		instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].name.resize(instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].editor_data.name_size);
		editor_file.read((char*)&instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].name[0], instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].editor_data.name_size);
		instance.backdrop_terrain_data[temp_index_holder.backdrop_terrain].object_index = Global::object_index_counter;
		temp_index_holder.backdrop_terrain++;
	};

	// Lambda for Terrain
	auto readTerrain = [&object_file, &temp_identifier, &readForegroundTerrain, &readFormergroundTerrain, &readBackgroundTerrain, &readBackdropTerrain]()->void {
		const std::function<void(uint8_t identifier)> terrain[6] = { readBackdropTerrain, readBackgroundTerrain, readBackgroundTerrain, readBackgroundTerrain, readForegroundTerrain, readFormergroundTerrain };
		uint8_t shape_identifier = 0;
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		object_file.read((char*)&shape_identifier, sizeof(uint8_t));
		terrain[temp_identifier](shape_identifier);
	};

	// Lambda for Directional Lights
	auto readDirectionalLights = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.directional_light_data[temp_index_holder.directional_lights].directional_data, sizeof(Object::Light::Directional::DirectionalData));
		object_file.read((char*)&instance.directional_light_data[temp_index_holder.directional_lights].light_data, sizeof(Object::Light::LightData));
		editor_file.read((char*)&instance.directional_light_data[temp_index_holder.directional_lights].editor_data, sizeof(EditorData));
		instance.directional_light_data[temp_index_holder.directional_lights].name.resize(instance.directional_light_data[temp_index_holder.directional_lights].editor_data.name_size);
		editor_file.read((char*)&instance.directional_light_data[temp_index_holder.directional_lights].name[0], instance.directional_light_data[temp_index_holder.directional_lights].editor_data.name_size);
		instance.directional_light_data[temp_index_holder.directional_lights].object_index = Global::object_index_counter;
		temp_index_holder.directional_lights++;
	};

	// Lambda for Point Lights
	auto readPointLights = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.point_light_data[temp_index_holder.point_lights].point_data, sizeof(Object::Light::Point::PointData));
		//object_file.read((char*)&instance.point_light_data[temp_index_holder.point_lights].light_data, sizeof(Object::Light::LightData));
		object_file.read((char*)&instance.point_light_data[temp_index_holder.point_lights].light_data, sizeof(instance.point_light_data[temp_index_holder.point_lights].light_data));
		editor_file.read((char*)&instance.point_light_data[temp_index_holder.point_lights].editor_data, sizeof(EditorData));
		instance.point_light_data[temp_index_holder.point_lights].name.resize(instance.point_light_data[temp_index_holder.point_lights].editor_data.name_size);
		editor_file.read((char*)&instance.point_light_data[temp_index_holder.point_lights].name[0], instance.point_light_data[temp_index_holder.point_lights].editor_data.name_size);
		instance.point_light_data[temp_index_holder.point_lights].object_index = Global::object_index_counter;
		temp_index_holder.point_lights++;
	};

	// Lambda for Spot Lights
	auto readSpotLights = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.spot_light_data[temp_index_holder.spot_lights].spot_data, sizeof(Object::Light::Spot::SpotData));
		object_file.read((char*)&instance.spot_light_data[temp_index_holder.spot_lights].light_data, sizeof(Object::Light::LightData));
		editor_file.read((char*)&instance.spot_light_data[temp_index_holder.spot_lights].editor_data, sizeof(EditorData));
		instance.spot_light_data[temp_index_holder.spot_lights].name.resize(instance.spot_light_data[temp_index_holder.spot_lights].editor_data.name_size);
		editor_file.read((char*)&instance.spot_light_data[temp_index_holder.spot_lights].name[0], instance.spot_light_data[temp_index_holder.spot_lights].editor_data.name_size);
		instance.spot_light_data[temp_index_holder.spot_lights].object_index = Global::object_index_counter;
		temp_index_holder.spot_lights++;
	};

	// Lambda for Beam Lights
	auto readBeamLights = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.beam_light_data[temp_index_holder.beam_lights].beam_data, sizeof(Object::Light::Beam::BeamData));
		object_file.read((char*)&instance.beam_light_data[temp_index_holder.beam_lights].light_data, sizeof(Object::Light::LightData));
		editor_file.read((char*)&instance.beam_light_data[temp_index_holder.beam_lights].editor_data, sizeof(EditorData));
		instance.beam_light_data[temp_index_holder.beam_lights].name.resize(instance.beam_light_data[temp_index_holder.beam_lights].editor_data.name_size);
		editor_file.read((char*)&instance.beam_light_data[temp_index_holder.beam_lights].name[0], instance.beam_light_data[temp_index_holder.beam_lights].editor_data.name_size);
		instance.beam_light_data[temp_index_holder.beam_lights].object_index = Global::object_index_counter;
		temp_index_holder.beam_lights++;
	};

	// Lambda for Lights
	auto readLights = [&object_file, &temp_identifier, &readDirectionalLights, &readPointLights, &readSpotLights, &readBeamLights]()->void {
		std::function<void()> lights[4] = { readDirectionalLights, readPointLights, readSpotLights, readBeamLights };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		lights[temp_identifier]();
	};

	// Lambda for Rigid Bodies Physics Objects
	auto readRigidBodies = [&object_file, &editor_file, &temp_index_holder, &instance, &readShapes]()->void {
		uint8_t shape_identifier = 0;
		object_file.read((char*)&shape_identifier, sizeof(uint8_t));
		object_file.read((char*)&instance.rigid_body_physics_data[temp_index_holder.physics_rigid].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.rigid_body_physics_data[temp_index_holder.physics_rigid].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.rigid_body_physics_data[temp_index_holder.physics_rigid].rigid_body_data, sizeof(Object::Physics::Rigid::RigidBodyData));
		readShapes(&instance.rigid_body_physics_data[temp_index_holder.physics_rigid].shape_data, shape_identifier);
		instance.rigid_body_physics_data[temp_index_holder.physics_rigid].name.resize(instance.rigid_body_physics_data[temp_index_holder.physics_rigid].editor_data.name_size);
		editor_file.read((char*)&instance.rigid_body_physics_data[temp_index_holder.physics_rigid].name[0], instance.rigid_body_physics_data[temp_index_holder.physics_rigid].editor_data.name_size);
		instance.rigid_body_physics_data[temp_index_holder.physics_rigid].object_index = Global::object_index_counter;
		temp_index_holder.physics_rigid++;
	};

	// Lambda for SpringMass Physics Objects
	auto readSpringMasses = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].uuid, sizeof(uint32_t));
		uint16_t file_name_size = 0;
		object_file.read((char*)&file_name_size, sizeof(uint16_t));
		object_file.read((char*)&instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].object_data, sizeof(Object::ObjectData));
		instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].file_name.resize(file_name_size);
		object_file.read((char*)&instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].file_name[0], file_name_size);
		instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].name.resize(instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].editor_data.name_size);
		editor_file.read((char*)&instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].name[0], instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].editor_data.name_size);
		instance.spring_mass_physics_data[temp_index_holder.physics_soft_springmass].object_index = Global::object_index_counter;
		temp_index_holder.physics_soft_springmass++;
	};

	// Lambda for Wire Physics Objects
	auto readWires = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.wire_physics_data[temp_index_holder.physics_soft_wires].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.wire_physics_data[temp_index_holder.physics_soft_wires].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.wire_physics_data[temp_index_holder.physics_soft_wires].wire_data, sizeof(Object::Physics::Soft::WireData));
		instance.wire_physics_data[temp_index_holder.physics_soft_wires].name.resize(instance.wire_physics_data[temp_index_holder.physics_soft_wires].editor_data.name_size);
		editor_file.read((char*)&instance.wire_physics_data[temp_index_holder.physics_soft_wires].name[0], instance.wire_physics_data[temp_index_holder.physics_soft_wires].editor_data.name_size);
		instance.wire_physics_data[temp_index_holder.physics_soft_wires].object_index = Global::object_index_counter;
		temp_index_holder.physics_soft_wires++;
	};

	// Labmda for Soft Bodies Physics Objects
	auto readSoftBodies = [&object_file, &temp_identifier, &readSpringMasses, &readWires]()->void {
		const std::function<void()> soft_bodies[2] = { readSpringMasses, readWires };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		soft_bodies[temp_identifier]();
	};

	// Lambda for Anchor Physics Objects
	auto readAnchors = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.anchor_physics_data[temp_index_holder.physics_anchors].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.anchor_physics_data[temp_index_holder.physics_anchors].anchor_data, sizeof(Object::Physics::Hinge::AnchorData));
		instance.anchor_physics_data[temp_index_holder.physics_anchors].name.resize(instance.anchor_physics_data[temp_index_holder.physics_anchors].editor_data.name_size);
		editor_file.read((char*)&instance.anchor_physics_data[temp_index_holder.physics_anchors].name[0], instance.anchor_physics_data[temp_index_holder.physics_anchors].editor_data.name_size);
		instance.anchor_physics_data[temp_index_holder.physics_anchors].object_index = Global::object_index_counter;
		temp_index_holder.physics_anchors++;
	};

	// Lambda for Hinge Physics Objects
	auto readHinges = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.hinge_physics_data[temp_index_holder.physics_hinges].uuid, sizeof(uint32_t));
		uint16_t file_name_size = 0;
		object_file.read((char*)&file_name_size, sizeof(uint16_t));
		object_file.read((char*)&instance.hinge_physics_data[temp_index_holder.physics_hinges].hinge_data, sizeof(Object::Physics::Hinge::HingeData));
		instance.hinge_physics_data[temp_index_holder.physics_hinges].file_name.resize(file_name_size);
		object_file.read((char*)&instance.hinge_physics_data[temp_index_holder.physics_hinges].file_name[0], file_name_size);
		instance.hinge_physics_data[temp_index_holder.physics_hinges].name.resize(instance.hinge_physics_data[temp_index_holder.physics_hinges].editor_data.name_size);
		editor_file.read((char*)&instance.hinge_physics_data[temp_index_holder.physics_anchors].name[0], instance.hinge_physics_data[temp_index_holder.physics_hinges].editor_data.name_size);
		instance.hinge_physics_data[temp_index_holder.physics_hinges].object_index = Global::object_index_counter;
		temp_index_holder.physics_hinges++;
	};

	// Lambda for HingeBase Physics Objects
	auto readHingeBases = [&object_file, &temp_identifier, &readAnchors, &readHinges]()->void {
		const std::function<void()> hinges[2] = { readAnchors, readHinges };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		hinges[temp_identifier]();
	};

	// Lambda for Physics Objects
	auto readPhysics = [&object_file, &temp_identifier, &readRigidBodies, &readSoftBodies, &readHingeBases]()->void {
		const std::function<void()> physics[3] = { readRigidBodies, readSoftBodies, readHingeBases };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		physics[temp_identifier]();
	};

	// Lambda for NPC Entities
	auto readNPCs = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].entity_data, sizeof(Object::Entity::EntityData));
		object_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].ai_script, sizeof(uint16_t));
		editor_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].editor_data, sizeof(EditorData));
		instance.npc_data[temp_index_holder.entity_npc].name.resize(instance.npc_data[temp_index_holder.entity_npc].editor_data.name_size);
		editor_file.read((char*)&instance.npc_data[temp_index_holder.entity_npc].name[0], instance.npc_data[temp_index_holder.entity_npc].editor_data.name_size);
		instance.npc_data[temp_index_holder.entity_npc].object_index = Global::object_index_counter;
		temp_index_holder.entity_npc++;
	};

	// Lambda for Controllable Entities
	auto readControllables = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.controllable_data[temp_index_holder.entity_controllable].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.controllable_data[temp_index_holder.entity_controllable].entity_data, sizeof(Object::Entity::EntityData));
		object_file.read((char*)&instance.controllable_data[temp_index_holder.entity_controllable].object_data, sizeof(Object::ObjectData));
		editor_file.read((char*)&instance.controllable_data[temp_index_holder.entity_controllable].editor_data, sizeof(EditorData));
		instance.controllable_data[temp_index_holder.entity_controllable].name.resize(instance.controllable_data[temp_index_holder.entity_controllable].editor_data.name_size);
		editor_file.read((char*)&instance.controllable_data[temp_index_holder.entity_controllable].name[0], instance.controllable_data[temp_index_holder.entity_controllable].editor_data.name_size);
		instance.controllable_data[temp_index_holder.entity_controllable].object_index = Global::object_index_counter;
		temp_index_holder.entity_controllable++;
	};

	// Lambda for Interactable Entities
	auto readInteractables = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].entity_data, sizeof(Object::Entity::EntityData));
		object_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].interactable_data, sizeof(Object::Entity::InteractableData));
		editor_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].editor_data, sizeof(EditorData));
		instance.interactable_data[temp_index_holder.entity_interactable].name.resize(instance.interactable_data[temp_index_holder.entity_interactable].editor_data.name_size);
		editor_file.read((char*)&instance.interactable_data[temp_index_holder.entity_interactable].name[0], instance.interactable_data[temp_index_holder.entity_interactable].editor_data.name_size);
		instance.interactable_data[temp_index_holder.entity_interactable].object_index = Global::object_index_counter;
		temp_index_holder.entity_interactable++;
	};

	// Lambda for Dynamic Entities
	auto readDynamics = [&object_file, &editor_file, &temp_index_holder, &instance]()->void {
		object_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].uuid, sizeof(uint32_t));
		object_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].entity_data, sizeof(Object::Entity::EntityData));
		object_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].object_data, sizeof(Object::ObjectData));
		object_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].dynamic_data, sizeof(Object::Entity::InteractableData));
		editor_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].editor_data, sizeof(EditorData));
		instance.dynamic_data[temp_index_holder.entity_dynamic].name.resize(instance.dynamic_data[temp_index_holder.entity_dynamic].editor_data.name_size);
		editor_file.read((char*)&instance.dynamic_data[temp_index_holder.entity_dynamic].name[0], instance.dynamic_data[temp_index_holder.entity_dynamic].editor_data.name_size);
		instance.dynamic_data[temp_index_holder.entity_dynamic].object_index = Global::object_index_counter;
		temp_index_holder.entity_dynamic++;
	};

	// Lambda for Entities
	auto readEntities = [&object_file, &temp_identifier, &readNPCs, &readControllables, &readInteractables, &readDynamics]()->void {
		std::function<void()> entities[4] = { readNPCs, readControllables, readInteractables, readDynamics };
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		entities[temp_identifier]();
	};

	// Lambda for Partical Generators

	// Array of Each Main Object Type
	std::function<void()> objects[5] = { readMasks, readTerrain, readLights, readPhysics, readEntities };

	// Iterate Through Object File Until All is Read
	//while (!object_file.eof())
	Global::object_index_counter++;
	while (object_file.tellg() < object_size)
	{
		object_file.read((char*)&temp_identifier, sizeof(uint8_t));
		objects[temp_identifier]();
		Global::object_index_counter++;
	}
	Global::object_index_counter++;

#endif

	// Close Files
	object_file.close();
	editor_file.close();
}

void Render::Objects::UnsavedLevel::buildObjectsHelper(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, LevelInstance& instance)
{
	// Static Iterator
	static uint16_t iterator = 0;

	// Allocate Memory for Active Array
	*active_objects = new Object::Object*[instance.header.number_of_loaded_objects];
	Object::Object** active_array = *active_objects;
	uint16_t active_index = 0;

	// Create Horizontal Line Floor Masks
	static HorizontalLineFloorData* horizontal_line_floor_data;
	static Object::Mask::Floor::FloorMaskLine* horizontal_line_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_horizontal_lines; iterator++)
	{
		horizontal_line_floor_data = &instance.horizontal_line_floor_data[iterator];
		horizontal_line_floor_mask = new Object::Mask::Floor::FloorMaskLine(horizontal_line_floor_data->horizontal_line_data, horizontal_line_floor_data->platform);
		horizontal_line_floor_mask->name = horizontal_line_floor_data->name;
		horizontal_line_floor_mask->clamp = horizontal_line_floor_data->editor_data.clamp;
		horizontal_line_floor_mask->lock = horizontal_line_floor_data->editor_data.lock;
		horizontal_line_floor_mask->object_index = horizontal_line_floor_data->object_index;
		objects[index] = horizontal_line_floor_mask;
		index++;
		active_array[active_index] = horizontal_line_floor_mask;
		active_index++;
	}

	// Create Slant Floor Masks
	static SlantFloorData* slant_floor_data;
	static Object::Mask::Floor::FloorMaskSlant* slant_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slants; iterator++)
	{
		slant_floor_data = &instance.slant_floor_data[iterator];
		slant_floor_mask = new Object::Mask::Floor::FloorMaskSlant(slant_floor_data->slant_data, slant_floor_data->platform);
		slant_floor_mask->name = slant_floor_data->name;
		slant_floor_mask->clamp = slant_floor_data->editor_data.clamp;
		slant_floor_mask->lock = slant_floor_data->editor_data.lock;
		slant_floor_mask->object_index = slant_floor_data->object_index;
		objects[index] = slant_floor_mask;
		index++;
		active_array[active_index] = slant_floor_mask;
		active_index++;
	}

	// Create Slope Floor Masks
	static SlopeFloorData* slope_floor_data;
	static Object::Mask::Floor::FloorMaskSlope* slope_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slopes; iterator++)
	{
		slope_floor_data = &instance.slope_floor_data[iterator];
		slope_floor_mask = new Object::Mask::Floor::FloorMaskSlope(slope_floor_data->slope_data, slope_floor_data->platform);
		slope_floor_mask->name = slope_floor_data->name;
		slope_floor_mask->clamp = slope_floor_data->editor_data.clamp;
		slope_floor_mask->lock = slope_floor_data->editor_data.lock;
		slope_floor_mask->object_index = slope_floor_data->object_index;
		objects[index] = slope_floor_mask;
		index++;
		active_array[active_index] = slope_floor_mask;
		active_index++;
	}

	// Create Vertical Line Left Wall Masks
	static VerticalLineLeftWallData* vertical_line_left_wall_data;
	static Object::Mask::Left::LeftMaskLine* vertical_line_left_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.left_mask_vertical_lines; iterator++)
	{
		vertical_line_left_wall_data = &instance.vertical_line_left_wall_data[iterator];
		vertical_line_left_wall_mask = new Object::Mask::Left::LeftMaskLine(vertical_line_left_wall_data->vertical_line_data);
		vertical_line_left_wall_mask->name = vertical_line_left_wall_data->name;
		vertical_line_left_wall_mask->clamp = vertical_line_left_wall_data->editor_data.clamp;
		vertical_line_left_wall_mask->lock = vertical_line_left_wall_data->editor_data.lock;
		vertical_line_left_wall_mask->object_index = vertical_line_left_wall_data->object_index;
		objects[index] = vertical_line_left_wall_mask;
		index++;
		active_array[active_index] = vertical_line_left_wall_mask;
		active_index++;
	}

	// Create Curve Left Wall Masks
	static CurveLeftWallData* curve_left_wall_data;
	static Object::Mask::Left::LeftMaskCurve* curve_left_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.left_mask_curves; iterator++)
	{
		curve_left_wall_data = &instance.curve_left_wall_data[iterator];
		curve_left_wall_mask = new Object::Mask::Left::LeftMaskCurve(curve_left_wall_data->curve_data);
		curve_left_wall_mask->name = curve_left_wall_data->name;
		curve_left_wall_mask->clamp = curve_left_wall_data->editor_data.clamp;
		curve_left_wall_mask->lock = curve_left_wall_data->editor_data.lock;
		curve_left_wall_mask->object_index = curve_left_wall_data->object_index;
		objects[index] = curve_left_wall_mask;
		index++;
		active_array[active_index] = curve_left_wall_mask;
		active_index++;
	}

	// Create Vertical Line Right Wall Masks
	static VerticalLineRightWallData* vertical_line_right_wall_data;
	static Object::Mask::Right::RightMaskLine* vertical_line_right_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.right_mask_vertical_lines; iterator++)
	{
		vertical_line_right_wall_data = &instance.vertical_line_right_wall_data[iterator];
		vertical_line_right_wall_mask = new Object::Mask::Right::RightMaskLine(vertical_line_right_wall_data->vertical_line_data);
		vertical_line_right_wall_mask->name = vertical_line_right_wall_data->name;
		vertical_line_right_wall_mask->clamp = vertical_line_right_wall_data->editor_data.clamp;
		vertical_line_right_wall_mask->lock = vertical_line_right_wall_data->editor_data.lock;
		vertical_line_right_wall_mask->object_index = vertical_line_right_wall_data->object_index;
		objects[index] = vertical_line_right_wall_mask;
		index++;
		active_array[active_index] = vertical_line_right_wall_mask;
		active_index++;
	}

	// Create Curve Right Wall Masks
	static CurveRightWallData* curve_right_wall_data;
	static Object::Mask::Right::RightMaskCurve* curve_right_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.right_mask_curves; iterator++)
	{
		curve_right_wall_data = &instance.curve_right_wall_data[iterator];
		curve_right_wall_mask = new Object::Mask::Right::RightMaskCurve(curve_right_wall_data->curve_data);
		curve_right_wall_mask->name = curve_right_wall_data->name;
		curve_right_wall_mask->clamp = curve_right_wall_data->editor_data.clamp;
		curve_right_wall_mask->lock = curve_right_wall_data->editor_data.lock;
		curve_right_wall_mask->object_index = curve_right_wall_data->object_index;
		objects[index] = curve_right_wall_mask;
		index++;
		active_array[active_index] = curve_right_wall_mask;
		active_index++;
	}

	// Create Horizontal Line Ceiling Masks
	static HorizontalLineCeilingData* horizontal_line_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskLine* horizontal_line_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_horizontal_lines; iterator++)
	{
		horizontal_line_ceiling_data = &instance.horizontal_line_ceiling_data[iterator];
		horizontal_line_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskLine(horizontal_line_ceiling_data->horizontal_line_data);
		horizontal_line_ceiling_mask->name = horizontal_line_ceiling_data->name;
		horizontal_line_ceiling_mask->clamp = horizontal_line_ceiling_data->editor_data.clamp;
		horizontal_line_ceiling_mask->lock = horizontal_line_ceiling_data->editor_data.lock;
		horizontal_line_ceiling_mask->object_index = horizontal_line_ceiling_data->object_index;
		objects[index] = horizontal_line_ceiling_mask;
		index++;
		active_array[active_index] = horizontal_line_ceiling_mask;
		active_index++;
	}

	// Create Slant Ceiling Masks
	static SlantCeilingData* slant_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskSlant* slant_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slants; iterator++)
	{
		slant_ceiling_data = &instance.slant_ceiling_data[iterator];
		slant_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskSlant(slant_ceiling_data->slant_data);
		slant_ceiling_mask->name = slant_ceiling_data->name;
		slant_ceiling_mask->clamp = slant_ceiling_data->editor_data.clamp;
		slant_ceiling_mask->lock = slant_ceiling_data->editor_data.lock;
		slant_ceiling_mask->object_index = slant_ceiling_data->object_index;
		objects[index] = slant_ceiling_mask;
		index++;
		active_array[active_index] = slant_ceiling_mask;
		active_index++;
	}

	// Create Slope Ceiling Masks
	static SlopeCeilingData* slope_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskSlope* slope_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slopes; iterator++)
	{
		slope_ceiling_data = &instance.slope_ceiling_data[iterator];
		slope_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskSlope(slope_ceiling_data->slope_data);
		slope_ceiling_mask->name = slope_ceiling_data->name;
		slope_ceiling_mask->clamp = slope_ceiling_data->editor_data.clamp;
		slope_ceiling_mask->lock = slope_ceiling_data->editor_data.lock;
		slope_ceiling_mask->object_index = slope_ceiling_data->object_index;
		objects[index] = slope_ceiling_mask;
		index++;
		active_array[active_index] = slope_ceiling_mask;
		active_index++;
	}

	// Create Trigger Masks
	static TriggerMaskData* trigger_data;
	static Object::Mask::Trigger::TriggerMask* trigger_mask;
	for (iterator = 0; iterator < instance.editor_header.trigger_masks; iterator++)
	{
		trigger_data = &instance.trigger_data[iterator];
		trigger_mask = new Object::Mask::Trigger::TriggerMask(trigger_data->trigger_data);
		trigger_mask->name = trigger_data->name;
		trigger_mask->clamp = trigger_data->editor_data.clamp;
		trigger_mask->lock = trigger_data->editor_data.lock;
		trigger_mask->object_index = trigger_data->object_index;
		objects[index] = trigger_mask;
		index++;
		active_array[active_index] = trigger_mask;
		active_index++;
	}

	// Create Foreground Objects
	static ForegroundTerrainData* foreground_terrain_data;
	static Object::Terrain::Foreground* foreground_terrain_object;
	for (iterator = 0; iterator < instance.editor_header.foreground_terrain; iterator++)
	{
		foreground_terrain_data = &instance.foreground_terrain_data[iterator];
		foreground_terrain_object = new Object::Terrain::Foreground(foreground_terrain_data->shape_data, foreground_terrain_data->object_data);
		foreground_terrain_object->name = foreground_terrain_data->name;
		foreground_terrain_object->clamp = foreground_terrain_data->editor_data.clamp;
		foreground_terrain_object->lock = foreground_terrain_data->editor_data.lock;
		foreground_terrain_object->object_index = foreground_terrain_data->object_index;
		objects[index] = foreground_terrain_object;
		index++;
		active_array[active_index] = foreground_terrain_object;
		active_index++;
	}

	// Create Formerground Objects
	static FormergroundTerrainData* formerground_terrain_data;
	static Object::Terrain::Formerground* formerground_terrain_object;
	for (iterator = 0; iterator < instance.editor_header.formerground_terrain; iterator++)
	{
		formerground_terrain_data = &instance.formerground_terrain_data[iterator];
		formerground_terrain_object = new Object::Terrain::Formerground(formerground_terrain_data->shape_data, formerground_terrain_data->object_data);
		formerground_terrain_object->name = formerground_terrain_data->name;
		formerground_terrain_object->clamp = formerground_terrain_data->editor_data.clamp;
		formerground_terrain_object->lock = formerground_terrain_data->editor_data.lock;
		formerground_terrain_object->object_index = formerground_terrain_data->object_index;
		objects[index] = formerground_terrain_object;
		index++;
		active_array[active_index] = formerground_terrain_object;
		active_index++;
	}

	// Create Background Objects
	static BackgroundTerrainData* background_terrain_data;
	static Object::Terrain::Background* background_terrain_object;
	for (iterator = 0; iterator < instance.editor_header.background_terrain; iterator++)
	{
		background_terrain_data = &instance.background_terrain_data[iterator];
		background_terrain_object = new Object::Terrain::Background(background_terrain_data->shape_data, background_terrain_data->object_data, background_terrain_data->layer);
		background_terrain_object->name = background_terrain_data->name;
		background_terrain_object->clamp = background_terrain_data->editor_data.clamp;
		background_terrain_object->lock = background_terrain_data->editor_data.lock;
		background_terrain_object->object_index = background_terrain_data->object_index;
		objects[index] = background_terrain_object;
		index++;
		active_array[active_index] = background_terrain_object;
		active_index++;
	}

	// Create Backdrop Objects
	static BackdropTerrainData* backdrop_terrain_data;
	static Object::Terrain::Backdrop* backdrop_terrain_object;
	for (iterator = 0; iterator < instance.editor_header.backdrop_terrain; iterator++)
	{
		backdrop_terrain_data = &instance.backdrop_terrain_data[iterator];
		backdrop_terrain_object = new Object::Terrain::Backdrop(backdrop_terrain_data->shape_data, backdrop_terrain_data->object_data);
		backdrop_terrain_object->name = backdrop_terrain_data->name;
		backdrop_terrain_object->clamp = backdrop_terrain_data->editor_data.clamp;
		backdrop_terrain_object->lock = backdrop_terrain_data->editor_data.lock;
		backdrop_terrain_object->object_index = backdrop_terrain_data->object_index;
		objects[index] = backdrop_terrain_object;
		index++;
		active_array[active_index] = backdrop_terrain_object;
		active_index++;
	}

	// Create Directional Lights
	static DirectionalLightData* directional_light_data;
	static Object::Light::Directional::Directional* directional_light_object;
	for (iterator = 0; iterator < instance.editor_header.directional_lights; iterator++)
	{
		directional_light_data = &instance.directional_light_data[iterator];
		directional_light_object = new Object::Light::Directional::Directional(directional_light_data->directional_data, directional_light_data->light_data);
		directional_light_object->name = directional_light_data->name;
		directional_light_object->clamp = directional_light_data->editor_data.clamp;
		directional_light_object->lock = directional_light_data->editor_data.lock;
		directional_light_object->object_index = directional_light_data->object_index;
		objects[index] = directional_light_object;
		index++;
		active_array[active_index] = directional_light_object;
		active_index++;
	}

	// Create Point Lights
	static PointLightData* point_light_data;
	static Object::Light::Point::Point* point_light_object;
	for (iterator = 0; iterator < instance.editor_header.point_lights; iterator++)
	{
		point_light_data = &instance.point_light_data[iterator];
		point_light_object = new Object::Light::Point::Point(point_light_data->point_data, point_light_data->light_data);
		point_light_object->name = point_light_data->name;
		point_light_object->clamp = point_light_data->editor_data.clamp;
		point_light_object->lock = point_light_data->editor_data.lock;
		point_light_object->object_index = point_light_data->object_index;
		objects[index] = point_light_object;
		index++;
		active_array[active_index] = point_light_object;
		active_index++;
	}

	// Create Spot Lights
	static SpotLightData* spot_light_data;
	static Object::Light::Spot::Spot* spot_light_object;
	for (iterator = 0; iterator < instance.editor_header.spot_lights; iterator++)
	{
		spot_light_data = &instance.spot_light_data[iterator];
		spot_light_object = new Object::Light::Spot::Spot(spot_light_data->spot_data, spot_light_data->light_data);
		spot_light_object->name = spot_light_data->name;
		spot_light_object->clamp = spot_light_data->editor_data.clamp;
		spot_light_object->lock = spot_light_data->editor_data.lock;
		spot_light_object->object_index = spot_light_data->object_index;
		objects[index] = spot_light_object;
		index++;
		active_array[active_index] = spot_light_object;
		active_index++;
	}

	// Create Beam Lights
	static BeamLightData* beam_light_data;
	static Object::Light::Beam::Beam* beam_light_object;
	for (iterator = 0; iterator < instance.editor_header.beam_lights; iterator++)
	{
		beam_light_data = &instance.beam_light_data[iterator];
		beam_light_object = new Object::Light::Beam::Beam(beam_light_data->beam_data, beam_light_data->light_data);
		beam_light_object->name = beam_light_data->name;
		beam_light_object->clamp = beam_light_data->editor_data.clamp;
		beam_light_object->lock = beam_light_data->editor_data.lock;
		beam_light_object->object_index = beam_light_data->object_index;
		objects[index] = beam_light_object;
		index++;
		active_array[active_index] = beam_light_object;
		active_index++;
	}

	// Create Rigid Bodies
	static RigidBodyPhysicsData* rigid_body_data;
	static Object::Physics::Rigid::RigidBody* rigid_body_object;
	for (iterator = 0; iterator < instance.editor_header.physics_rigid; iterator++)
	{
		rigid_body_data = &instance.rigid_body_physics_data[iterator];
		switch (rigid_body_data->shape_data->shape) {
		case Shape::RECTANGLE: {rigid_body_object = new Object::Physics::Rigid::RigidRectangle(rigid_body_data->uuid, rigid_body_data->object_data, rigid_body_data->rigid_body_data, rigid_body_data->shape_data); break; }
		case Shape::TRAPEZOID: {rigid_body_object = new Object::Physics::Rigid::RigidTrapezoid(rigid_body_data->uuid, rigid_body_data->object_data, rigid_body_data->rigid_body_data, rigid_body_data->shape_data); break; }
		case Shape::TRIANGLE:  {rigid_body_object = new Object::Physics::Rigid::RigidTriangle (rigid_body_data->uuid, rigid_body_data->object_data, rigid_body_data->rigid_body_data, rigid_body_data->shape_data); break; }
		case Shape::CIRCLE:    {rigid_body_object = new Object::Physics::Rigid::RigidCircle   (rigid_body_data->uuid, rigid_body_data->object_data, rigid_body_data->rigid_body_data, rigid_body_data->shape_data); break; }
		case Shape::POLYGON:   {rigid_body_object = new Object::Physics::Rigid::RigidPolygon  (rigid_body_data->uuid, rigid_body_data->object_data, rigid_body_data->rigid_body_data, rigid_body_data->shape_data); break; }
		}
		rigid_body_object->name = rigid_body_data->name;
		rigid_body_object->clamp = rigid_body_data->editor_data.clamp;
		rigid_body_object->lock = rigid_body_data->editor_data.lock;
		rigid_body_object->object_index = rigid_body_data->object_index;
		if (!physics.appendStatic(rigid_body_object))
			delete rigid_body_object;
	}

	// Create Spring Masses
	static SpringMassPhysicsData* spring_mass_data;
	static Object::Physics::Soft::SpringMass* spring_mass_object;
	for (iterator = 0; iterator < instance.editor_header.physics_soft_springmass; iterator++)
	{
		spring_mass_data = &instance.spring_mass_physics_data[iterator];
		spring_mass_object = new Object::Physics::Soft::SpringMass(spring_mass_data->uuid, spring_mass_data->object_data, spring_mass_data->file_name);
		spring_mass_object->name = spring_mass_data->name;
		spring_mass_object->clamp = spring_mass_data->editor_data.clamp;
		spring_mass_object->lock = spring_mass_data->editor_data.lock;
		spring_mass_object->object_index = spring_mass_data->object_index;
		physics.appendStatic(spring_mass_object);
	}

	// Create Wires
	static WirePhysicsData* wire_data;
	static Object::Physics::Soft::Wire* wire_object;
	for (iterator = 0; iterator < instance.editor_header.physics_soft_wires; iterator++)
	{
		wire_data = &instance.wire_physics_data[iterator];
		wire_object = new Object::Physics::Soft::Wire(wire_data->uuid, wire_data->object_data, wire_data->wire_data);
		wire_object->name = wire_data->name;
		wire_object->clamp = wire_data->editor_data.clamp;
		wire_object->lock = wire_data->editor_data.lock;
		wire_object->object_index = wire_data->object_index;
		physics.appendStatic(wire_object);
	}

	// Create Anchor
	static AnchorPhysicsData* anchor_data;
	static Object::Physics::Hinge::Anchor* anchor_object;
	for (iterator = 0; iterator < instance.editor_header.physics_anchors; iterator++)
	{
		anchor_data = &instance.anchor_physics_data[iterator];
		anchor_object = new Object::Physics::Hinge::Anchor(anchor_data->uuid, anchor_data->anchor_data);
		anchor_object->name = anchor_data->name;
		anchor_object->clamp = anchor_data->editor_data.clamp;
		anchor_object->lock = anchor_data->editor_data.lock;
		anchor_object->object_index = anchor_data->object_index;
		physics.appendStatic(anchor_object);
	}

	// Create Hinge
	static HingePhysicsData* hinge_data;
	static Object::Physics::Hinge::Hinge* hinge_object;
	for (iterator = 0; iterator < instance.editor_header.physics_hinges; iterator++)
	{
		hinge_data = &instance.hinge_physics_data[iterator];
		hinge_object = new Object::Physics::Hinge::Hinge(hinge_data->uuid, hinge_data->hinge_data, hinge_data->file_name);
		hinge_object->name = hinge_data->name;
		hinge_object->clamp = hinge_data->editor_data.clamp;
		hinge_object->lock = hinge_data->editor_data.lock;
		hinge_object->object_index = hinge_data->object_index;
		physics.appendStatic(hinge_object);
	}

	// Create NPCs
	static NPC_Data* npc_data;
	static Object::Entity::NPC* npc_object;
	for (iterator = 0; iterator < instance.editor_header.entity_npc; iterator++)
	{
		npc_data = &instance.npc_data[iterator];
		npc_object = new Object::Entity::NPC(npc_data->uuid, npc_data->entity_data, npc_data->object_data, npc_data->ai_script);
		npc_object->name = npc_data->name;
		npc_object->clamp = npc_data->editor_data.clamp;
		npc_object->lock = npc_data->editor_data.lock;
		npc_object->object_index = npc_data->object_index;
		entities.appendStatic(npc_object);
	}

	// Create Controllables
	static ControllableData* controllable_data;
	static Object::Entity::Controllables* controllable_object;
	for (iterator = 0; iterator < instance.editor_header.entity_controllable; iterator++)
	{
		controllable_data = &instance.controllable_data[iterator];
		controllable_object = new Object::Entity::Controllables(controllable_data->uuid, controllable_data->entity_data, controllable_data->object_data);
		controllable_object->name = controllable_data->name;
		controllable_object->clamp = controllable_data->editor_data.clamp;
		controllable_object->lock = controllable_data->editor_data.lock;
		controllable_object->object_index = controllable_data->object_index;
		entities.appendStatic(controllable_object);
	}

	// Create Interactables
	static InteractableData* interactable_data;
	static Object::Entity::Interactables* interactable_object;
	for (iterator = 0; iterator < instance.editor_header.entity_interactable; iterator++)
	{
		interactable_data = &instance.interactable_data[iterator];
		interactable_object = new Object::Entity::Interactables(interactable_data->uuid, interactable_data->entity_data, interactable_data->object_data, interactable_data->interactable_data);
		interactable_object->name = interactable_data->name;
		interactable_object->clamp = interactable_data->editor_data.clamp;
		interactable_object->lock = interactable_data->editor_data.lock;
		interactable_object->object_index = interactable_data->object_index;
		entities.appendStatic(interactable_object);
	}

	// Create Dynamics
	static DynamicData* dynamic_data;
	static Object::Entity::Dynamics* dynamic_object;
	for (iterator = 0; iterator < instance.editor_header.entity_dynamic; iterator++)
	{
		dynamic_data = &instance.dynamic_data[iterator];
		dynamic_object = new Object::Entity::Dynamics(dynamic_data->uuid, dynamic_data->entity_data, dynamic_data->object_data, dynamic_data->dynamic_data);
		dynamic_object->name = dynamic_data->name;
		dynamic_object->clamp = dynamic_data->editor_data.clamp;
		dynamic_object->lock = dynamic_data->editor_data.lock;
		dynamic_object->object_index = dynamic_data->object_index;
		entities.appendStatic(dynamic_object);
	}
}

void Render::Objects::UnsavedLevel::writeInstance(LevelInstance& instance)
{
	// Array of Objects
	Object::Object** objects = new Object::Object*[instance.header.number_of_loaded_objects];
	uint16_t temp_var = 0;

	// Array of Physics
	Struct::List<Object::Physics::PhysicsBase> physics = Struct::List<Object::Physics::PhysicsBase>();

	// Array of Entities
	Struct::List<Object::Entity::EntityBase> entities = Struct::List<Object::Entity::EntityBase>();

	// Build Objects
	buildObjects(objects, temp_var, physics, entities);

	// Files
	std::ofstream object_file;
	std::ofstream editor_file;

	// Open Files
	object_file.open(object_path, std::ios::binary);
	editor_file.open(editor_path, std::ios::binary);

	// Write Headers
	object_file.write((char*)&instance.header, sizeof(LevelHeader));
	editor_file.write((char*)&instance.editor_header, sizeof(EditorHeader));

	// Write Data to Files
	for (int i = 0; i < temp_var; i++)
	{
		objects[i]->write(object_file, editor_file);
		delete objects[i];
	}

	for (Struct::List<Object::Physics::PhysicsBase>::Iterator it = physics.beginStatic(); it != physics.endStatic(); it++)
		(*it).write(object_file, editor_file);

	for (Struct::List<Object::Entity::EntityBase>::Iterator it = entities.beginStatic(); it != entities.endStatic(); it++)
		(*it).write(object_file, editor_file);

	// Delete Arrays
	delete[] objects;

	// Close Files
	object_file.close();
	editor_file.close();
}

Render::Objects::UnsavedLevel::LevelInstance* Render::Objects::UnsavedLevel::makeCopyOfInstance(LevelInstance& instance)
{
	// Iterator
	uint16_t iterator = 0;

	// Create a New Instance
	LevelInstance* new_instance_ = new LevelInstance;
	LevelInstance& new_instance = *new_instance_;

	// Copy Header Data
	new_instance.header = instance.header;
	new_instance.editor_header = instance.editor_header;

	// Set Number of Stack Instances to 1
	new_instance.master_stack_instances = 1;

	// Reallocate Memory for Arrays
	allocateForArrays(new_instance);

	// Copy Data Into Arrays

	// Copy Horizontal Line Floor Data
	for (iterator = 0; iterator < instance.editor_header.floor_mask_horizontal_lines; iterator++)
		new_instance.horizontal_line_floor_data[iterator] = instance.horizontal_line_floor_data[iterator];

	// Copy Slant Floor Data
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slants; iterator++)
		new_instance.slant_floor_data[iterator] = instance.slant_floor_data[iterator];

	// Copy Slope Floor Data
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slopes; iterator++)
		new_instance.slope_floor_data[iterator] = instance.slope_floor_data[iterator];

	// Copy Vertical Line Left Wall Data
	for (iterator = 0; iterator < instance.editor_header.left_mask_vertical_lines; iterator++)
		new_instance.vertical_line_left_wall_data[iterator] = instance.vertical_line_left_wall_data[iterator];

	// Copy Curve Left Wall Data
	for (iterator = 0; iterator < instance.editor_header.left_mask_curves; iterator++)
		new_instance.curve_left_wall_data[iterator] = instance.curve_left_wall_data[iterator];

	// Copy Vertical Line Right Wall Data
	for (iterator = 0; iterator < instance.editor_header.right_mask_vertical_lines; iterator++)
		new_instance.vertical_line_right_wall_data[iterator] = instance.vertical_line_right_wall_data[iterator];

	// Copy Curve Right Wall Data
	for (iterator = 0; iterator < instance.editor_header.right_mask_curves; iterator++)
		new_instance.curve_right_wall_data[iterator] = instance.curve_right_wall_data[iterator];

	// Copy Horizontal Line Ceiling Data
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_horizontal_lines; iterator++)
		new_instance.horizontal_line_ceiling_data[iterator] = instance.horizontal_line_ceiling_data[iterator];

	// Copy Slant Ceiling Data
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slants; iterator++)
		new_instance.slant_ceiling_data[iterator] = instance.slant_ceiling_data[iterator];

	// Copy Slope Ceiling Data
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slopes; iterator++)
		new_instance.slope_ceiling_data[iterator] = instance.slope_ceiling_data[iterator];

	// Copy Trigger Mask Data
	for (iterator = 0; iterator < instance.editor_header.trigger_masks; iterator++)
		new_instance.trigger_data[iterator] = instance.trigger_data[iterator];

	// Copy Foreground Terrain Data
	for (iterator = 0; iterator < instance.editor_header.foreground_terrain; iterator++)
		new_instance.foreground_terrain_data[iterator] = instance.foreground_terrain_data[iterator];

	// Copy Formerground Terrain Data
	for (iterator = 0; iterator < instance.editor_header.formerground_terrain; iterator++)
		new_instance.formerground_terrain_data[iterator] = instance.formerground_terrain_data[iterator];

	// Copy Background Terrain Data
	for (iterator = 0; iterator < instance.editor_header.background_terrain; iterator++)
		new_instance.background_terrain_data[iterator] = instance.background_terrain_data[iterator];

	// Copy Backdrop Terrain Data
	for (iterator = 0; iterator < instance.editor_header.backdrop_terrain; iterator++)
		new_instance.backdrop_terrain_data[iterator] = instance.backdrop_terrain_data[iterator];

	// Copy Directional Light Data
	for (iterator = 0; iterator < instance.editor_header.directional_lights; iterator++)
		new_instance.directional_light_data[iterator] = instance.directional_light_data[iterator];

	// Copy Point Light Data
	for (iterator = 0; iterator < instance.editor_header.point_lights; iterator++)
		new_instance.point_light_data[iterator] = instance.point_light_data[iterator];

	// Copy Spot Light Data
	for (iterator = 0; iterator < instance.editor_header.spot_lights; iterator++)
		new_instance.spot_light_data[iterator] = instance.spot_light_data[iterator];

	// Copy Beam Light Data
	for (iterator = 0; iterator < instance.editor_header.beam_lights; iterator++)
		new_instance.beam_light_data[iterator] = instance.beam_light_data[iterator];

	// Copy Rigid Body Data
	for (iterator = 0; iterator < instance.editor_header.physics_rigid; iterator++)
		new_instance.rigid_body_physics_data[iterator] = instance.rigid_body_physics_data[iterator];

	// Copy Spring Mass Data
	for (iterator = 0; iterator < instance.editor_header.physics_soft_springmass; iterator++)
		new_instance.spring_mass_physics_data[iterator] = instance.spring_mass_physics_data[iterator];

	// Copy Wire Data
	for (iterator = 0; iterator < instance.editor_header.physics_soft_wires; iterator++)
		new_instance.wire_physics_data[iterator] = instance.wire_physics_data[iterator];

	// Copy Anchor Data
	for (iterator = 0; iterator < instance.editor_header.physics_anchors; iterator++)
		new_instance.anchor_physics_data[iterator] = instance.anchor_physics_data[iterator];

	// Copy Hinge Data
	for (iterator = 0; iterator < instance.editor_header.physics_hinges; iterator++)
		new_instance.hinge_physics_data[iterator] = instance.hinge_physics_data[iterator];

	// Copy NPC Data
	for (iterator = 0; iterator < instance.editor_header.entity_npc; iterator++)
		new_instance.npc_data[iterator] = instance.npc_data[iterator];

	// Copy Controllable Entity Data
	for (iterator = 0; iterator < instance.editor_header.entity_controllable; iterator++)
		new_instance.controllable_data[iterator] = instance.controllable_data[iterator];

	// Copy Interactable Entity Data
	for (iterator = 0; iterator < instance.editor_header.entity_interactable; iterator++)
		new_instance.interactable_data[iterator] = instance.interactable_data[iterator];

	// Copy Dynamic Entity Data
	for (iterator = 0; iterator < instance.editor_header.entity_dynamic; iterator++)
		new_instance.dynamic_data[iterator] = instance.dynamic_data[iterator];

	// Return Pointer to Instance
	return new_instance_;
}

Shape::Shape* Render::Objects::UnsavedLevel::getShapePointer(Editor::Selector* selector)
{
	switch (selector->object_identifier[2])
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Rectangle(selector->rectangle_data));
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		return static_cast<Shape::Shape*>(new Shape::Trapezoid(selector->trapezoid_data));
	}

	// Triangle Data
	case Shape::TRIANGLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Triangle(selector->triangle_data));
	}

	// Circle Data
	case Shape::CIRCLE:
	{
		return static_cast<Shape::Shape*>(new Shape::Circle(selector->circle_data));
	}

	// Polygon Data
	case Shape::POLYGON:
	{
		return static_cast<Shape::Shape*>(new Shape::Polygon(selector->polygon_data));
	}

	}

	return nullptr;
}

Render::Objects::UnsavedLevel::UnsavedLevel()
{
	// Constant Vertex Positions
	const float vertices[] = {
		0.1f,   0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		127.9f, 63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   63.9f, -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
		0.1f,   0.1f,  -1.0f, 0.0f, 0.9f, 0.0f, 1.0f,
	};

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(128.0f * level_x, 64.0f * level_y, 0.0f));

	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Store Vertex Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Render::Objects::UnsavedLevel::~UnsavedLevel()
{
	// Delete the Stack
	slave_stack.deleteStack();

	// Delete the Unmodified Data
	slave_stack.deleteInstance(unmodified_data);
}

void Render::Objects::UnsavedLevel::constructUnmodifiedData(int16_t x, int16_t y, uint8_t z)
{
	// Store Coordinates of Level
	level_x = x;
	level_y = y;
	level_version = z;

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(128.0f * level_x, 64.0f * level_y, 0.0f));

	// Generate File Name
	std::string file_name = "";
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(x)));
	file_name.append(",");
	file_name.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(y)));

	// Get Paths of Level
	object_path = Global::level_data_path + file_name;
	editor_path = Global::editor_level_data_path + file_name;

	// Create Memory for Unmodified Data
	unmodified_data = new LevelInstance();

	// Increment Stack Instance Count
	unmodified_data->master_stack_instances++;

	// Store Memory in First Index in Stack Array
	slave_stack.storeUnmodified(unmodified_data);

	// Read Unmodified Data
	constructUnmodifiedDataHelper(*unmodified_data);
}

void Render::Objects::UnsavedLevel::buildObjects(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities)
{
	// Get Instance From Stack
	LevelInstance* instance = slave_stack.returnInstance();

	// Construct Objects
	buildObjectsHelper(objects, index, physics, entities, *instance);
}

void Render::Objects::UnsavedLevel::changeToModified()
{
	float colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::changeToUnmodified()
{
	float colors[4] = { 0.0f, 0.7f, 1.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::changeToSaved()
{
	float colors[4] = { 0.0f, 0.9f, 0.0f, 1.0f };
	changeColors(colors);
}

void Render::Objects::UnsavedLevel::switchInstance(uint8_t selected_instance)
{
	// Set Stack Index
	slave_stack.switchInstance(selected_instance);

	// Selected Unmodified is Equal to Unmodified Bool in Instance
	selected_unmodified = false;
	if (slave_stack.returnInstance()->unmodified)
	{
		selected_unmodified = true;
		changeToUnmodified();
	}
}

void Render::Objects::UnsavedLevel::changeColors(float* color)
{
	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Change Colors
	for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), color);
	}

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::Objects::UnsavedLevel::incrementStackApperance(uint8_t index)
{
	slave_stack.incrementStackInstances(index);
}

void Render::Objects::UnsavedLevel::decrementStackApperance(uint8_t index)
{
	slave_stack.decrementStackInstances(index);
}

Render::Objects::LevelHeader Render::Objects::UnsavedLevel::returnObjectHeader()
{
	return slave_stack.returnInstance()->header;
}

void Render::Objects::UnsavedLevel::drawVisualizer()
{
	// If Stack Size is Less Than or Equal to 1, Don't Draw Visualizer
	if (slave_stack.isEmpty() && !saved)
		return;

	// Bind Model Matrix
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	
	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 8);
	glBindVertexArray(0);
}

void Render::Objects::UnsavedLevel::returnFloorMasks(Object::Mask::Floor::FloorMask*** floor_masks, int& floor_masks_size, int uuid)
{
	// Get Current Instance
	int iterator = 0;
	LevelInstance& instance = *slave_stack.returnInstance();
	
	// Determine the Number of Objects to Load
	floor_masks_size = 0;
	for (int i = 0; i < instance.editor_header.floor_mask_horizontal_lines; i++)
		if (instance.horizontal_line_floor_data[i].object_index != uuid)
			floor_masks_size++;
	for (int i = 0; i < instance.editor_header.floor_mask_slants; i++)
		if (instance.slant_floor_data[i].object_index != uuid)
			floor_masks_size++;
	for (int i = 0; i < instance.editor_header.floor_mask_slopes; i++)
		if (instance.slope_floor_data[i].object_index != uuid)
			floor_masks_size++;

	// Allocate Memory for Objects
	*floor_masks = new Object::Mask::Floor::FloorMask*[floor_masks_size];
	floor_masks_size = 0;

	// Create Horizontal Line Floor Masks
	static HorizontalLineFloorData* horizontal_line_floor_data;
	static Object::Mask::Floor::FloorMaskLine* horizontal_line_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_horizontal_lines; iterator++)
	{
		horizontal_line_floor_data = &instance.horizontal_line_floor_data[iterator];
		if (horizontal_line_floor_data->object_index != uuid)
		{
			horizontal_line_floor_mask = new Object::Mask::Floor::FloorMaskLine(horizontal_line_floor_data->horizontal_line_data, horizontal_line_floor_data->platform);
			horizontal_line_floor_mask->name = horizontal_line_floor_data->name;
			horizontal_line_floor_mask->clamp = horizontal_line_floor_data->editor_data.clamp;
			horizontal_line_floor_mask->lock = horizontal_line_floor_data->editor_data.lock;
			horizontal_line_floor_mask->object_index = 0;
			(*floor_masks)[floor_masks_size] = horizontal_line_floor_mask;
			floor_masks_size++;
		}
	}

	// Create Slant Floor Masks
	static SlantFloorData* slant_floor_data;
	static Object::Mask::Floor::FloorMaskSlant* slant_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slants; iterator++)
	{
		slant_floor_data = &instance.slant_floor_data[iterator];
		if (slant_floor_data->object_index != uuid)
		{
			slant_floor_mask = new Object::Mask::Floor::FloorMaskSlant(slant_floor_data->slant_data, slant_floor_data->platform);
			slant_floor_mask->name = slant_floor_data->name;
			slant_floor_mask->clamp = slant_floor_data->editor_data.clamp;
			slant_floor_mask->lock = slant_floor_data->editor_data.lock;
			slant_floor_mask->object_index = 1;
			(*floor_masks)[floor_masks_size] = slant_floor_mask;
			floor_masks_size++;
		}
	}

	// Create Slope Floor Masks
	static SlopeFloorData* slope_floor_data;
	static Object::Mask::Floor::FloorMaskSlope* slope_floor_mask;
	for (iterator = 0; iterator < instance.editor_header.floor_mask_slopes; iterator++)
	{
		slope_floor_data = &instance.slope_floor_data[iterator];
		if (slope_floor_data->object_index != uuid)
		{
			slope_floor_mask = new Object::Mask::Floor::FloorMaskSlope(slope_floor_data->slope_data, slope_floor_data->platform);
			slope_floor_mask->name = slope_floor_data->name;
			slope_floor_mask->clamp = slope_floor_data->editor_data.clamp;
			slope_floor_mask->lock = slope_floor_data->editor_data.lock;
			slope_floor_mask->object_index = 2;
			(*floor_masks)[floor_masks_size] = slope_floor_mask;
			floor_masks_size++;
		}
	}
}

void Render::Objects::UnsavedLevel::returnLeftMasks(Object::Mask::Left::LeftMask*** left_masks, int& left_masks_size, int uuid)
{
	// Get Current Instance
	int iterator = 0;
	LevelInstance& instance = *slave_stack.returnInstance();

	// Determine the Number of Objects to Load
	left_masks_size = 0;
	for (int i = 0; i < instance.editor_header.left_mask_vertical_lines; i++)
		if (instance.vertical_line_left_wall_data[i].object_index != uuid)
			left_masks_size++;
	for (int i = 0; i < instance.editor_header.left_mask_curves; i++)
		if (instance.curve_left_wall_data[i].object_index != uuid)
			left_masks_size++;

	// Allocate Memory for Objects
	*left_masks = new Object::Mask::Left::LeftMask*[instance.editor_header.left_mask_vertical_lines + instance.editor_header.left_mask_curves];
	left_masks_size = 0;

	// Create Vertical Line Left Wall Masks
	static VerticalLineLeftWallData* vertical_line_left_wall_data;
	static Object::Mask::Left::LeftMaskLine* vertical_line_left_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.left_mask_vertical_lines; iterator++)
	{
		vertical_line_left_wall_data = &instance.vertical_line_left_wall_data[iterator];
		if (vertical_line_left_wall_data->object_index != uuid)
		{
			vertical_line_left_wall_mask = new Object::Mask::Left::LeftMaskLine(vertical_line_left_wall_data->vertical_line_data);
			vertical_line_left_wall_mask->name = vertical_line_left_wall_data->name;
			vertical_line_left_wall_mask->clamp = vertical_line_left_wall_data->editor_data.clamp;
			vertical_line_left_wall_mask->lock = vertical_line_left_wall_data->editor_data.lock;
			vertical_line_left_wall_mask->object_index = vertical_line_left_wall_data->object_index;
			(*left_masks)[left_masks_size] = vertical_line_left_wall_mask;
			left_masks_size++;
		}
	}

	// Create Curve Left Wall Masks
	static CurveLeftWallData* curve_left_wall_data;
	static Object::Mask::Left::LeftMaskCurve* curve_left_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.left_mask_curves; iterator++)
	{
		curve_left_wall_data = &instance.curve_left_wall_data[iterator];
		if (curve_left_wall_data->object_index != uuid)
		{
			curve_left_wall_mask = new Object::Mask::Left::LeftMaskCurve(curve_left_wall_data->curve_data);
			curve_left_wall_mask->name = curve_left_wall_data->name;
			curve_left_wall_mask->clamp = curve_left_wall_data->editor_data.clamp;
			curve_left_wall_mask->lock = curve_left_wall_data->editor_data.lock;
			curve_left_wall_mask->object_index = curve_left_wall_data->object_index;
			(*left_masks)[left_masks_size] = curve_left_wall_mask;
			left_masks_size++;
		}
	}
}

void Render::Objects::UnsavedLevel::returnRightMasks(Object::Mask::Right::RightMask*** right_masks, int& right_masks_size, int uuid)
{
	// Get Current Instance
	int iterator = 0;
	LevelInstance& instance = *slave_stack.returnInstance();

	// Determine the Number of Objects to Load
	right_masks_size = 0;
	for (int i = 0; i < instance.editor_header.right_mask_vertical_lines; i++)
		if (instance.vertical_line_right_wall_data[i].object_index != uuid)
			right_masks_size++;
	for (int i = 0; i < instance.editor_header.right_mask_curves; i++)
		if (instance.curve_right_wall_data[i].object_index != uuid)
			right_masks_size++;

	// Allocate Memory for Objects
	*right_masks = new Object::Mask::Right::RightMask*[right_masks_size];
	right_masks_size = 0;

	// Create Vertical Line Right Wall Masks
	static VerticalLineRightWallData* vertical_line_right_wall_data;
	static Object::Mask::Right::RightMaskLine* vertical_line_right_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.right_mask_vertical_lines; iterator++)
	{
		vertical_line_right_wall_data = &instance.vertical_line_right_wall_data[iterator];
		if (vertical_line_right_wall_data->object_index != uuid)
		{
			vertical_line_right_wall_mask = new Object::Mask::Right::RightMaskLine(vertical_line_right_wall_data->vertical_line_data);
			vertical_line_right_wall_mask->name = vertical_line_right_wall_data->name;
			vertical_line_right_wall_mask->clamp = vertical_line_right_wall_data->editor_data.clamp;
			vertical_line_right_wall_mask->lock = vertical_line_right_wall_data->editor_data.lock;
			vertical_line_right_wall_mask->object_index = vertical_line_right_wall_data->object_index;
			(*right_masks)[right_masks_size] = vertical_line_right_wall_mask;
			right_masks_size++;
		}
	}

	// Create Curve Right Wall Masks
	static CurveRightWallData* curve_right_wall_data;
	static Object::Mask::Right::RightMaskCurve* curve_right_wall_mask;
	for (iterator = 0; iterator < instance.editor_header.right_mask_curves; iterator++)
	{
		curve_right_wall_data = &instance.curve_right_wall_data[iterator];
		if (curve_right_wall_data->object_index != uuid)
		{
			curve_right_wall_mask = new Object::Mask::Right::RightMaskCurve(curve_right_wall_data->curve_data);
			curve_right_wall_mask->name = curve_right_wall_data->name;
			curve_right_wall_mask->clamp = curve_right_wall_data->editor_data.clamp;
			curve_right_wall_mask->lock = curve_right_wall_data->editor_data.lock;
			curve_right_wall_mask->object_index = curve_right_wall_data->object_index;
			(*right_masks)[right_masks_size] = curve_right_wall_mask;
			right_masks_size++;
		}
	}
}

void Render::Objects::UnsavedLevel::returnCeilingMasks(Object::Mask::Ceiling::CeilingMask*** ceiling_masks, int& ceiling_masks_size, int uuid)
{
	// Get Current Instance
	int iterator = 0;
	LevelInstance& instance = *slave_stack.returnInstance();

	// Determine the Number of Objects to Load
	ceiling_masks_size = 0;
	for (int i = 0; i < instance.editor_header.ceiling_mask_horizontal_lines; i++)
		if (instance.horizontal_line_ceiling_data[i].object_index != uuid)
			ceiling_masks_size++;
	for (int i = 0; i < instance.editor_header.ceiling_mask_slants; i++)
		if (instance.slant_ceiling_data[i].object_index != uuid)
			ceiling_masks_size++;
	for (int i = 0; i < instance.editor_header.ceiling_mask_slopes; i++)
		if (instance.slope_ceiling_data[i].object_index != uuid)
			ceiling_masks_size++;

	// Allocate Memory for Objects
	*ceiling_masks = new Object::Mask::Ceiling::CeilingMask*[ceiling_masks_size];
	ceiling_masks_size = 0;

	// Create Horizontal Line Ceiling Masks
	static HorizontalLineCeilingData* horizontal_line_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskLine* horizontal_line_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_horizontal_lines; iterator++)
	{
		horizontal_line_ceiling_data = &instance.horizontal_line_ceiling_data[iterator];
		if (horizontal_line_ceiling_data->object_index != uuid)
		{
			horizontal_line_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskLine(horizontal_line_ceiling_data->horizontal_line_data);
			horizontal_line_ceiling_mask->name = horizontal_line_ceiling_data->name;
			horizontal_line_ceiling_mask->clamp = horizontal_line_ceiling_data->editor_data.clamp;
			horizontal_line_ceiling_mask->lock = horizontal_line_ceiling_data->editor_data.lock;
			horizontal_line_ceiling_mask->object_index = horizontal_line_ceiling_data->object_index;
			(*ceiling_masks)[ceiling_masks_size] = horizontal_line_ceiling_mask;
			ceiling_masks_size++;
		}
	}

	// Create Slant Ceiling Masks
	static SlantCeilingData* slant_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskSlant* slant_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slants; iterator++)
	{
		slant_ceiling_data = &instance.slant_ceiling_data[iterator];
		if (slant_ceiling_data->object_index != uuid)
		{
			slant_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskSlant(slant_ceiling_data->slant_data);
			slant_ceiling_mask->name = slant_ceiling_data->name;
			slant_ceiling_mask->clamp = slant_ceiling_data->editor_data.clamp;
			slant_ceiling_mask->lock = slant_ceiling_data->editor_data.lock;
			slant_ceiling_mask->object_index = slant_ceiling_data->object_index;
			(*ceiling_masks)[ceiling_masks_size] = slant_ceiling_mask;
			ceiling_masks_size++;
		}
	}

	// Create Slope Ceiling Masks
	static SlopeCeilingData* slope_ceiling_data;
	static Object::Mask::Ceiling::CeilingMaskSlope* slope_ceiling_mask;
	for (iterator = 0; iterator < instance.editor_header.ceiling_mask_slopes; iterator++)
	{
		slope_ceiling_data = &instance.slope_ceiling_data[iterator];
		if (slope_ceiling_data->object_index != uuid)
		{
			slope_ceiling_mask = new Object::Mask::Ceiling::CeilingMaskSlope(slope_ceiling_data->slope_data);
			slope_ceiling_mask->name = slope_ceiling_data->name;
			slope_ceiling_mask->clamp = slope_ceiling_data->editor_data.clamp;
			slope_ceiling_mask->lock = slope_ceiling_data->editor_data.lock;
			slope_ceiling_mask->object_index = slope_ceiling_data->object_index;
			(*ceiling_masks)[ceiling_masks_size] = slope_ceiling_mask;
			ceiling_masks_size++;
		}
	}
}

void Render::Objects::UnsavedLevel::returnTerrainObjects(Object::Terrain::TerrainBase*** terrain_objects, int& terrain_objects_size, int layer, int uuid)
{
	// Get Current Instance
	int iterator = 0;
	LevelInstance& instance = *slave_stack.returnInstance();

	// Parse Layer
	switch (layer)
	{

	// Backdrop
	case Object::Terrain::BACKDROP:
	{
		// Determine the Number of Objects to Load
		terrain_objects_size = 0;
		for (int i = 0; i < instance.editor_header.backdrop_terrain; i++)
		{
			if (instance.backdrop_terrain_data[i].object_index != uuid)
				terrain_objects_size++;
		}

		// Allocate Memory for Objects
		*terrain_objects = new Object::Terrain::TerrainBase*[terrain_objects_size];
		terrain_objects_size = 0;

		// Create Backdrop Objects
		static BackdropTerrainData* backdrop_terrain_data;
		static Object::Terrain::Backdrop* backdrop_terrain_object;
		for (iterator = 0; iterator < instance.editor_header.backdrop_terrain; iterator++)
		{
			backdrop_terrain_data = &instance.backdrop_terrain_data[iterator];
			if (backdrop_terrain_data->object_index != uuid)
			{
				backdrop_terrain_object = new Object::Terrain::Backdrop(backdrop_terrain_data->shape_data, backdrop_terrain_data->object_data);
				backdrop_terrain_object->name = backdrop_terrain_data->name;
				backdrop_terrain_object->clamp = backdrop_terrain_data->editor_data.clamp;
				backdrop_terrain_object->lock = backdrop_terrain_data->editor_data.lock;
				backdrop_terrain_object->object_index = backdrop_terrain_data->object_index;
				(*terrain_objects)[terrain_objects_size] = backdrop_terrain_object;
				terrain_objects_size++;
			}
		}

		break;
	}

	// Backgrounds
	case Object::Terrain::BACKGROUND_3:
	case Object::Terrain::BACKGROUND_2:
	case Object::Terrain::BACKGROUND_1:
	{
		// Count the Number of Background Objects With Matching Layer
		layer--;
		terrain_objects_size = 0;
		for (int i = 0; i < instance.editor_header.background_terrain; i++)
		{
			if (instance.background_terrain_data[i].layer == layer && instance.background_terrain_data[i].object_index != uuid)
				terrain_objects_size++;
		}

		// Allocate Memory for Objects
		*terrain_objects = new Object::Terrain::TerrainBase*[terrain_objects_size];
		terrain_objects_size = 0;

		// Create Backdrop Objects
		static BackgroundTerrainData* background_terrain_data;
		static Object::Terrain::Background* background_terrain_object;
		for (iterator = 0; iterator < instance.editor_header.background_terrain; iterator++)
		{
			background_terrain_data = &instance.background_terrain_data[iterator];
			if (background_terrain_data->layer == layer && background_terrain_data->object_index != uuid)
			{
				background_terrain_object = new Object::Terrain::Background(background_terrain_data->shape_data, background_terrain_data->object_data, background_terrain_data->layer);
				background_terrain_object->name = background_terrain_data->name;
				background_terrain_object->clamp = background_terrain_data->editor_data.clamp;
				background_terrain_object->lock = background_terrain_data->editor_data.lock;
				background_terrain_object->object_index = background_terrain_data->object_index;
				(*terrain_objects)[terrain_objects_size] = background_terrain_object;
				terrain_objects_size++;
			}
		}

		break;
	}

	// Foreground
	case Object::Terrain::FOREGROUND:
	{
		// Determine the Number of Objects to Load
		terrain_objects_size = 0;
		for (int i = 0; i < instance.editor_header.foreground_terrain; i++)
		{
			if (instance.foreground_terrain_data[i].object_index != uuid)
				terrain_objects_size++;
		}

		// Allocate Memory for Objects
		*terrain_objects = new Object::Terrain::TerrainBase*[terrain_objects_size];
		terrain_objects_size = 0;

		// Create Foreground Objects
		static ForegroundTerrainData* foreground_terrain_data;
		static Object::Terrain::Foreground* foreground_terrain_object;
		for (iterator = 0; iterator < instance.editor_header.foreground_terrain; iterator++)
		{
			foreground_terrain_data = &instance.foreground_terrain_data[iterator];
			if (foreground_terrain_data->object_index != uuid)
			{
				foreground_terrain_object = new Object::Terrain::Foreground(foreground_terrain_data->shape_data, foreground_terrain_data->object_data);
				foreground_terrain_object->name = foreground_terrain_data->name;
				foreground_terrain_object->clamp = foreground_terrain_data->editor_data.clamp;
				foreground_terrain_object->lock = foreground_terrain_data->editor_data.lock;
				foreground_terrain_object->object_index = foreground_terrain_data->object_index;
				(*terrain_objects)[terrain_objects_size] = foreground_terrain_object;
				terrain_objects_size++;
			}
		}

		break;
	}

	// Formerground
	case Object::Terrain::FORMERGROUND:
	{
		// Determine the Number of Objects to Load
		terrain_objects_size = 0;
		for (int i = 0; i < instance.editor_header.formerground_terrain; i++)
		{
			if (instance.formerground_terrain_data[i].object_index != uuid)
				terrain_objects_size++;
		}

		// Allocate Memory for Objects
		*terrain_objects = new Object::Terrain::TerrainBase*[terrain_objects_size];
		terrain_objects_size = 0;

		// Create Formerground Objects
		static FormergroundTerrainData* formerground_terrain_data;
		static Object::Terrain::Formerground* formerground_terrain_object;
		for (iterator = 0; iterator < instance.editor_header.formerground_terrain; iterator++)
		{
			formerground_terrain_data = &instance.formerground_terrain_data[iterator];
			if (formerground_terrain_data->object_index != uuid)
			{
				formerground_terrain_object = new Object::Terrain::Formerground(formerground_terrain_data->shape_data, formerground_terrain_data->object_data);
				formerground_terrain_object->name = formerground_terrain_data->name;
				formerground_terrain_object->clamp = formerground_terrain_data->editor_data.clamp;
				formerground_terrain_object->lock = formerground_terrain_data->editor_data.lock;
				formerground_terrain_object->object_index = formerground_terrain_data->object_index;
				(*terrain_objects)[terrain_objects_size] = formerground_terrain_object;
				terrain_objects_size++;
			}
		}

		break;
	}
	}
}

void Render::Objects::UnsavedLevel::write(bool& save)
{
	// If Unmodified Data is Selected, Don't Do Anything
	if (selected_unmodified) {}

	// Else, Write Currently Selected Instance 
	else 
	{
		save = true;
		writeInstance(*slave_stack.returnInstance());
	}

	// Delete Level
	delete this;
}

uint8_t Render::Objects::UnsavedLevel::revertChanges(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities)
{
	// Build Unmodified Data
	buildObjects(objects, index, physics, entities);

	// Set Selected Unmodified Flag
	selected_unmodified = true;

	// Increment Number of Times Unmodified Appears in Stack
	unmodified_data->master_stack_instances++;

	// Return Index of Unmodified Data in Stack Once it Appended
	return slave_stack.appendInstance(unmodified_data);
}

uint8_t Render::Objects::UnsavedLevel::createInstanceAppend(Editor::Selector* selector)
{
	// Create New Instance That is a Copy of Previous Instance
	LevelInstance* tempa = slave_stack.returnInstance();
	LevelInstance* current_instance = makeCopyOfInstance(*slave_stack.returnInstance());
	if (selected_unmodified)
		changeToModified();
	selected_unmodified = false;

	// Parse Object Identifier
	switch (selector->object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		switch (selector->object_identifier[1])
		{

		// Floor Masks
		case Object::Mask::FLOOR:
		{
			switch (selector->object_identifier[2])
			{

			// Horizontal Line Floor Mask
			case Object::Mask::HORIZONTAL_LINE:
			{
				// Generate New Object Data
				HorizontalLineFloorData new_horizontal_line_floor_data;
				new_horizontal_line_floor_data.name = selector->editor_data.name;
				new_horizontal_line_floor_data.editor_data.name_size = selector->editor_data.name_size;
				new_horizontal_line_floor_data.editor_data.clamp = selector->editor_data.clamp;
				new_horizontal_line_floor_data.editor_data.lock = selector->editor_data.lock;
				new_horizontal_line_floor_data.horizontal_line_data = selector->horizontal_line_data;
				new_horizontal_line_floor_data.platform = selector->floor_mask_platform;
				new_horizontal_line_floor_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->horizontal_line_floor_data, current_instance->editor_header.floor_mask_horizontal_lines, new_horizontal_line_floor_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->horizontal_line_floor_data, new_horizontal_line_floor_data, current_instance->editor_header.floor_mask_horizontal_lines);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.floor_count++;
				}
				else {
					current_instance->horizontal_line_floor_data[array_index] = std::move(new_horizontal_line_floor_data);
				}

				break;
			}

			// Slant Floor Mask
			case Object::Mask::HORIZONTAL_SLANT:
			{
				// Generate New Object Data
				SlantFloorData new_slant_floor_data;
				new_slant_floor_data.name = selector->editor_data.name;
				new_slant_floor_data.editor_data.name_size = selector->editor_data.name_size;
				new_slant_floor_data.editor_data.clamp = selector->editor_data.clamp;
				new_slant_floor_data.editor_data.lock = selector->editor_data.lock;
				new_slant_floor_data.slant_data = selector->slant_data;
				new_slant_floor_data.platform = selector->floor_mask_platform;
				new_slant_floor_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->slant_floor_data, current_instance->editor_header.floor_mask_slants, new_slant_floor_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->slant_floor_data, new_slant_floor_data, current_instance->editor_header.floor_mask_slants);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.floor_count++;
				}
				else {
					current_instance->slant_floor_data[array_index] = std::move(new_slant_floor_data);
				}

				break;
			}

			// Slope Floor Mask
			case Object::Mask::HORIZONTAL_SLOPE:
			{
				// Generate New Object Data
				SlopeFloorData new_slope_floor_data;
				new_slope_floor_data.name = selector->editor_data.name;
				new_slope_floor_data.editor_data.name_size = selector->editor_data.name_size;
				new_slope_floor_data.editor_data.clamp = selector->editor_data.clamp;
				new_slope_floor_data.editor_data.lock = selector->editor_data.lock;
				new_slope_floor_data.slope_data = selector->slope_data;
				new_slope_floor_data.platform = selector->floor_mask_platform;
				new_slope_floor_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->slope_floor_data, current_instance->editor_header.floor_mask_slopes, new_slope_floor_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->slope_floor_data, new_slope_floor_data, current_instance->editor_header.floor_mask_slopes);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.floor_count++;
				}
				else {
					current_instance->slope_floor_data[array_index] = std::move(new_slope_floor_data);
				}

				break;
			}

			}

			break;
		}

		// Left Masks
		case Object::Mask::LEFT_WALL:
		{
			switch (selector->object_identifier[2])
			{

			// Vertical Line Left Wall Mask
			case Object::Mask::VERTICAL_LINE:
			{
				// Generate New Object Data
				VerticalLineLeftWallData new_vertical_line_left_wall_data;
				new_vertical_line_left_wall_data.name = selector->editor_data.name;
				new_vertical_line_left_wall_data.editor_data.name_size = selector->editor_data.name_size;
				new_vertical_line_left_wall_data.editor_data.clamp = selector->editor_data.clamp;
				new_vertical_line_left_wall_data.editor_data.lock = selector->editor_data.lock;
				new_vertical_line_left_wall_data.vertical_line_data = selector->vertical_line_data;
				new_vertical_line_left_wall_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->vertical_line_left_wall_data, current_instance->editor_header.left_mask_vertical_lines, new_vertical_line_left_wall_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->vertical_line_left_wall_data, new_vertical_line_left_wall_data, current_instance->editor_header.left_mask_vertical_lines);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.left_count++;
				}
				else {
					current_instance->vertical_line_left_wall_data[array_index] = std::move(new_vertical_line_left_wall_data);
				}

				break;
			}

			// Curve Left Wall Mask
			case Object::Mask::VERTICAL_CURVE:
			{
				// Generate New Object Data
				CurveLeftWallData new_curve_left_wall_data;
				new_curve_left_wall_data.name = selector->editor_data.name;
				new_curve_left_wall_data.editor_data.name_size = selector->editor_data.name_size;
				new_curve_left_wall_data.editor_data.clamp = selector->editor_data.clamp;
				new_curve_left_wall_data.editor_data.lock = selector->editor_data.lock;
				new_curve_left_wall_data.curve_data = selector->curve_data;
				new_curve_left_wall_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->curve_left_wall_data, current_instance->editor_header.left_mask_curves, new_curve_left_wall_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->curve_left_wall_data, new_curve_left_wall_data, current_instance->editor_header.left_mask_curves);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.left_count++;
				}
				else {
					current_instance->curve_left_wall_data[array_index] = std::move(new_curve_left_wall_data);
				}

				break;
			}

			}

			break;
		}

		// Right Masks
		case Object::Mask::RIGHT_WALL:
		{
			switch (selector->object_identifier[2])
			{

			// Vertical Line Right Wall Mask
			case Object::Mask::VERTICAL_LINE:
			{
				// Generate New Object Data
				VerticalLineRightWallData new_vertical_line_right_wall_data;
				new_vertical_line_right_wall_data.name = selector->editor_data.name;
				new_vertical_line_right_wall_data.editor_data.name_size = selector->editor_data.name_size;
				new_vertical_line_right_wall_data.editor_data.clamp = selector->editor_data.clamp;
				new_vertical_line_right_wall_data.editor_data.lock = selector->editor_data.lock;
				new_vertical_line_right_wall_data.vertical_line_data = selector->vertical_line_data;
				new_vertical_line_right_wall_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->vertical_line_right_wall_data, current_instance->editor_header.right_mask_vertical_lines, new_vertical_line_right_wall_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->vertical_line_right_wall_data, new_vertical_line_right_wall_data, current_instance->editor_header.right_mask_vertical_lines);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.right_count++;
				}
				else {
					current_instance->vertical_line_right_wall_data[array_index] = std::move(new_vertical_line_right_wall_data);
				}

				break;
			}
			
			// Curve Right Wall Mask
			case Object::Mask::VERTICAL_CURVE:
			{
				// Generate New Object Data
				CurveRightWallData new_curve_right_wall_data;
				new_curve_right_wall_data.name = selector->editor_data.name;
				new_curve_right_wall_data.editor_data.name_size = selector->editor_data.name_size;
				new_curve_right_wall_data.editor_data.clamp = selector->editor_data.clamp;
				new_curve_right_wall_data.editor_data.lock = selector->editor_data.lock;
				new_curve_right_wall_data.curve_data = selector->curve_data;
				new_curve_right_wall_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->curve_right_wall_data, current_instance->editor_header.right_mask_curves, new_curve_right_wall_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->curve_right_wall_data, new_curve_right_wall_data, current_instance->editor_header.right_mask_curves);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.right_count++;
				}
				else {
					current_instance->curve_right_wall_data[array_index] = std::move(new_curve_right_wall_data);
				}

				break;
			}

			}

			break;
		}

		// Ceiling Masks
		case Object::Mask::CEILING:
		{
			switch (selector->object_identifier[2])
			{

			// Horizontal Line Ceiling Mask
			case Object::Mask::HORIZONTAL_LINE:
			{
				// Generate New Object Data
				HorizontalLineCeilingData new_horizontal_line_ceiling_data;
				new_horizontal_line_ceiling_data.name = selector->editor_data.name;
				new_horizontal_line_ceiling_data.editor_data.name_size = selector->editor_data.name_size;
				new_horizontal_line_ceiling_data.editor_data.clamp = selector->editor_data.clamp;
				new_horizontal_line_ceiling_data.editor_data.lock = selector->editor_data.lock;
				new_horizontal_line_ceiling_data.horizontal_line_data = selector->horizontal_line_data;
				new_horizontal_line_ceiling_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->horizontal_line_ceiling_data, current_instance->editor_header.ceiling_mask_horizontal_lines, new_horizontal_line_ceiling_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->horizontal_line_ceiling_data, new_horizontal_line_ceiling_data, current_instance->editor_header.ceiling_mask_horizontal_lines);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.ceiling_count++;
				} 
				else {
					current_instance->horizontal_line_ceiling_data[array_index] = std::move(new_horizontal_line_ceiling_data);
				}

				break;
			}

			// Slant Ceiling Mask
			case Object::Mask::HORIZONTAL_SLANT:
			{
				// Generate New Object Data
				SlantCeilingData new_slant_ceiling_data;
				new_slant_ceiling_data.name = selector->editor_data.name;
				new_slant_ceiling_data.editor_data.name_size = selector->editor_data.name_size;
				new_slant_ceiling_data.editor_data.clamp = selector->editor_data.clamp;
				new_slant_ceiling_data.editor_data.lock = selector->editor_data.lock;
				new_slant_ceiling_data.slant_data = selector->slant_data;
				new_slant_ceiling_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->slant_ceiling_data, current_instance->editor_header.ceiling_mask_slants, new_slant_ceiling_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->slant_ceiling_data, new_slant_ceiling_data, current_instance->editor_header.ceiling_mask_slants);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.ceiling_count++;
				}
				else {
					current_instance->slant_ceiling_data[array_index] = std::move(new_slant_ceiling_data);
				}

				break;
			}

			// Slope Ceiling Mask
			case Object::Mask::HORIZONTAL_SLOPE:
			{
				// Generate New Object Data
				SlopeCeilingData new_slope_ceiling_data;
				new_slope_ceiling_data.name = selector->editor_data.name;
				new_slope_ceiling_data.editor_data.name_size = selector->editor_data.name_size;
				new_slope_ceiling_data.editor_data.clamp = selector->editor_data.clamp;
				new_slope_ceiling_data.editor_data.lock = selector->editor_data.lock;
				new_slope_ceiling_data.slope_data = selector->slope_data;
				new_slope_ceiling_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->slope_ceiling_data, current_instance->editor_header.ceiling_mask_slopes, new_slope_ceiling_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->slope_ceiling_data, new_slope_ceiling_data, current_instance->editor_header.ceiling_mask_slopes);
					current_instance->editor_header.total_objects++;
					current_instance->header.number_of_loaded_objects++;
					current_instance->header.ceiling_count++;
				}
				else {
					current_instance->slope_ceiling_data[array_index] = std::move(new_slope_ceiling_data);
				}

				break;
			}

			}

			break;
		}

		// Trigger Masks
		case Object::Mask::TRIGGER:
		{
			// Generate New Object Data
			TriggerMaskData new_trigger_data;
			new_trigger_data.name = selector->editor_data.name;
			new_trigger_data.editor_data.name_size = selector->editor_data.name_size;
			new_trigger_data.editor_data.clamp = selector->editor_data.clamp;
			new_trigger_data.editor_data.lock = selector->editor_data.lock;
			new_trigger_data.trigger_data = selector->trigger_data;
			new_trigger_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->trigger_data, current_instance->editor_header.trigger_masks, new_trigger_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->trigger_data, new_trigger_data, current_instance->editor_header.trigger_masks);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.trigger_count++;
			}
			else {
				current_instance->trigger_data[array_index] = std::move(new_trigger_data);
			}
		}

		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		switch (selector->object_identifier[1])
		{

		// Foreground
		case Object::Terrain::FOREGROUND:
		{
			// Generate New Object Data
			ForegroundTerrainData new_foreground_terrain_data;
			new_foreground_terrain_data.name = selector->editor_data.name;
			new_foreground_terrain_data.editor_data.name_size = selector->editor_data.name_size;
			new_foreground_terrain_data.editor_data.clamp = selector->editor_data.clamp;
			new_foreground_terrain_data.editor_data.lock = selector->editor_data.lock;
			new_foreground_terrain_data.shape_data = getShapePointer(selector);
			new_foreground_terrain_data.object_data = selector->object_data;
			new_foreground_terrain_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->foreground_terrain_data, current_instance->editor_header.foreground_terrain, new_foreground_terrain_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->foreground_terrain_data, new_foreground_terrain_data, current_instance->editor_header.foreground_terrain);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.terrain_count++;
			}
			else {
				current_instance->foreground_terrain_data[array_index] = std::move(new_foreground_terrain_data);
			}

			break;
		}

		// Formerground
		case Object::Terrain::FORMERGROUND:
		{
			// Generate New Object Data
			FormergroundTerrainData new_formerground_terrain_data;
			new_formerground_terrain_data.name = selector->editor_data.name;
			new_formerground_terrain_data.editor_data.name_size = selector->editor_data.name_size;
			new_formerground_terrain_data.editor_data.clamp = selector->editor_data.clamp;
			new_formerground_terrain_data.editor_data.lock = selector->editor_data.lock;
			new_formerground_terrain_data.shape_data = getShapePointer(selector);
			new_formerground_terrain_data.object_data = selector->object_data;
			new_formerground_terrain_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->formerground_terrain_data, current_instance->editor_header.formerground_terrain, new_formerground_terrain_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->formerground_terrain_data, new_formerground_terrain_data, current_instance->editor_header.formerground_terrain);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.terrain_count++;
			}
			else {
				current_instance->formerground_terrain_data[array_index] = std::move(new_formerground_terrain_data);
			}

			break;
		}

		// Background
		case Object::Terrain::BACKGROUND_1:
		case Object::Terrain::BACKGROUND_2:
		case Object::Terrain::BACKGROUND_3:
		{
			// Generate New Object Data
			BackgroundTerrainData new_background_terrain_data;
			new_background_terrain_data.name = selector->editor_data.name;
			new_background_terrain_data.editor_data.name_size = selector->editor_data.name_size;
			new_background_terrain_data.editor_data.clamp = selector->editor_data.clamp;
			new_background_terrain_data.editor_data.lock = selector->editor_data.lock;
			new_background_terrain_data.shape_data = getShapePointer(selector);
			new_background_terrain_data.object_data = selector->object_data;
			new_background_terrain_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->background_terrain_data, current_instance->editor_header.background_terrain, new_background_terrain_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->background_terrain_data, new_background_terrain_data, current_instance->editor_header.background_terrain);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.terrain_count++;
			}
			else {
				current_instance->background_terrain_data[array_index] = std::move(new_background_terrain_data);
			}

			break;
		}

		// Backdrop
		case Object::Terrain::BACKDROP:
		{
			// Generate New Object Data
			BackdropTerrainData new_backdrop_terrain_data;
			new_backdrop_terrain_data.name = selector->editor_data.name;
			new_backdrop_terrain_data.editor_data.name_size = selector->editor_data.name_size;
			new_backdrop_terrain_data.editor_data.clamp = selector->editor_data.clamp;
			new_backdrop_terrain_data.editor_data.lock = selector->editor_data.lock;
			new_backdrop_terrain_data.shape_data = getShapePointer(selector);
			new_backdrop_terrain_data.object_data = selector->object_data;
			new_backdrop_terrain_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->backdrop_terrain_data, current_instance->editor_header.backdrop_terrain, new_backdrop_terrain_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->backdrop_terrain_data, new_backdrop_terrain_data, current_instance->editor_header.backdrop_terrain);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.terrain_count++;
			}
			else {
				current_instance->backdrop_terrain_data[array_index] = std::move(new_backdrop_terrain_data);
			}

			break;
		}

		}

		break;
	}

	// Lights
	case Object::LIGHT:
	{
		switch (selector->object_identifier[1])
		{
		
		// Directional Light
		case Object::Light::DIRECTIONAL:
		{
			// Generate New Object Data
			DirectionalLightData new_directional_light_data;
			new_directional_light_data.name = selector->editor_data.name;
			new_directional_light_data.editor_data.name_size = selector->editor_data.name_size;
			new_directional_light_data.editor_data.clamp = selector->editor_data.clamp;
			new_directional_light_data.editor_data.lock = selector->editor_data.lock;
			new_directional_light_data.directional_data = selector->directional_data;
			new_directional_light_data.light_data = selector->light_data;
			new_directional_light_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->directional_light_data, current_instance->editor_header.directional_lights, new_directional_light_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->directional_light_data, new_directional_light_data, current_instance->editor_header.directional_lights);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.directional_count++;
			}
			else {
				current_instance->directional_light_data[array_index] = std::move(new_directional_light_data);
			}

			break;
		}

		// Point Light
		case Object::Light::POINT:
		{
			// Generate New Object Data
			PointLightData new_point_light_data;
			new_point_light_data.name = selector->editor_data.name;
			new_point_light_data.editor_data.name_size = selector->editor_data.name_size;
			new_point_light_data.editor_data.clamp = selector->editor_data.clamp;
			new_point_light_data.editor_data.lock = selector->editor_data.lock;
			new_point_light_data.point_data = selector->point_data;
			new_point_light_data.light_data = selector->light_data;
			new_point_light_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->point_light_data, current_instance->editor_header.point_lights, new_point_light_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->point_light_data, new_point_light_data, current_instance->editor_header.point_lights);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.point_count++;
			}
			else {
				current_instance->point_light_data[array_index] = std::move(new_point_light_data);
			}

			break;
		}

		// Spot Light
		case Object::Light::SPOT:
		{
			// Generate New Object Data
			SpotLightData new_spot_light_data;
			new_spot_light_data.name = selector->editor_data.name;
			new_spot_light_data.editor_data.name_size = selector->editor_data.name_size;
			new_spot_light_data.editor_data.clamp = selector->editor_data.clamp;
			new_spot_light_data.editor_data.lock = selector->editor_data.lock;
			new_spot_light_data.spot_data = selector->spot_data;
			new_spot_light_data.light_data = selector->light_data;
			new_spot_light_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->spot_light_data, current_instance->editor_header.spot_lights, new_spot_light_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->spot_light_data, new_spot_light_data, current_instance->editor_header.spot_lights);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.spot_count++;
			}
			else {
				current_instance->spot_light_data[array_index] = std::move(new_spot_light_data);
			}

			break;
		}

		// Beam Light
		case Object::Light::BEAM:
		{
			// Generate New Object Data
			BeamLightData new_beam_light_data;
			new_beam_light_data.name = selector->editor_data.name;
			new_beam_light_data.editor_data.name_size = selector->editor_data.name_size;
			new_beam_light_data.editor_data.clamp = selector->editor_data.clamp;
			new_beam_light_data.editor_data.lock = selector->editor_data.lock;
			new_beam_light_data.beam_data = selector->beam_data;
			new_beam_light_data.light_data = selector->light_data;
			new_beam_light_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->beam_light_data, current_instance->editor_header.beam_lights, new_beam_light_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->beam_light_data, new_beam_light_data, current_instance->editor_header.beam_lights);
				current_instance->editor_header.total_objects++;
				current_instance->header.number_of_loaded_objects++;
				current_instance->header.beam_count++;
			}
			else {
				current_instance->beam_light_data[array_index] = std::move(new_beam_light_data);
			}

			break;
		}

		}

		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		switch (selector->object_identifier[1])
		{

		// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			// Generate New Object Data
			RigidBodyPhysicsData new_rigid_body_physics_data;
			new_rigid_body_physics_data.name = selector->editor_data.name;
			new_rigid_body_physics_data.editor_data.name_size = selector->editor_data.name_size;
			new_rigid_body_physics_data.editor_data.clamp = selector->editor_data.clamp;
			new_rigid_body_physics_data.editor_data.lock = selector->editor_data.lock;
			new_rigid_body_physics_data.uuid = selector->uuid;
			new_rigid_body_physics_data.shape_data = getShapePointer(selector);
			new_rigid_body_physics_data.object_data = selector->object_data;
			new_rigid_body_physics_data.rigid_body_data = selector->rigid_body_data;
			new_rigid_body_physics_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->rigid_body_physics_data, current_instance->editor_header.physics_rigid, new_rigid_body_physics_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->rigid_body_physics_data, new_rigid_body_physics_data, current_instance->editor_header.physics_rigid);
			}
			else {
				current_instance->rigid_body_physics_data[array_index] = std::move(new_rigid_body_physics_data);
			}

			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			switch (selector->object_identifier[2])
			{

			// SpringMass
			case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
			{
				// Generate New Object Data
				SpringMassPhysicsData new_spring_mass_physics_data;
				new_spring_mass_physics_data.name = selector->editor_data.name;
				new_spring_mass_physics_data.editor_data.name_size = selector->editor_data.name_size;
				new_spring_mass_physics_data.editor_data.clamp = selector->editor_data.clamp;
				new_spring_mass_physics_data.editor_data.lock = selector->editor_data.lock;
				new_spring_mass_physics_data.uuid = selector->uuid;
				new_spring_mass_physics_data.object_data = selector->object_data;
				new_spring_mass_physics_data.file_name = selector->file_name;
				new_spring_mass_physics_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->spring_mass_physics_data, current_instance->editor_header.physics_soft_springmass, new_spring_mass_physics_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->spring_mass_physics_data, new_spring_mass_physics_data, current_instance->editor_header.physics_soft_springmass);
				}
				else {
					current_instance->spring_mass_physics_data[array_index] = std::move(new_spring_mass_physics_data);
				}

				break;
			}

			// Wire
			case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
			{
				// Generate New Object Data
				WirePhysicsData new_wire_physics_data;
				new_wire_physics_data.name = selector->editor_data.name;
				new_wire_physics_data.editor_data.name_size = selector->editor_data.name_size;
				new_wire_physics_data.editor_data.clamp = selector->editor_data.clamp;
				new_wire_physics_data.editor_data.lock = selector->editor_data.lock;
				new_wire_physics_data.uuid = selector->uuid;
				new_wire_physics_data.object_data = selector->object_data;
				new_wire_physics_data.wire_data = selector->wire_data;
				new_wire_physics_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->wire_physics_data, current_instance->editor_header.physics_soft_wires, new_wire_physics_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->wire_physics_data, new_wire_physics_data, current_instance->editor_header.physics_soft_wires);
				}
				else {
					current_instance->wire_physics_data[array_index] = std::move(new_wire_physics_data);
				}

				break;
			}

			}

			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			switch (selector->object_identifier[2])
			{

			// Anchor
			case (int)Object::Physics::HINGES::ANCHOR:
			{
				// Generate New Object Data
				AnchorPhysicsData new_anchor_physics_data;
				new_anchor_physics_data.name = selector->editor_data.name;
				new_anchor_physics_data.editor_data.name_size = selector->editor_data.name_size;
				new_anchor_physics_data.editor_data.clamp = selector->editor_data.clamp;
				new_anchor_physics_data.editor_data.lock = selector->editor_data.lock;
				new_anchor_physics_data.uuid = selector->uuid;
				new_anchor_physics_data.anchor_data = selector->anchor_data;
				new_anchor_physics_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->anchor_physics_data, current_instance->editor_header.physics_anchors, new_anchor_physics_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->anchor_physics_data, new_anchor_physics_data, current_instance->editor_header.physics_anchors);
				}
				else {
					current_instance->anchor_physics_data[array_index] = std::move(new_anchor_physics_data);
				}

				break;
			}

			// Hinge
			case (int)Object::Physics::HINGES::HINGE:
			{
				// Generate New Object Data
				HingePhysicsData new_hinge_physics_data;
				new_hinge_physics_data.name = selector->editor_data.name;
				new_hinge_physics_data.editor_data.name_size = selector->editor_data.name_size;
				new_hinge_physics_data.editor_data.clamp = selector->editor_data.clamp;
				new_hinge_physics_data.editor_data.lock = selector->editor_data.lock;
				new_hinge_physics_data.uuid = selector->uuid;
				new_hinge_physics_data.hinge_data = selector->hinge_data;
				new_hinge_physics_data.file_name = selector->file_name;
				new_hinge_physics_data.object_index = selector->object_index;

				// Determine Where to Insert Object Into Array
				int16_t array_index = testIN(current_instance->hinge_physics_data, current_instance->editor_header.physics_hinges, new_hinge_physics_data.object_index);
				if (array_index == -1) {
					appendIntoArray(&current_instance->hinge_physics_data, new_hinge_physics_data, current_instance->editor_header.physics_hinges);
				}
				else {
					current_instance->hinge_physics_data[array_index] = std::move(new_hinge_physics_data);
				}

				break;
			}

			}

			break;
		}
		}

		break;
	}

	// Entities
	case Object::ENTITY:
	{
		switch (selector->object_identifier[1])
		{

		// NPC
		case Object::Entity::ENTITY_NPC:
		{
			// Generate New Object Data
			NPC_Data new_npc_data;
			new_npc_data.name = selector->editor_data.name;
			new_npc_data.editor_data.name_size = selector->editor_data.name_size;
			new_npc_data.editor_data.clamp = selector->editor_data.clamp;
			new_npc_data.editor_data.lock = selector->editor_data.lock;
			new_npc_data.uuid = selector->uuid;
			new_npc_data.entity_data = selector->entity_data;
			new_npc_data.object_data = selector->object_data;
			new_npc_data.ai_script = selector->npc_ai;
			new_npc_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->npc_data, current_instance->editor_header.entity_npc, new_npc_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->npc_data, new_npc_data, current_instance->editor_header.entity_npc);
			}
			else {
				current_instance->npc_data[array_index] = std::move(new_npc_data);
			}

			break;
		}

		// Controllable Entity
		case Object::Entity::ENTITY_CONTROLLABLE:
		{
			// Generate New Object Data
			ControllableData new_controllable_data;
			new_controllable_data.name = selector->editor_data.name;
			new_controllable_data.editor_data.name_size = selector->editor_data.name_size;
			new_controllable_data.editor_data.clamp = selector->editor_data.clamp;
			new_controllable_data.editor_data.lock = selector->editor_data.lock;
			new_controllable_data.uuid = selector->uuid;
			new_controllable_data.entity_data = selector->entity_data;
			new_controllable_data.object_data = selector->object_data;
			new_controllable_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->controllable_data, current_instance->editor_header.entity_controllable, new_controllable_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->controllable_data, new_controllable_data, current_instance->editor_header.entity_controllable);
			}
			else {
				current_instance->controllable_data[array_index] = std::move(new_controllable_data);
			}

			break;
		}

		// Interactable Entity
		case Object::Entity::ENTITY_INTERACTABLE:
		{
			// Generate New Object Data
			InteractableData new_interactable_data;
			new_interactable_data.name = selector->editor_data.name;
			new_interactable_data.editor_data.name_size = selector->editor_data.name_size;
			new_interactable_data.editor_data.clamp = selector->editor_data.clamp;
			new_interactable_data.editor_data.lock = selector->editor_data.lock;
			new_interactable_data.uuid = selector->uuid;
			new_interactable_data.entity_data = selector->entity_data;
			new_interactable_data.object_data = selector->object_data;
			new_interactable_data.interactable_data = selector->interactable_data;
			new_interactable_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->interactable_data, current_instance->editor_header.entity_interactable, new_interactable_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->interactable_data, new_interactable_data, current_instance->editor_header.entity_interactable);
			}
			else {
				current_instance->interactable_data[array_index] = std::move(new_interactable_data);
			}

			break;
		}

		// Dynamic Entity
		case Object::Entity::ENTITY_DYNAMIC:
		{
			// Generate New Object Data
			DynamicData new_dynamic_data;
			new_dynamic_data.name = selector->editor_data.name;
			new_dynamic_data.editor_data.name_size = selector->editor_data.name_size;
			new_dynamic_data.editor_data.clamp = selector->editor_data.clamp;
			new_dynamic_data.editor_data.lock = selector->editor_data.lock;
			new_dynamic_data.uuid = selector->uuid;
			new_dynamic_data.entity_data = selector->entity_data;
			new_dynamic_data.object_data = selector->object_data;
			new_dynamic_data.dynamic_data = selector->dynamic_data;
			new_dynamic_data.object_index = selector->object_index;

			// Determine Where to Insert Object Into Array
			int16_t array_index = testIN(current_instance->dynamic_data, current_instance->editor_header.entity_dynamic, new_dynamic_data.object_index);
			if (array_index == -1) {
				appendIntoArray(&current_instance->dynamic_data, new_dynamic_data, current_instance->editor_header.entity_dynamic);
			}
			else {
				current_instance->dynamic_data[array_index] = std::move(new_dynamic_data);
			}

			break;
		}
		}

		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	}

	// Return Index of New Instance Once Allocated in Stack
	return slave_stack.appendInstance(current_instance);
}

uint8_t Render::Objects::UnsavedLevel::createInstancePop(uint32_t index_to_remove, uint8_t object_identifier[3])
{
	// Create New Instance That is a Copy of Previous Instance
	LevelInstance* current_instance = makeCopyOfInstance(*slave_stack.returnInstance());
	if (selected_unmodified)
		changeToModified();
	selected_unmodified = false;

	//throw "ass\n";

	// Parse Object Identifier
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		switch (object_identifier[1])
		{

		// Floor Masks
		case Object::Mask::FLOOR:
		{
			switch (object_identifier[2])
			{

			// Horizontal Line Floor Mask
			case Object::Mask::HORIZONTAL_LINE:
			{
				if (!popFromArray(&current_instance->horizontal_line_floor_data, current_instance->editor_header.floor_mask_horizontal_lines, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.floor_count--;
				}

				break;
			}

			// Slant Floor Mask
			case Object::Mask::HORIZONTAL_SLANT:
			{
				if (!popFromArray(&current_instance->slant_floor_data, current_instance->editor_header.floor_mask_slants, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.floor_count--;
				}

				break;
			}

			// Slope Floor Mask
			case Object::Mask::HORIZONTAL_SLOPE:
			{
				if (!popFromArray(&current_instance->slope_floor_data, current_instance->editor_header.floor_mask_slopes, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.floor_count--;
				}

				break;
			}

			}

			break;
		}

		// Left Wall Masks
		case Object::Mask::LEFT_WALL:
		{
			switch (object_identifier[2])
			{

			// Vertical Line Left Wall Mask
			case Object::Mask::VERTICAL_LINE:
			{
				if (!popFromArray(&current_instance->vertical_line_left_wall_data, current_instance->editor_header.left_mask_vertical_lines, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.left_count--;
				}

				break;
			}

			// Curve Left Wall Mask
			case Object::Mask::VERTICAL_CURVE:
			{
				if (!popFromArray(&current_instance->curve_left_wall_data, current_instance->editor_header.left_mask_curves, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.left_count--;
				}

				break;
			}

			}

			break;
		}

		// Right Wall Masks
		case Object::Mask::RIGHT_WALL:
		{
			switch (object_identifier[2])
			{

			// Vertical Line Right Wall Mask
			case Object::Mask::VERTICAL_LINE:
			{
				if (!popFromArray(&current_instance->vertical_line_right_wall_data, current_instance->editor_header.right_mask_vertical_lines, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.right_count--;
				}

				break;
			}

			// Curve Right Wall Mask
			case Object::Mask::VERTICAL_CURVE:
			{
				if (!popFromArray(&current_instance->curve_right_wall_data, current_instance->editor_header.right_mask_curves, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.right_count--;
				}

				break;
			}

			}

			break;
		}

		// Ceiling Masks
		case Object::Mask::CEILING:
		{
			switch (object_identifier[2])
			{

			// Horizontal Line Ceiling Mask
			case Object::Mask::HORIZONTAL_LINE:
			{
				if (!popFromArray(&current_instance->horizontal_line_ceiling_data, current_instance->editor_header.ceiling_mask_horizontal_lines, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.ceiling_count--;
				}

				break;
			}

			// Slant Ceiling Mask
			case Object::Mask::HORIZONTAL_SLANT:
			{
				if (!popFromArray(&current_instance->slant_ceiling_data, current_instance->editor_header.ceiling_mask_slants, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}
				
				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.ceiling_count--;
				}

				break;
			}

			// Slope Ceiling Mask
			case Object::Mask::HORIZONTAL_SLOPE:
			{
				if (!popFromArray(&current_instance->slope_ceiling_data, current_instance->editor_header.ceiling_mask_slopes, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				else
				{
					current_instance->editor_header.total_objects--;
					current_instance->header.number_of_loaded_objects--;
					current_instance->header.ceiling_count--;
				}

				break;
			}

			}

			break;
		}

		// Trigger Masks
		case Object::Mask::TRIGGER:
		{
			if (!popFromArray(&current_instance->trigger_data, current_instance->editor_header.trigger_masks, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.trigger_count--;
			}

			break;
		}

		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		switch (object_identifier[1])
		{

		// Foreground
		case Object::Terrain::FOREGROUND:
		{
			if (!popFromArray(&current_instance->foreground_terrain_data, current_instance->editor_header.foreground_terrain, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.terrain_count--;
			}

			break;
		}

		// Formerground
		case Object::Terrain::FORMERGROUND:
		{
			if (!popFromArray(&current_instance->formerground_terrain_data, current_instance->editor_header.formerground_terrain, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.terrain_count--;
			}

			break;
		}

		// Background
		case Object::Terrain::BACKGROUND_1:
		case Object::Terrain::BACKGROUND_2:
		case Object::Terrain::BACKGROUND_3:
		{
			if (!popFromArray(&current_instance->background_terrain_data, current_instance->editor_header.background_terrain, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.terrain_count--;
			}

			break;
		}

		// Backdrop
		case Object::Terrain::BACKDROP:
		{
			if (!popFromArray(&current_instance->backdrop_terrain_data, current_instance->editor_header.backdrop_terrain, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.terrain_count--;
			}

			break;
		}

		}

		break;
	}

	// Lights
	case Object::LIGHT:
	{
		switch (object_identifier[1])
		{

		// Directional Lights
		case Object::Light::DIRECTIONAL:
		{
			if (!popFromArray(&current_instance->directional_light_data, current_instance->editor_header.directional_lights, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.directional_count--;
			}

			break;
		}

		// Point Lights
		case Object::Light::POINT:
		{
			if (!popFromArray(&current_instance->point_light_data, current_instance->editor_header.point_lights, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.point_count--;
			}

			break;
		}

		// Spot Lights
		case Object::Light::SPOT:
		{
			if (!popFromArray(&current_instance->spot_light_data, current_instance->editor_header.spot_lights, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}
			
			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.spot_count--;
			}

			break;
		}

		// Beam Lights
		case Object::Light::BEAM:
		{
			if (!popFromArray(&current_instance->beam_light_data, current_instance->editor_header.beam_lights, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			else
			{
				current_instance->editor_header.total_objects--;
				current_instance->header.number_of_loaded_objects--;
				current_instance->header.beam_count--;
			}

			break;
		}

		}

		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		switch (object_identifier[1])
		{

		// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			if (!popFromArray(&current_instance->rigid_body_physics_data, current_instance->editor_header.physics_rigid, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			switch (object_identifier[2])
			{

			// SpringMass
			case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
			{
				if (!popFromArray(&current_instance->spring_mass_physics_data, current_instance->editor_header.physics_soft_springmass, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				break;
			}

			// Wire
			case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
			{
				if (!popFromArray(&current_instance->wire_physics_data, current_instance->editor_header.physics_soft_wires, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				break;
			}

			}

			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			switch (object_identifier[2])
			{

			// Anchor
			case (int)Object::Physics::HINGES::ANCHOR:
			{
				if (!popFromArray(&current_instance->anchor_physics_data, current_instance->editor_header.physics_anchors, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				break;
			}

			// Hinge
			case (int)Object::Physics::HINGES::HINGE:
			{
				if (!popFromArray(&current_instance->hinge_physics_data, current_instance->editor_header.physics_hinges, index_to_remove))
				{
					slave_stack.deleteInstance(current_instance);
					return slave_stack.returnCurrentIndex();
				}

				break;
			}

			}

			break;
		}

		}

		break;
	}

	// Entities
	case Object::ENTITY:
	{
		switch (object_identifier[1])
		{

		// NPC
		case Object::Entity::ENTITY_NPC:
		{
			if (!popFromArray(&current_instance->npc_data, current_instance->editor_header.entity_npc, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			break;
		}

		// Controllable Entity
		case Object::Entity::ENTITY_CONTROLLABLE:
		{
			if (!popFromArray(&current_instance->controllable_data, current_instance->editor_header.entity_controllable, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			break;
		}

		// Interactable Entity
		case Object::Entity::ENTITY_INTERACTABLE:
		{
			if (!popFromArray(&current_instance->interactable_data, current_instance->editor_header.entity_interactable, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			break;
		}

		// Dynamic Entity
		case Object::Entity::ENTITY_DYNAMIC:
		{
			if (!popFromArray(&current_instance->dynamic_data, current_instance->editor_header.entity_dynamic, index_to_remove))
			{
				slave_stack.deleteInstance(current_instance);
				return slave_stack.returnCurrentIndex();
			}

			break;
		}

		}

		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	}

	// Return Index of New Instance Once Allocated in Stack
	return slave_stack.appendInstance(current_instance);
}

Render::Objects::UnsavedLevel::SlaveStack::SlaveStack()
{
	// Create Initial Allocation for Stack
	stack_array = new LevelInstance*[array_size];

	// Increment Size By 1 for Implementation of Unmodified Data
	stack_size++;
	head++;
}

uint8_t Render::Objects::UnsavedLevel::SlaveStack::appendInstance(LevelInstance* instance)
{
	// Increment Stack Index
	stack_index++;
	stack_size++;

	// If Current Index is Equal to the Size of Array, Reallocate Array
	if (stack_index == array_size)
	{
		// Create New Array With Size 1.5 X Size of Original Array
		array_size = (int)(array_size * 1.5f);
		LevelInstance** new_array = new LevelInstance*[array_size];

		// Move Data From Old Array to New Array
		for (uint8_t i = 0; i < stack_index; i++)
			new_array[i] = std::move(stack_array[i]);

		// Delete Old Array
		delete[] stack_array;

		// Store Pointer to New Array in Old Array Location
		stack_array = new_array;
	}

	// Insert Instance Into Array
	stack_array[stack_index] = instance;

	// Return Index of New Array in Stack
	return stack_index;
}

Render::Objects::UnsavedLevel::LevelInstance* Render::Objects::UnsavedLevel::SlaveStack::returnInstance()
{
	return stack_array[stack_index];
}

bool Render::Objects::UnsavedLevel::SlaveStack::isEmpty()
{
	return (stack_size == 0 || stack_size == 1);
}

void Render::Objects::UnsavedLevel::SlaveStack::storeUnmodified(LevelInstance* unmodified_data)
{
	stack_array[0] = unmodified_data;
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteInstance(LevelInstance* instance)
{
	// Delete Data Arrays
	if (instance->editor_header.floor_mask_horizontal_lines) { delete[] instance->horizontal_line_floor_data; }
	if (instance->editor_header.floor_mask_slants) { delete[] instance->slant_floor_data; }
	if (instance->editor_header.floor_mask_slopes) { delete[] instance->slope_floor_data; }
	if (instance->editor_header.left_mask_vertical_lines) { delete[] instance->vertical_line_left_wall_data; }
	if (instance->editor_header.left_mask_curves) { delete[] instance->curve_left_wall_data; }
	if (instance->editor_header.right_mask_vertical_lines) { delete[] instance->vertical_line_right_wall_data; }
	if (instance->editor_header.right_mask_curves) { delete[] instance->curve_right_wall_data; }
	if (instance->editor_header.ceiling_mask_horizontal_lines) { delete[] instance->horizontal_line_ceiling_data; }
	if (instance->editor_header.ceiling_mask_slants) { delete[] instance->slant_ceiling_data; }
	if (instance->editor_header.ceiling_mask_slopes) { delete[] instance->slope_ceiling_data; }
	if (instance->editor_header.trigger_masks) { delete[] instance->trigger_data; }
	if (instance->editor_header.foreground_terrain) { delete[] instance->foreground_terrain_data; }
	if (instance->editor_header.formerground_terrain) { delete[] instance->formerground_terrain_data; }
	if (instance->editor_header.background_terrain) { delete[] instance->background_terrain_data; }
	if (instance->editor_header.backdrop_terrain) { delete[] instance->backdrop_terrain_data; }
	if (instance->editor_header.directional_lights) { delete[] instance->directional_light_data; }
	if (instance->editor_header.point_lights) { delete[] instance->point_light_data; }
	if (instance->editor_header.spot_lights) { delete[] instance->spot_light_data; }
	if (instance->editor_header.beam_lights) { delete[] instance->beam_light_data; }
	if (instance->editor_header.physics_rigid) { delete[] instance->rigid_body_physics_data; }
	if (instance->editor_header.physics_soft_springmass) { delete[] instance->spring_mass_physics_data; }
	if (instance->editor_header.physics_soft_wires) { delete[] instance->wire_physics_data; }
	if (instance->editor_header.physics_anchors) { delete[] instance->anchor_physics_data; }
	if (instance->editor_header.physics_hinges) { delete[] instance->hinge_physics_data; }
	if (instance->editor_header.entity_npc) { delete[] instance->npc_data; }
	if (instance->editor_header.entity_controllable) { delete[] instance->controllable_data; }
	if (instance->editor_header.entity_interactable) { delete[] instance->interactable_data; }
	if (instance->editor_header.entity_dynamic) { delete[] instance->dynamic_data; }

	// Delete Actual Object
	delete instance;
}

void Render::Objects::UnsavedLevel::SlaveStack::deleteStack()
{
	// Iterate Through Array and Delete Instances
	for (int i = tail; i < head; i++)
	{
		// Don't Delete Unmodified
		if (stack_array[i]->unmodified)
			continue;

		// Delete Instance
		deleteInstance(stack_array[i]);
	}
}

uint8_t Render::Objects::UnsavedLevel::SlaveStack::returnCurrentIndex()
{
	return stack_index;
}

void Render::Objects::UnsavedLevel::SlaveStack::incrementStackInstances(uint8_t index)
{
	stack_array[index]->master_stack_instances++;
}

void Render::Objects::UnsavedLevel::SlaveStack::decrementStackInstances(uint8_t index)
{
	stack_array[index]->master_stack_instances--;

	// If Number of Instances is 0, Delete Instance
	if (stack_array[index]->master_stack_instances == 0)
		deleteInstance(stack_array[index]);
}

void Render::Objects::UnsavedLevel::SlaveStack::switchInstance(uint8_t instance_index)
{
	stack_index = instance_index;
}

