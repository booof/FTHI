#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

// GUI Objects
#include "Render/GUI/MasterElement.h"
#include "Render/GUI/Box.h"
#include "Render/GUI/TextObject.h"
#include "Render/GUI/VerticalScrollBar.h"
#include "Render/GUI/ColorWheel.h"

// Objects Data List
#include "Object/Object.h"
#include "Render/Shape/Rectangle.h"
#include "Render/Shape/Trapezoid.h"
#include "Render/Shape/Triangle.h"
#include "Render/Shape/Circle.h"
#include "Render/Shape/Polygon.h"
#include "Object/Collision/Horizontal/Line/HorizotnalLine.h"
#include "Object/Collision/Horizontal/Slant/Slant.h"
#include "Object/Collision/Horizontal/Slope/Slope.h"
#include "Object/Collision/Vertical/Line/VerticalLine.h"
#include "Object/Collision/Vertical/Curve/Curve.h"
#include "Object/Collision/Trigger/TriggerMask.h"
#include "Object/Lighting/Directional.h"
#include "Object/Lighting/Point.h"
#include "Object/Lighting/Spot.h"
#include "Object/Lighting/Beam.h"
#include "Object/Physics/RigidBody/RigidRectangle.h"
#include "Object/Physics/RigidBody/RigidTrapezoid.h"
#include "Object/Physics/RigidBody/RigidTriangle.h"
#include "Object/Physics/RigidBody/RigidCircle.h"
#include "Object/Physics/RigidBody/RigidPolygon.h"
#include "Object/Physics/Softody/SpringMass.h"
#include "Object/Physics/Softody/Wire.h"
#include "Object/Physics/Hinge/Anchor.h"
#include "Object/Physics/Hinge/Hinge.h"
#include "Object/Entity/EntityBase.h"
#include "Object/Entity/NPC.h"
#include "Object/Entity/Controllables.h"
#include "Object/Entity/Interactables.h"
#include "Object/Entity/Dynamics.h"

// Externals
#include "ExternalLibs.h"
#include "Macros.h"

namespace Editor
{
	// Editing Modes
	enum class EDITING_MODES : uint8_t
	{
		NEW_OBJECT,
		EDIT_OBJECT,
		NEW_SPRINGMASS,
		NEW_HINGE,
		LEVEL_SELECT
	};

	// String Map Types
	enum class STRING_MAPS : uint8_t
	{
		SCRIPT,
		TEXTURE,
		MATERIAL,
		PHYSICS_MATERIAL
	};

	// Editor Specific Data
	struct EditorSpecificData
	{
		// Name of Object
		std::string name = "";

		// Length of Name
		uint8_t name_size = 0;

		// Clamp Flag
		bool clamp = false;

		// Lock Flag
		bool lock = false;
	};

	// Main GUI for Editing Objects
	class EditorWindow
	{
		// Total Amount of Vertices for Background
		const int BACKGROUND_VERTICES_COUNT = 48;

		// Window Background Vertex Object
		GLuint backgroundVAO, backgroundVBO;

		// Highlighter for New Object
		GLuint highlighterVAO, highlighterVBO;

		// Window Mid Section Vertex Object
		GLuint editing_screenVAO, editing_screenVBO;

		// Visualizer Vertex Object
		GLuint object_visualizerVAO, object_visualzierVBO;

		// Position of Object
		glm::vec2 position = glm::vec2(0.0f, 50.0f);

		// Width of Window
		float width;

		// Height of Window
		float height;

		// Scale of Objects
		float scale;

		// Total Height of Editor Window
		float editorHeight = 0;
		float editorHeightFull = 0;
		float textureHeightFull = 0;
		float editingOffset = 0;

		// X Positions of Scrolllbars
		float bar1X = 0;
		float bar2X = 0;

		// Locations of Borders on Window (In Percents of Total Width)
		float border1X, border2X;

		// The Vertex Count of Editor Window
		int EditorVertexCount = 0;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Model Matrix of Editing Screen
		glm::mat4 editing_model = glm::mat4(1.0f);

		// Model Matrix of Selected Object
		glm::mat4 selected_model = glm::mat4(1.0f);

		// A Nullified View Matrix for GUI
		glm::mat4 view_null = glm::mat4(1.0f);

		// Temporary Holder for Box Values
		GUI::BoxData temp_box_data;

		// Temporary Holder for Text Values
		GUI::TextData temp_text_data;

		// Master Element
		GUI::MasterElement master;

		// Array of Boxes
		GUI::Box** boxes;
		uint8_t boxes_size = 0;

		// Array of Box Values
		void** box_values;

		// Array of Text Objects
		GUI::TextObject** texts;
		uint8_t texts_size = 0;

		// Scroll Bar 
		GUI::VerticalScrollBar bar1;
		GUI::VerticalScrollBar bar2;

		// Color Wheel
		ColorWheel wheel;
		ColorWheel wheelAmbient, wheelDiffuse, wheelSpecular;
		bool wheel_active = false;
		bool light_active = false;

		// Starting Index for Color Boxes
		uint8_t wheel_box_start = 0;
		uint8_t light_wheel_box_start = 0;

