#ifndef CONTAINER_H
#define CONTAINER_H

#include "ExternalLibs.h"
#include "Macros.h"

#include "Class/Render/Struct/LinkedList.h"
#include "Class/Render/Struct/List.h"

// This is the Refactored Level Object To Alloy GUI Objects to Share the same Functionality

// List of Things That Will be Refactored:
//	- Loading / Generation / Unloading of Objects
//  - Rendering of Objects
//	- Selection of Objects
//  - Camera

// This is a Parent Class to the Level and GUI Objects

// Unique Level Objects:
//	- Collision Mask Objects
//	- Physics Objects
//	- Entities

// Unique GUI Objects:
//	- GUI Elements

namespace DataClass
{
	class Data_Object;
}

namespace Editor
{
	class Selector;
	class ObjectInfo;
}

namespace Render::Objects
{
	class UnsavedLevel;
	class ChangeController;
	struct ObjectContainer_Level;
}

namespace Render::GUI
{
	struct ObjectContainer_GUI;
}

namespace Render::Camera
{
	class Camera;
}

namespace Object
{
	class Object;
	class TempObject;

	namespace Mask
	{
		namespace Floor
		{
			class FloorMask;
		}

		namespace Left
		{
			class LeftMask;
		}

		namespace Right
		{
			class RightMask;
		}

		namespace Ceiling
		{
			class CeilingMask;
		}

		namespace Trigger
		{
			class TriggerMask;
		}
	}

	namespace Terrain
	{
		class TerrainBase;
	}

	namespace Light
	{
		namespace Directional
		{
			class Directional;
		}

		namespace Point
		{
			class Point;
		}

		namespace Spot
		{
			class Spot;
		}

		namespace Beam
		{
			class Beam;
		}
	}

	namespace Physics
	{
		class PhysicsBase;
	}

	namespace Entity
	{
		class EntityBase;
	}

	namespace Group
	{
		class GroupObject;
	}
}

namespace Render
{
	// Number of Different Storage Types
	const uint32_t STORAGE_TYPE_COUNT = 13;

	// The Two Types of Container Objects
	enum class CONTAINER_TYPES : uint8_t
	{
		LEVEL = 0,
		GUI
	};

	// Container for Currently Loaded Objects
	// TODO: Make Subclasses For This Class For Both GUIs and Levels
	struct ObjectContainer
	{
		// Number of Objects Contained by Container
		uint32_t total_object_count = 0; // Total Number of Objects Currently Loaded
		uint16_t directional_size = 0;
		uint16_t point_size = 0;
		uint16_t spot_size = 0;
		uint16_t beam_size = 0;
		uint16_t group_size = 0;

		// Pointer to Start of Each Object Type in Container
		Object::Object** object_array = nullptr; // The Array of All Objects Together
		Object::Group::GroupObject** group_start = nullptr;
		Object::Light::Directional::Directional** directional_start = nullptr;
		Object::Light::Point::Point** point_start = nullptr;
		Object::Light::Spot::Spot** spot_start = nullptr;
		Object::Light::Beam::Beam** beam_start = nullptr;
	};

	// The Object Container Master Class
	class Container
	{

	protected:

		// Determines if Initialized
		bool initialized = false;

		// Vertex Array Objects for Terrain
		GLuint terrainVAO, terrainVBO;

		// Vertex Array Objects for Border
		GLuint borderVAO, borderVBO;

		// The Container Name
		std::string container_name = "";

		// Temporary Index Holder for Adding New Objects Into Temporary Vector
		uint32_t temp_index_holder = 0;

		// Array of Temporary Removed Objects From Container
		std::vector<Object::TempObject*> temp_objects;

		// Function to Finish Initializing Any Container Object
		void initContainer();

		// Only to be Used When Reallocating Objects Array
		virtual ObjectContainer& getContainer() = 0;

		// Reload the Terrain Objects of Container
		virtual void constructTerrain() = 0;

		// Helper Function to Load Terrain Objects
		void constructTerrainHelper(int vertex_count, uint16_t terrain_size, uint16_t group_size, Object::Terrain::TerrainBase** terrain_start, Object::Group::GroupObject** group_start);

		// Construct Terrain Using the Correct Algorithm
		virtual void constructTerrainAlgorithm(int& instance, int& instance_index) = 0;

		// Reload the Lighting Objects of Container
		virtual void loadLights() = 0;

		// Reallocate Memory For Textures
		void reallocateTextures();

		// Determine the Starting Pointers the Different Objects in Container Through Sorting
		virtual void segregateObjects() = 0;

