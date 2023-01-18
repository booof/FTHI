#include "DataClasses.h"

#include "Globals.h"

#include "Object/Collision/Horizontal/Line/FloorMaskLine.h"
#include "Object/Collision/Horizontal/Slant/FloorMaskSlant.h"
#include "Object/Collision/Horizontal/Slope/FloorMaskSlope.h"
#include "Object/Collision/Vertical/Line/LeftMaskLine.h"
#include "Object/Collision/Vertical/Curve/LeftMaskCurve.h"
#include "Object/Collision/Vertical/Line/RightMaskLine.h"
#include "Object/Collision/Vertical/Curve/RightMaskCurve.h"
#include "Object/Collision/Horizontal/Line/CeilingMaskLine.h"
#include "Object/Collision/Horizontal/Slant/CeilingMaskSlant.h"
#include "Object/Collision/Horizontal/Slope/CeilingMaskSlope.h"
#include "Object/Terrain/TerrainBase.h"
#include "Object/Terrain/Backdrop.h"
#include "Object/Terrain/Background.h"
#include "Object/Terrain/Foreground.h"
#include "Object/Terrain/Formerground.h"
#include "Object/Physics/RigidBody/RigidRectangle.h"
#include "Object/Physics/RigidBody/RigidTrapezoid.h"
#include "Object/Physics/RigidBody/RigidTriangle.h"
#include "Object/Physics/RigidBody/RigidCircle.h"
#include "Object/Physics/RigidBody/RigidPolygon.h"

void DataClass::Data_Object::readEditorData(std::ifstream& editor_file)
{
	editor_file.read((char*)&editor_data, sizeof(EditorData));
	name.resize(editor_data.name_size);
	editor_file.read(&name[0], editor_data.name_size);
}

void DataClass::Data_Object::writeEditorData(std::ofstream& editor_file)
{
	editor_file.write((char*)&editor_data, sizeof(EditorData));
	editor_file.write(name.c_str(), name.size());
}

DataClass::Data_Object::Data_Object()
{
	object_index = Global::object_index_counter;
	Global::object_index_counter++;
}

void DataClass::Data_Object::writeObject(std::ofstream& object_file, std::ofstream& editor_file)
{
	writeObjectData(object_file);
	writeEditorData(editor_file);
}

void DataClass::Data_Object::readObject(std::ifstream& object_file, std::ifstream& editor_file)
{
	readObjectData(object_file);
	readEditorData(editor_file);
}

Object::Object* DataClass::Data_Object::generateObject()
{
	Object::Object* new_object = genObject();
	new_object->name = name;
	new_object->clamp = editor_data.clamp;
	new_object->lock = editor_data.lock;
	new_object->object_index = object_index;
	return new_object;
}


uint8_t* DataClass::Data_Object::getObjectIdentifier()
{
	return object_identifier;
}

uint32_t DataClass::Data_Object::getObjectIndex()
{
	return object_index;
}

DataClass::EditorData& DataClass::Data_Object::getEditorData()
{
	return editor_data;
}

std::string& DataClass::Data_Object::getName()
{
	return name;
}

Object::ObjectData& DataClass::Data_SubObject::getObjectData()
{
	return data;
}

int& DataClass::Data_SubObject::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_SubObject::getPosition()
{
	return data.position;
}

Object::Mask::HorizontalLineData& DataClass::Data_HorizontalLine::getHorizontalLineData()
{
	return data;
}

int& DataClass::Data_HorizontalLine::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_HorizontalLine::getPosition()
{
	return data.position;
}

void DataClass::Data_HorizontalLine::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size;
	data.script = 0;
	data.material = 0;
}

Object::Mask::SlantData& DataClass::Data_Slant::getSlantData()
{
	return data;
}

int& DataClass::Data_Slant::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Slant::getPosition()
{
	return data.position;
}

void DataClass::Data_Slant::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.position2 = position + glm::vec2(size, 0.0f);
	data.script = 0;
	data.material = 0;
}

Object::Mask::SlopeData& DataClass::Data_Slope::getSlopeData()
{
	return data;
}

int& DataClass::Data_Slope::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Slope::getPosition()
{
	return data.position;
}

