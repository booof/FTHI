#pragma once
#ifndef DATA_CLASSES_H
#define DATA_CLASSES_H

#include "ExternalLibs.h"
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
#include "Object/Physics/Softody/SpringMass.h"
#include "Object/Physics/Softody/Wire.h"
#include "Object/Physics/Hinge/Anchor.h"
#include "Object/Physics/Hinge/Hinge.h"
#include "Object/Entity/NPC.h"
#include "Object/Entity/Controllables.h"
#include "Object/Entity/Interactables.h"
#include "Object/Entity/Dynamics.h"

// This File Contains the Classes that Only Contain the Data for Engine Objects With Some Helper Functions
// This is to be Used for the Improved Unsaved Level and Allow for Multiple Objects to be Selected at Once

// Declaration for Shapes
namespace Shape
{
	class Shape;
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
		uint8_t object_identifier[3] = { NULL };

		// Editor Data
		EditorData editor_data;

		// Name of the Object
		std::string name = "";

		// The Index of the Object
		uint32_t object_index = 0;

		// Lambdas to Read a Shape
		std::function<Shape::Shape* (std::ifstream& object_file)> shapes[5] = {
			[](std::ifstream& object_file)->Shape::Shape* {return new Shape::Rectangle(object_file); },
			[](std::ifstream& object_file)->Shape::Shape* {return new Shape::Trapezoid(object_file); },
			[](std::ifstream& object_file)->Shape::Shape* {return new Shape::Triangle(object_file); },
			[](std::ifstream& object_file)->Shape::Shape* {return new Shape::Circle(object_file); },
			[](std::ifstream& object_file)->Shape::Shape* {return new Shape::Polygon(object_file); }
		};

		// Read Editor Data in Generated Object
		void readEditorData(std::ifstream& editor_file);

		// Write Editor Data in Generated Object
		void writeEditorData(std::ofstream& editor_file);

		// Function to Read Data and Create an Object
		virtual Object::Object* genObject() = 0;

		// Function to Write Data to File
		virtual void writeObjectData(std::ofstream& object_file) = 0;

		// Function to Read Data From File
		virtual void readObjectData(std::ifstream& object_file) = 0;

		// Constructor for Base Class
		Data_Object();

	public:

		// Write Object
		void writeObject(std::ofstream& object_file, std::ofstream& editor_file);

		// Read Object
		void readObject(std::ifstream& object_file, std::ifstream& editor_file);

		// Generate Object
		Object::Object* generateObject();

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