		// Segregate Object Pointers of Container
		void segregateHelper(ObjectContainer& container, uint16_t** count_map, void** pointer_map);

		// Draw Visualizers Common Between Container Types
		void drawCommonVisualizers(ObjectContainer& container);

		// Functions Below Are Used for Selecting Objects

		// Test Selector on Terrain Objects
		uint8_t testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on All Terrain Objects
		uint8_t testSelectorTerrainAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container);

		// Test Selector on Lighting Objects
		uint8_t testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on Lighting Objects
		uint8_t testSelectorLightsAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container);

		// Test Selector on Effect Objects
		uint8_t testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on All Effect Objects
		uint8_t testSelectorEffectsAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container);

		// Test Selector on Physics Objects
		uint8_t testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on Entity Objects
		uint8_t testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on Group Objects
		uint8_t testSelectorGroup(Editor::Selector& selector, Editor::ObjectInfo& object_info, ObjectContainer& container);

		// Test Selector on Collision Mask Objects
		uint8_t testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container);

		// Test Selector on GUI Elements
		uint8_t testSelectorElements(Editor::Selector& selector, Editor::ObjectInfo& objectinfo, Render::GUI::ObjectContainer_GUI& container);

		// Test Selector on an Individual Object
		uint8_t testSelectorOnObject(Object::Object*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info, ObjectContainer& container);

		// Test Selector on an Object List
		template <class Type> uint8_t testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info);

		// Remove Object From Primary Object List
		void removeMarkedFromList(Object::Object* marked_object, glm::vec2* new_selected_position, ObjectContainer& container);

		// Remove Marked Children From List Without Causing a Reload
		void removeMarkedChildrenFromListHelper(DataClass::Data_Object* marked_parent, ObjectContainer& container);

		// Add Objects From List of New Objects Without Causing a Reload
		void incorperatNewObjectsHelper(Object::Object** new_objects, int new_objects_size, ObjectContainer& container);

		// Reset the Object Pass Over Flag
		virtual void resetObjectPassOver() = 0;

		// Helper Function to Build a Sorted List of Object Indicies
		void getObjectIndicies(DataClass::Data_Object* parent, uint32_t** indicies, int& indicies_size);

		// Perform Binary Search on a List of Object Indicies
		bool searchObjectIndicies(uint32_t* indicies, int left, int right, uint32_t test_value);

		// Store Level of Origin
		virtual void storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, Object::Object* real_object) = 0;

	public:

		// The Camera Object for the Container
		Render::Camera::Camera* camera;

		// Returns the Type of Container Object
		virtual CONTAINER_TYPES getContainerType() = 0;

		// Update the Level Coordinates of Container
		virtual void updateLevelPos(glm::vec2 position, glm::i16vec2& level) = 0;

		// Update the Camera During Main Game Loop
		virtual void updateCamera() = 0;

		// Update the Objects In Container During Main Game Loop
		virtual void updateContainer() = 0;

		// Draw the Objects In Container During Main Game Loop
		virtual void drawContainer() = 0;

		// Draw Visualizers for Objects That Don't Get Rendered During Gameplay
		virtual void drawVisualizers() = 0;

		// Load Objects After Reading From Files
		virtual void loadObjects() = 0;

		// Reload All Objects in Container
		virtual void reloadAll() = 0;

		// Add Object and Decendants Into Container
		virtual void genObjectIntoContainer(DataClass::Data_Object* new_object, Object::Object* real_parent, uint16_t& index, int16_t delta_size) = 0;

		// Generate Objects Into This Container
		virtual void buildObjectsGenerator(std::vector<DataClass::Data_Object*>& data_object_array, uint16_t& index, Object::Object* parent, glm::vec2 position_offset, uint16_t& active_index, Objects::UnsavedLevel& unsaved_level) = 0;

		// The Function to Select Objects in a Given Container
		virtual uint8_t testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info) = 0;

		// Remove Marked Object From List With Unkown Container Type
		virtual void removeMarkedChildrenFromList(DataClass::Data_Object* parent) = 0;

		// Incorporate a List of Objects Into Loaded Level With Unkown Container Type
		virtual void incorperatNewObjects(Object::Object** new_objects, int new_objects_size) = 0;

		// Clear the Temproary Objects of Container Used for Selection
		void clearTemps();

		// Get the Level Size
		virtual void getSublevelSize(glm::vec2& sizes) = 0;

		// Reallocate the Objects Array to Accomidate Objects Returned to From Editing
		int32_t reallocateObjectsArray(int32_t delta_size);
	};

}

#endif