void DataClass::Data_Slope::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size * 2.0f;
	data.height = size;
	data.script = 0;
	data.material = 0;
}

bool& DataClass::Data_Floor::getPlatform()
{
	return platform;
}

Object::Object* DataClass::Data_FloorMaskHorizontalLine::genObject()
{
	return new Object::Mask::Floor::FloorMaskLine(data, platform);
}

void DataClass::Data_FloorMaskHorizontalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::HorizontalLineData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskHorizontalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::HorizontalLineData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskHorizontalLine::Data_FloorMaskHorizontalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_LINE;
}

Object::Object* DataClass::Data_FloorMaskSlant::genObject()
{
	return new Object::Mask::Floor::FloorMaskSlant(data, platform);
}

void DataClass::Data_FloorMaskSlant::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskSlant::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskSlant::Data_FloorMaskSlant()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLANT;
}

Object::Object* DataClass::Data_FloorMaskSlope::genObject()
{
	return new Object::Mask::Floor::FloorMaskSlope(data, platform);
}

void DataClass::Data_FloorMaskSlope::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
	object_file.write((char*)&platform, sizeof(bool));
}

void DataClass::Data_FloorMaskSlope::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlopeData));
	object_file.read((char*)&platform, sizeof(bool));
}

DataClass::Data_FloorMaskSlope::Data_FloorMaskSlope()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::FLOOR;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLOPE;
}

Object::Object* DataClass::Data_CeilingMaskHorizontalLine::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskLine(data);
}

void DataClass::Data_CeilingMaskHorizontalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::HorizontalLineData));
}

void DataClass::Data_CeilingMaskHorizontalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::HorizontalLineData));
}

DataClass::Data_CeilingMaskHorizontalLine::Data_CeilingMaskHorizontalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_LINE;
}

Object::Object* DataClass::Data_CeilingMaskSlant::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskSlant(data);
}

void DataClass::Data_CeilingMaskSlant::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlantData));
}

void DataClass::Data_CeilingMaskSlant::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlantData));
}

DataClass::Data_CeilingMaskSlant::Data_CeilingMaskSlant()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLANT;
}

Object::Object* DataClass::Data_CeilingMaskSlope::genObject()
{
	return new Object::Mask::Ceiling::CeilingMaskSlope(data);
}

void DataClass::Data_CeilingMaskSlope::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::SlopeData));
}

void DataClass::Data_CeilingMaskSlope::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::SlopeData));
}

DataClass::Data_CeilingMaskSlope::Data_CeilingMaskSlope()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::CEILING;
	object_identifier[2] = Object::Mask::HORIZONTAL_SLOPE;
}

Object::Mask::VerticalLineData& DataClass::Data_VerticalLine::getVerticalLineData()
{
	return data;
}

int& DataClass::Data_VerticalLine::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_VerticalLine::getPosition()
{
	return data.position;
}

void DataClass::Data_VerticalLine::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.height = size;
	data.script = 0;
	data.material = 0;
}

Object::Mask::CurveData& DataClass::Data_Curve::getCurveData()
{
	return data;
}

int& DataClass::Data_Curve::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Curve::getPosition()
{
	return data.position;
}

void DataClass::Data_Curve::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size;
	data.height = size;
	data.script = 0;
	data.material = 0;
}

Object::Object* DataClass::Data_LeftMaskVerticalLine::genObject()
{
	return new Object::Mask::Left::LeftMaskLine(data);
}

void DataClass::Data_LeftMaskVerticalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

void DataClass::Data_LeftMaskVerticalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

DataClass::Data_LeftMaskVerticalLine::Data_LeftMaskVerticalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::LEFT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_LINE;
}

Object::Object* DataClass::Data_LeftMaskCurve::genObject()
{
	return new Object::Mask::Left::LeftMaskCurve(data);
}

void DataClass::Data_LeftMaskCurve::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::CurveData));
}

void DataClass::Data_LeftMaskCurve::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::CurveData));
}

DataClass::Data_LeftMaskCurve::Data_LeftMaskCurve()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::LEFT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_CURVE;
}

Object::Object* DataClass::Data_RightMaskVerticalLine::genObject()
{
	return new Object::Mask::Right::RightMaskLine(data);
}

void DataClass::Data_RightMaskVerticalLine::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

