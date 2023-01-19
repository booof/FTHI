#include "RigidBody.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Shapes
#include "Render/Shape/Rectangle.h"
#include "Render/Shape/Trapezoid.h"
#include "Render/Shape/Triangle.h"
#include "Render/Shape/Circle.h"
#include "Render/Shape/Polygon.h"

// Selector
#include "Render/Editor/Selector.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

#include "Source/Algorithms/Quick Math/QuickMath.h"

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

Object::Physics::Rigid::RigidBody::RigidBody(uint32_t& uuid_, ObjectData& data_, RigidBodyData& rigid_, Shape::Shape* shape_, int vertex_count)
{
	// Store Object Data
	data = data_;
	shape = shape_;
	rigid = rigid_;
	uuid = uuid_;

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

void Object::Physics::Rigid::RigidBody::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(PHYSICS);
	object_file.put((uint8_t)PHYSICS_BASES::RIGID_BODY);
	object_file.put(shape->shape);

	// Write Data
	object_file.write((char*)&uuid, sizeof(uint32_t));
	object_file.write((char*)&data, sizeof(ObjectData));
	object_file.write((char*)&rigid, sizeof(RigidBodyData));

	// Write Shape
	switch (shape->shape)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		Shape::Rectangle* temp_rect = static_cast<Shape::Rectangle*>(shape);
		object_file.write((char*)(temp_rect)+8, sizeof(Shape::Rectangle) - 8);
		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		Shape::Trapezoid* temp_trap = static_cast<Shape::Trapezoid*>(shape);
		object_file.write((char*)(temp_trap)+8, sizeof(Shape::Trapezoid) - 8);
		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		Shape::Triangle* temp_tri = static_cast<Shape::Triangle*>(shape);
		object_file.write((char*)(temp_tri)+8, sizeof(Shape::Triangle) - 8);
		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		Shape::Circle* temp_circle = static_cast<Shape::Circle*>(shape);
		object_file.write((char*)(temp_circle)+8, sizeof(Shape::Circle) - 8);
		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		Shape::Polygon* temp_poly = static_cast<Shape::Polygon*>(shape);
		object_file.write((char*)(temp_poly)+8, sizeof(Shape::Polygon) - 8);
		break;
	}

	}

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

void Object::Physics::Rigid::RigidBody::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Information
	info(object_info, name, data, shape);

	// Selector Helper
	select2(selector);
}

bool Object::Physics::Rigid::RigidBody::testMouseCollisions(float x, float y)
{
	return shape->testMouseCollisions(x, y, data.position.x, data.position.y);
}

glm::vec2 Object::Physics::Rigid::RigidBody::returnPosition()
{
	return data.position;
}

void Object::Physics::Rigid::RigidBody::info(Editor::ObjectInfo& object_info, std::string& name, ObjectData& data, Shape::Shape* shape)
{
	// Map to Shape Names
	static std::string shape_name_map[] = { "Rectangle", "Trapezoid", "Triangle", "Circle", "Polygon" };

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Rigid Body", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addTextValue("Shape: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &shape_name_map[shape->shape], glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	shape->selectInfo(object_info);
}
