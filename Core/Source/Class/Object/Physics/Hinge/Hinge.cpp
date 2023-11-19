#include "Hinge.h"
#include "Render/Struct/DataClasses.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Math Functions
#include "Source/Algorithms/Common/Common.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"

// Other Physics Objects
#include "Object/Physics/RigidBody/RigidRectangle.h"
#include "Object/Physics/RigidBody/RigidTrapezoid.h"
#include "Object/Physics/RigidBody/RigidTriangle.h"
#include "Object/Physics/RigidBody/RigidCircle.h"
#include "Object/Physics/RigidBody/RigidPolygon.h"
#include "Object/Physics/SoftBody/SpringMass.h"
#include "Object/Physics/SoftBody/Wire.h"
#include "Object/Physics/Hinge/Anchor.h"

// Shapes
#include "Render/Shape/Rectangle.h"
#include "Render/Shape/Trapezoid.h"
#include "Render/Shape/Triangle.h"
#include "Render/Shape/Circle.h"
#include "Render/Shape/Polygon.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Visualizer/Visualizer.h"

// Object Info
#include "Render/Editor/ObjectInfo.h"

// Shader
#include "Render/Shader/Shader.h"

Object::Physics::Hinge::Hinge::Hinge(uint32_t& uuid_, HingeData& data_, std::string& file_name_, glm::vec2& offset)
{
	// Store Object Type
	type = PHYSICS_TYPES::TYPE_HINGE;

	// Store Data
	data = data_;
	file_name = file_name_;
	initial_position = data.position;
	path = "../Resources/Models/Hinges/" + file_name;
	Axis_of_Rotation = initial_position;
	uuid = uuid_;
	data.position += offset;

	// Store Storage Type
	storage_type = PHYSICS_COUNT;

	// Store Physics Base
	base = PHYSICS_BASES::HINGE_BASE;

	// Create Visualizer if Needed
	initializeVisualizer();

#ifdef EDITOR

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif

}

Object::Physics::Hinge::Hinge::~Hinge()
{
	// Only Delete Objects if Initialized
	if (initialized)
	{
		delete[] objects;
		delete[] limbs;
		delete[] children;
	}
}

void Object::Physics::Hinge::Hinge::updateObject()
{
	glm::vec2 total_pos = glm::vec2(0.0f, 0.0f);
	float total_y_pos_grounded = 0.0f;
	int grounded_count = 0;

	// Update Objects
	for (int i = 0; i < number_of_objects; i++)
	{
		objects[i]->updateObject();
	}

	// Determine Respective Position of Object
	for (int i = 0; i < number_of_objects; i++)
	{
		getHingePosFromObject(*objects[i], *limbs[i], total_pos, total_y_pos_grounded, grounded_count);
	}

	// Perform Rotation
	rotate();

	// Move Hinge To Average Position of Objects
	if (grounded_count)
	{
		initial_position.x = total_pos.x / (float)number_of_objects;
		initial_position.y = total_y_pos_grounded / (float)grounded_count;
	}

	else
	{
		initial_position = total_pos / (float)number_of_objects;
	}


	// Make Objects Stay In Bounds of Hinge
	for (int i = 0; i < number_of_objects; i++)
	{
		// Move Object
		getObjectPosFromHinge(*objects[i], *limbs[i], grounded_count != 0);
	}

	// Update Children
	for (int i = 0; i < number_of_children; i++)
	{
		children[i]->updateObject();
	}
}

