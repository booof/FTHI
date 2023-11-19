#include "RigidBody.h"
#include "Render/Struct/DataClasses.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Shapes
#include "Render/Shape/Rectangle.h"
#include "Render/Shape/Trapezoid.h"
#include "Render/Shape/Triangle.h"
#include "Render/Shape/Circle.h"
#include "Render/Shape/Polygon.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Source/Algorithms/Quick Math/QuickMath.h"

#include "RigidRectangle.h"
#include "RigidTrapezoid.h"
#include "RigidTriangle.h"
#include "RigidCircle.h"
#include "RigidPolygon.h"

// Compare Values of Named Nodes
bool operator==(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2)
{
	return node1.value == node2.value;
}

// Compare Values of Named Node
bool operator>(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2)
{
	return node1.value > node2.value;
}

// Compare Values of Named Nodes
bool operator<(const Object::Physics::Rigid::Named_Node node1, const Object::Physics::Rigid::Named_Node node2)
{
	return node1.value < node2.value;
}

// Compare Values of Named Nodes
bool operator==(const Object::Physics::Rigid::Named_Node node1, const float value)
{
	return node1.value == value;
}

// Compare Values of Named Node
bool operator>(const Object::Physics::Rigid::Named_Node node1, const float value)
{
	return node1.value > value;
}

// Compare Values of Named Nodes
bool operator<(const Object::Physics::Rigid::Named_Node node1, const float value)
{
	return node1.value < value;
}

// Compare Values of Named Nodes
bool operator==(const float value, const Object::Physics::Rigid::Named_Node node1)
{
	return node1.value == value;
}

// Compare Values of Named Node
bool operator>(const float value, const Object::Physics::Rigid::Named_Node node1)
{
	return value > node1.value;
}

// Compare Values of Named Nodes
bool operator<(const float value, const Object::Physics::Rigid::Named_Node node1)
{
	return value < node1.value;
}

Object::Physics::Rigid::RigidBody::RigidBody(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, int vertex_count, glm::vec2& offset)
{
	// Store Object Data
	data = data_;
	shape = shape_;
	rigid = rigid_;
	uuid = uuid_;
	data.position += offset;

	// Store Initial Position
	physics.Position = data.position;

	// Store Storage Type
	storage_type = PHYSICS_COUNT;

	// Store Physics Base
	base = PHYSICS_BASES::RIGID_BODY;

	// Generate Vertex Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Object
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Allocate Buffer Memory
	glBufferData(GL_ARRAY_BUFFER, vertex_count, NULL, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Enable Instance
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// Unbind Vertex Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Object::Physics::Rigid::RigidBody::~RigidBody()
{
	// Delete the Shape
	delete shape;
}

void Object::Physics::Rigid::RigidBody::initializeVertices(int & instance_, int & instance_index_)
{
	//model = glm::translate(glm::mat4(1.0f), glm::vec3(physics.Position.x, physics.Position.y, 0.0f));

	// Store Instance
	instance = instance_;

	// Assign Vertices
	int dummyvar = 0;
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	shape->initializeVertices(data, dummyvar, instance_index_);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	data.zpos = -1.0f;

	// Assign Instance
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, instance, 64, glm::value_ptr(model));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 64, 16, glm::value_ptr(data.colors));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 80, 12, glm::value_ptr(data.normals));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 92, 4, &data.texture_name);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 96, 4, &data.material_name);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)instance + 100, 4, &data.zpos);

	// Increment Instance
	instance_ += Constant::INSTANCE_SIZE;
	instance_index_++;
}

void Object::Physics::Rigid::RigidBody::updateObject()
{
	// Caulculate Potential Energy
	calcPotentialEnergy();

	// Apply Forces
	applyForces();

	// Reset Some Variables
	possible_rotation_vertices.clear();

	// Disable Grounding
	physics.grounded = false;

	// Apply Gravity
	applyGravity();

	// Prepare for Collisions
	prepCollisions();

	// Find Extreme Vertices
	findExtremeVertices();
}

void Object::Physics::Rigid::RigidBody::applyGravity()
{
	// Apply force of weight
	physics.Forces.y -= physics.Mass * Constant::GRAVITATIONAL_ACCELERATION;

	// Apply terminal velocity if needed
	if (!physics.grounded && physics.Velocity.y <= -Constant::TERMINALVELOCITY)
	{
		physics.Velocity.y = -Constant::TERMINALVELOCITY;
	}
}

void Object::Physics::Rigid::RigidBody::calculateIAR()
{
	physics.IAR = physics.Rotation_Vertex + physics.Position;
}

