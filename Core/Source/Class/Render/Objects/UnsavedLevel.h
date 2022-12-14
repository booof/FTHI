#include "Macros.h"
#ifdef EDITOR

#ifndef UNSAVED_LEVEL_H
#define UNSAVED_LEVEL_H

#include "SubLevel.h"

#include "Class/Render/Struct/List.h"

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
#include "Object/Physics/PhysicsBase.h"
#include "Object/Physics/RigidBody/RigidBody.h"
#include "Object/Physics/Softody/SpringMass.h"
#include "Object/Physics/Softody/Wire.h"
#include "Object/Physics/Hinge/Anchor.h"
#include "Object/Physics/Hinge/Hinge.h"
#include "Object/Entity/NPC.h"
#include "Object/Entity/Controllables.h"
#include "Object/Entity/Interactables.h"
#include "Object/Entity/Dynamics.h"

// This is a Class That Acts as a Storage Container for SubLevel Objects
// Similar Containers to Main Level, However Are Specific to Static Objects From a Sub Level File
// All Changes Made in Editor Will Not Affect Sub Levels, But Will Instead Affect Contents From This Contianer
// Each Container Object Will Contain a Stack That Contains All Changes Made While Editing
// This Stack is Used for Undo/Redo Chains
// A New Item to the Stack is Only Updated if The Selector Deselects or Deletes an Object in Corrisponding SubLevel
// All Stacks for All Instances an Unsaved Level Container is Controlled by a Master Stack in Another Class Specifically for handling the Undo/Redo Chain
// This Container Will Persist Until The Levels Are Saved
// Even if the Corrisponding SubLevel Becomes Unloaded, This Container Will Persist
// If a Corrisponding SubLevel is Needed to Load Again During Editing, This Container Will Load in Its Place
// Upon Loading a Level for the First Time, Data Will be Stored in Memory Seperate From Stack

namespace Editor
{
	class Selector;
}

namespace Render::Objects
{
	class UnsavedLevel
	{
		// Possible Change to Method of Switching Between Instances (More Memory / Performance Friendly)
		// Since Each Instance Can Only be Accessed From the Undo / Redo Chain, Each Instance Only Needs the Changes Made From The Previous/Next Instance
		// Each Unsaved Level Only Has two Full Instances: The Unmodified Data and The Current Instance Data
		// Unmodied Data is Created Upon First Initialization of Unsaved Level, Current Instance is Immediatly Copied After
		// Instances Should Remian the Same With the Same Data and Arrays
		// To Get To Another Instance, Parse Through Corrisponding Change Array and Either Add, Modify, or Remove Objects From the Current Instance
		// If Moving Backwards Through Undo Chain (Undo), Use Previous Change Data. If Moving Forwards Through Undo Chain (Redo), Use Next Change Data
		// If Unmodified Data is To be Added to the Stack, Find and Store All Neccessary Changes to Make to Convert Current Instance to Unmodified Instance, Then Copy Unmodified to Current
		// The Change Controller Should Remain Unchanged
		// Doing This Should Allow Less Memory to be Used and Allow Switching Between Instances to be Easier and More Efficient
		// This Could Allow The Amount of Instances in the Undo/Redo Chain to be Increased Drastically

		enum CHANGE_TYPES : uint8_t
		{
			ADD = 1,
			MODIFY,
			REMOVE
		};

		// Individual Object Modification
		struct change
		{
			// The Type of Change
			uint8_t change_type;

			// The Type of Object
			uint8_t object_type;

			// The Object Identifier
			uint32_t object_identifier;

			// Pointer to Data Object At Identifier is to be Replaced With
			// Such as Horizontal Line Floor Data or Foreground Terrain Data
			void* data_pointer;
		};

		// Instance Changes
		struct changes
		{
			// Number of Changes From Previous Instance
			uint16_t previous_change_count;

			// Number of Changes Into Next Instance
			uint16_t next_change_count;

			// Changes From Previous Instance
			change** previous_changes;

			// Changes Into Next Instance
			change** next_changes;
		};

		// Verbose Editor Header
		struct EditorHeader
		{
			uint16_t total_objects = 0;
			uint16_t floor_mask_horizontal_lines = 0;
			uint16_t floor_mask_slants = 0;
			uint16_t floor_mask_slopes = 0;
			uint16_t left_mask_vertical_lines = 0;
			uint16_t left_mask_curves = 0;
			uint16_t right_mask_vertical_lines = 0;
			uint16_t right_mask_curves = 0;
			uint16_t ceiling_mask_horizontal_lines = 0;
			uint16_t ceiling_mask_slants = 0;
			uint16_t ceiling_mask_slopes = 0;
			uint16_t trigger_masks = 0;
			uint16_t foreground_terrain = 0;
			uint16_t formerground_terrain = 0;
			uint16_t background_terrain = 0;
			uint16_t backdrop_terrain = 0;
			uint16_t directional_lights = 0;
			uint16_t point_lights = 0;
			uint16_t spot_lights = 0;
			uint16_t beam_lights = 0;
			uint16_t physics_rigid = 0;
			uint16_t physics_soft_springmass = 0;
			uint16_t physics_soft_wires = 0;
			uint16_t physics_anchors = 0;
			uint16_t physics_hinges = 0;
			uint16_t entity_npc = 0;
			uint16_t entity_controllable = 0;
			uint16_t entity_interactable = 0;
			uint16_t entity_dynamic = 0;
			uint16_t particle_generators = 0;
		};

