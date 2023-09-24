#pragma once
#ifndef DATA_CLASSES_H
#define DATA_CLASSES_H

#include "ExternalLibs.h"
#include "Render/Objects/UnsavedBase.h"
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
#include "Object/Collision/CollisionMask.h"
#include "Object/Lighting/Directional.h"
#include "Object/Lighting/Point.h"
#include "Object/Lighting/Spot.h"
#include "Object/Lighting/Beam.h"
#include "Object/Physics/RigidBody/RigidBody.h"
#include "Object/Physics/SoftBody/SpringMass.h"
#include "Object/Physics/SoftBody/Wire.h"
#include "Object/Physics/Hinge/Anchor.h"
#include "Object/Physics/Hinge/Hinge.h"
#include "Object/Entity/NPC.h"
#include "Object/Entity/Controllables.h"
#include "Object/Entity/Interactables.h"
#include "Object/Entity/Dynamics.h"
#include "Object/Group/GroupObject.h"

// This File Contains the Classes that Only Contain the Data for Engine Objects With Some Helper Functions
// This is to be Used for the Improved Unsaved Level and Allow for Multiple Objects to be Selected at Once

// Declaration for Shapes
namespace Shape
{
	class Shape;
}

// Declaration for Unsaved Objects
namespace Render::Objects {
	class UnsavedCollection;
}

namespace DataClass
{
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

	class Data_Object
	{

	protected:

		// Identifier for the Object
		uint8_t object_identifier[4] = { NULL };

		// Editor Data
		EditorData editor_data;

		// Name of the Object
		std::string name = "";

		// The Index of the Object
		uint32_t object_index = 0;

		// The Pointer to the Unsaved Group (Or Unsaved Complex)
		Render::Objects::UnsavedCollection* group_object = nullptr;

		// The Pointer to the Current Parent DataClass
		Data_Object* parent = nullptr;

		// Determines if the Object Should Move With Its Parent
		Render::Objects::MOVE_WITH_PARENT move_with_parent = Render::Objects::MOVE_WITH_PARENT::MOVE_ENABLED;

		// The Layer the Object is in In a Group
		int8_t group_layer = 0;
		
		// Determines if the Object is Able to be Selected
		bool selectable = true;

		// The Array of Objects This DataClass Represents
		// This Array is Unique to Each Object Index
		std::vector<Object::Object*>* object_pointers = nullptr;

		// Helper Function to Update Selected Positions
		void updateSelectedPositionsHelper(float deltaX, float deltaY, bool update_real);

		// Read Editor Data in Generated Object
		void readEditorData(std::ifstream& editor_file);

		// Write Editor Data in Generated Object
		void writeEditorData(std::ofstream& editor_file);

		// Function to Read Data and Create an Object
		virtual Object::Object* genObject(glm::vec2& offset) = 0;

		// Function to Write Data to File
		virtual void writeObjectData(std::ofstream& object_file) = 0;

		// Function to Read Data From File
		virtual void readObjectData(std::ifstream& object_file) = 0;

		// Function to Generate a New Shape
		Shape::Shape* readNewShape(std::ifstream& object_file, uint8_t index);

		// Set Position 2 and 3 Pointers to Null
		void position23Null(int& index2, int& index3, glm::vec2** position2, glm::vec2** position3);

		// Set Position 3 Pointer to Null
		void position3Null(int& index3, glm::vec2** position3);

		// Constructor for Base Class
		Data_Object();

	public:

		// Write Object
		virtual void writeObject(std::ofstream& object_file, std::ofstream& editor_file);

		// Read Object
		void readObject(std::ifstream& object_file, std::ifstream& editor_file);

		// Function to Add a Child to a Data Object
		void addChild(DataClass::Data_Object* data_object);

		// Function to Add a Child to a Data Object Via Selection
		void addChildViaSelection(DataClass::Data_Object* data_object, Render::Objects::MOVE_WITH_PARENT disable_move);

		// Function to Draw Group Visualizer Points and Lines With the Given Offsets
		void drawGroupVisualizerHelper(glm::vec2& left_offset, glm::vec2& right_offset, glm::vec2& point_offset, glm::vec2 new_offset);

		// Function to Draw Group Visualizers
		void drawGroupVisualizer(glm::vec2 current_offset);