void DataClass::Data_RightMaskVerticalLine::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::VerticalLineData));
}

DataClass::Data_RightMaskVerticalLine::Data_RightMaskVerticalLine()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::RIGHT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_LINE;
}

Object::Object* DataClass::Data_RightMaskCurve::genObject()
{
	return new Object::Mask::Right::RightMaskCurve(data);
}

void DataClass::Data_RightMaskCurve::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::CurveData));
}

void DataClass::Data_RightMaskCurve::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::CurveData));
}

DataClass::Data_RightMaskCurve::Data_RightMaskCurve()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::RIGHT_WALL;
	object_identifier[2] = Object::Mask::VERTICAL_CURVE;
}

Object::Object* DataClass::Data_TriggerMask::genObject()
{
	return new Object::Mask::Trigger::TriggerMask(data);
}

void DataClass::Data_TriggerMask::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&data, sizeof(Object::Mask::Trigger::TriggerData));
}

void DataClass::Data_TriggerMask::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&data, sizeof(Object::Mask::Trigger::TriggerData));
}

DataClass::Data_TriggerMask::Data_TriggerMask()
{
	// Set Object Identifier
	object_identifier[0] = Object::MASK;
	object_identifier[1] = Object::Mask::TRIGGER;
	object_identifier[2] = 0;
}

Object::Mask::Trigger::TriggerData& DataClass::Data_TriggerMask::getTriggerData()
{
	return data;
}

int& DataClass::Data_TriggerMask::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_TriggerMask::getPosition()
{
	return data.position;
}

void DataClass::Data_TriggerMask::generateInitialValues(glm::vec2& position, float& size)
{
	data.position = position;
	data.width = size * 2.0f;
	data.height = size;
	data.check_type = Object::Mask::Trigger::CHECK_TYPE::NONE;
	data.script = 0;
}

Shape::Shape* DataClass::Data_Shape::getShape()
{
	return shape;
}

Object::Object* DataClass::Data_Terrain::genObject()
{
	switch (object_identifier[1])
	{
	case Object::Terrain::BACKDROP: return new Object::Terrain::Backdrop(shape, data); break;
	case Object::Terrain::BACKGROUND_3:
	case Object::Terrain::BACKGROUND_2: 
	case Object::Terrain::BACKGROUND_1: return new Object::Terrain::Background(shape, data, layer); break;
	case Object::Terrain::FOREGROUND: return new Object::Terrain::Foreground(shape, data); break;
	default: return new Object::Terrain::Formerground(shape, data);
	}
}

void DataClass::Data_Terrain::writeObjectData(std::ofstream& object_file)
{
	shape->writeShape(object_file);
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	if (testIfBackground())
		object_file.write((char*)&layer, sizeof(uint8_t));
}

void DataClass::Data_Terrain::readObjectData(std::ifstream& object_file)
{
	shape = shapes[object_identifier[2]](object_file);
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	if (testIfBackground())
		object_file.read((char*)&layer, sizeof(uint8_t));

}

bool DataClass::Data_Terrain::testIfBackground()
{
	return object_identifier[1] == Object::Terrain::BACKGROUND_1 ||
		object_identifier[1] == Object::Terrain::BACKGROUND_2 ||
		object_identifier[1] == Object::Terrain::BACKGROUND_3;
}

DataClass::Data_Terrain::Data_Terrain(uint8_t layer_identifier, uint8_t shape_identifier)
{
	// Set Object Identifier
	object_identifier[0] = Object::TERRAIN;
	object_identifier[1] = layer_identifier;
	object_identifier[2] = shape_identifier;
}

void DataClass::Data_Terrain::generateInitialValues(glm::vec2& position, glm::vec4 color, Shape::Shape* new_shape)
{
	data.position = position;
	data.zpos = -1.0f;
	data.colors = color;
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	shape = new_shape;
}

Object::Light::LightData& DataClass::Data_Light::getLightData()
{
	return light_data;
}

int& DataClass::Data_Light::getScript()
{
	return light_data.script;
}

glm::vec2& DataClass::Data_Light::getPosition()
{
	return light_data.position;
}

void DataClass::Data_Light::generateInitialLightValues(glm::vec2& new_position)
{
	light_data.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.intensity = 1.0f;
}