void Object::Physics::Hinge::Hinge::drawObject()
{

#ifdef EDITOR

	// Draw Visualizers if Enabled
	if (Global::editing)
	{
		// Bind Static Color Shader
		Global::colorShaderStatic.Use();

		// Send Model Matrix
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Draw Central Object Visualizer
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// Draw Objects and Connectors to Objects
		for (int i = 0; i < number_of_objects; i++)
		{
			// Draw Visualizer
			if (limb_to_position)
			{
				Vertices::Visualizer::visualizeLine(glm::vec2(data.position.x, data.position.y), glm::vec2(objects[i]->returnPosition().x, objects[i]->returnPosition().y), 0.2f, glm::vec4(0.0f, 0.0f, 0.4f, 1.0f));
			}

			else
			{
				Vertices::Visualizer::visualizeLine(data.position, data.position + limbs[i]->initial_distance * Algorithms::Math::parameterizeAngle(limbs[i]->initial_angle + rotation), 0.2f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			}
		}
		
		// Return to Object Shader
		Global::objectShader.Use();
	}

#endif

	for (int i = 0; i < number_of_objects; i++)
	{
		objects[i]->drawObject();
	}

	for (int i = 0; i < number_of_children; i++)
	{
		children[i]->drawObject();
	}
}

bool Object::Physics::Hinge::Hinge::Test_Mouse_Collisions()
{
	return false;
}

void Object::Physics::Hinge::Hinge::readFile()
{
	// If NULL, dont read
	if (file_name == "NULL")
		return;

	// If Already Initialized, Delete Currently Initialized Objects
	if (initialized)
	{
		delete[] objects;
		delete[] limbs;
		delete[] children;
	}

	// Open File
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);

#ifdef EDITOR

	// Open Editor File
	std::ifstream editor_file;
	editor_file.open(path + ".edit", std::ios::binary);
	uint16_t temp_name_size = 0;

#endif

	// If File is Empty, Return
	if (file.eof())
		return;

	// Read Header
	file.read((char*)&number_of_objects, sizeof(int));
	file.read((char*)&number_of_children, sizeof(int));

	// Allocate Data
	objects = new PhysicsBase*[number_of_objects];
	limbs = new Limb*[number_of_objects];
	children = new Hinge*[number_of_children];

	// File Reading Data
	uint32_t temp_uuid = 0;
	uint16_t temp_object_index = 0;
	uint16_t temp_hinge_index = 0;
	uint8_t temp_identifier[3] = { 0 };
	ObjectData temp_object_data;
	Rigid::RigidBodyData temp_rigid_data;
	Soft::WireData temp_wire_data;
	AnchorData temp_anchor_data;
	HingeData temp_hinge_data;
	int temp_file_name_size = 0;
	std::string temp_file_name = "";
	glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);

	// Iterate Through File Until All is Read
	while (!file.eof())
	{
		// Read Object Identifier
		file.read((char*)&temp_identifier, 3);

		// Read UUID
		file.read((char*)&temp_uuid, sizeof(int));

		// Generate Object
		switch (temp_identifier[1])
		{
		// Rigid Body
		case (int)PHYSICS_BASES::RIGID_BODY:
		{
			switch (temp_identifier[2])
			{
			// Object Rectangle
			case Shape::SHAPES::RECTANGLE:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_rigid_data, sizeof(Rigid::RigidBodyData));
				Shape::Rectangle* new_rect = new Shape::Rectangle;
				file.read((char*)(new_rect)+8, sizeof(Shape::Rectangle) - 8);
				objects[temp_object_index] = new Rigid::RigidRectangle(temp_uuid, temp_object_data, temp_rigid_data, new_rect, null_vec);
				temp_object_index++;
				break;
			}
			// Object Trapezoid
			case Shape::SHAPES::TRAPEZOID:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_rigid_data, sizeof(Rigid::RigidBodyData));
				Shape::Trapezoid* new_trap = new Shape::Trapezoid;
				file.read((char*)(new_trap)+8, sizeof(Shape::Trapezoid) - 8);
				objects[temp_object_index] = new Rigid::RigidTrapezoid(temp_uuid, temp_object_data, temp_rigid_data, new_trap, null_vec);
				temp_object_index++;
				break;
			}
			// Object Triangle
			case Shape::SHAPES::TRIANGLE:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_rigid_data, sizeof(Rigid::RigidBodyData));
				Shape::Triangle* new_tri = new Shape::Triangle;
				file.read((char*)(new_tri)+8, sizeof(Shape::Triangle) - 8);
				objects[temp_object_index] = new Rigid::RigidTriangle(temp_uuid, temp_object_data, temp_rigid_data, new_tri, null_vec);
				temp_object_index++;
				break;
			}
			// Object Circle
			case Shape::SHAPES::CIRCLE:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_rigid_data, sizeof(Rigid::RigidBodyData));
				Shape::Circle* new_circle = new Shape::Circle;
				file.read((char*)(new_circle)+8, sizeof(Shape::Circle) - 8);
				objects[temp_object_index] = new Rigid::RigidCircle(temp_uuid, temp_object_data, temp_rigid_data, new_circle, null_vec);
				temp_object_index++;
				break;
			}
			// Object Polygon
			case Shape::SHAPES::POLYGON:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_rigid_data, sizeof(Rigid::RigidBodyData));
				Shape::Polygon* new_poly = new Shape::Polygon;
				file.read((char*)(new_poly)+8, sizeof(Shape::Polygon) - 8);
				objects[temp_object_index] = new Rigid::RigidPolygon(temp_uuid, temp_object_data, temp_rigid_data, new_poly, null_vec);
				temp_object_index++;
				break;
			}
			}

			break;
		}

		// Soft Body
		case (int)PHYSICS_BASES::SOFT_BODY:
		{
			switch (temp_identifier[2])
			{
			// Object Soft Body
			case (int)SOFT_BODY_TYPES::SPRING_MASS:
			{
				file.read((char*)&temp_file_name_size, sizeof(uint16_t));
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				temp_file_name.resize(temp_file_name_size);
				file.read((char*)&file_name[0], temp_file_name_size);
				//objects[temp_object_index] = new Soft::SpringMass(temp_uuid, temp_object_data, temp_file_name);
				temp_object_index++;
				break;
			}
			// Object Wire
			case (int)SOFT_BODY_TYPES::WIRE:
			{
				file.read((char*)&temp_object_data, sizeof(ObjectData));
				file.read((char*)&temp_wire_data, sizeof(Soft::WireData));
				objects[temp_object_index] = new Soft::Wire(temp_uuid, temp_object_data, temp_wire_data, null_vec);
				temp_object_index++;
				break;
			}
			}

			break;
		}

		// Hinge Base
		case (int)PHYSICS_BASES::HINGE_BASE:
		{
			switch (temp_identifier[2])
			{
			// Object Anchor
			case (int)HINGES::ANCHOR:
			{
				file.read((char*)&temp_anchor_data, sizeof(AnchorData));
				objects[temp_object_index] = new Anchor(temp_uuid, temp_anchor_data, null_vec);
				temp_object_index++;
				break;
			}
			// Object Hinge
			case (int)HINGES::HINGE:
			{
				file.read((char*)&temp_file_name_size, sizeof(uint16_t));
				file.read((char*)&temp_hinge_data, sizeof(HingeData));
				temp_file_name.resize(temp_file_name_size);
				file.read((char*)&file_name[0], temp_file_name_size);
				children[temp_hinge_index] = new Hinge(temp_uuid, temp_hinge_data, file_name, null_vec);
				temp_hinge_index++;
				break;
			}
			}

			break;
		}
		}