void Object::Physics::Rigid::RigidBody::applyForces()
{
	// Get Acceleration
	glm::vec2 Acceleration = physics.Forces / physics.Mass;
	physics.Forces = glm::vec2(0, 0);

	// Perform Kinematic Calculations
	physics.Velocity += Acceleration * Global::deltaTime;
	physics.Position += physics.Velocity * Global::deltaTime;

	// Get Instantaneous Axis of Rotation
	calculateIAR();

	// Get Angular Acceleration
	float angular_acceleration = physics.torque / calcMomentOfInertia();
	physics.torque = 0;

	// Perform Rotation
	physics.rotation_velocity += angular_acceleration * Global::deltaTime;
	physics.rotation += physics.rotation_velocity * Global::deltaTime;

	// Calculate Rotations
	if (physics.Rotation_Vertex != glm::vec2(0.0f, 0.0f))
	{
		float distance = glm::distance(physics.Rotation_Vertex, glm::vec2(0.0f));
		float rotation_angle = Algorithms::Math::angle_from_vector(physics.Rotation_Vertex);
		rotation_angle += physics.rotation_velocity * Global::deltaTime;
		physics.Position = physics.IAR - distance * Algorithms::Math::parameterizeAngle(rotation_angle);
	}

	// Calculate New Center of Mass
	if (!physics.fluid)
	{
		float angle_offset = physics.Center_of_Mass_Angle + physics.rotation;
		physics.Center_of_Mass = glm::vec2(cos(angle_offset), sin(angle_offset)) * physics.Center_of_Mass_Distance;
	}

	// Clamp Rotation
	while (physics.rotation < 0) { physics.rotation += 6.2832f; }
	while (physics.rotation > 6.2832f) { physics.rotation -= 6.2832f; }
	//if (physics.rotation_velocity < 0.01f)
	//	physics.rotation_velocity = 0.0f;
	physics.rotation_velocity *= 0.995f;
}

void Object::Physics::Rigid::RigidBody::drawObject()
{
	// Bind Model Matrix
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, instance, 64, glm::value_ptr(model));

	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
	glBindVertexArray(0);
}

glm::vec2* Object::Physics::Rigid::RigidBody::pointerToPosition()
{
	return &physics.Position;
}

bool Object::Physics::Rigid::RigidBody::testMouseCollisions(float x, float y)
{
	return shape->testMouseCollisions(x, y, data.position.x, data.position.y);
}

glm::vec2 Object::Physics::Rigid::RigidBody::returnPosition()
{
	return data.position;
}

Object::Object* DataClass::Data_RigidBody::genObject(glm::vec2& offset)
{
	Shape::Shape* shape_copy = shape->makeCopy();
	switch (object_identifier[2])
	{
	case Shape::TRAPEZOID: return new Object::Physics::Rigid::RigidTrapezoid(uuid, data, rigid, shape_copy, offset);
	case Shape::TRIANGLE: return new Object::Physics::Rigid::RigidTriangle(uuid, data, rigid, shape_copy, offset);
	case Shape::CIRCLE: return new Object::Physics::Rigid::RigidCircle(uuid, data, rigid, shape_copy, offset);
	case Shape::POLYGON: return new Object::Physics::Rigid::RigidPolygon(uuid, data, rigid, shape_copy, offset);
	default: return new Object::Physics::Rigid::RigidRectangle(uuid, data, rigid, shape_copy, offset);
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
	shape = readNewShape(object_file, object_identifier[2]);
}

DataClass::Data_RigidBody::Data_RigidBody(uint8_t shape_identifier, uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::RIGID_BODY;
	object_identifier[2] = shape_identifier;
	object_identifier[3] = children_size;
}

void DataClass::Data_RigidBody::info(Editor::ObjectInfo& object_info)
{
	// Map to Shape Names
	static std::string shape_name_map[] = { "Rectangle", "Trapezoid", "Triangle", "Circle", "Polygon" };

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Rigid Body", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addTextValue("Shape: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &shape_name_map[shape->shape], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
	shape->selectInfo(object_info);
	object_info.addColorValue("Color: ", glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), &data.colors, true);
}

DataClass::Data_Object* DataClass::Data_RigidBody::makeCopy()
{
	Data_RigidBody* new_rigid_body = new Data_RigidBody(*this);
	new_rigid_body->shape = new_rigid_body->shape->makeCopy();
	return new_rigid_body;
}

Object::Physics::Rigid::RigidBodyData& DataClass::Data_RigidBody::getRigidData()
{
	return rigid;
}

void DataClass::Data_RigidBody::generateInitialValues(glm::vec2& position, Shape::Shape* new_shape)
{
	// Generate Object Data
	generateUUID();
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