Object::Object* DataClass::Data_Directional::genObject()
{
	return new Object::Light::Directional::Directional(directional, light_data);
}

void DataClass::Data_Directional::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&directional, sizeof(Object::Light::Directional::DirectionalData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Directional::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&directional, sizeof(Object::Light::Directional::DirectionalData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Directional::Data_Directional()
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::DIRECTIONAL;
	object_identifier[2] = 0;
}

Object::Light::Directional::DirectionalData& DataClass::Data_Directional::getDirectionalData()
{
	return directional;
}

void DataClass::Data_Directional::generateInitialValues(glm::vec2& position, float& size)
{
	generateInitialLightValues(position);
	directional.position2 = position + glm::vec2(size, 0.0f);
}

Object::Object* DataClass::Data_Point::genObject()
{
	return new Object::Light::Point::Point(point, light_data);
}

void DataClass::Data_Point::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&point, sizeof(Object::Light::Point::PointData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Point::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&point, sizeof(Object::Light::Point::PointData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Point::Data_Point()
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::POINT;
	object_identifier[2] = 0;
}

Object::Light::Point::PointData& DataClass::Data_Point::getPointData()
{
	return point;
}

void DataClass::Data_Point::generateInitialValues(glm::vec2& position)
{
	generateInitialLightValues(position);
	point.linear = DEFAULT_LINEAR;
	point.quadratic = DEFAULT_QUADRATIC;
}

Object::Object* DataClass::Data_Spot::genObject()
{
	return new Object::Light::Spot::Spot(spot, light_data);
}

void DataClass::Data_Spot::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&spot, sizeof(Object::Light::Spot::SpotData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Spot::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&spot, sizeof(Object::Light::Spot::SpotData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Spot::Data_Spot()
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::SPOT;
	object_identifier[2] = 0;
}

Object::Light::Spot::SpotData& DataClass::Data_Spot::getSpotData()
{
	return spot;
}

void DataClass::Data_Spot::generateInitialValues(glm::vec2& position)
{
	generateInitialLightValues(position);
	spot.direction = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	spot.angle1 = 0.2f;
	spot.angle2 = 0.5f;
	spot.linear = DEFAULT_LINEAR;
	spot.quadratic = DEFAULT_QUADRATIC;
}

Object::Object* DataClass::Data_Beam::genObject()
{
	return new Object::Light::Beam::Beam(beam, light_data);
}

void DataClass::Data_Beam::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&beam, sizeof(Object::Light::Beam::BeamData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Beam::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&beam, sizeof(Object::Light::Beam::BeamData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Beam::Data_Beam()
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::BEAM;
	object_identifier[2] = 0;
}

Object::Light::Beam::BeamData& DataClass::Data_Beam::getBeamData()
{
	return beam;
}

void DataClass::Data_Beam::generateInitialValues(glm::vec2& position, float& size)
{
	generateInitialLightValues(position);
	beam.position2 = position + glm::vec2(size, 0.0f);
	beam.linear = DEFAULT_LINEAR;
	beam.quadratic = DEFAULT_QUADRATIC;
}

uint32_t& DataClass::Data_UUID::getUUID()
{
	return uuid;
}

void DataClass::Data_UUID::generateUUID(uint32_t& new_uuid)
{
	uuid = new_uuid;
}

Object::Object* DataClass::Data_RigidBody::genObject()
{
	switch (object_identifier[2])
	{
	case Shape::TRAPEZOID: return new Object::Physics::Rigid::RigidTrapezoid(uuid, data, rigid, shape);
	case Shape::TRIANGLE: return new Object::Physics::Rigid::RigidTriangle(uuid, data, rigid, shape);
	case Shape::CIRCLE: return new Object::Physics::Rigid::RigidCircle(uuid, data, rigid, shape);
	case Shape::POLYGON: return new Object::Physics::Rigid::RigidPolygon(uuid, data, rigid, shape);
	default: return new Object::Physics::Rigid::RigidRectangle(uuid, data, rigid, shape);
	}
}

void DataClass::Data_RigidBody::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&rigid, sizeof(Object::Physics::Rigid::RigidBodyData));
	shape->writeShape(object_file);
}

void DataClass::Data_RigidBody::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&rigid, sizeof(Object::Physics::Rigid::RigidBodyData));
	shape = shapes[object_identifier[2]](object_file);
}