#ifdef EDITOR

		// Read Name Size
		editor_file.read((char*)&temp_name_size, sizeof(short));

		// Read Editor File for Hinge
		if (temp_identifier[1] == (uint8_t)PHYSICS_BASES::HINGE_BASE && temp_identifier[2] == (uint8_t)HINGES::HINGE)
		{
			Hinge& temp_hinge = *children[temp_hinge_index - 1];
			temp_hinge.name.resize(temp_name_size);
			editor_file.read((char*)&temp_hinge.clamp, sizeof(bool));
			editor_file.read((char*)&temp_hinge.lock, sizeof(bool));
			editor_file.read((char*)&temp_hinge.name[0], temp_name_size);
			temp_hinge.object_index = Global::object_index_counter;
		}

		// Read Editor File for Object
		else
		{
			PhysicsBase& temp_object = *objects[temp_hinge_index - 1];
			temp_object.name.resize(temp_name_size);
			editor_file.read((char*)&temp_object.clamp, sizeof(bool));
			editor_file.read((char*)&temp_object.lock, sizeof(bool));
			editor_file.read((char*)&temp_object.name[0], temp_name_size);
			temp_object.object_index = Global::object_index_counter;
		}

		// Increment Object Index Counter
		Global::object_index_counter++;

#endif

	}

	// Establish Limbs
	for (int i = 0; i < number_of_objects; i++)
	{
		establishLib(i);
	}

	// Get Mass
	getMass();
	Axis_of_Rotation = calcCenterOfMass();

	// Set State of Object to be Initialized
	initialized = true;
}

void Object::Physics::Hinge::Hinge::writeFile()
{
	// If Null, Dont Read
	if (file_name == "NULL")
		return;

	// Open Files
	std::ofstream file, editor_file;
	file.open(path, std::ios::binary);
	editor_file.open(path + ".edit", std::ios::binary);

	// Write Header
	file.write((char*)&number_of_objects, sizeof(int));
	file.write((char*)&number_of_children, sizeof(int));

	// Write Physics Objects
	//for (int i = 0; i < number_of_objects; i++)
	//	objects[i]->write(file, editor_file);

	// Write Hinges
	//for (int i = 0; i < number_of_children; i++)
	//	children[i]->write(file, editor_file);
	
	// Close Files
	file.close();
}