		// Function to Draw Group Visualizers Specifically for Selector
		void drawSelectedGroupVisualizer(glm::vec2 new_offset);

		// Modified Function to Draw Group Visualizers But With an Offset
		void drawSelectedGroupVisualizerOffset(glm::vec2 new_offset, glm::vec2 new_offset2);

		// Function to Draw Group Visualizer With Parent
		void drawParentConnection();

		// Returns the Group Object
		Render::Objects::UnsavedCollection* getGroup();

		// Set the Parent Object
		void setParent(Data_Object* new_parent);

		// Get the Parent Object
		Data_Object* getParent();

		// Disable the Move With Parent Feature
		void disableMoveWithParent(Render::Objects::MOVE_WITH_PARENT mode);

		// Re-Enable the Move With Parent Feature
		void enableMoveWithParent();

		// Get the Move With Parent Flag
		Render::Objects::MOVE_WITH_PARENT getMoveWithParent();

		// Set the Layer the Object is in a Group
		virtual void setGroupLayer(int8_t new_layer);

		// Get the Group Layer
		int8_t& getGroupLayer();

		// Get the List of the Real Objects
		std::vector<Object::Object*>& getObjects();

		// Remove an Object from Real Object List
		void removeObject(Object::Object* object);

		// Clear the Pointers to the Real Objects
		void clearObjects();

		// Generate Object
		Object::Object* generateObject(glm::vec2& offset);

		// Get the Object Identifier
		uint8_t* getObjectIdentifier();

		// Get the Object Index
		uint32_t getObjectIndex();

		// Get Editor Data
		EditorData& getEditorData();

		// Get the Name of the Object
		std::string& getName();

		// Get the Script of an Object
		virtual int& getScript() = 0;

		// Get the Position of an Object
		virtual glm::vec2& getPosition() = 0;

		// Set the Object Info of the Object
		virtual void info(Editor::ObjectInfo& object_info) = 0;

		// Create a Copy of the Object
		virtual Data_Object* makeCopy() = 0;

		// Make a Copy of the Object for Selecting
		Data_Object* makeCopySelected(Editor::Selector& selector);

		// Make a Unique Copy of the Object  
		Data_Object* makeCopyUnique();

		// Update the Selected Position of an Object
		virtual void updateSelectedPosition(float deltaX, float deltaY, bool update_real) = 0;

		// Generate Children Recursively
		void genChildrenRecursive(Object::Object*** object_list, int& list_size, Object::Object* parent, glm::vec2& offset, Editor::Selector* selector, bool test_groups);

		// Tests if an Object Index is a Parent of the Object
		bool testIsParent(DataClass::Data_Object* parent);

		// Disables the Object for Selecting
		void disableSelecting();

		// Enables the Object for Selecting
		void enableSelection();

		// Enables Selecting For This Object Only
		void enableSelectionNonRecursive();

		// Determines if the Object can be Selected
		bool isSelectable();

		// Offset the Position of the Object
		virtual void offsetPosition(glm::vec2& offset);

		// Offset the Position of the Object and All Children
		void offsetPositionRecursive(glm::vec2& offset);

		// Helper Function to Offset Position of Object and All Children
		void offsetPositionRecursiveHelper(glm::vec2& offset);

		// For Objects With Multiple Positions, Only Offset the Opposite Positions
		virtual void offsetOppositePosition(glm::vec2& offset);

		// Returns the Color at the Specified Index
		glm::vec4& returnLineColor(int8_t index);

		// Gets the Indicies and Position Pointers for Object Info
		virtual void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3) = 0;