DataClass::Data_RigidBody::Data_RigidBody(uint8_t shape_identifier)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::RIGID_BODY;
	object_identifier[2] = shape_identifier;
}

Object::Physics::Rigid::RigidBodyData& DataClass::Data_RigidBody::getRigidData()
{
	return rigid;
}

void DataClass::Data_RigidBody::generateInitialValues(glm::vec2& position, Shape::Shape* new_shape, uint32_t& new_uuid)
{
	// Generate Object Data
	generateUUID(new_uuid);
	data.position = position;
	data.zpos = -1.0f;
	data.colors = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;

	// Generate Rigid Body Data
	rigid.mass = 1.0f;
	rigid.max_health = 10.0f;
	rigid.center_of_gravity = glm::vec2(0.0f, 0.0f);
	rigid.initial_rotation = 0.0f;
	rigid.fluid = false;

	// Store Shape
	shape = new_shape;
}

Object::Object* DataClass::Data_SpringMass::genObject()
{
	return new Object::Physics::Soft::SpringMass(uuid, data, file_name);
}

void DataClass::Data_SpringMass::writeObjectData(std::ofstream& object_file)
{
	uint16_t file_name_size = file_name.size();
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&file_name_size, sizeof(uint16_t));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write(file_name.c_str(), file_name_size);
}

void DataClass::Data_SpringMass::readObjectData(std::ifstream& object_file)
{
	uint16_t file_name_size;
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&file_name_size, sizeof(uint16_t));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	file_name.resize(file_name_size);
	object_file.read(&file_name[0], file_name_size);
}

DataClass::Data_SpringMass::Data_SpringMass()
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS;
}

std::string& DataClass::Data_SpringMass::getFile()
{
	return file_name;
}

void DataClass::Data_SpringMass::generateInitialValues(glm::vec2& position, uint32_t& new_uuid)
{
	generateUUID(new_uuid);
	data.position = position;
	data.zpos = -1.0f;
	data.colors = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	file_name = "NULL";
}

Object::Object* DataClass::Data_Wire::genObject()
{
	return new Object::Physics::Soft::Wire(uuid, data, wire);
}

void DataClass::Data_Wire::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&wire, sizeof(Object::Physics::Soft::WireData));
}

void DataClass::Data_Wire::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&wire, sizeof(Object::Physics::Soft::WireData));
}

DataClass::Data_Wire::Data_Wire()
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::SOFT_BODY;
	object_identifier[2] = (uint8_t)Object::Physics::SOFT_BODY_TYPES::WIRE;
}

void DataClass::Data_Wire::generateInitialValues(glm::vec2& position, float& size, uint32_t& new_uuid)
{
	generateUUID(new_uuid);
	data.position = position;
	data.zpos = -1.0f;
	data.colors = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
	data.normals = glm::vec3(0.0f, 0.0f, 1.0f);
	data.texture_name = 0;
	data.script = 0;
	data.material_name = 0;
	wire.position2 = position + glm::vec2(size, 0.0f);
	wire.Mass = 1.0f;
	wire.Health = 10.0f;
	wire.Radius = 0.2f;
	wire.node_instances = 5;
	wire.total_rest_length = size;
	wire.break_distance = size * 2.0f;
	wire.Stiffness = 1.0f;
	wire.Dampening = 1.0f;
}

Object::Physics::Soft::WireData& DataClass::Data_Wire::getWireData()
{
	return wire;
}

Object::Object* DataClass::Data_Anchor::genObject()
{
	return new Object::Physics::Hinge::Anchor(uuid, data);
}

void DataClass::Data_Anchor::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
}

void DataClass::Data_Anchor::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
}

DataClass::Data_Anchor::Data_Anchor()
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::HINGE_BASE;
	object_identifier[2] = (uint8_t)Object::Physics::HINGES::ANCHOR;
}

int& DataClass::Data_Anchor::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Anchor::getPosition()
{
	return data.position;
}

void DataClass::Data_Anchor::generateInitialValues(glm::vec2& position, uint32_t& new_uuid)
{
	generateUUID(new_uuid);
	data.position = position;
	data.script = 0;
}