void Object::Physics::Hinge::Hinge::append(PhysicsBase& object, bool insert_limb, Limb limb)
{
	// Normal Object
	if ((uint8_t)object.base < 9)
	{
		// Increment Size of Objects and Limbs
		number_of_objects++;

		// Reallocate Memory to Objects and Limbs
		PhysicsBase** new_objects = new PhysicsBase*[number_of_objects];
		Limb** new_limbs = new Limb * [number_of_objects];

		// Copy Data Over to New Location
		for (int i = 0; i < number_of_objects - 1; i++)
		{
			new_objects[i] = objects[i];
			new_limbs[i] = limbs[i];
		}

		// Delete Old Location
		delete objects;
		delete limbs;

		// Swap Locations
		objects = new_objects;
		limbs = new_limbs;

		Limb* new_limb;

		// Insert Existing Limb
		if (insert_limb)
		{
			new_limb = new Limb(limb.offset_x, limb.offset_y, limb.theta_max, limb.theta_min);
		}

		// Generate New Limb
		else
		{
			new_limb = new Limb(Global::mouseRelativeX - data.position.x, Global::mouseRelativeY - data.position.y, 0.0f, 0.0f);
		}

		// Append New Object
		objects[number_of_objects - 1] = &object;
		limbs[number_of_objects - 1] = new_limb;
		establishLib(number_of_objects - 1);
		getMass();
	}

	// Write Change
	writeFile();
}

Object::Physics::PhysicsBase* Object::Physics::Hinge::Hinge::pop(int index, bool hinge, Limb& limb)
{
	PhysicsBase* object;

	if (hinge)
	{
		// Store Pointer to Hinge to be Popped
		object = children[index];

		// Create a New Array of Hinges
		Hinge** new_children = new Hinge*[number_of_children - 1];

		// Copy Memory to New Array Without the Hinge to be Popped
		for (int i = 0; i < index; i++)
		{
			new_children[i] = children[i];
		}
		for (int i = index; i < number_of_children - 1; i++)
		{
			new_children[i] = children[i + 1];
		}

		// Delete Old Hinge Array
		delete children;

		// Swap Location
		children = new_children;

		// Decrement Number of Children
		number_of_children--;
	}

	else
	{
		// Store Pointer to Object to be Popped
		object = objects[index];
		Limb temp_limb = *(limbs[index]);
		limb = temp_limb;

		// Create New Arrays of Objects
		PhysicsBase** new_objects = new PhysicsBase*[number_of_objects - 1];
		Limb** new_limbs = new Limb*[number_of_objects - 1];

		// Copy Memory to New Array Without the Object to be Popped
		for (int i = 0; i < index; i++)
		{
			new_objects[i] = objects[i];
			new_limbs[i] = limbs[i];
		}
		for (int i = index; i < number_of_objects - 1; i++)
		{
			new_objects[i] = objects[i + 1];
			new_limbs[i] = limbs[i + 1];
		}

		// Delete Old Object Arrays
		delete objects;
		delete limbs;

		// Swap Locations
		objects = new_objects;
		limbs = new_limbs;

		// Decrement Number of Objects
		number_of_objects--;

		getMass();
	}

	// Write Change
	writeFile();

	return object;
}

glm::vec2* Object::Physics::Hinge::Hinge::pointerToPosition()
{
	return &data.position;
}

bool Object::Physics::Hinge::Hinge::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 0.5f && x < data.position.x + 0.5f)
	{
		if (y > data.position.y - 0.5f && y < data.position.y + 0.5f)
		{
			return true;
		}
	}

	return false;
}

void Object::Physics::Hinge::Hinge::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
}

glm::vec2 Object::Physics::Hinge::Hinge::returnPosition()
{
	return data.position;
}