		// Update Flags for Color Wheels
		uint8_t update_wheel;
		uint8_t update_ambient;
		uint8_t update_diffuse;
		uint8_t update_specular;

		// Un-Normalized Colors
		unsigned int wheel_color[4] = { 0 };
		unsigned int ambient_color[4] = { 0 };
		unsigned int diffuse_color[4] = { 0 };
		unsigned int specular_color[4] = { 0 };

		// The File List on Right Side of Object
		std::string* files;
		int filesSize = 0;

		// Determines if a Wheel is Being Edited
		uint8_t wheel_index = 0;

		// Index of New Object
		short index = 0;

		// Determines Which Part of Window is Selected
		short collision_type = 0;

		// Offsets for Moving Part of Window
		float offset_x, offset_y;

		// Determines if Window is Being Moved
		int8_t moving = false;

		// Determines Which Object Index to Modify During New Object Creation
		uint8_t object_identifier_index = 0;

		// Colors for Each Terrain Layer Type
		const glm::vec4 terrain_layer_colors[6] = {
			glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			glm::vec4(0.6f, 0.6f, 0.6f, 1.0f),
			glm::vec4(0.4f, 0.4f, 0.4f, 1.0f),
			glm::vec4(0.2f, 0.2f, 0.2f, 1.0f),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		};

		// Top of Window
		float windowTop;

		// File Directory of Object Script
		std::string script_path = "";
		int* script_pointer = nullptr;
		uint8_t update_script = 0;
		bool script_active = false;

		// File Directory of Texture Data
		std::string texture_path = "";
		int* texture_pointer = nullptr;
		uint8_t update_texture = 0;
		bool texture_active = false;

		// File Directory of Material Data
		std::string material_path = "";
		int* material_pointer = nullptr;
		uint8_t update_material = 0;
		bool material_active = false;

		// Name of the Physics Material Data
		std::string physics_material_name = "";
		int* physics_material_pointer = nullptr;
		uint8_t update_physics_material = 0;
		bool physics_material_active = false;

		// Secondary Object Identifier for Complex Physics Objects
		uint8_t secondary_object_identifier[2];
		uint8_t secondary_identifier_index = 0;

	protected:

		// Initialize Window
		void initializeWindow();

		// Generate Window Background
		void genBackground();

		// Generate Object Editing Segregators
		void genSegregators();

		// Find String From an Ordered String Map
		void getStringFromStringMap(STRING_MAPS map_type, int& key);

		// Find Key From an Ordered String Map
		void getKeyFromStringMap(STRING_MAPS map_type);

		// Reset Boxes
		void resetBoxes(uint8_t new_box_size, uint8_t new_text_size);

		// Common Boxes
		// Boxes = 6, Text = 6
		void genBoxesCommon(uint8_t& box_offset, uint8_t& text_offset, float* posX, float* posY);