		// Struct for Data Specific to Editor Mode
		struct EditorData
		{
			// Size of Name of Object
			uint16_t name_size = 0;

			// Enable Clamp
			bool clamp = false;

			// Enable Lock
			bool lock = false;
		};

		// Horizontal Line Floor Data
		struct HorizontalLineFloorData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::HorizontalLineData horizontal_line_data;
			bool platform = false;
		};

		// Slant Floor Data
		struct SlantFloorData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::SlantData slant_data;
			bool platform = false;
		};

		// Slope Floor Data
		struct SlopeFloorData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::SlopeData slope_data;
			bool platform = false;
		};

		// Vertical Line Left Wall Data
		struct VerticalLineLeftWallData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::VerticalLineData vertical_line_data;
		};

		// Curve Left Wall Data
		struct CurveLeftWallData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::CurveData curve_data;
		};

		// Vertical Line Right Wall Data
		struct VerticalLineRightWallData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::VerticalLineData vertical_line_data;
		};

		// Curve Right Wall Data
		struct CurveRightWallData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::CurveData curve_data;
		};

		// Horizontal Line Ceiling Data
		struct HorizontalLineCeilingData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::HorizontalLineData horizontal_line_data;
		};

		// Slant Ceiling Data
		struct SlantCeilingData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::SlantData slant_data;
		};

		// Slope Ceiling Data
		struct SlopeCeilingData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::SlopeData slope_data;
		};

		// Trigger Mask Data
		struct TriggerMaskData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Mask::Trigger::TriggerData trigger_data;
		};

		// Foreground Terrain Data
		struct ForegroundTerrainData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::ObjectData object_data;
			Shape::Shape* shape_data;
		};

		// Formerground Terrain Data
		struct FormergroundTerrainData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::ObjectData object_data;
			Shape::Shape* shape_data;
		};

		// Background Terrain Data
		struct BackgroundTerrainData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::ObjectData object_data;
			Shape::Shape* shape_data;
			uint8_t layer;
		};

		// Backdrop Terrain Data
		struct BackdropTerrainData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::ObjectData object_data;
			Shape::Shape* shape_data;
		};

		// Directional Light Data
		struct DirectionalLightData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Light::LightData light_data;
			Object::Light::Directional::DirectionalData directional_data;
		};

		// Point Light Data
		struct PointLightData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Light::LightData light_data;
			Object::Light::Point::PointData point_data;
		};

		// Spot Light Data
		struct SpotLightData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Light::LightData light_data;
			Object::Light::Spot::SpotData spot_data;
		};

		// Beam Light Data
		struct BeamLightData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			Object::Light::LightData light_data;
			Object::Light::Beam::BeamData beam_data;
		};

		// Rigid Body Data
		struct RigidBodyPhysicsData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Physics::Rigid::RigidBodyData rigid_body_data;
			Shape::Shape* shape_data;
		};

		// SpringMass Data
		struct SpringMassPhysicsData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			std::string file_name = "";
		};

		// Wire Data
		struct WirePhysicsData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Physics::Soft::WireData wire_data;
		};

		// Anchor Data
		struct AnchorPhysicsData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::Physics::Hinge::AnchorData anchor_data;
		};

		// Hinge Data
		struct HingePhysicsData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::Physics::Hinge::HingeData hinge_data;
			std::string file_name = "";
		};

		// NPC Data
		struct NPC_Data
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Entity::EntityData entity_data;
			uint16_t ai_script;
		};

		// Controllable Data
		struct ControllableData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Entity::EntityData entity_data;
		};

		// Interactable Data
		struct InteractableData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Entity::EntityData entity_data;
			Object::Entity::InteractableData interactable_data;
		};

		// Dynamic Data
		struct DynamicData
		{
			std::string name = "";
			uint32_t object_index = 0;
			EditorData editor_data;
			uint32_t uuid = 0;
			Object::ObjectData object_data;
			Object::Entity::EntityData entity_data;
			Object::Entity::DynamicData dynamic_data;
		};

		// Instance of Level
		struct LevelInstance
		{
			// If True, This Instance is the Unmodified Instance
			bool unmodified = false;

			// The Amount of Times This Instance Appears in Master Stack
			// Once This Count Reaches 0, This Instance Will be Deleted
			// Unmodified Instance Will Allways Have an One More Than Is Used In Stack
			uint8_t master_stack_instances = 1;

			// Header
			LevelHeader header;

			// Editor Header
			EditorHeader editor_header;
			
			// Arrays of Object Data
			HorizontalLineFloorData*     horizontal_line_floor_data;
			SlantFloorData*              slant_floor_data;
			SlopeFloorData*              slope_floor_data;
			VerticalLineLeftWallData*    vertical_line_left_wall_data;
			CurveLeftWallData*           curve_left_wall_data;
			VerticalLineRightWallData*   vertical_line_right_wall_data;
			CurveRightWallData*          curve_right_wall_data;
			HorizontalLineCeilingData*   horizontal_line_ceiling_data;
			SlantCeilingData*            slant_ceiling_data;
			SlopeCeilingData*            slope_ceiling_data;
			TriggerMaskData*             trigger_data;
			ForegroundTerrainData*       foreground_terrain_data;
			FormergroundTerrainData*     formerground_terrain_data;
			BackgroundTerrainData*       background_terrain_data;
			BackdropTerrainData*         backdrop_terrain_data;
			DirectionalLightData*        directional_light_data;
			PointLightData*              point_light_data;
			SpotLightData*               spot_light_data;
			BeamLightData*               beam_light_data;
			RigidBodyPhysicsData*        rigid_body_physics_data;
			SpringMassPhysicsData*       spring_mass_physics_data;
			WirePhysicsData*             wire_physics_data;
			AnchorPhysicsData*           anchor_physics_data;
			HingePhysicsData*            hinge_physics_data;
			NPC_Data*                    npc_data;
			ControllableData*            controllable_data;
			InteractableData*            interactable_data;
			DynamicData*                 dynamic_data;
		};

		// Slave Stack
		class SlaveStack
		{
			// Size of Array
			uint8_t array_size = 10;

			// Items in Stack
			uint8_t stack_size = 0;

			// Tail of Stack
			uint8_t tail = 0;

			// Current Head of Stack
			uint8_t head = 0;

			// Current Index in Stack
			uint8_t stack_index = 0;

			// Array
			LevelInstance** stack_array = nullptr;

		public:

			// Initialize Stack
			SlaveStack();

			// Add Instance to Stack
			uint8_t appendInstance(LevelInstance* instance);

			// Get Instance From Stack
			LevelInstance* returnInstance();

			// Returns True if Stack is Empty
			bool isEmpty();

			// Store Unmodified Data in First Index of Array
			void storeUnmodified(LevelInstance* instance);

			// Delete an Instance
			void deleteInstance(LevelInstance* instance);

			// Delete Entire Stack
			void deleteStack();

			// Return Current Index in Stack
			uint8_t returnCurrentIndex();

			// Switch to a Different Instance
			void switchInstance(uint8_t instance_index);

			// Increment Stack Apperences
			void incrementStackInstances(uint8_t index);

			// Decrement Stack Apperences
			void decrementStackInstances(uint8_t index);
		};

		// Slave Stack
		SlaveStack slave_stack;

		// Pointer to Unmodified Data
		// Stored In Case There is a Need to Revert All Changes in an Individual Level
		// If Reverted To, Data Will Appear as a New Instance in Stack, Allowing for Undoing if Needed
		LevelInstance* unmodified_data = nullptr;
		bool selected_unmodified = true;
		// IDEA: If Selected Unmodified is False, Create Yellow Border Around Level to Signify It was Changed
		//       If Selected Unmodified is True and There are Instances in Stack, Create Blue Border around Level
		//       If Selected Unmodified is True and The Stack is Empty, Show No Level Border
		//       If Currently No Border Criteria is met, and Level was Saved, Show Green Border
		//       These Colors Should be Similar to Visual Studio Colors in Respect to Making Changes in a File

		// Path to Object Data
		std::string object_path = "";

		// Path to Editor Configuration Data
		std::string editor_path = "";

		// Vertex Object for Unsaved Level State Outline
		GLuint VAO = 0, VBO = 0;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);

		// Allocate Memory for Arrays
		void allocateForArrays(LevelInstance& instance);

		// Construct Unmodified Data Helper
		void constructUnmodifiedDataHelper(LevelInstance& instance);

		// Build Objects Helper
		void buildObjectsHelper(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities, LevelInstance& instance);

		// Write Instance to File
		void writeInstance(LevelInstance& instance);

		// Mask a Copy of an Instance
		LevelInstance* makeCopyOfInstance(LevelInstance& instance);

		// Test if Object Identifier is In Array
		template <class Type> int16_t testIN(Type* type_array, uint16_t array_size, uint32_t object_index)
		{
			for (int16_t i = 0; i < array_size; i++)
			{
				// If The Object Index in Array Matches the Test Index, Return Index of Object in Array
				if (type_array[i].object_index == object_index)
					return i;
			}
			
			// If Object Identifier is Not In Array, Return -1
			return -1;
		}

		// Resize and Append a New Object Into Array
		template <class Type> void appendIntoArray(Type** type_array, Type& new_object, uint16_t& array_size)
		{
			// Generate New Array With Size + 1
			Type* new_array = new Type[array_size + 1];

			// Copy All Instances Into New Array
			for (uint16_t i = 0; i < array_size; i++)
			{
				new_array[i] = std::move((*type_array)[i]);
			}

			// Append New Object
			new_array[array_size] = std::move(new_object);

			// Delte Old Array
			delete[] *type_array;

			// Move New Array to Old Array Location
			*type_array = new_array;

			// Increment Array Size 
			array_size++;
		}

		// Return Pointer to Shape Data
		Shape::Shape* getShapePointer(Editor::Selector* selector);

		// Remove Index in Array. Returns True if Object was Removed
		template <class Type> bool popFromArray(Type** type_array, uint16_t& array_size, uint32_t object_index)
		{
			// Get Index of Object in Array
			int16_t index = testIN(*type_array, array_size, object_index);

			// If Index is -1, return false
			if (index == -1)
				return false;

			// Create New Array With Size - 1
			Type* new_array = new Type[array_size - 1];

			// Copy All Data From Old Array to New Array, Excluding Object Index
			uint16_t new_array_index = 0;
			for (uint16_t i = 0; i < array_size; i++)
			{
				// Only Copy Data if Not Currently at Object Index
				if (i != index)
				{
					new_array[new_array_index] = std::move((*type_array)[i]);
					new_array_index++;
				}
			}

			// Delete Old Array
			delete[] *type_array;

			// Store Pointer to New Array in Old Array Location
			*type_array = new_array;

			// Decrement Array Size
			array_size--;

			// Return True
			return true;
		}

		// Change Vertex Colors to Modified
		void changeToModified();

		// Change Vertex Colors to Unmodified
		void changeToUnmodified();

		// Change Vertex Colors to Saved
		void changeToSaved();

		// Change Vertex Colors
		void changeColors(float* color);

	public:

		// Coordinates of Level
		int16_t level_x = 0;
		int16_t level_y = 0;
		int8_t level_version = 0;

		// Index of Level in Change Controller List
		uint8_t unsaved_level_index = 0;

		// Determines if Level was Previously Saved
		bool saved = false;

		// Pointer to Active Objects Array
		Object::Object*** active_objects;

		// Initialize Object
		UnsavedLevel();

		// Deconstructor
		~UnsavedLevel();

		// Construct Unmodified Data
		void constructUnmodifiedData(int16_t x, int16_t y, uint8_t z);

		// Build Objects to Main Level
		void buildObjects(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities);

		// Switch to Another Instance
		void switchInstance(uint8_t selected_instance);

		// Write Current Instance to File
		void write(bool& save);

		// Revert Changes to Unmodified Data
		uint8_t revertChanges(Object::Object** objects, uint16_t& index, Struct::List<Object::Physics::PhysicsBase>& physics, Struct::List<Object::Entity::EntityBase>& entities);

		// Create a New Instance by Appending a New Object
		uint8_t createInstanceAppend(Editor::Selector* selector);

		// Create a New Instance by Removing an Object
		uint8_t createInstancePop(uint32_t index_to_remove, uint8_t object_identifier[3]);

		// Increment Stack Appearances for Instance
		void incrementStackApperance(uint8_t index);

		// Decrement Stack Apperances for Instance
		void decrementStackApperance(uint8_t index);

		// Return Object Header
		LevelHeader returnObjectHeader();

		// Draw Object State Visualizer
		void drawVisualizer();

		// Return All Floor Masks to be Used for Clamping
		void returnFloorMasks(Object::Mask::Floor::FloorMask*** floor_masks, int& floor_masks_size, int uuid);

		// Return All Left Wall Masks to be Used for Clamping
		void returnLeftMasks(Object::Mask::Left::LeftMask*** left_masks, int& left_masks_size, int uuid);

		// Return All Right Wall Masks to be Used for Clamping
		void returnRightMasks(Object::Mask::Right::RightMask*** right_masks, int& right_masks_size, int uuid);

		// Return All Ceiling Masks to be Used for Clamping
		void returnCeilingMasks(Object::Mask::Ceiling::CeilingMask*** ceiling_masks, int& ceiling_masks_size, int uuid);

		// Return All Terrain Objects at Specified Layer to be Used for Clamping
		void returnTerrainObjects(Object::Terrain::TerrainBase*** terrain_objects, int& terrain_objects_size, int layer, int uuid);
	};
}

#endif

#endif