Object::Physics::Hinge::AnchorData& DataClass::Data_Anchor::getAnchorData()
{
	return data;
}

Object::Object* DataClass::Data_Hinge::genObject()
{
	return new Object::Physics::Hinge::Hinge(uuid, data, file_name);
}

void DataClass::Data_Hinge::writeObjectData(std::ofstream& object_file)
{
	uint16_t file_name_size = file_name.size();
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&file_name_size, sizeof(uint16_t));
	object_file.write((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
	object_file.write(file_name.c_str(), file_name_size);
}

void DataClass::Data_Hinge::readObjectData(std::ifstream& object_file)
{
	uint16_t file_name_size;
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&file_name_size, sizeof(uint16_t));
	object_file.read((char*)&data, sizeof(Object::Physics::Hinge::HingeData));
	file_name.resize(file_name_size);
	object_file.read(&file_name[0], file_name_size);
}

DataClass::Data_Hinge::Data_Hinge()
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::HINGE_BASE;
	object_identifier[2] = (uint8_t)Object::Physics::HINGES::HINGE;
}

int& DataClass::Data_Hinge::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Hinge::getPosition()
{
	return data.position;
}

void DataClass::Data_Hinge::generateInitialValues(glm::vec2& position, uint32_t& new_uuid)
{
	generateUUID(new_uuid);
	data.position = position;
	data.script = 0;
	file_name = "NULL";
}

Object::Physics::Hinge::HingeData& DataClass::Data_Hinge::getHingeData()
{
	return data;
}

std::string& DataClass::Data_Hinge::getFile()
{
	return file_name;
}

Object::Entity::EntityData& DataClass::Data_Entity::getEntityData()
{
	return entity;
}

void DataClass::Data_Entity::generateInitialData(glm::vec2& position, uint32_t& new_uuid)
{
	generateUUID(new_uuid);
	data.position = position;
	entity.half_width = 2.0f;
	entity.half_height = 2.0f;
	entity.half_collision_width = 2.0f;
	entity.half_collision_height = 2.0f;
	data.script = 0;
}

Object::Object* DataClass::Data_NPC::genObject()
{
	return new Object::Entity::NPC(uuid, entity, data, ai);
}

void DataClass::Data_NPC::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&ai, sizeof(uint16_t));
}

void DataClass::Data_NPC::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&ai, sizeof(uint16_t));
}

DataClass::Data_NPC::Data_NPC()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_NPC;
	object_identifier[2] = 0;
}

Object::Object* DataClass::Data_Controllable::genObject()
{
	return new Object::Entity::Controllables(uuid, entity, data);
}

void DataClass::Data_Controllable::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
}

void DataClass::Data_Controllable::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
}

DataClass::Data_Controllable::Data_Controllable()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_CONTROLLABLE;
	object_identifier[2] = 0;
}

Object::Object* DataClass::Data_Interactable::genObject()
{
	return new Object::Entity::Interactables(uuid, entity, data, interactable);
}

void DataClass::Data_Interactable::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&interactable, sizeof(Object::Entity::InteractableData));
}

void DataClass::Data_Interactable::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&interactable, sizeof(Object::Entity::InteractableData));
}

DataClass::Data_Interactable::Data_Interactable()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_INTERACTABLE;
	object_identifier[2] = 0;
}

Object::Object* DataClass::Data_Dynamic::genObject()
{
	return new Object::Entity::Dynamics(uuid, entity, data, dynamic);
}

void DataClass::Data_Dynamic::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.write((char*)&data, sizeof(Object::ObjectData));
	object_file.write((char*)&dynamic, sizeof(Object::Entity::DynamicData));
}

void DataClass::Data_Dynamic::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&uuid, sizeof(uint32_t));
	object_file.read((char*)&entity, sizeof(Object::Entity::EntityData));
	object_file.read((char*)&data, sizeof(Object::ObjectData));
	object_file.read((char*)&dynamic, sizeof(Object::Entity::DynamicData));
}

DataClass::Data_Dynamic::Data_Dynamic()
{
	// Set Object Identifier
	object_identifier[0] = Object::ENTITY;
	object_identifier[1] = Object::Entity::ENTITY_DYNAMIC;
	object_identifier[2] = 0;
}