void Object::Physics::Hinge::Hinge::initializeVisualizer()
{
	// Generate Buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Get Data
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 1.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	vertices[3] = 1.0f;
	vertices[11] = 1.0f;
	vertices[31] = 1.0f;
	vertices[36] = 1.0f;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Object::Physics::Hinge::Hinge::establishLib(int index)
{
	// Offset to Position

	// Calculate Distance
	limbs[index]->initial_distance = glm::distance(glm::vec2(limbs[index]->offset_x, limbs[index]->offset_y), glm::vec2(0.0f, 0.0f));

	// Calculate Angle
	limbs[index]->initial_angle = Algorithms::Math::angle_from_vector(limbs[index]->offset_x, limbs[index]->offset_y);

	// Object to Offset

	// Calculate Distance
	glm::vec2 relative_limb_pos = glm::vec2(limbs[index]->offset_x, limbs[index]->offset_y) + glm::vec2(data.position.x, data.position.y);
	float delta_w = relative_limb_pos.x - objects[index]->pointerToPosition()->x;
	float delta_h = relative_limb_pos.y - objects[index]->pointerToPosition()->y;
	limbs[index]->pos_to_offset_distance = glm::distance(glm::vec2(delta_w, delta_h), glm::vec2(0.0f, 0.0f));

	// Calculate Angle
	limbs[index]->pos_to_offset_angle = Algorithms::Math::angle_from_vector(delta_w, delta_h);

	// Store Pointer to Position and Mass of Object
	switch (objects[index]->base)
	{
	// Rigid Body
	case PHYSICS_BASES::RIGID_BODY:
	{
		Rigid::RigidBody* object = static_cast<Rigid::RigidBody*>(objects[index]);
		limbs[index]->current_position = &object->physics.Position;
		limbs[index]->mass = object->physics.Mass;
		break;
	}
	// Soft Body
	case PHYSICS_BASES::SOFT_BODY:
	{
		Soft::SoftBody* object = static_cast<Soft::SoftBody*>(objects[index]);
		int node_index = limbs[index]->node_index;
		limbs[index]->current_position = &object->nodes[node_index].Position;
		limbs[index]->mass = object->nodes[node_index].Mass;
		break;
	}
	// Anchor
	case PHYSICS_BASES::HINGE_BASE:
	{
		Anchor* object = static_cast<Anchor*>(objects[index]);
		limbs[index]->current_position = &object->data.position;
		limbs[index]->mass = 0.0f;
		break;
	}
	}
}

void Object::Physics::Hinge::Hinge::getHingePosFromObject(PhysicsBase& object, Limb& limb, glm::vec2& total_pos, float& total_y_pos_grounded, int& grounded_count)
{
	// Determine if Object is Grounded to Get Rotation Vertex
	PHYSICS_BASES object_shape = object.base;
	bool grounded = false;
	switch (object_shape)
	{
	// Rigid Body
	case PHYSICS_BASES::RIGID_BODY:
	{
		Rigid::RigidBody* temp_object = static_cast<Rigid::RigidBody*>(&object);
		temp_object->physics.rotation = rotation;
		grounded = temp_object->physics.grounded;
		if (temp_object->physics.grounded)
			Axis_of_Rotation = temp_object->physics.IAR;
		break;
	}
	}

	// Determine Location of Limb Offset
	glm::vec2 limb_offset = *limb.current_position + limb.pos_to_offset_distance * Algorithms::Math::parameterizeAngle(limb.pos_to_offset_angle + rotation);

	// Determine Location of Hinge Pos
	glm::vec2 hinge_pos_loc = limb_offset - limb.initial_distance * Algorithms::Math::parameterizeAngle(limb.initial_angle + rotation);

	if (grounded)
	{
		// Add Y to Grounded Total and X to Total
		total_y_pos_grounded += hinge_pos_loc.y;
		total_pos.x += hinge_pos_loc.x;
		grounded_count++;
	}

	else
	{
		// Add to Total Position
		total_pos += hinge_pos_loc;
	}
}

void Object::Physics::Hinge::Hinge::getObjectPosFromHinge(PhysicsBase& object, Limb& limb, bool grounded)
{
	// Determine Location of Hinge Pos
	glm::vec2 limb_offset = initial_position + limb.initial_distance * Algorithms::Math::parameterizeAngle(limb.initial_angle + rotation);

	// Determine Location of Object
	glm::vec2 object_position = limb_offset - limb.pos_to_offset_distance * Algorithms::Math::parameterizeAngle(limb.pos_to_offset_angle + rotation);

	// Change Position of Object
	*limb.current_position = object_position;

	PHYSICS_BASES object_shape = object.base;
	switch (object_shape)
	{
	// Rigid Body
	case PHYSICS_BASES::RIGID_BODY:
	{
		Rigid::RigidBody* temp_object = static_cast<Rigid::RigidBody*>(&object);
		temp_object->physics.rotation = rotation;
		temp_object->physics.rotation_velocity = 0;
		temp_object->physics.grounded = grounded;
		if (grounded)
			temp_object->physics.Velocity.y = 0.0f;
		break;
	}
	}
}

void Object::Physics::Hinge::Hinge::getMass()
{
	total_mass = 0.0f;
	for (int i = 0; i < number_of_objects; i++)
	{
		total_mass += limbs[i]->mass;
	}
}

float Object::Physics::Hinge::Hinge::calcMomentOfInertia()
{
	float value = 0.0f;
	for (int i = 0; i < number_of_objects; i++)
	{
		value += glm::distance(Axis_of_Rotation, *limbs[i]->current_position);
	}
	return value;
}

glm::vec2 Object::Physics::Hinge::Hinge::calcCenterOfMass()
{
	glm::vec2 coords = glm::vec2(0.0f, 0.0f);
	for (int i = 0; i < number_of_objects; i++)
	{
		coords += limbs[i]->mass * *limbs[i]->current_position;
	}
	return coords / total_mass;
}

void Object::Physics::Hinge::Hinge::rotate()
{
	// Find Center of Mass
	glm::vec2 Center_of_Mass = calcCenterOfMass();

	// Find Potential Energy
	float torque = -total_mass * Constant::GRAVITATIONAL_ACCELERATION * (Center_of_Mass.x - Axis_of_Rotation.x) * Constant::POTENTIAL_ENERGY_CONSTANT * .01f;

	// Find Angular Acceleration
	float angular_acceleration = torque / calcMomentOfInertia();

	// Update Rotation
	rotation_velocity += angular_acceleration * Global::deltaTime;
	float rotation_step = rotation_velocity * Global::deltaTime;
	rotation += rotation_step;

	// Calculate New Positions for Objects
	for (int i = 0; i < number_of_objects; i++)
	{
		perambulate(*limbs[i]->current_position, rotation_step);
	}

	// Calculate New Position for Hinge
	perambulate(initial_position, rotation_step);

	// Clamp Rotation
	Axis_of_Rotation = Center_of_Mass;
	while (rotation < 0) { rotation += 6.2832f; }
	while (rotation > 6.2832f) { rotation -= 6.2832f; }
	rotation_velocity *= 0.995f;
}

void Object::Physics::Hinge::Hinge::perambulate(glm::vec2& object_position, float& rotation_step)
{
	static float distance, rotation_of_axis;
	static glm::vec2 relative_axis;

	distance = glm::distance(Axis_of_Rotation, object_position);
	relative_axis = Axis_of_Rotation - object_position;
	rotation_of_axis = atan(-relative_axis.y / -relative_axis.x) + rotation_step;
	if (relative_axis.x > 0)
		rotation_of_axis += 3.14159f;
	if (relative_axis == glm::vec2(0, 0))
		rotation_of_axis = rotation;
	object_position = glm::vec2(Axis_of_Rotation.x + distance * cos(rotation_of_axis), Axis_of_Rotation.y + distance * sin(rotation_of_axis));
}

Object::Object* DataClass::Data_Hinge::genObject(glm::vec2& offset)
{
	return new Object::Physics::Hinge::Hinge(uuid, data, file_name, offset);
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

DataClass::Data_Hinge::Data_Hinge(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::PHYSICS;
	object_identifier[1] = (uint8_t)Object::Physics::PHYSICS_BASES::HINGE_BASE;
	object_identifier[2] = (uint8_t)Object::Physics::HINGES::HINGE;
	object_identifier[3] = children_size;
}

void DataClass::Data_Hinge::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Hinge", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &data.position, false);
	object_info.addTextValue("File: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &file_name, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
}

DataClass::Data_Object* DataClass::Data_Hinge::makeCopy()
{
	return new Data_Hinge(*this);
}

void DataClass::Data_Hinge::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
}

int& DataClass::Data_Hinge::getScript()
{
	return data.script;
}

glm::vec2& DataClass::Data_Hinge::getPosition()
{
	return data.position;
}

void DataClass::Data_Hinge::generateInitialValues(glm::vec2& position)
{
	generateUUID();
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

void DataClass::Data_Hinge::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}