		// Make a Unique Copy of the Object
		Data_Object* makeCopyUnique();
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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Horizontal Line Data 
		Data_FloorMaskHorizontalLine();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Floor Mask Slant 
	class Data_FloorMaskSlant : public Data_Floor, public Data_Slant
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Slant Data
		Data_FloorMaskSlant();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Floor Mask Slope
	class Data_FloorMaskSlope : public Data_Floor, public Data_Slope
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Floor Mask Slope Data
		Data_FloorMaskSlope();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Horizontal Line
	class Data_CeilingMaskHorizontalLine : public Data_HorizontalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Horizontal Line Data
		Data_CeilingMaskHorizontalLine();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Slant
	class Data_CeilingMaskSlant : public Data_Slant
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Slant Data
		Data_CeilingMaskSlant();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Ceiling Mask Slope
	class Data_CeilingMaskSlope : public Data_Slope
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Ceiling Mask Slope Data
		Data_CeilingMaskSlope();

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
	};

	// Left Mask Vertical Line
	class Data_LeftMaskVerticalLine : public Data_VerticalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Left Mask Vertical Line Data
		Data_LeftMaskVerticalLine();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Left Mask Curve
	class Data_LeftMaskCurve : public Data_Curve
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Left Mask Curve Data
		Data_LeftMaskCurve();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Right Mask Vertical Line
	class Data_RightMaskVerticalLine : public Data_VerticalLine
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Right Mask Vertical Line Data
		Data_RightMaskVerticalLine();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Right Mask Curve
	class Data_RightMaskCurve : public Data_Curve
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Right Mask Curve Data
		Data_RightMaskCurve();

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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Trigger Mask Data
		Data_TriggerMask();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Mask::Trigger::TriggerData& getTriggerData();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position, float& size);
	};

	// Shape
	class Data_Shape : public Data_SubObject
	{

	protected:

		// Pointer to Shape
		Shape::Shape* shape;

	public:

		Shape::Shape* getShape();
	};
	
	// Terrain Object
	class Data_Terrain : public Data_Shape
	{
		// Layer
		uint8_t layer;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

		// Function to Test if Object is a Background Terrain Object
		bool testIfBackground();

	public:

		// Terrain Data
		Data_Terrain(uint8_t layer_identifier, uint8_t shape_identifier);

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		void generateInitialValues(glm::vec2& position, glm::vec4 color, Shape::Shape* new_shape);
	};

	// Light Object
	class Data_Light : public Data_Object
	{
		
	protected:

		// Light Data
		Object::Light::LightData light_data;

		const float DEFAULT_LINEAR = 1.0f;
		const float DEFAULT_QUADRATIC = 1.0f;

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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Directional Light Data
		Data_Directional();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Light::Directional::DirectionalData& getDirectionalData();

		void generateInitialValues(glm::vec2& position, float& size);
	};

	// Point Light
	class Data_Point : public Data_Light
	{
		// Point Data
		Object::Light::Point::PointData point;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Point Light Data
		Data_Point();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Light::Point::PointData& getPointData();

		void generateInitialValues(glm::vec2& position);
	};

	// Spot Light
	class Data_Spot : public Data_Light
	{
		// Spot Data
		Object::Light::Spot::SpotData spot;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Spot Light Data
		Data_Spot();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Light::Spot::SpotData& getSpotData();

		void generateInitialValues(glm::vec2& position);
	};

	// Beam Light
	class Data_Beam : public Data_Light
	{
		// Beam Data
		Object::Light::Beam::BeamData beam;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Beam Light Data
		Data_Beam();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		Object::Light::Beam::BeamData& getBeamData();

		void generateInitialValues(glm::vec2& position, float& size);
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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Rigid Body Data
		Data_RigidBody(uint8_t shape_identifier);

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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// SpringMass Data
		Data_SpringMass();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		std::string& getFile();

		void generateInitialValues(glm::vec2& position);
	};

	// Wire Physics Object
	class Data_Wire : public Data_SubObject, public Data_UUID
	{
		// Wire Data
		Object::Physics::Soft::WireData wire;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Wire Data
		Data_Wire();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		void generateInitialValues(glm::vec2& position, float& size);

		Object::Physics::Soft::WireData& getWireData();
	};

	// Anchor Physics Object
	class Data_Anchor : public Data_Object, public Data_UUID
	{
		// Anchor Data
		Object::Physics::Hinge::AnchorData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Anchor Data
		Data_Anchor();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position);

		Object::Physics::Hinge::AnchorData& getAnchorData();
	};

	// Hinge Physics Object
	class Data_Hinge : public Data_Object, public Data_UUID
	{
		// File Name
		std::string file_name;

		// Hinge Data
		Object::Physics::Hinge::HingeData data;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Hinge Data
		Data_Hinge();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();

		int& getScript();

		glm::vec2& getPosition();

		void generateInitialValues(glm::vec2& position);

		Object::Physics::Hinge::HingeData& getHingeData();

		std::string& getFile();
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
	};

	// NPC Entities
	class Data_NPC : public Data_Entity
	{
		// AI
		uint16_t ai = 0;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// NPC Data
		Data_NPC();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};

	// Controllable Entities
	class Data_Controllable : public Data_Entity
	{
		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Controllable Entity Data
		Data_Controllable();

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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Interactable Entity Data
		Data_Interactable();

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
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Dynamic Entity Data
		Data_Dynamic();

		// Set the Object Info of the Object
		void info(Editor::ObjectInfo& object_info);

		// Create a Copy of the Object
		Data_Object* makeCopy();
	};
}

#endif