		// Horizontal Line Boxes
		// Boxes = 1, Text = 1
		void genBoxesHorizontalLine(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Slant Boxes
		// Boxes = 2, Text = 2
		void genBoxesSlant(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Slope Boxes
		// Boxes = 2, Text = 2
		void genBoxesSlope(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Platform Box
		// Boxes = 1, Text = 0
		void genBoxesPlatform(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Vertical Line Boxes
		// Boxes = 1, Text = 1
		void genBoxesVerticalLine(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Curve Boxes
		// Boxes = 2, Text = 2
		void genBoxesCurve(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Material Box
		// Boxes = 1, Text = 1
		void genBoxesMaterial(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Trigger Boxes
		// Boxes = 2, Text = 2
		void genBoxesTrigger(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Object Boxes
		// Boxes = 10, Text = 11
		void genBoxesObject(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Rectangle Boxes
		// Boxes = 2, Text = 2
		void genBoxesRectangle(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Trapezoid Boxes
		// Boxes = 4, Text = 4
		void genBoxesTrapezoid(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Triangle Boxes
		// Boxes = 4, Text = 4
		void genBoxesTriangle(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Circle Boxes
		// Boxes = 2, Text = 2
		void genBoxesCircle(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Polygon Boxes
		// Boxes = 4, Text = 4
		void genBoxesPolygon(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Light Boxes
		// Boxes = 13, Text = 16
		void genBoxesLight(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Directional Boxes
		// Boxes = 2, Text = 2
		void genBoxesDirectional(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Point Boxes
		// Boxes = 2, Text = 2
		void genBoxesPoint(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Spot Boxes
		// Boxes = 7, 8
		void genBoxesSpot(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Beam Boxes
		// Boxes = 4, Text = 4
		void genBoxesBeam(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Rigid Body Boxes
		// Boxes = 7, Text = 8
		void genBoxesRigidBody(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// SpringMass Boxes
		// Boxes = 2, Text = 1
		void genBoxesSpringMass(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// SpringMass Node Boxes
		// Boxes = 6, Text = 7
		void genBoxesSpringMassNode(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// SpringMass Spring Boxes
		// Boxes = 6, Text = 6
		void genBoxesSpringMassSpring(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Wire Boxes
		// Boxes = 10, Text = 10
		void genBoxesWire(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Hinge Boxes
		// Boxes = 2, Text = 1
		void genBoxesHinge(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// Gen Entity Boxes
		// Boxes = 4, Text = 4
		void genBoxesEntity(uint8_t& box_offset, uint8_t& text_offset, float height_offset);

		// NPC AI Box
		// Boxes = 1, Text = 1
		void genBoxesAI(uint8_t& box_offset, uint8_t&  text_offset, float height_offset);

		// Assign Color Wheel
		void assignColorWheel(ColorWheel* wheel_, uint8_t& box_offset, uint8_t& text_offset, unsigned int* color, float height_offset);

		// Display Text
		void displayText();

		// Update Scroll Bars
		void updateScrollBars();

		// Update New Object Mode
		void updateNewObject();

		// Change New Object Data
		void changeNewObject();

		// Change New Object to SpringMass Objects
		void changeNewObjectSpringMass();

		// Change New Object to Hinge Objects
		void changeNewObjectHinge();

		// Initialize Selection of New SpringMass Object
		void initializeSpringMassSelection();

		// Initialize Selection of New Hinge Object
		void initializeHingeSelection();

		// Generate Shape Vertices for New Objects
		void genNewObjectShapes(glm::vec4 color, float& distance, int& offset);

		// Generate Horizontal Mask Vertices
		void genNewObjectHorizontalMasks(glm::vec4 colors[3], float& distance, int& offset);

		// Generate Horizontal Mask Vertices
		void genNewObjectVerticalMasks(glm::vec4 colors[2], float& distance, int& offset, int8_t direction);

		// Finalize New Object Vertices
		void finalizeNewObjectVertices(float distance);

		// Generate an Object
		void generateNewObject();

		// Generate a Shape for New Objects
		void generateNewShape(float new_size);

		// Update Editor Mode
		void updateEditorMode();

		// Update Color Wheels
		void updateColorWheels(ColorWheel& wheel_, glm::vec4& color, unsigned int* wheel_color_, double mouseStaticX, double mouseStaticY, int offsetx, bool update);

		// Function to Generate a New UUID
		uint32_t genNewUUID();

	public:

		// Current Editing Mode of Window
		EDITING_MODES editing_mode = EDITING_MODES::NEW_OBJECT;

		// Position of Window
		glm::vec2 window_position;

		// Forces Selector to Initialize
		bool force_selector_initialization = false;

		// Determines if Window is Active
		bool active_window = false;

		// Object Identifier
		unsigned char object_identifier[3];

		// Lists of Object Values
		EditorSpecificData                            editor_data;
		Object::ObjectData                            object_data;
		Shape::Rectangle                              rectangle_data;
		Shape::Trapezoid                              trapezoid_data;
		Shape::Triangle                               triangle_data;
		Shape::Circle                                 circle_data;
		Shape::Polygon                                polygon_data;
		Object::Mask::HorizontalLineData              horizontal_line_data;
		Object::Mask::SlantData                       slant_data;
		Object::Mask::SlopeData                       slope_data;
		Object::Mask::VerticalLineData                vertical_line_data;
		Object::Mask::CurveData                       curve_data;
		Object::Mask::Trigger::TriggerData            trigger_data;
		Object::Light::LightData                      light_data;
		Object::Light::Directional::DirectionalData   directional_data;
		Object::Light::Point::PointData               point_data;
		Object::Light::Spot::SpotData                 spot_data;
		Object::Light::Beam::BeamData                 beam_data;
		Object::Physics::Rigid::RigidBodyData         rigid_body_data;
		Object::Physics::Soft::WireData				  wire_data;
		Object::Physics::Hinge::AnchorData            anchor_data;
		Object::Physics::Hinge::HingeData             hinge_data;
		Object::Entity::EntityData                    entity_data;
		Object::Entity::InteractableData              interactable_data;
		Object::Entity::DynamicData                   dynamic_data;
		Object::Physics::Soft::NodeData               node_data;
		Object::Physics::Soft::Spring                 spring_data;
		bool                                          floor_mask_platform;
		uint16_t                                      npc_ai;
		std::string                                   file_name;
		uint32_t                                      uuid;

		// Single Texture for Textured Objects
		Struct::SingleTexture texture;

		// Object Index in Unsaved Levels
		uint32_t object_index = 0;

		// Shape of Editing Object
		unsigned char editing_shape;

		// Determines if Object Has Been Modified
		bool modified = false;

		// Determines if SpringMass Node is Being Modified
		bool springmass_node_modified = false;

		// Determines if SpringMass Spring is Being Modified
		bool springmass_spring_modified = false;

		// Determines if Hinge Object is Being Modified
		bool hinge_object_modified = false;

		// Determines if a New Child Object Should be Added
		bool add_child_object = false;

		// Create New Object Window
		void genNewObjectWindow();

		// Create Object Editing Window
		void genObjectEditorWindow();

		// Traverse Backwards Through New Object Window
		bool traverseBackNewObject();

		// Update Window
		void updateWindow();

		// Draw Window
		void drawWindow();
	};
}

#endif
