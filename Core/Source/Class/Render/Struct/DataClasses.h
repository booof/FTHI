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
		uint8_t* get_object_identifier();
	};

	// Sub Object Data Class
	class Data_SubObject : public Data_Object
	{

	protected:

		// Object Data
		Object::ObjectData data;
	};

	// Horizontal Line
	class Data_HorizontalLine
	{

	protected:

		Object::Mask::HorizontalLineData data;
	};

	// Slant
	class Data_Slant
	{

	protected:

		Object::Mask::SlantData data;
	};

	// Slope
	class Data_Slope
	{

	protected:

		Object::Mask::SlopeData data;
	};

	// Floor Mask
	class Data_Floor : public Data_Object
	{

	protected:

		// Determines if the Mask is a Platform
		bool platform;
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
	};

	// Ceiling Mask Horizontal Line
	class Data_CeilingMaskHorizontalLine : public Data_Object, public Data_HorizontalLine
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
	};

	// Ceiling Mask Slant
	class Data_CeilingMaskSlant : public Data_Object, public Data_Slant
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
	};

	// Ceiling Mask Slope
	class Data_CeilingMaskSlope : public Data_Object, public Data_Slope
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
	};

	// Vertical Line
	class Data_VerticalLine
	{

	protected:

		Object::Mask::VerticalLineData data;
	};

	// Curve
	class Data_Curve
	{

	protected:

		Object::Mask::CurveData data;
	};

	// Left Mask Vertical Line
	class Data_LeftMaskVerticalLine : public Data_Object, Data_VerticalLine
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
	};

	// Left Mask Curve
	class Data_LeftMaskCurve : public Data_Object, Data_Curve
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
	};

	// Right Mask Vertical Line
	class Data_RightMaskVerticalLine : public Data_Object, public Data_VerticalLine
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
	};

	// Right Mask Curve
	class Data_RightMaskCurve : public Data_Object, public Data_Curve
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
	};
	
	// Terrain Object
	class Data_Terrain : public Data_SubObject
	{
		// Pointer to Shape
		Shape::Shape* shape;

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
	};

	// Light Object
	class Data_Light : public Data_Object
	{
		
	protected:

		// Light Data
		Object::Light::LightData light_data;
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
	};

	// Rigid Body Physics Object
	class Data_RigidBody : public Data_SubObject
	{
		// Pointer to Shape
		Shape::Shape* shape;

		// UUID
		uint32_t uuid;

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
	};

	// SpringMass Physics Object
	class Data_SpringMass : public Data_SubObject
	{
		// File Name
		std::string file_name;

		// UUID
		uint32_t uuid;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// SpringMass Data
		Data_SpringMass();
	};

	// Wire Physics Object
	class Data_Wire : public Data_SubObject
	{
		// Wire Data
		Object::Physics::Soft::WireData wire;

		// UUID
		uint32_t uuid;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Wire Data
		Data_Wire();
	};

	// Anchor Physics Object
	class Data_Anchor : public Data_Object
	{
		// Anchor Data
		Object::Physics::Hinge::AnchorData data;

		// UUID
		uint32_t uuid;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// Anchor Data
		Data_Anchor();
	};

	// Hinge Physics Object
	class Data_Hinge : public Data_Object
	{
		// File Name
		std::string file_name;

		// UUID
		uint32_t uuid;

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
	};

	// Entity Object
	class Data_Entity : public Data_SubObject
	{

	protected:

		// Entity Data
		Object::Entity::EntityData entity;

		// UUID
		uint32_t uuid;
	};

	// NPC Entities
	class Data_NPC : public Data_Entity
	{
		// AI
		uint16_t ai;

		// Function to Read Data and Create an Object
		Object::Object* genObject();

		// Function to Write Data to File
		void writeObjectData(std::ofstream& object_file);

		// Function to Read Data From File
		void readObjectData(std::ifstream& object_file);

	public:

		// NPC Data
		Data_NPC();
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
	};
}

#endif