		// Returns True if the Object Has Real Objects
		bool hasReals();
	};

	// Sub Object Data Class
	class Data_SubObject : public Data_Object
	{

	protected:

		// Object Data
		Object::ObjectData data;

	public:

		Object::ObjectData& getObjectData();

		int& getScript();

		glm::vec2& getPosition();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);
	};

	// A Helper Class Specifically for Complex Object to Hold Files
	class Data_Complex : public Data_Object
	{

	protected:

		// File Name
		std::string file_name;

		// File Path
		std::string file_path;

	public:

		// Get the File Path
		std::string& getFilePath();

		// Set the Group Object
		void setGroup(Render::Objects::UnsavedComplex* new_group);
	};

	// Horizontal Line
	class Data_HorizontalLine : public Data_Object
	{

	protected:

		Object::Mask::HorizontalLineData data;
		
	public:

		Object::Mask::HorizontalLineData& getHorizontalLineData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Slant
	class Data_Slant : public Data_Object
	{

	protected:

		Object::Mask::SlantData data;

	public:

		Object::Mask::SlantData& getSlantData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		// Offset the Position of the Object
		void offsetPosition(glm::vec2& offset);

		// Offset the Opposite Position
		void offsetOppositePosition(glm::vec2& offset);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Slope
	class Data_Slope : public Data_Object
	{

	protected:

		Object::Mask::SlopeData data;

	public:

		Object::Mask::SlopeData& getSlopeData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Floor Mask
	class Data_Floor
	{

	protected:

		// Determines if the Mask is a Platform
		bool platform = false;

	public:

		bool& getPlatform();
	};

	// Floor Mask Hoizontal Line
	class Data_FloorMaskHorizontalLine : public Data_Floor, public Data_HorizontalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Horizontal Line Data 
		Data_FloorMaskHorizontalLine(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Floor Mask Slant 
	class Data_FloorMaskSlant : public Data_Floor, public Data_Slant
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Slant Data
		Data_FloorMaskSlant(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Floor Mask Slope
	class Data_FloorMaskSlope : public Data_Floor, public Data_Slope
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Slope Data
		Data_FloorMaskSlope(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Horizontal Line
	class Data_CeilingMaskHorizontalLine : public Data_HorizontalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Horizontal Line Data
		Data_CeilingMaskHorizontalLine(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Slant
	class Data_CeilingMaskSlant : public Data_Slant
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Slant Data
		Data_CeilingMaskSlant(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Slope
	class Data_CeilingMaskSlope : public Data_Slope
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Slope Data
		Data_CeilingMaskSlope(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Vertical Line
	class Data_VerticalLine : public Data_Object
	{

	protected:

		Object::Mask::VerticalLineData data;

	public:

		Object::Mask::VerticalLineData& getVerticalLineData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Curve
	class Data_Curve : public Data_Object
	{

	protected:

		Object::Mask::CurveData data;

	public:

		Object::Mask::CurveData& getCurveData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Left Mask Vertical Line
	class Data_LeftMaskVerticalLine : public Data_VerticalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Left Mask Vertical Line Data
		Data_LeftMaskVerticalLine(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Left Mask Curve
	class Data_LeftMaskCurve : public Data_Curve
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Left Mask Curve Data
		Data_LeftMaskCurve(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Right Mask Vertical Line
	class Data_RightMaskVerticalLine : public Data_VerticalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Right Mask Vertical Line Data
		Data_RightMaskVerticalLine(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Right Mask Curve
	class Data_RightMaskCurve : public Data_Curve
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Right Mask Curve Data
		Data_RightMaskCurve(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Trigger Mask
	class Data_TriggerMask : public Data_Object
	{
		Object::Mask::Trigger::TriggerData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Trigger Mask Data
		Data_TriggerMask(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		Object::Mask::Trigger::TriggerData& getTriggerData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Shape
	class Data_Shape : public Data_SubObject
	{

	protected:

		// Pointer to Shape
		Shape::Shape* shape;

	public:

		Shape::Shape* getShape();

		// Offset the Position of the Object
		void offsetPosition(glm::vec2& offset);

		// If Triangle, Offset the Opposite Positions
		void offsetOppositePosition(glm::vec2& offset);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};
	
	// Terrain Object
	class Data_Terrain : public Data_Shape
	{
		// Layer
		uint8_t layer;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Function to Test if Object is a Background Terrain Object
		bool testIfBackground();

	public:

		// Terrain Data
		Data_Terrain(uint8_t layer_identifier, uint8_t shape_identifier, uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		void generateInitialValues(glm::vec2& position, glm::vec4 color, Shape::Shape* new_shape);

		uint8_t& getLayer();
	};

	// Light Object
	class Data_Light : public Data_Object
	{
		
	protected:

		// Light Data
		Object::Light::LightData light_data;

		const float DEFAULT_LINEAR = 1.0f;
		const float DEFAULT_QUADRATIC = 1.0f;

		// Set Object Info Colors
		void infoColors(Editor::ObjectInfo& object_info);

	public:

		Object::Light::LightData& getLightData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialLightValues(glm::vec2& new_position);
	};

	// Directional Light
	class Data_Directional : public Data_Light
	{
		// Directional Data
		Object::Light::Directional::DirectionalData directional;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Directional Light Data
		Data_Directional(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		Object::Light::Directional::DirectionalData& getDirectionalData();

		void generateInitialValues(glm::vec2& position, float& size);

		// Offset the Position of the Object
		void offsetPosition(glm::vec2& offset);

		// Offset the Opposite Position
		void offsetOppositePosition(glm::vec2& offset);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Point Light
	class Data_Point : public Data_Light
	{
		// Point Data
		Object::Light::Point::PointData point;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Point Light Data
		Data_Point(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		Object::Light::Point::PointData& getPointData();

		void generateInitialValues(glm::vec2& position);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Spot Light
	class Data_Spot : public Data_Light
	{
		// Spot Data
		Object::Light::Spot::SpotData spot;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Spot Light Data
		Data_Spot(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		Object::Light::Spot::SpotData& getSpotData();

		void generateInitialValues(glm::vec2& position);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Beam Light
	class Data_Beam : public Data_Light
	{
		// Beam Data
		Object::Light::Beam::BeamData beam;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Beam Light Data
		Data_Beam(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		Object::Light::Beam::BeamData& getBeamData();

		void generateInitialValues(glm::vec2& position, float& size);

		// Offset the Position of the Object
		void offsetPosition(glm::vec2& offset);

		// Offset the Opposite Position
		void offsetOppositePosition(glm::vec2& offset);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Object with a UUID
	class Data_UUID
	{

	protected:

		// UUID
		uint32_t uuid;

	public:

		uint32_t& getUUID();

		void generateUUID();
	};

	// Rigid Body Physics Object
	class Data_RigidBody : public Data_Shape, public Data_UUID
	{
		// Rigid Body Data
		Object::Physics::Rigid::RigidBodyData rigid;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Rigid Body Data
		Data_RigidBody(uint8_t shape_identifier, uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Physics::Rigid::RigidBodyData& getRigidData();

		void generateInitialValues(glm::vec2& position, Shape::Shape* new_shape);
	};

	// SpringMass Physics Object
	class Data_SpringMass : public Data_SubObject, public Data_UUID
	{
		// File Name
		std::string file_name;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// SpringMass Data
		Data_SpringMass(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		std::string& getFile();

		void generateInitialValues(glm::vec2& position);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// SpringMass Node Object
	class Data_SpringMassNode : public Data_Object
	{
		// Pointer to Parent Object
		Data_SpringMass* parent;

		// Node Data of Object
		Object::Physics::Soft::NodeData node_data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Get the Script of an Object
		int& getScript();

		// Get the Position of an Object
		glm::vec2& getPosition();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

	public:

		// SpringMass Node Data
		Data_SpringMassNode(uint8_t children_size);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Function to Generate the Node Object
		Object::Physics::Soft::Node genNode();

		// Function to Store Parent SpringMass Object
		void storeParent(Data_SpringMass* springmass_object);

		// Function to Retrieve the Parent SpringMass Object
		Data_SpringMass* getParent();

		// Function to Get the Node Data
		Object::Physics::Soft::NodeData& getNodeData();

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// SpringMass Node Object
	class Data_SpringMassSpring : public Data_Object
	{
		// Pointer to Parent Object
		Data_SpringMass* parent;

		// Spring Data of Object
		Object::Physics::Soft::Spring spring_data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Get the Script of an Object
		int& getScript();

		// Get the Position of an Object
		glm::vec2& getPosition();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

	public:

		// SpringMass Spring Data
		Data_SpringMassSpring(uint8_t children_size);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Function to Generate the Spring Object
		Object::Physics::Soft::Spring genSpring();

		// Function to Store Parent SpringMass Object
		void storeParent(Data_SpringMass* springmass_object);

		// Function to Retrieve the Parent SpringMass Object
		Data_SpringMass* getParent();

		// Function to Get the Spring Data
		Object::Physics::Soft::Spring& getSpringData();

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Wire Physics Object
	class Data_Wire : public Data_SubObject, public Data_UUID
	{
		// Wire Data
		Object::Physics::Soft::WireData wire;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Wire Data
		Data_Wire(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		void generateInitialValues(glm::vec2& position, float& size);

		Object::Physics::Soft::WireData& getWireData();

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Anchor Physics Object
	class Data_Anchor : public Data_Object, public Data_UUID
	{
		// Anchor Data
		Object::Physics::Hinge::AnchorData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Anchor Data
		Data_Anchor(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position);

		Object::Physics::Hinge::AnchorData& getAnchorData();

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Hinge Physics Object
	class Data_Hinge : public Data_Object, public Data_UUID
	{
		// File Name
		std::string file_name;

		// Hinge Data
		Object::Physics::Hinge::HingeData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Hinge Data
		Data_Hinge(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position);

		Object::Physics::Hinge::HingeData& getHingeData();

		std::string& getFile();

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// Entity Object
	class Data_Entity : public Data_SubObject, public Data_UUID
	{

	protected:

		// Entity Data
		Object::Entity::EntityData entity;

	public:

		Object::Entity::EntityData& getEntityData();

		void generateInitialData(glm::vec2& position);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// NPC Entities
	class Data_NPC : public Data_Entity
	{
		// AI
		uint16_t ai = 0;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// NPC Data
		Data_NPC(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Controllable Entities
	class Data_Controllable : public Data_Entity
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Controllable Entity Data
		Data_Controllable(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Interactable Entities
	class Data_Interactable : public Data_Entity
	{
		// Interactable Data
		Object::Entity::InteractableData interactable;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Interactable Entity Data
		Data_Interactable(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Dynamic Entities
	class Data_Dynamic : public Data_Entity
	{
		// Dynamic Data
		Object::Entity::DynamicData dynamic;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Dynamic Entity Data
		Data_Dynamic(uint8_t children_size);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Group Objects
	class Data_GroupObject : public Data_Complex
	{
		// Group Data
		Object::Group::GroupData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Get the Script of an Object
		int& getScript();

		// Get the Position of an Object
		glm::vec2& getPosition();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

	public:

		// Group Object Data
		Data_GroupObject();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		// Get Group Data
		Object::Group::GroupData& getGroupData();

		void generateInitialData(glm::vec2& position);

		// Write Object
		void writeObject(std::ofstream& object_file, std::ofstream& editor_file);

		// Set the Layer the Object is in a Group
		void setGroupLayer(int8_t new_layer);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};

	// DataClass for Complex Object While Editing
	// Used as Parent of Data Classes When Editing a Complex Object
	// Will Be Created and Used Only While Editing, Will be Deleted After
	class Data_ComplexParent : public Data_Object
	{
		// A Null Value
		static int null_value;

		// Determines if the Object is Active
		bool active = false;

		// Position Offset for Object
		glm::vec2 position_offset = glm::vec2(0.0f, 0.0f);

		// Root Parent of Object
		// Will be Set When Activated
		Object::Object* root_parent = nullptr;

		// Function to Read Data and Create an Object
		Object::Object* genObject(glm::vec2& offset);

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Get the Script of an Object
		int& getScript();

		// Get the Position of an Object
		glm::vec2& getPosition();

		// Update the Selected Position of an Object
		void updateSelectedPosition(float deltaX, float deltaY, bool update_real);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

	public:

		// Default Constructor for Object
		Data_ComplexParent();

		// Initialize Object
		void setGroup(Render::Objects::UnsavedComplex* complex_group);

		// Set the Position Offset of Object
		void setPositionOffset(glm::vec2 new_offset);

		// Returns the Position Offset of Object
		glm::vec2 getPositionOffset();

		// Store the Root Parent
		void storeRootParent(Object::Object* parent);

		// Get the Root Parent
		Object::Object* getRootParent();

		// Determines if the Object is Active
		bool isActive();

		// Sets the Object to be Active
		void setActive();

		// Disables the Object
		void setInactive();

		// Set the Layer the Object is in a Group
		void setGroupLayer(int8_t new_layer);

		void setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3);
	};
}

#endif
