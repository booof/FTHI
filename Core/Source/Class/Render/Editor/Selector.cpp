#include "Selector.h"
#include "Class/Render/Camera/Camera.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Source/Collisions/Point Collisions/PointCollisions.h"
#include "Class/Render/Objects/Level.h"
#include "Class/Render/Objects/ChangeController.h"
#include "Class/Render/Objects/UnsavedLevel.h"
#include "Class/Render/Editor/ObjectInfo.h"

// Globals
#include "Globals.h"
#include "Constants.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Trapezoid/TrapezoidVertices.h"
#include "Source/Vertices/Triangle/TriangleVertices.h"
#include "Source/Vertices/Circle/CircleVertices.h"
#include "Source/Vertices/Line/LineVertices.h"
#include "Source/Vertices/Visualizer/Visualizer.h"

// Objects
#include "Object/Collision/Horizontal/FloorMask.h"
#include "Object/Collision/Vertical/LeftMask.h"
#include "Object/Collision/Vertical/RightMask.h"
#include "Object/Collision/Horizontal/CeilingMask.h"
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
#include "Object/Collision/Trigger/TriggerMask.h"
#include "Object/Terrain/TerrainBase.h"
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

// Shader
#include "Class/Render/Shader/Shader.h"

// Change Controller
#include "Class/Render/Objects/ChangeController.h"

// Notification
#include "Class/Render/Editor/Notification.h"

// Comparison Operation for Node Data
bool operator== (const Object::Physics::Soft::NodeData& node1, const Object::Physics::Soft::NodeData& node2)
{
	return node1.name == node2.name;
}

// Comparison Operation for Node Data
bool operator> (const Object::Physics::Soft::NodeData& node1, const Object::Physics::Soft::NodeData& node2)
{
	return node1.name > node2.name;
}

// Comparison Operation for Node Data
bool operator< (const Object::Physics::Soft::NodeData& node1, const Object::Physics::Soft::NodeData& node2)
{
	return node1.name < node2.name;
}

// Heap
#include "Class/Render/Struct/Heap.h"

Editor::Selector::Selector()
{
	// Generate Vertex Objects for Object
	glGenVertexArrays(1, &objectVAO);
	glGenBuffers(1, &objectVBO);

	// Generate Vertex Object for Outline
	glGenVertexArrays(1, &outlineVAO);
	glGenBuffers(1, &outlineVBO);

	// Generate Vertex Object for Pivot
	genPivotVertices();

	// Generate the Copy Buffer
	glGenBuffers(1, &copyBuffer);

	// Initialize Window
	initializeWindow();
	genBackground();
}

void Editor::Selector::activateHighlighter()
{
	// Allocate Memory for Selector Vertices
	allocateSelectorVertices();

	// Generate Selector Vertices
	genSelectorVertices();

	// Bind Outline Vertex Objects
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Enable Position Vertices for Outline VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices for Outline VBO
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::updateSelector()
{
	// If Initialization is Forced, Initialize Selector
	if (force_selector_initialization)
	{
		editing = true;
		active = true;
		force_selector_initialization = false;
		initializeSelector();
	}

	// If Active and Not Editing, Initialize Selector
	else if (!editing && active)
		initializeSelector();

	// If Editing, Edit Object
	else if (editing)
		editObject();
}

void Editor::Selector::blitzSelector()
{
	// Draw Object
	
	// Object is a Object With Color and Texture
	if (visualize_object)
	{
		// Bind Object Shader
		Global::objectShaderStatic.Use();

		// Send Matrix to Shader
		//glm::mat4 matrix = level->returnProjectionViewMatrix(4) * model;
		//glUniformMatrix4fv(Global::objectStaticMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
		Global::modelLocObjectStatic = glGetUniformLocation(Global::objectShaderStatic.Program, "model");
		glUniformMatrix4fv(Global::modelLocObjectStatic, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4f(glGetUniformLocation(Global::objectShaderStatic.Program, "view_pos"), level->camera->Position.x, level->camera->Position.y, 0.0f, 0.0f);
		//glUniform1f(glGetUniformLocation(Global::objectShaderStatic.Program, "material.shininess"), 1.0);

		// Draw Object
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, object_vertex_count);
		glBindVertexArray(0);
	}

	// Object is Composed of Lines
	else if (visualize_lines)
	{
		// Bind Static Color Shader
		Global::colorShaderStatic.Use();
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Draw Object
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_LINES, 0, object_vertex_count);
		glBindVertexArray(0);
	}

	// Object Only Has a Texture
	else if (visualize_texture)
	{
		// Bind Static Texture Shader
		Global::texShaderStatic.Use();
		glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

		// Load Texture
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, texture.texture);

		// Draw Object
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, object_vertex_count);
		glBindVertexArray(0);
	}

	// Object Only Has a Color
	else
	{
		// Bind Static Color Shader
		Global::colorShaderStatic.Use();

		// Draw Object
		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, object_vertex_count);
		glBindVertexArray(0);
	}

	// Draw Highlighter

	// Bind Highlighter Shader
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Draw Highllighter
	glBindVertexArray(outlineVAO);
	glDrawArrays(GL_LINES, 0, outline_vertex_count);
	glBindVertexArray(0);

	// Draw Pivot, if Enabled
	if (rotating)
	{
		glm::mat4 pivot_model = glm::translate(glm::mat4(1.0f), glm::vec3(pivot.x, pivot.y, 0.0f));
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(pivot_model));
		glBindVertexArray(pivotVAO);
		glDrawArrays(GL_TRIANGLES, 0, 30);
		glBindVertexArray(0);
	}
}

void Editor::Selector::blitzHighlighter()
{
	// Bind Highlighter Shader
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Draw Highllighter
	glBindVertexArray(outlineVAO);
	glDrawArrays(GL_LINES, 0, outline_vertex_count);
	glBindVertexArray(0);
}

void Editor::Selector::deselectObject()
{
	// Selector is Not Loaded
	active = false;
	editing = false;
	highlighting = false;

	// Clamp Objects, if Needed
	clampBase();

	// Sort Vertices, if Needed
	//sortVertices(false);

	change_controller->handleSelectorReturn(this);

	// Reset Some Variables
	originated_from_level = false;
	object_index = 0;
	lighting_object = false;
}

void Editor::Selector::deselectNode()
{
	// Reset Connected List if Needed
	if (connected_limbs_count)
	{
		delete[] connected_limbs;
		connected_limbs_count = 0;
	}

	// Copy File Data Into Stream
	std::stringstream file_stream;
	std::ifstream in_file;
	in_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);
	file_stream << in_file.rdbuf();
	in_file.close();

	// Open File for Writing
	std::ofstream out_file;
	out_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);

	// Project File is Empty
	if (file_stream.str().size() < 2)
	{
		// Write Initial Header
		out_file.put((char)1);
		out_file.put((char)0);

		// Write New Node
		out_file.put((char)0);
		out_file.write((char*)&node_data, sizeof(Object::Physics::Soft::NodeData));
	}

	// There is Data Already in File
	else
	{
		// Copy the Number of Nodes Incremented by 1
		char temp_byte;
		file_stream.read(&temp_byte, 1);
		temp_byte++;
		out_file.put(temp_byte);

		// Copy the Number of Springs
		file_stream.read(&temp_byte, 1);
		out_file.put(temp_byte);

		// Write Rest of File
		//out_file << file_stream.rdbuf();
		while (!file_stream.eof())
		{
			file_stream.read(&temp_byte, 1);
			if (file_stream.eof())
				break;
			out_file.put(temp_byte);
		}

		// Write New Node
		out_file.put((char)0);
		out_file.write((char*)&node_data, sizeof(Object::Physics::Soft::NodeData));
	}

	// Close File
	out_file.close();

	// Reset Add Child Node Flag
	add_child_object = false;

	// Reset Some Variables
	active = false;
	editing = false;
	highlighting = false;
	originated_from_level = false;
	object_index = 0;
	lighting_object = false;
	uuid = 0;
}

void Editor::Selector::deselectSpring()
{
	// Copy File Data Into Stream
	std::stringstream file_stream;
	std::ifstream in_file;
	in_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);
	file_stream << in_file.rdbuf();
	in_file.close();

	// Open File for Writing
	std::ofstream out_file;
	out_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);

	// Copy the Number of Nodes
	char temp_byte;
	file_stream.read(&temp_byte, 1);
	out_file.put(temp_byte);

	// Copy the Number of Springs Incremented by 1
	file_stream.read(&temp_byte, 1);
	temp_byte++;
	out_file.put(temp_byte);

	// Write Rest of File
	//out_file << file_stream.rdbuf();
	while (!file_stream.eof())
	{
		file_stream.read(&temp_byte, 1);
		if (file_stream.eof())
			break;
		out_file.put(temp_byte);
	}

	// Write New Spring
	out_file.put((char)1);
	out_file.write((char*)&spring_data, sizeof(Object::Physics::Soft::Spring));

	// Close File
	out_file.close();

	// Reset Add Child Node Flag
	add_child_object = false;

	// Reset Some Variables
	active = false;
	editing = false;
	highlighting = false;
	originated_from_level = false;
	object_index = 0;
	lighting_object = false;
	uuid = 0;

	// Delete the Node Array
	delete[] node_list;
}

void Editor::Selector::readSpringMassFile()
{
	// Open File
	std::ifstream temp_file;
	temp_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);

	// Determine the Number of Nodes
	uint8_t temp_byte = 0;
	uint8_t node_iterator = 0;
	temp_file.read((char*)&node_count, 1);
	temp_file.read((char*)&temp_byte, 1);

	// Allocate Memory for Nodes
	node_list = new Object::Physics::Soft::NodeData[node_count];

	// Temp Data to Read To
	Object::Physics::Soft::NodeData temp_node_data;
	Object::Physics::Soft::Spring temp_spring_data;

	// Reset Node Count
	node_count = 0;

	// Read Rest of File
	while (!temp_file.eof())
	{
		// Read Type of Next Object
		temp_file.read((char*)&temp_byte, 1);

		// If File is Finished Reading, Stop Iterating
		if (temp_file.eof())
			break;

		// Read Node Into Array
		if (temp_byte == 0)
		{
			temp_file.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
			temp_node_data.position += object_data.position;
			node_list[node_count] = temp_node_data;
			node_count++;
		}

		// Read Spring Into Dummy Var
		else
		{
			temp_file.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
		}
	}

	// Close File
	temp_file.close();
}

void Editor::Selector::readHingeFile()
{
}

void Editor::Selector::moveWithCamera(Render::Camera::Camera& camera, uint8_t direction)
{
	// If Resizing or Not Moving, Don't Move Object
	if (resizing || !moving)
		return;

	// The Distance to Move the Object
	float distance = Constant::SPEED * Global::deltaTime;

	// Process Movements
	if (direction == NORTH) { *object_y += distance * camera.accelerationY; }
	if (direction == SOUTH) { *object_y -= distance * camera.accelerationY; }
	if (direction == EAST) { *object_x -= distance * camera.accelerationL; }
	if (direction == WEST) { *object_x += distance * camera.accelerationR; }

	// Update Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
}

void Editor::Selector::moveWithArrowKeys(uint8_t direction)
{
	// If Inactive, Don't Do Anything
	if (!active)
		return;

	// If Resizing, Don't Move Object
	if (resizing)
		return;

	// The Distance to Move the Object
	float distance = Global::editor_options->option_shift_speed * Global::deltaTime;

	// Process Movements
	if (direction == NORTH) { *object_y += distance; }
	if (direction == SOUTH) { *object_y -= distance; }
	if (direction == EAST) { *object_x += distance; }
	if (direction == WEST) { *object_x -= distance; }

	// Update Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
}

void Editor::Selector::stopResizing()
{
	change_horizontal = 0;
	change_vertical = 0;
	selected_vertex = 0;
	mouse_angle = 0.0f;
	moving = false;
	Global::LeftClick = false;
}

void Editor::Selector::clear()
{
	// Set State to Inactive
	editing = false;
	active = false;
	highlighting = false;

	// If Originated From Level, Remove From Unsaved Level
	if (originated_from_level)
		change_controller->handleSelectorDelete(this);

	// Reset Some Variables
	originated_from_level = false;
	object_index = 0;
}

glm::vec2 Editor::Selector::getObjectPosition()
{
	return glm::vec2(*object_x, *object_y);
}

void Editor::Selector::storeLimbPointers(int index, Object::Physics::Soft::Spring* limbs, int limbs_size)
{
	// Note: This Function Assumes Nodes Were Already Mapped

	// Reset Connected List if Needed
	if (connected_limbs_count)
	{
		delete[] connected_limbs;
		connected_limbs_count = 0;
	}

	// Determine the Number of Connected Nodes
	for (int i = 0; i < limbs_size; i++)
		connected_limbs += ((limbs[i].Node1 == index) || (limbs[i].Node2 == index));

	// Allocate Memory for Connected Limb List
	connected_limbs = new ConnectedLimb[connected_limbs_count];

	// Store Pointers to Connected Limbs
	int connected_index = 0;
	for (int i = 0; i < limbs_size; i++)
	{
		if ((limbs[i].Node1 == index) || (limbs[i].Node2 == index))
		{
			connected_limbs[connected_index].limb_ptr = &limbs[i];
			connected_limbs[connected_index].connected_first = limbs[i].Node1 == index;
			connected_index++;
		}
	}
}

void Editor::Selector::initializeSelector()
{
	// Allocate Memory for Selector Vertices
	allocateSelectorVertices();

	// Generate Selector Vertices
	genSelectorVertices();

	// Store Object Data
	storeSelectorData();

	// Bind Object Vertex Objects
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Object with Color and Texture
	if (visualize_object)
	{
		// Enable Position Vertices for Object VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Enable Normal Vertices for Object VAO
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);

		// Enable Color Vertices for Object VAO
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), (void*)(6 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);

		// Enable Texture Coordinates for Object VAO
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GL_FLOAT), (void*)(10 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(3);
	}

	// Object with Only Texture
	else if (visualize_texture)
	{
		// Enable Position Vertices for Object VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Enable Texture Vertices for Object VAO
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
	}

	// Object with Only Color
	else
	{
		// Enable Position Vertices for Object VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
		glEnableVertexAttribArray(0);

		// Enable Color Vertices for Object VAO
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
	}

	// Bind Outline Vertex Objects
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Enable Position Vertices for Outline VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices for Outline VBO
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Set Editing Flag to True
	editing = true;
}

void Editor::Selector::allocateSelectorVertices()
{
	// Bind Vertex Array Object
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Object
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			allocateSelectorVerticesHorizontalMasks();
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			allocateSelectorVerticesVerticalMasks();
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			allocateSelectorVerticesTriggerMasks();
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		allocateSelectorVerticesShapes(object_identifier[2]);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		allocateSelectorVerticesLights();
		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		// Parse Physics Object Base

		switch (object_identifier[1])
		{

			// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			allocateSelectorVerticesShapes(object_identifier[2]);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			allocateSelectorVerticesSoftBody();
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			allocateSelectorVerticesHinge();
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		alocateSelectorVerticesEntity();
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	}

	// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::genSelectorVertices()
{
	// Bind Vertex Array Object
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Object
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			genSelectorVerticesHorizontalMasks();
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			genSelectorVerticesVerticalMasks();
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			genSelectorVerticesTriggerMasks();
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		genSelectorVerticesShapes(object_identifier[2]);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		genSelectorVerticesLights();
		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		// Parse Physics Object Base

		switch (object_identifier[1])
		{

		// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			genSelectorVerticesShapes(object_identifier[2]);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			genSelectorVerticesSoftBody();
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			genSelectorVerticesHinge();
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		genSelectorVerticesEntity();
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	}

	// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::storeSelectorData()
{
	// Parse Object
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			storeSelectorDataHorizontalMasks();
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			storeSelectorDataVerticalMasks();
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			storeSelectorDataTriggerMasks();
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		storeSelectorDataShapes(object_identifier[2]);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		storeSelectorDataLights();
		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		// Parse Physics Object Base

		switch (object_identifier[1])
		{

		// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			storeSelectorDataShapes(object_identifier[2]);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			storeSelectorDataSoftBody();
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			storeSelectorDataHinge();
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		storeSelectorDataEntity();
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	}
}

void Editor::Selector::allocateSelectorVerticesHorizontalMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Horizontal Mask Shapes
	switch (object_identifier[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 616, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	}

	// Object Does Not Consist of Color and Texture
	visualize_object = false;

	// Object is Composed of Lines
	visualize_lines = true;

	// Object Only Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesHorizontalMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Horizontal Mask Shapes
	switch (object_identifier[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::FLOOR ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 1.0f);
		float half_width = horizontal_line_data.width * 0.5f;
		float object_vertices[] = {
			0.0f - half_width, 0.0f, -1.0f,  temp_color.x, temp_color.y, temp_color.z, 1.0f,
			0.0f + half_width, 0.0f, -1.0f,  temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, horizontal_line_data.width, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(horizontal_line_data.position.x, horizontal_line_data.position.y, 0.0f));

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::FLOOR ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(0.28f, 0.0f, 0.34f);
		float object_vertices[] = {
			0.0f,                                           0.0f,				                          	-1.0f,  temp_color.x, temp_color.y, temp_color.z, 1.0f,
			slant_data.position2.x - slant_data.position.x, slant_data.position2.y - slant_data.position.y, -1.0f,  temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, slant_data.position2.x - slant_data.position.x, 0.0f, slant_data.position2.y - slant_data.position.y, -0.9f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Generate Line Data
		slope = (slant_data.position2.y - slant_data.position.y) / (slant_data.position2.x - slant_data.position.x);
		intercept = slant_data.position.y - (slope * slant_data.position.x);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(slant_data.position.x, slant_data.position.y, 0.0f));

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Generate and Store Object Vertices
		float object_vertices[154];
		Vertices::Line::genLineSimplifiedCurve1(0.0f, 0.0f, -1.0f, slope_data.height, slope_data.width, object_identifier[1] == Object::Mask::FLOOR ? glm::vec4(0.04f, 0.24f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.45f, 1.0f), 11, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 616, object_vertices);
		object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, slope_data.width, slope_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(slope_data.position.x, slope_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataHorizontalMasks()
{
	// Parse Horizontal Mask Shapes
	switch (object_identifier[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Shape is a Horizontal Line
		editing_shape = HORIZONTAL_LINE;

		// Store Pointers to Data
		object_x = &horizontal_line_data.position.x;
		object_y = &horizontal_line_data.position.y;
		object_width = &horizontal_line_data.width;

		// Get Object Info
		if (object_identifier[1] == Object::Mask::FLOOR)
			Object::Mask::Floor::FloorMaskLine::info(*info, editor_data.name, horizontal_line_data, floor_mask_platform);
		else
			Object::Mask::Ceiling::CeilingMaskLine::info(*info, editor_data.name, horizontal_line_data);

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Shape is a Line
		editing_shape = LINE;

		// Store Pointers to Data
		object_x = &slant_data.position.x;
		object_y = &slant_data.position.y;
		object_opposite_x = &slant_data.position2.x;
		object_opposite_y = &slant_data.position2.y;

		// Get Object Info
		if (object_identifier[1] == Object::Mask::FLOOR)
			Object::Mask::Floor::FloorMaskSlant::info(*info, editor_data.name, slant_data, floor_mask_platform);
		else
			Object::Mask::Ceiling::CeilingMaskSlant::info(*info, editor_data.name, slant_data);

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Shape is a Rectangle
		editing_shape = RECTANGLE;

		// Store Pointers to Data
		object_x = &slope_data.position.x;
		object_y = &slope_data.position.y;
		object_width = &slope_data.width;
		object_height = &slope_data.height;

		// Get Object Info
		if (object_identifier[1] == Object::Mask::FLOOR)
			Object::Mask::Floor::FloorMaskSlope::info(*info, editor_data.name, slope_data, floor_mask_platform);
		else
			Object::Mask::Ceiling::CeilingMaskSlope::info(*info, editor_data.name, slope_data);

		break;
	}

	}

	// Enable Resize
	enable_resize = true;
}

void Editor::Selector::allocateSelectorVerticesVerticalMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Vertical Mask Shapes
	switch (object_identifier[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 616, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	}

	// Object Does Not Consist of Color and Texture
	visualize_object = false;

	// Object is Composed of Lines
	visualize_lines = true;

	// Object Only Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesVerticalMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Vertical Mask Shapes
	switch (object_identifier[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::LEFT_WALL ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
		float half_height = vertical_line_data.height * 0.5f;
		float object_vertices[] = {
			0.0f, 0.0f + half_height, -1.0f, temp_color.x, temp_color.y, temp_color.z, 1.0f,
			0.0f, 0.0f - half_height, -1.0f, temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, 1.0f, vertical_line_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(vertical_line_data.position.x, vertical_line_data.position.y, 0.0f));

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Bind Object VAO
		glBindVertexArray(objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

		// Generate and Store Object Vertices
		float slope = curve_data.height / 6.0f;
		float amplitude = (1.0f + (6.0f / curve_data.width));
		float x_offset = (21.0f * curve_data.width - 25.0f) / 30.0f;
		int8_t sign = (object_identifier[1] == 1) ? 1 : -1;
		float object_vertices[154];
		Vertices::Line::genLineSimplifiedCurve2(-curve_data.width / 2 * sign, -curve_data.height / 2, -3.0f, curve_data.width, slope, amplitude, x_offset, sign, object_identifier[1] == Object::Mask::LEFT_WALL ? glm::vec4(1.0f, 0.4f, 0.0f, 1.0f) : glm::vec4(0.04f, 0.0f, 0.27f, 1.0f), 11, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 616, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, curve_data.width, curve_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(curve_data.position.x, curve_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataVerticalMasks()
{
	// Parse Vertical Mask Shapes
	switch (object_identifier[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Shape is a Vertical Line
		editing_shape = VERTICAL_LINE;

		// Store Pointers to Data
		object_x = &vertical_line_data.position.x;
		object_y = &vertical_line_data.position.y;
		object_height = &vertical_line_data.height;

		// Get Object Info
		if (object_identifier[1] == Object::Mask::LEFT_WALL)
			Object::Mask::Left::LeftMaskLine::info(*info, editor_data.name, vertical_line_data);
		else
			Object::Mask::Right::RightMaskLine::info(*info, editor_data.name, vertical_line_data);

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Shape is a Rectangle
		editing_shape = RECTANGLE;

		// Store Pointers to Data
		object_x = &curve_data.position.x;
		object_y = &curve_data.position.y;
		object_width = &curve_data.width;
		object_height = &curve_data.height;

		// Get Object Info
		if (object_identifier[1] == Object::Mask::LEFT_WALL)
			Object::Mask::Left::LeftMaskCurve::info(*info, editor_data.name, curve_data);
		else
			Object::Mask::Right::RightMaskCurve::info(*info, editor_data.name, curve_data);

		break;
	}

	}

	// Enable Resize
	enable_resize = true;
}

void Editor::Selector::allocateSelectorVerticesTriggerMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
	object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Allocate Memory Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	outline_vertex_count = 8;

	// Object Does Not Consist of Color and Texture
	visualize_object = false;

	// Object is Compose of Triangles
	visualize_lines = false;

	// Object Only Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesTriggerMasks()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Generate and Store Object Vertices
	float object_vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, trigger_data.width, trigger_data.height, glm::vec4(0.4f, 0.0f, 0.0f, 0.5f), object_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, trigger_data.width, trigger_data.height, outline_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

	// Set Initial Position Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Generate and Store Object Vertices
	float object_vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, trigger_data.width, trigger_data.height, glm::vec4(0.4f, 0.0f, 0.0f, 0.5f), object_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(object_vertices), object_vertices, GL_STATIC_DRAW);
	object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, trigger_data.width, trigger_data.height, outline_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(outline_vertices), outline_vertices, GL_STATIC_DRAW);
	outline_vertex_count = 8;

	// Object Does Not Consist of Color and Texture
	visualize_object = false;

	// Object is Compose of Triangles
	visualize_lines = false;

	// Object Only Has Color
	visualize_texture = false;

	// Set Initial Position Model Matrix
	object_x = &trigger_data.position.x;
	object_y = &trigger_data.position.y;
	model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	*/
}

void Editor::Selector::storeSelectorDataTriggerMasks()
{
	// Shape is a Rectangle
	editing_shape = RECTANGLE;

	// Store Object Data
	object_x = &trigger_data.position.x;
	object_y = &trigger_data.position.y;

	// Get Object Info
	Object::Mask::Trigger::TriggerMask::info(*info, editor_data.name, trigger_data);
}

void Editor::Selector::allocateSelectorVerticesShapes(int index)
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 144, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 3;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 6;

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		//Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 2880, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 60;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 1120, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 40;

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Allocate Memory for Object Vertices
		unsigned char number_of_sides = *polygon_data.pointerToNumberOfSides();
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)144 * number_of_sides, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = number_of_sides * 3;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)56 * number_of_sides, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = number_of_sides * 2;

		break;
	}

	}

	// Object Consists of Color and Texture
	visualize_object = true;

	// Object is Compose of Triangles
	visualize_lines = false;

	// Object Also Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesShapes(int index)
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Generate and Store Object Vertices
		float object_vertices[72];
		Vertices::Rectangle::genRectObjectFull(glm::vec2(0.0f), -1.0f, *rectangle_data.pointerToWidth(), *rectangle_data.pointerToHeight(), object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, *rectangle_data.pointerToWidth(), *rectangle_data.pointerToHeight(), outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Generate and Store Object Vertices
		float object_vertices[72];
		Vertices::Trapezoid::genTrapObjectFull(glm::vec2(0.0f), -1.0f, *trapezoid_data.pointerToWidth(), *trapezoid_data.pointerToHeight(), *trapezoid_data.pointerToWidthOffset(), *trapezoid_data.pointerToHeightOffset(), object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Trapezoid::genTrapHighlighter(0.0f, 0.0f, -0.9f, *trapezoid_data.pointerToWidth(), *trapezoid_data.pointerToHeight(), *trapezoid_data.pointerToWidthOffset(), *trapezoid_data.pointerToHeightOffset(), outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Store Object Data
		glm::vec2 temp_coords1 = object_data.position;
		glm::vec2 temp_coords2 = *triangle_data.pointerToSecondPosition();
		glm::vec2 temp_coords3 = *triangle_data.pointerToThirdPosition();

		// Generate and Store Object Vertices
		float object_vertices[36];
		Vertices::Triangle::genTriObjectFull(glm::vec2(0.0f), temp_coords2 - temp_coords1, temp_coords3 - temp_coords1, -1.0f, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 144, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[42];
		Vertices::Triangle::genTriHighlighter(glm::vec2(0.0f), temp_coords2 - temp_coords1, temp_coords3 - temp_coords1, -0.9f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, outline_vertices);

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		// Generate and Store Object Vertices
		float object_vertices[720];
		Vertices::Circle::genCircleObjectFull(glm::vec2(0.0f), -1.0f, *circle_data.pointerToRadius(), 20, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 2880, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[280];
		Vertices::Circle::genCircleHighlighter(0.0f, 0.0f, -0.9f, *circle_data.pointerToRadius(), 20, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 1120, outline_vertices);

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Generate and Store Object Vertices
		unsigned char number_of_sides = *polygon_data.pointerToNumberOfSides();
		float* object_vertices = new float[(int)number_of_sides * 36];
		Vertices::Circle::genCircleObjectFull(glm::vec2(0.0f), -1.0f, *polygon_data.pointerToRadius(), (int)number_of_sides, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)number_of_sides * 144, object_vertices);
		delete[] object_vertices;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float* outline_vertices = new float[number_of_sides * 14];
		Vertices::Circle::genCircleHighlighter(0.0f, 0.0f, -0.9f, *polygon_data.pointerToRadius(), number_of_sides, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56 * number_of_sides, outline_vertices);
		delete[] outline_vertices;

		break;
	}

	}

	// Set Initial Position Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataShapes(int index)
{
	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Shape is a Rectangle
		editing_shape = RECTANGLE;

		// Store Object Data
		object_width = rectangle_data.pointerToWidth();
		object_height = rectangle_data.pointerToHeight();

		// Get Object Info
		getShapeInfo(&rectangle_data);

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Shape is a Trapezoid
		editing_shape = TRAPEZOID;

		// Store Object Data
		object_width = trapezoid_data.pointerToWidth();
		object_height = trapezoid_data.pointerToHeight();
		object_width_modifier = trapezoid_data.pointerToWidthOffset();
		object_height_modifier = trapezoid_data.pointerToHeightOffset();

		// Get Object Info
		getShapeInfo(&trapezoid_data);

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Shape is a Triangle
		editing_shape = TRIANGLE;

		// Store Object Data
		coords1 = object_data.position;
		coords2 = *triangle_data.pointerToSecondPosition();
		coords3 = *triangle_data.pointerToThirdPosition();

		// Get Object Info
		getShapeInfo(&triangle_data);

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		// Shape is a Circle
		editing_shape = CIRCLE;

		// Store Object Data
		object_radius = circle_data.pointerToRadius();
		object_inner_radius = circle_data.pointerToRadiusInner();

		// Get Object Info
		getShapeInfo(&circle_data);

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Shape is a Circle
		editing_shape = CIRCLE;

		// Store Object Data
		object_radius = polygon_data.pointerToRadius();
		object_inner_radius = polygon_data.pointerToRaidusInner();

		// Get Object Info
		getShapeInfo(&polygon_data);

		break;
	}

	}

	// Store Initial Position Data
	object_x = &object_data.position.x;
	object_y = &object_data.position.y;

	// Enable Resize
	enable_resize = true;
}

void Editor::Selector::allocateSelectorVerticesLights()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Light Types
	switch (object_identifier[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		break;
	}

	}

	// Object Only Has Texture
	visualize_object = false;

	// Object Only Has Texture
	visualize_texture = true;

	// Object is Composed of Triangles
	visualize_lines = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesLights()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Light Types
	switch (object_identifier[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Line::genLineTexture(0.0f, directional_data.position2.x - light_data.position.x, 0.0f, directional_data.position2.y - light_data.position.y, -0.9f, 0.4f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, directional_data.position2.x - light_data.position.x, 0.0f, directional_data.position2.y - light_data.position.y, -0.8f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Generate Line Data
		slope = (directional_data.position2.y - light_data.position.y) / (directional_data.position2.x - light_data.position.x);
		intercept = light_data.position.y - (slope * light_data.position.x);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -0.9f, 2.0f, 3.0f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.8f, 2.0f, 3.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -0.9f, 2.0f, 4.0f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.8f, 2.0f, 4.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Line::genLineTexture(0.0f, beam_data.position2.x - light_data.position.x, 0.0f, beam_data.position2.y - light_data.position.y, -0.9f, 0.4f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, beam_data.position2.x - light_data.position.x, 0.0f, beam_data.position2.y - light_data.position.y, -0.8f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Generate Line Data
		slope = (beam_data.position2.y - light_data.position.y) / (beam_data.position2.x - light_data.position.x);
		intercept = light_data.position.y - (slope * light_data.position.x);

		break;
	}

	}

	// Set Initial Position Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(light_data.position.x, light_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataLights()
{
	static float temp_width;
	static float temp_height;

	// Parse Light Types
	switch (object_identifier[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Shape is a Line
		editing_shape = LINE;

		// Store Pointers to Opposite Position Data
		object_opposite_x = &directional_data.position2.x;
		object_opposite_y = &directional_data.position2.y;

		// Store Texture
		texture = Global::Visual_Textures.find("DirectionalLight.png")->second;

		// Get Size of Directional Light Buffer
		light_buffer_offset = level->returnDirectionalBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::DirectionalBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)light_buffer_offset + 96, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::DirectionalBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)light_buffer_offset + 96, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count = 1;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		// Enable Resize
		enable_resize = true;

		// Get Object Info
		Object::Light::Directional::Directional::info(*info, editor_data.name, light_data, directional_data);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Shape is a Rectangle
		editing_shape = RECTANGLE;

		// Store Static Sizes
		temp_width = 5.0f;
		temp_height = 5.0f;
		object_width = &temp_width;
		object_height = &temp_height;
		enable_resize = false;

		// Store Texture
		texture = Global::Visual_Textures.find("PointLight.png")->second;

		// Get Size of Point Light Buffer
		light_buffer_offset = level->returnPointBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 80 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::PointBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 80, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::PointBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 80, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count = 1;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		// Disable Resize
		enable_resize = false;

		// Get Object Info
		Object::Light::Point::Point::info(*info, editor_data.name, light_data, point_data);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Shape is a Rectangle
		editing_shape = RECTANGLE;

		// Store Static Sizes
		temp_width = 5.0f;
		temp_height = 5.0f;
		object_width = &temp_width;
		object_height = &temp_height;
		enable_resize = false;

		// Store Texture
		texture = Global::Visual_Textures.find("SpotLight.png")->second;

		// Get Size of Spot Light Buffer
		light_buffer_offset = level->returnSpotBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::SpotBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::SpotBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count = 1;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		// Disable Resize
		enable_resize = false;

		// Get Object Info
		Object::Light::Spot::Spot::info(*info, editor_data.name, light_data, spot_data);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Shape is a Line
		editing_shape = LINE;

		// Store Pointers to Opposite Position Data
		object_opposite_x = &beam_data.position2.x;
		object_opposite_y = &beam_data.position2.y;

		// Store Texture
		texture = Global::Visual_Textures.find("BeamLight.png")->second;

		// Get Size of Beam Light Buffer
		light_buffer_offset = level->returnBeamBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::BeamBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::BeamBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count = 1;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		// Enable Resize
		enable_resize = true;

		// Get Object Info
		Object::Light::Beam::Beam::info(*info, editor_data.name, light_data, beam_data);

		break;
	}

	}

	// Set Object Position
	object_x = &light_data.position.x;
	object_y = &light_data.position.y;

	// Object is a Light
	lighting_object = true;

	// Store Shader Data
	storeSelectorShaderDataLights();
}

void Editor::Selector::storeSelectorShaderDataLights()
{
	// Parse Light Types
	switch (object_identifier[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Bind Directional Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::DirectionalBuffer);

		// Calculate Directional Vectors
		float slope = (directional_data.position2.y - light_data.position.y) / (directional_data.position2.x - light_data.position.x);
		float perpendicular_slope = -1.0f / slope;	
		float direction = (directional_data.position2.x < light_data.position.x) ? -1.0f : 1.0f;
		glm::vec4 light_direction = glm::vec4(direction, perpendicular_slope * direction, 0, 0);
		glm::vec4 line_direction = glm::vec4(1.0f, slope, 0, 0);
		light_direction = glm::normalize(-light_direction);
		light_direction.z = 1.0f;
		line_direction = glm::normalize(line_direction);

		// Add Light and Line Direction Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset, 16, glm::value_ptr(light_direction));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 16, 16, glm::value_ptr(line_direction));

		// Add Ambient, Diffuse, and Specular Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 32, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 48, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 64, 16, glm::value_ptr(light_data.specular * light_data.intensity));

		// Add Endpoint Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 80, 8, glm::value_ptr(light_data.position));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 88, 8, glm::value_ptr(directional_data.position2));

		// Unbind Directional Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Bind Point Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);

		// Add Position Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset, 16, glm::value_ptr(glm::vec4(light_data.position.x, light_data.position.y, 2.0f, 1.0f)));

		// Add Ambient, Diffuse, and Specular Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 16, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 32, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 48, 16, glm::value_ptr(light_data.specular * light_data.intensity));

		// Add Attenuation Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 64, 4, &point_data.linear);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 68, 4, &point_data.quadratic);

		// Unbind Point Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Bind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::SpotBuffer);

		// Add Position Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset, 16, glm::value_ptr(glm::vec4(light_data.position.x, light_data.position.y, 2.0f, 0.0f)));

		// Add Direction Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 16, 16, glm::value_ptr(spot_data.direction));

		// Add Ambient, Diffuse, and Specular Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 32, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 48, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 64, 16, glm::value_ptr(light_data.specular * light_data.intensity));

		// Add Attenuation Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 80, 4, &spot_data.linear);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 84, 4, &spot_data.quadratic);

		// Add Angle Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 88, 4, &spot_data.angle1);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 92, 4, &spot_data.angle2);

		// Unbind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Bind Beam Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);

		// Calculate Directional Vectors
		float slope = (beam_data.position2.y - light_data.position.y) / (beam_data.position2.x - light_data.position.x);
		float perpendicular_slope = -1.0f / slope;
		glm::vec4 direction = glm::vec4(1.0f, slope, 0, 0);
		direction = glm::normalize(direction);

		// Add Light and Line Direction Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset, 16, glm::value_ptr(direction));

		// Add Ambient, Diffuse, and Specular Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 16, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 32, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 48, 16, glm::value_ptr(light_data.specular * light_data.intensity));

		// Add Attenuation Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 64, 4, &beam_data.linear);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 68, 4, &beam_data.quadratic);

		// Add Endpoint Data
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 72, 8, glm::value_ptr(light_data.position));
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, light_buffer_offset + 80, 8, glm::value_ptr(beam_data.position2));

		// Unbind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	}
}

void Editor::Selector::allocateSelectorVerticesSoftBody()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Soft Body Types
	switch (object_identifier[2])
	{

	// SpringMass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		// Object is Not Composed of Lines
		visualize_lines = false;

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		outline_vertex_count = 8;

		// Object is Composed of Lines
		visualize_lines = true;

		break;
	}

	}

	// Object is a Static Color
	visualize_object = false;

	// Object Only Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesSoftBody()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Soft Body Types
	switch (object_identifier[2])
	{

	// Spring Mass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Edit Node
		if (springmass_node_modified)
		{
			// Generate and Store Object Vertices
			float object_vertices[42];
			float double_radius = node_data.radius * 2.0f;
			Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, double_radius, double_radius, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

			// Generate and Store Outline Vertices
			double_radius *= 1.2f;
			float outline_vertices[56];
			Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, double_radius, double_radius, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			model = glm::translate(glm::mat4(1.0f), glm::vec3(node_data.position.x, node_data.position.y, 0.0f));
		}

		// Edit Spring
		else if (springmass_spring_modified)
		{
			// Generate and Store Object Vertices
			float object_vertices[42];
			Vertices::Line::genLineColor(connection_pos_left.x, connection_pos_right.x, connection_pos_left.y, connection_pos_right.y, -1.0f, 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

			// Generate and Store Outline Vertices
			float outline_vertices[56];
			Vertices::Line::genLineHighlighterWidth(connection_pos_left.x, connection_pos_right.x, connection_pos_left.y, connection_pos_right.y, -0.9f, 0.3f, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		}

		// Edit Core
		else
		{
			// Generate and Store Object Vertices
			float object_vertices[42];
			Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 2.0f, 2.0f, object_data.colors, object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

			// Generate and Store Outline Vertices
			float outline_vertices[56];
			Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, 2.0f, 2.0f, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));
		}

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Generate and Store Object Vertices
		float object_vertices[] = {
			0.0f,                                           0.0f,				                          	-1.0f,  0.0f, 1.0f, 1.0f, 1.0f,
			wire_data.position2.x - object_data.position.x, wire_data.position2.y - object_data.position.y, -1.0f,  0.0f, 1.0f, 1.0f, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, wire_data.position2.x - object_data.position.x, 0.0f, wire_data.position2.y - object_data.position.y, -0.9f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Generate Line Data
		slope = (wire_data.position2.y - object_data.position.y) / (wire_data.position2.x - object_data.position.x);
		intercept = object_data.position.y - (slope * object_data.position.x);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataSoftBody()
{
	// Parse Soft Body Types
	switch (object_identifier[2])
	{

	// SpringMass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Edit Node
		if (springmass_node_modified)
		{
			// Shape is a SpringMass Node
			editing_shape = SPRINGMASS_NODE;

			// Set Object Position
			object_x = &node_data.position.x;
			object_y = &node_data.position.y;

			// Get Object Info
			info->clearAll();
			info->setObjectType("SpringMass Node", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			info->addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &node_data.name, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
			info->addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &node_data.position.x, &node_data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
		}

		// Edit Spring
		else if (springmass_spring_modified)
		{
			// Shape is a SpringMass Spring
			editing_shape = SPRINGMASS_SPRING;

			// Get Object Info
			info->clearAll();
			info->setObjectType("SpringMass Spring", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			info->addDoubleValue("Nodes: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "L: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " R: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &spring_data.Node1, &spring_data.Node2, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), true);
			info->addSingleValue("Rest Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.RestLength, glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), false);
			info->addSingleValue("Max Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.MaxLength, glm::vec4(0.0f, 0.9f, 0.0f, 1.0f), false);
			info->addSingleValue("Spring Constant: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.Stiffness, glm::vec4(0.0f, 0.0f, 0.9f, 1.0f), false);
			info->addSingleValue("Dampening: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.Dampening, glm::vec4(0.9f, 0.0f, 0.9f, 1.0f), false);
		}

		// Edit Core
		else
		{
			// Shape is a SpringMass Object
			editing_shape = SPRINGMASS_OBJECT;

			// Set Object Position
			object_x = &object_data.position.x;
			object_y = &object_data.position.y;

			// Get Object Info
			Object::Physics::Soft::SpringMass::info(*info, editor_data.name, object_data, file_name);
		}

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Shape is a Line
		editing_shape = LINE;

		// Store Pointers to Data
		object_opposite_x = &slant_data.position2.x;
		object_opposite_y = &slant_data.position2.y;

		// Set Object Position
		object_x = &object_data.position.x;
		object_y = &object_data.position.y;

		// Get Object Info
		Object::Physics::Soft::Wire::info(*info, editor_data.name, object_data, wire_data);

		break;
	}

	}

	// Disable Resize
	enable_resize = false;
}

void Editor::Selector::allocateSelectorVerticesHinge()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
	object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Allocate Memory for Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	outline_vertex_count = 8;

	// Object is a Static Color
	visualize_object = false;

	// Object is Not Composed of Lines
	visualize_lines = false;

	// Object Only Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesHinge()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Parse Hinge Types
	switch (object_identifier[2])
	{

	// Anchor
	case (int)Object::Physics::HINGES::ANCHOR:
	{
		// Generate and Store Object Vertices
		float object_vertices[42];
		Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 1.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, 1.0f, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(anchor_data.position.x, anchor_data.position.y, 0.0f));

		break;
	}

	// Hinge
	case (int)Object::Physics::HINGES::HINGE:
	{
		// Generate and Store Object Vertices
		float object_vertices[42];
		Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 1.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, 1.0f, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		model = glm::translate(glm::mat4(1.0f), glm::vec3(hinge_data.position.x, hinge_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataHinge()
{
	static float temp_width;
	static float temp_height;

	// Parse Hinge Types
	switch (object_identifier[2])
	{

	// Anchor
	case (int)Object::Physics::HINGES::ANCHOR:
	{
		// Set Object Position
		object_x = &anchor_data.position.x;
		object_y = &anchor_data.position.y;

		// Get Object Info
		Object::Physics::Hinge::Anchor::info(*info, editor_data.name, anchor_data);

		break;
	}

	// Hinge
	case (int)Object::Physics::HINGES::HINGE:
	{
		// Set Object Position
		object_x = &hinge_data.position.x;
		object_y = &anchor_data.position.y;

		// Get Object Info
		Object::Physics::Hinge::Hinge::info(*info, editor_data.name, hinge_data, file_name);

		break;
	}

	}

	// Object is a Rectangle
	editing_shape = RECTANGLE;

	// Store Pointers to Data
	temp_width = 2.0f;
	temp_height = 2.0f;
	object_width = &temp_width;
	object_height = &temp_height;

	// Disable Resize
	enable_resize = false;
}

void Editor::Selector::alocateSelectorVerticesEntity()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
	object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Allocate Memory for Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	outline_vertex_count = 8;

	// Object Consists of Color and Texture
	visualize_object = true;

	// Object is Compose of Triangles
	visualize_lines = false;

	// Object Also Has Color
	visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesEntity()
{
	// Bind Object VAO
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
	
	// Generate and Store Object Vertices
	float object_vertices[72];
	object_data.colors = glm::vec4(1.0f);
	Vertices::Rectangle::genRectObjectFull(glm::vec2(0.0f), -1.0f, entity_data.half_width * 2.0f, entity_data.half_height * 2.0f, object_data.colors, object_data.normals, object_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

	// Bind Outline VAO
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, entity_data.half_collision_width * 2.0f, entity_data.half_collision_height * 2.0f, outline_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

	// Set Initial Position Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::storeSelectorDataEntity()
{
	static float full_width, full_height;

	// Shape is a Rectangle
	editing_shape = RECTANGLE;

	// Store Object Data
	full_width = entity_data.half_collision_width * 2.0f;
	full_height = entity_data.half_collision_height * 2.0f;
	object_width = &full_width;
	object_height = &full_height;

	// Store Initial Position Data
	object_x = &object_data.position.x;
	object_y = &object_data.position.y;

	// Get Object Info
	Object::Entity::NPC::info(*info, editor_data.name, object_data);

	// Disable Resize
	enable_resize = false;
}

void Editor::Selector::getShapeInfo(Shape::Shape* shape)
{
	// Selected Object is Terrain
	if (object_identifier[0] == Object::TERRAIN)
		Object::Terrain::TerrainBase::info(*info, editor_data.name, object_data, shape);

	// Selected Object is Rigid Body
	else
		Object::Physics::Rigid::RigidBody::info(*info, editor_data.name, object_data, shape);
}

void Editor::Selector::uninitializeSelector()
{
	// Delete Object VAO
	glDeleteVertexArrays(1, &objectVAO);
	glDeleteBuffers(1, &objectVBO);

	// Delete Outline VAO
	glDeleteVertexArrays(1, &outlineVAO);
	glDeleteBuffers(1, &outlineVBO);

	// If Rotation is Possible, Delete Pivot Value
	if (editing_shape == TRAPEZOID || editing_shape == TRIANGLE || editing_shape == LINE)
	{
		glDeleteVertexArrays(1, &pivotVAO);
		glDeleteBuffers(1, &pivotVBO);
	}
}

void Editor::Selector::outlineForResize()
{
	float colors[4] = { 0.0f, 0.0f, 0.85f, 1.0f };
	outlineChangeColor(colors);
	resizing = true;
	mouse_intersects_object = true;
}

void Editor::Selector::outlineForMove()
{
	float colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	outlineChangeColor(colors);
	resizing = false;
	mouse_intersects_object = true;
}

void Editor::Selector::outlineForNotSelected()
{
	float colors[4] = { 0.55f, 0.55f, 0.0f, 0.95f };
	outlineChangeColor(colors);
	resizing = false;
	mouse_intersects_object = false;
}

void Editor::Selector::outlineChangeColor(float* colors)
{
	// Bind Highlighter Object
	glBindVertexArray(outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);

	// Iterate Through Each Vertex and Change Color
	for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < outline_vertex_count; i += 7 * sizeof(GL_FLOAT), j++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), colors);
	}

	// Unbind Highlighter Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::genPivotVertices()
{
	// Generate Pivot VAO
	glGenVertexArrays(1, &pivotVAO);
	glGenBuffers(1, &pivotVBO);

	// Bind Vertex Objects
	glBindVertexArray(pivotVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pivotVBO);

	// Generate new Vertices for pointer
	float vertices[210];
	Vertices::Circle::genCircleColorFull(pivot.x, pivot.y, -0.5f, 0.25f, 10, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);

	// Store Data in VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertex
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enble Color Vertex
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::editObject()
{
	// Test if Child Object Should be Added
	if (add_child_object)
	{
		addChild();

		return;
	}

	// Edit Object
	update_functions[editing_shape]();

	// Update Object Info
	info->forceResize();

	// If Object is a Lighting Object, Update Shader Data
	if (lighting_object)
		storeSelectorShaderDataLights();
}

void Editor::Selector::testResizeRectangle(bool enable_horizontal, bool enable_vertical)
{
	// Test if Resizing Should be Considered
	if (Global::editor_options->option_resize && !Global::Keys[GLFW_KEY_F] && enable_resize)
	{
		// How Far the Mouse Must be Inside Object to Stop Resizing
		float resize_width = 1.5f * Global::zoom_scale;

		// Change Horizontal Size of Object
		if (enable_horizontal)
		{
			float half_absolute_width = abs(*object_width) * 0.5f;
			int8_t size_sign = Algorithms::Math::getSign(*object_width); 

			// Test if Left Side Should Shift
			if (Global::mouseRelativeX < (*object_x - half_absolute_width + resize_width))
			{
				// Sign of Boolian Determines Which Side Doesn't Move
				change_horizontal = -size_sign;
				Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;
			}

			// Test if Right Side Should Shift
			else if (Global::mouseRelativeX > (*object_x + half_absolute_width - resize_width))
			{
				change_horizontal = size_sign;
				Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;
			}
		}

		// Change Vertical Size of Object
		if (enable_vertical)
		{
			float half_absolute_height = abs(*object_height) * 0.5f;
			int8_t size_sign = Algorithms::Math::getSign(*object_height);

			// Test if Top Side Should Shift
			if (Global::mouseRelativeY > (*object_y + half_absolute_height - resize_width))
			{
				change_vertical = size_sign;
				Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;
			}

			// Test if Bottom Side Should Shift
			else if (Global::mouseRelativeY < (*object_y - half_absolute_height + resize_width))
			{
				change_vertical = -size_sign;
				Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;
			}
		}

		// Test if Color of Outline Should Change
		if (!resizing && (change_vertical || change_horizontal))
			outlineForResize();
	}
}

void Editor::Selector::moveRectangle(bool enable_negative)
{
	// Stop Moving if LeftClick is no Longer Being Held
	if (!Global::LeftClick)
	{
		moving = false;

		//// If Selected Object is a File, Change Values
		//if (enable_file_visualizer)
		//{
		//	file_visualizer->Initial_Position.x = xPos;
		//	file_visualizer->Initial_Position.y = yPos;
		//	if (type_minor == 6)
		//	{
		//		Object_SpringMass* temp = static_cast<Object_SpringMass*>(file_visualizer);
		//		//temp->Read();
		//	}
		//	else if (type_minor == 9)
		//	{
		//		Object_Hinge* temp = static_cast<Object_Hinge*>(file_visualizer);
		//		temp->Read();
		//	}
		//}
	}

	else
	{
		// Shift Horizontal
		if (change_horizontal != 0)
		{
			// Set Selected Cursor
			Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;

			// Change Relative X if Enabled
			if (Global::editor_options->option_disable_pass_through)
			{
				if (change_horizontal < 0 && Global::mouseRelativeX > *object_x) { Global::mouseRelativeX = *object_x - 0.05f; }
				else if (change_horizontal > 0 && Global::mouseRelativeX < *object_x) { Global::mouseRelativeX = *object_x + 0.05f; }
			}

			// Calculate New xPos by Taking the Average of the x Position of the Unchanging (opposite) Side and the x Position of the Mouse
			*object_x = (Global::mouseRelativeX + (*object_x - ((*object_width * 0.5f) * change_horizontal))) * 0.5f;

			// Calculate New Width by Multiplying the Distance Between the Mouse and xPos by 2
			*object_width = 2 * (Global::mouseRelativeX - *object_x) * change_horizontal;

			// Prevent Size from Going Negative
			if (*object_width <= 0.1f && !enable_negative)
			{
				*object_width = 0.1f;
			}
		}

		// Shift Verticle
		if (change_vertical != 0)
		{
			// Set Selected Cursor
			Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;

			// Change Relative Y if Enabled
			if (Global::editor_options->option_disable_pass_through)
			{
				if (change_vertical < 0 && Global::mouseRelativeY > *object_y) { Global::mouseRelativeY = *object_y - 0.05f; }
				else if (change_vertical > 0 && Global::mouseRelativeY < *object_y) { Global::mouseRelativeY = *object_y + 0.05f; }
			}

			// Calculate New yPos by Taking the Average of the y Position of the Unchanging (opposite) Side and the y Position of the Mouse
			*object_y = (Global::mouseRelativeY + (*object_y - ((*object_height * 0.5f) * change_vertical))) * 0.5f;

			// Calculate New Height by Multiplying the Distance Between the Mouse and yPos by 2
			*object_height = 2 * (Global::mouseRelativeY - *object_y) * change_vertical;

			// Prevent Size from Going Negative
			if (*object_height <= 0.1f && !enable_negative)
			{
				*object_height = 0.1f;
			}
		}

		// Move if Size Doesn't Change
		if (!(change_vertical || change_horizontal))
		{
			Global::Selected_Cursor = Global::CURSORS::HAND;
			*object_x = (float)(Global::mouseRelativeX + offset_x);
			*object_y = (float)(Global::mouseRelativeY + offset_y);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
		}

		// Else Update Vertices and Buffer Objects
		else
		{
			genSelectorVertices();
		}
	}
}

void Editor::Selector::updateRectangle()
{
	// Test if Mouse is Inside Object
	if ((*object_x - (*object_width * 0.5f) < Global::mouseRelativeX) && (*object_x + (*object_width * 0.5f) > Global::mouseRelativeX) && (*object_y - (*object_height * 0.5f) < Global::mouseRelativeY) && (*object_y + (*object_height * 0.5f) > Global::mouseRelativeY))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			change_horizontal = 0;
			change_vertical = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeRectangle(true, true);

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !(change_vertical || change_horizontal)))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Rectangle
	moveRectangle(true);
}

void Editor::Selector::testResizeTrapezoid()
{
	if (Global::editor_options->option_resize && !Global::Keys[GLFW_KEY_F] && enable_resize)
	{
		// How Far the Mouse Must be Inside Object to Stop Resizing
		float resize_width = 1.5f * Global::zoom_scale;

		// Calculate Half Values
		float half_width = *object_width * 0.5f;
		float half_height = *object_height * 0.5f;

		// Values of Corners
		glm::vec2 bottom_left = glm::vec2(*object_x - half_width, *object_y - half_height);
		glm::vec2 bottom_right = glm::vec2(*object_x + half_width, *object_y - half_height + *object_height_modifier);
		glm::vec2 top_right = glm::vec2(*object_x + half_width + *object_width_modifier, *object_y + half_height + *object_height_modifier);
		glm::vec2 top_left = glm::vec2(*object_x - half_width + *object_width_modifier, *object_y + half_height);

		// Test if Bottom Shoud Resize
		if (Global::mouseRelativeX > bottom_left.x && Global::mouseRelativeX < bottom_right.x)
		{
			// Calculate Relative Values
			float relative_y_bottom = ((bottom_left.y - bottom_right.y) / (bottom_left.x - bottom_right.x)) * (Global::mouseRelativeX - bottom_left.x) + bottom_left.y;

			// Test if Y is Inside Reasonable Range
			if (Global::mouseRelativeY > relative_y_bottom && Global::mouseRelativeY < relative_y_bottom + resize_width)
			{
				change_horizontal = 1;
				Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;

				// Calculate Offsets
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_height_modifier * ((Global::mouseRelativeX - bottom_left.x) / (bottom_right.x - bottom_left.x));
			}
		}

		// Test if Right Shoud Resize
		if (Global::mouseRelativeY > bottom_right.y && Global::mouseRelativeY < top_right.y && !change_horizontal)
		{
			// Calculate Relative Values
			float relative_x_right = ((bottom_right.x - top_right.x) / (bottom_right.y - top_right.y)) * (Global::mouseRelativeY - bottom_right.y) + bottom_right.x;

			// Test if X is Inside Reasonable Range
			if (Global::mouseRelativeX < relative_x_right && Global::mouseRelativeX > relative_x_right - resize_width)
			{
				change_vertical = 1;
				Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;

				// Calculate Offsets
				offset_x = *object_width_modifier * ((Global::mouseRelativeY - bottom_right.y) / (top_right.y - bottom_right.y));
				offset_y = (*object_y + *object_height_modifier) - Global::mouseRelativeY;
			}
		}

		// Test if Top Should Resize
		if (Global::mouseRelativeX < top_right.x && Global::mouseRelativeX > top_left.x && !change_vertical)
		{
			// Calculate Relative Values
			float relative_y_top = ((top_right.y - top_left.y) / (top_right.x - top_left.x)) * (Global::mouseRelativeX - top_right.x) + top_right.y;

			// Test if Y is Inside Reasonable Range
			if (Global::mouseRelativeY < relative_y_top && Global::mouseRelativeY > relative_y_top - resize_width)
			{
				change_horizontal = 2;
				Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;

				// Calculate Offsets
				offset_x = (*object_x + *object_width_modifier) - Global::mouseRelativeX;
				offset_y = *object_height_modifier * ((Global::mouseRelativeX - top_left.x) / (top_right.x - bottom_left.x));
			}
		}

		// Test if Left Should Resize
		if (Global::mouseRelativeY < top_left.y && Global::mouseRelativeY > bottom_left.y && !change_horizontal)
		{
			// Calculate Relative Values
			float relative_x_left = ((top_left.x - bottom_left.x) / (top_left.y - bottom_left.y)) * (Global::mouseRelativeY - top_left.y) + top_left.x;

			// Test if X is Inside Reasonable Range
			if (Global::mouseRelativeX > relative_x_left && Global::mouseRelativeX < relative_x_left + resize_width)
			{
				change_vertical = 2;
				Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;

				// Calculate Offsets
				offset_x = *object_width_modifier * ((Global::mouseRelativeY - bottom_left.y) / (top_left.y - bottom_left.y));
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}

		// Test if Color of Outline Should Change
		if (!resizing && (change_vertical || change_horizontal))
			outlineForResize();
	}
}

void Editor::Selector::moveTrapezoid()
{
	// Stop Moving if LeftClick is no Longer Being Held
	if (!Global::LeftClick)
	{
		moving = false;
	}

	else
	{
		// Shift Horizontal
		if (change_horizontal)
		{
			// Shift Bottom
			if (change_horizontal == 1)
			{
				// Calculate Change in X Values
				float change = (*object_x - Global::mouseRelativeX - offset_x) * 0.5f;

				// Calculate Y Changes
				float temp_yPos = (Global::mouseRelativeY - offset_y + (*object_y - ((*object_height * 0.5f) * -1))) * 0.5f;
				float temp_Size2 = 2 * (Global::mouseRelativeY - offset_y - temp_yPos) * -1;

				// If Pass Through is Disabled, Only Apply Changes if New Size is Large Enough
				if (!Global::editor_options->option_disable_pass_through || temp_Size2 > 0.1f)
				{
					// Apply Change
					*object_x -= change;
					*object_width_modifier += change;

					// Apply Calculated Size Change
					*object_y = temp_yPos;
					*object_height = temp_Size2;
				}
			}

			// Shift Top
			else if (change_horizontal == 2)
			{
				// Set Object X Offset
				float temp_SizeOffset1 = (Global::mouseRelativeX + offset_x) - *object_x;

				// Calculate Y Changes
				float temp_yPos = (Global::mouseRelativeY - offset_y + (*object_y - (*object_height * 0.5f))) * 0.5f;
				float temp_Size2 = 2 * (Global::mouseRelativeY - offset_y - temp_yPos);

				// If Pass Through is Disabled, Only Apply Changes if New Size is Large Enough
				if (!Global::editor_options->option_disable_pass_through || temp_Size2 > 0.1f)
				{
					// Apply Calculated Size Change
					*object_width_modifier = temp_SizeOffset1;
					*object_y = temp_yPos;
					*object_height = temp_Size2;
				}
			}

			// Prevent Size from Going Negative
			if (*object_height <= 0.1f)
			{
				*object_height = 0.1f;
			}
		}

		// Shift Vertical
		else if (change_vertical)
		{
			// Shift Right
			if (change_vertical == 1)
			{
				// Set Object Y Offset
				float temp_SizeOffset2 = (Global::mouseRelativeY + offset_y) - *object_y;

				// Calculate X Changes
				float temp_xPos = (Global::mouseRelativeX - offset_x + (*object_x - (*object_width * 0.5f))) * 0.5f;
				float temp_Size1 = 2 * (Global::mouseRelativeX - offset_x - temp_xPos);

				// If Pass Through is Disabled, Only Apply Changes if New Size is Large Enough
				if (!Global::editor_options->option_disable_pass_through || temp_Size1 > 0.1f)
				{
					// Apply Calculated Size Change
					*object_height_modifier = temp_SizeOffset2;
					*object_x = temp_xPos;
					*object_width = temp_Size1;
				}
			}

			// Shift Left
			else if (change_vertical == 2)
			{
				// Calculate Change in Y Values
				float change = (*object_y - Global::mouseRelativeY - offset_y) * 0.5f;

				// Calculate X Changes
				float temp_xPos = (Global::mouseRelativeX - offset_x + (*object_x - ((*object_width * 0.5f) * -1))) * 0.5f;
				float temp_Size1 = 2 * (Global::mouseRelativeX - offset_x - temp_xPos) * -1;

				// If Pass Through is Disabled, Only Apply Changes if New Size is Large Enough
				if (!Global::editor_options->option_disable_pass_through || temp_Size1 > 0.1f)
				{
					// Apply Change
					*object_y -= change;
					*object_height_modifier += change;

					// Apply Calculated Size Change
					*object_x = temp_xPos;
					*object_width = temp_Size1;
				}
			}

			// Prevent Size from Going Negative
			if (*object_width <= 0.1f)
			{
				*object_width = 0.1f;
			}
		}

		// Move if Size Doesn't Change
		if (!(change_vertical || change_horizontal))
		{
			*object_x = (float)(Global::mouseRelativeX + offset_x);
			*object_y = (float)(Global::mouseRelativeY + offset_y);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
		}

		// Else Update Vertices and Buffer Objects
		else
		{
			genSelectorVertices();
		}
	}
}

void Editor::Selector::updateTrapezoid()
{
	// Test if Mouse is Inside Object
	if (Source::Collisions::Point::testTrapCollisions(*object_x, *object_y, *object_width, *object_height, *object_width_modifier, *object_height_modifier))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			change_horizontal = 0;
			change_vertical = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeTrapezoid();

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !(change_vertical || change_horizontal)))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;

				// Only Use Offset if Not Resizing (Offset is Already Defined for Resize)
				if (!(change_vertical || change_horizontal))
				{
					offset_x = *object_x - Global::mouseRelativeX;
					offset_y = *object_y - Global::mouseRelativeY;
				}
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// Rotate Object
	if (rotating)
	{
		sortVertices(true);
	}

	else
	{
		// If Currently Moving, Move Trapezoid
		moveTrapezoid();
	}
}

void Editor::Selector::testResizeTriangle()
{
	if (Global::editor_options->option_resize && !Global::Keys[GLFW_KEY_F] && enable_resize)
	{
		// Test if the First Vector Should Move
		if ((Global::mouseRelativeX >= coords1.x - 2 && Global::mouseRelativeX <= coords1.x + 2) && (Global::mouseRelativeY >= coords1.y - 2 && Global::mouseRelativeY <= coords1.y + 2))
		{
			selected_vertex = 1;
		}

		// Test if the Second Vector Should Move
		else if ((Global::mouseRelativeX >= coords2.x - 2 && Global::mouseRelativeX <= coords2.x + 2) && (Global::mouseRelativeY >= coords2.y - 2 && Global::mouseRelativeY <= coords2.y + 2))
		{
			selected_vertex = 2;
		}

		// Test if the Third Vector Should Move
		else if ((Global::mouseRelativeX >= coords3.x - 2 && Global::mouseRelativeX <= coords3.x + 2) && (Global::mouseRelativeY >= coords3.y - 2 && Global::mouseRelativeY <= coords3.y + 2))
		{
			selected_vertex = 3;
		}

		// Test if Color of Outline Should Change
		if (!resizing && selected_vertex)
			outlineForResize();
	}
}

void Editor::Selector::moveTriangle()
{
	// Stop Moving if LeftClick is no Longer Being Held
	if (!Global::LeftClick)
	{
		moving = false;
	}

	else
	{
		// Test if Triangle Should be Resized
		if (selected_vertex)
		{
			// Move Highest Vertex
			if (selected_vertex == 1)
			{
				coords1.x = Global::mouseRelativeX;
				coords1.y = Global::mouseRelativeY;
			}

			// Move Second Highest Vertex
			else if (selected_vertex == 2)
			{
				coords2.x = Global::mouseRelativeX;
				coords2.y = Global::mouseRelativeY;
			}

			// Move Lowest Vertex
			else
			{
				coords3.x = Global::mouseRelativeX;
				coords3.y = Global::mouseRelativeY;
			}

			// Store Vertices in Object Data
			object_data.position = coords1;
			triangle_data = Shape::Triangle(coords2, coords3);
			should_sort = true;
		}

		// Move if Size Doesn't Change
		if (!selected_vertex)
		{
			// Calclate New Coordinates
			glm::vec2 delta_coords2 = coords2 - coords1;
			glm::vec2 delta_coords3 = coords3 - coords1;
			coords1.x = (float)(Global::mouseRelativeX + offset_x);
			coords1.y = (float)(Global::mouseRelativeY + offset_y);
			coords2 = delta_coords2 + coords1;
			coords3 = delta_coords3 + coords1;

			// Store New Position Data
			object_data.position = coords1;
			object_x = &object_data.position.x;
			object_y = &object_data.position.y;
			triangle_data = Shape::Triangle(coords2, coords3);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
		}

		// Else Update Vertices and Buffer Objects
		else
		{
			genSelectorVertices();
		}
	}
}

void Editor::Selector::updateTriangle()
{
	Vertices::Visualizer::visualizePoint(coords1, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(coords2, 0.3f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(coords3, 0.3f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if (Source::Collisions::Point::testTriCollisions(coords1, coords2, coords3))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			selected_vertex = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeTriangle();

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !selected_vertex))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = coords1.x - Global::mouseRelativeX;
				offset_y = coords1.y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// Rotate Object
	if (rotating)
	{
		sortVertices(true);
	}

	else
	{
		// If Currently Moving, Move Rectangle
		moveTriangle();
	}
}

void Editor::Selector::testResizeCircle(float& distance, float& delta_w, float& delta_h)
{
	if (Global::editor_options->option_resize && !Global::Keys[GLFW_KEY_F] && enable_resize)
	{
		// How Far the Mouse Must be Inside Object to Stop Resizing
		float resize_width = 1.5f * Global::zoom_scale;

		// Test if Mouse Touches Edge of Circle
		if (distance > *object_radius - resize_width)
		{
			// Get Mouse Angle
			mouse_angle = atan(delta_h / delta_w);
			if (delta_w < 0)
				mouse_angle += 3.141459f;

			// Select Vertex Flag
			selected_vertex = 1;

			// Select Cursor
			Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;

			// Test if Color of Outline Should Change
			if (!resizing)
				outlineForResize();
		}
	}
}

void Editor::Selector::moveCircle()
{
	// Stop Moving if LeftClick is no Longer Being Held
	if (!Global::LeftClick)
	{
		moving = false;
	}

	else
	{
		// Test if Circle is Resizing
		if (selected_vertex)
		{
			// Radius is New Distance Between Mouse and Center
			*object_radius = glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), glm::vec2(*object_x, *object_y));
		}

		// Move if Size Doesn't Change
		if (!selected_vertex)
		{
			*object_x = (float)(Global::mouseRelativeX + offset_x);
			*object_y = (float)(Global::mouseRelativeY + offset_y);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
		}

		// Else Update Vertices and Buffer Objects
		else
		{
			genSelectorVertices();
		}
	}
}

void Editor::Selector::updateCircle()
{
	// Get Distance Between Mouse and Center of Circle
	float delta_w = Global::mouseRelativeX - *object_x;
	float delta_h = Global::mouseRelativeY - *object_y;
	float distance = glm::distance(glm::vec2(delta_w, delta_h), glm::vec2(0.0f, 0.0f));

	// Test if Mouse is Inside Circle Object
	if (distance < *object_radius)
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			selected_vertex = 0;
			mouse_angle = 0.0f;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeCircle(distance, delta_w, delta_h);

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !selected_vertex))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Rectangle
	moveCircle();
}

void Editor::Selector::testResizeLine()
{
	if (Global::editor_options->option_resize && !Global::Keys[GLFW_KEY_F] && enable_resize)
	{
		// How Far the Mouse Must be Inside Object to Stop Resizing
		float resize_width = 1.5f * Global::zoom_scale;

		// Test if Mouse is Close to Origin Vertex
		if (glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), glm::vec2(*object_x, *object_y)) < resize_width)
		{
			// Set Vertex to First Vertex
			selected_vertex = 1;

			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::POINT;
		}

		// Test if Mouse is Close to Opposite Vertex
		else if (glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), glm::vec2(*object_opposite_x, *object_opposite_y)) < resize_width)
		{
			// Set Vertex to Second Vertex
			selected_vertex = 2;

			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::POINT;
		}

		// Test if Color of Outline Should Change
		if (!resizing && selected_vertex)
			outlineForResize();
	}
}

void Editor::Selector::moveLine()
{
	// Stop Moving if LeftClick is no Longer Being Held
	if (!Global::LeftClick)
	{
		moving = false;
	}

	else
	{
		// Test if Object Origin Vertex is Being Moved
		if (selected_vertex == 1)
		{
			// Move Object Position
			*object_x = Global::mouseRelativeX;
			*object_y = Global::mouseRelativeY;
		}
		
		// Test if Object Opposite Vertex is Being Moved
		else if (selected_vertex == 2)
		{
			// Move Object Opposite Position
			*object_opposite_x = Global::mouseRelativeX;
			*object_opposite_y = Global::mouseRelativeY;
		}

		// Move if Size Doesn't Change
		if (!selected_vertex)
		{
			// Find Distance Between Positions
			glm::vec2 delta_pos = glm::vec2(*object_opposite_x, *object_opposite_y) - glm::vec2(*object_x, *object_y);

			// Get New Origin Position
			*object_x = (float)(Global::mouseRelativeX + offset_x);
			*object_y = (float)(Global::mouseRelativeY + offset_y);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));

			// Generate Line Data
			//intercept = slant_data.position.y - (slope * slant_data.position.x);
			intercept = *object_y - (slope * *object_x);

			// Find New Opposite Position
			*object_opposite_x = *object_x + delta_pos.x;
			*object_opposite_y = *object_y + delta_pos.y;
		}

		// Else Update Vertices and Buffer Objects
		else
		{
			genSelectorVertices();
		}
	}
}

void Editor::Selector::updateLine()
{
	// Localized Y Value at MouseX
	float LocalY = slope * Global::mouseRelativeX + intercept;

	// Test if Mouse Intersects Lines
	if (((*object_x < Global::mouseRelativeX && *object_opposite_x > Global::mouseRelativeX) || (*object_opposite_x < Global::mouseRelativeX && *object_x > Global::mouseRelativeX)) && LocalY < Global::mouseRelativeY + 1 && LocalY > Global::mouseRelativeY - 1)
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			selected_vertex = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Line Should Resize
			testResizeLine();

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !selected_vertex))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Line
	moveLine();
}

void Editor::Selector::updateHorizontalLine()
{
	// Test if Mouse is Inside Object
	if ((*object_x - (*object_width * 0.5f) < Global::mouseRelativeX) && (*object_x + (*object_width * 0.5f) > Global::mouseRelativeX) && (*object_y - 0.5f < Global::mouseRelativeY) && (*object_y + 0.5f > Global::mouseRelativeY))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			change_horizontal = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeRectangle(true, false);

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !change_horizontal))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Rectangle
	moveRectangle(true);
}

void Editor::Selector::updateVerticalLine()
{
	// Test if Mouse is Inside Object
	if ((*object_x - 0.5f < Global::mouseRelativeX) && (*object_y + 0.5f > Global::mouseRelativeX) && (*object_y - (*object_height / 2) < Global::mouseRelativeY) && (*object_y + (*object_height / 2) > Global::mouseRelativeY))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			change_vertical = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			testResizeRectangle(false, true);

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !change_vertical))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Rectangle
	moveRectangle(true);
}

void Editor::Selector::updateSpringMassObject()
{
	// Test if Mouse is Inside Object
	if (Global::mouseRelativeX > *object_x - 0.5f && Global::mouseRelativeX < *object_x + 0.5f && Global::mouseRelativeY > *object_y - 0.5f && Global::mouseRelativeY < *object_y + 0.5f)
	{
		Global::Selected_Cursor = Global::CURSORS::HAND;
		outlineForMove();

		// If Left Mouse Button is Held, Start Moving
		if (Global::LeftClick)
		{
			moving = true;
			offset_x = *object_x - Global::mouseRelativeX;
			offset_y = *object_y - Global::mouseRelativeY;
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectObject();
		}
	}

	// If Currently Moving, Move Object
	if (moving)
	{
		// Test if Object Should Stop Moving
		if (!Global::LeftClick)
			moving = false;

		// Move Object
		else
		{
			Global::Selected_Cursor = Global::CURSORS::HAND;
			*object_x = (float)(Global::mouseRelativeX + offset_x);
			*object_y = (float)(Global::mouseRelativeY + offset_y);
			model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
		}
	}
}

void Editor::Selector::updateSpringMassNode()
{
	// Test if Mouse is Inside Node
	if (Global::mouseRelativeX > node_data.position.x - node_data.radius && Global::mouseRelativeX < node_data.position.x + node_data.radius && Global::mouseRelativeY > node_data.position.y - node_data.radius && Global::mouseRelativeY < node_data.position.y + node_data.radius)
	{
		// If Not Moving, Test if Node Should Move
		if (!moving)
		{
			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Highlight Color Should be Changed
			if (!mouse_intersects_object)
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				offset_x = *object_x - Global::mouseRelativeX;
				offset_y = *object_y - Global::mouseRelativeY;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			node_data.position -= object_data.position;
			deselectNode();
			level->reloadAll();
			return;
		}
	}

	// If Currently Moving, Move Node
	if (moving)
	{
		// Test if Node Should Stop Moving
		if (!Global::LeftClick)
			moving = false;

		// Move Node
		Global::Selected_Cursor = Global::CURSORS::HAND;
		*object_x = (float)(Global::mouseRelativeX + offset_x);
		*object_y = (float)(Global::mouseRelativeY + offset_y);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	}

	// Update Node in Origin Object
	*node_pointer = Object::Physics::Soft::Node(node_data);
	//std::cout << node_pointer->Position.x << " " << node_pointer->Position.y << "  i\n";
}

void Editor::Selector::updateSpringMassSpring()
{
	// IDEA:
	// Springs Can Only Be Moved by Their Ends
	// If a Spring is Moved, It Will Snap to the Node Whose Origin is Closest to the Coordinates of the Mouse
	// The Nodes That are Checked are The Nodes From the Same File as the Spring's Origin and The Positions of the Nodes Are Offset by The Position of the Individual Object the Spring is From
	// The Checking of the Closest Node Happens Continuously and Will be Applied When the Mouse is Moved

	// IDEA:
	// Springs Can Be Attachted Between the Same Two Nodes Multiple Times, Use the "SKIP" Key to Cycle Through Which Spring Should be Selected

	// Idea:
	// To Allow Selection of Multiple Springs to be Clearer and To Tell The User Which Spring is Selected, There Should be an Area of the Window Dedicated to Showing the Data for the Currently Moused Over Object
	// This Should Hold True for All Objects
	// For Most Objects, Name, Object Type, and Position Should be Displayed
	// For Springs, Spring Name (Index) and Spring Constant Should be Displayed

	// IDEA:
	// Springs Can be Moved By Parts That Are Not Ends, Moves Both Connected Nodes at the Same Time
	// This Also Means Springs Can be Rotated, Rotating Both Nodes Around The Pivot
	// When Saving the Changes to the Spring, Both the Nodes and Spring Must be Saved

	// Test if Mouse Intersects First Position
	if (glm::distance(connection_pos_left, glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY)) < 0.5f)
	{
		// If Not Moving, Test if Spring Should Move
		if (!moving)
		{
			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Highlight Color Should be Changed
			if (!mouse_intersects_object || !resizing)
				outlineForResize();

			// Set Values
			resizing = true;
			mouse_intersects_object = true;
			change_horizontal = 0;
			change_vertical = 0;

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				change_horizontal = 1;
			}
		}
	}

	// Test if Mouse Intersects Second Position
	else if (glm::distance(connection_pos_right, glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY)) < 0.5f)
	{
		// If Not Moving, Test if Spring Should Move
		if (!moving)
		{
			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Highlight Color Should be Changed
			if (!mouse_intersects_object || !resizing)
				outlineForResize();

			// Set Values
			resizing = true;
			mouse_intersects_object = true;
			change_horizontal = 0;
			change_vertical = 0;

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				change_vertical = 1;
			}
		}
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
		{
			deselectSpring();
			level->reloadAll();
		}
	}

	// If Currently Moving, Move Spring
	if (moving)
	{
		// Test if Node Should Stop Moving
		if (!Global::LeftClick)
			moving = false;

		// Move Endpoints
		if (resizing)
		{
			// Iterate Through All Nodes and Find the Closest Match to Mouse Position
			float shortest_length = 1000000.0f;
			float temp_distance = 0;
			int shortest_index = 0;
			for (int i = 0; i < node_count; i++)
			{
				temp_distance = glm::distance(node_list[i].position, glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));
				if (temp_distance < shortest_length)
				{
					shortest_length = temp_distance;
					shortest_index = i;
				}
			}

			// Set Endpoint to Closest Value if Index is New
			if (spring_data.Node1 != node_list[shortest_index].name && spring_data.Node2 != node_list[shortest_index].name)
			{
				glm::vec2 other_pos = glm::vec2(0.0f);

				// Left Endpoint
				if (change_horizontal)
				{
					spring_data.Node1 = node_list[shortest_index].name;
					connection_pos_left = node_list[shortest_index].position;
					for (int i = 0; i < node_count; i++)
						if (node_list[i].name == spring_data.Node2)
							other_pos = node_list[i].position;

				}

				// Right Endpoint
				else if (change_vertical)
				{
					spring_data.Node2 = node_list[shortest_index].name;
					connection_pos_right = node_list[shortest_index].position;
					for (int i = 0; i < node_count; i++)
						if (node_list[i].name == spring_data.Node1)
							other_pos = node_list[i].position;
				}

				// Reset Rest and Max Lengths
				spring_data.RestLength = glm::distance(node_list[shortest_index].position, other_pos);
				spring_data.MaxLength = 2 * spring_data.RestLength;

				// Change Vertices
				genSelectorVerticesSoftBody();
			}
		}
	}
}

void Editor::Selector::updateHinge()
{
}

void Editor::Selector::sortVertices(bool enable_rotation)
{
	// Rectangle
	if (editing_shape == RECTANGLE)
	{
		// Transform Data into Vertices
		float half_width = *object_width * 0.5f;
		float half_height = *object_height * 0.5f;
		float vertices[8] = {
			*object_x - half_width, *object_y - half_height, *object_x + half_width, *object_height - half_height,
			*object_x + half_width, *object_y + half_height, *object_x - half_width, *object_height + half_height
		};

		// Sort Vertices

		// Test if Bottom Vertices are Not Bottom
		if (vertices[1] < vertices[7])
		{
			float intermediate = 0;

			// Swap Vertex 1 and 4
			intermediate = vertices[1];
			vertices[1] = vertices[7];
			vertices[7] = intermediate;

			// Swap Vertex 2 and 3
			intermediate = vertices[3];
			vertices[3] = vertices[5];
			vertices[5] = intermediate;
		}

		// Test if Left Vertices are Not Left
		if (vertices[0] < vertices[2])
		{
			float intermediate = 0;

			// Swap Vertex 1 and 2
			intermediate = vertices[0];
			vertices[0] = vertices[2];
			vertices[2] = intermediate;

			// Swap Vertex 3 and 4
			intermediate = vertices[6];
			vertices[6] = vertices[4];
			vertices[4] = intermediate;
		}

		// Finsish Endpoints
		*object_x = (vertices[0] + vertices[2] + vertices[4] + vertices[6]) / 4;
		*object_y = (vertices[1] + vertices[3] + vertices[5] + vertices[7]) / 4;
		*object_width = abs(vertices[2] - vertices[0]);
		if (*object_width < 0.1f) { *object_width = 0.2f; }
		*object_height = abs(vertices[5] - vertices[3]);
		if (*object_height < 0.1f) { *object_height = 0.2f; }
	}

	// Trapezoid
	else if (editing_shape == TRAPEZOID)
	{
		// Transform Data into Vertices
		float half_width = *object_width * 0.5f;
		float half_height = *object_height * 0.5f;
		float vertices[8] = {
			*object_x - half_width,                          *object_y - half_height,
			*object_x + half_width,                          *object_y - half_height + *object_height_modifier,
			*object_x + half_width + *object_width_modifier, *object_y + half_height + *object_height_modifier,
			*object_x - half_width + *object_width_modifier, *object_y + half_height
		};

		// Rotate Object if Enabled
		if (enable_rotation)
		{
			// Rotate Each Individual Vertex
			for (int i = 0; i < 8; i += 2)
			{
				// Move Data Into Vertices Object
				glm::vec4 vertex = glm::vec4(vertices[i], vertices[i + 1], 0.0f, 1.0f);

				// Shift vertex to localized point around origin
				vertex[0] -= pivot[0];
				vertex[1] -= pivot[1];

				// Convert to polar coords
				float distance = (float)glm::sqrt(glm::pow(vertex[0], 2) + glm::pow(vertex[1], 2));
				double angle = atan(vertex[1] / vertex[0]); // + 3.1415927 * vertex[0] < 0;
				if (vertex[0] < 0)
					angle += 3.141593;

				// Apply Angle Manipulation
				angle -= Global::deltaMouse.x * Constant::ROTATION_CONSTANT;

				// Convert back to cartecian coords
				vertex[0] = distance * (float)cos(angle) + pivot[0];
				vertex[1] = distance * (float)sin(angle) + pivot[1];

				// Move Data Back Into Array
				vertices[i] = vertex.x;
				vertices[i + 1] = vertex.y;
			}
		}

		// Sort Vertices
		float intermediate = 0;

		// Swap Vertex 1 and 4
		if (vertices[7] < vertices[1])
		{
			// Swap X
			intermediate = vertices[0];
			vertices[0] = vertices[6];
			vertices[6] = intermediate;

			// Swap Y
			intermediate = vertices[1];
			vertices[1] = vertices[7];
			vertices[7] = intermediate;
		}

		// Swap Vertex 2 and 3
		if (vertices[5] < vertices[3])
		{
			// Swap X
			intermediate = vertices[2];
			vertices[2] = vertices[4];
			vertices[4] = intermediate;

			// Swap Y
			intermediate = vertices[3];
			vertices[3] = vertices[5];
			vertices[5] = intermediate;
		}

		// Swap Vertex 1 and 2
		if (vertices[2] < vertices[0])
		{
			// Swap X
			intermediate = vertices[0];
			vertices[0] = vertices[2];
			vertices[2] = intermediate;

			// Swap Y
			intermediate = vertices[1];
			vertices[1] = vertices[3];
			vertices[3] = intermediate;
		}

		// Swap Vertex 3 and 4
		if (vertices[4] < vertices[6])
		{
			// Swap X
			intermediate = vertices[6];
			vertices[6] = vertices[4];
			vertices[4] = intermediate;

			// Swap Y
			intermediate = vertices[7];
			vertices[7] = vertices[5];
			vertices[5] = intermediate;
		}

		// Finsish Endpoints
		*object_width = abs(vertices[2] - vertices[0]);
		if (*object_width < 0.1f) { *object_width = 0.2f; }
		*object_height = abs(vertices[7] - vertices[1]);
		if (*object_height < 0.1f) { *object_height = 0.2f; }
		*object_width_modifier = vertices[4] - vertices[2];
		*object_height_modifier = vertices[5] - vertices[7];
		*object_x = (vertices[2] + vertices[0]) / 2;
		*object_y = (vertices[7] + vertices[1]) / 2;
	}

	// Triangle
	else if (editing_shape == TRIANGLE)
	{
		// Transform Data into Vertices
		float vertices[6] = {
			coords1.x, coords1.y,
			coords2.x, coords2.y,
			coords3.x, coords3.y
			//*object_x + *object_width * cos(*object_width_modifier), *object_y + *object_width * sin(*object_width_modifier),
			//*object_x + *object_height * cos(*object_height_modifier), *object_y + *object_height * sin(*object_height_modifier),
		};

		// Rotate Object if Enabled
		if (enable_rotation)
		{
			// Calculate Rotation Matrix
			glm::mat4 rotation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
			rotation_matrix = glm::rotate(rotation_matrix, -Global::deltaMouse.x * Constant::ROTATION_CONSTANT, glm::vec3(0.0f, 0.0f, 1.0f));
			rotation_matrix = glm::translate(rotation_matrix, glm::vec3(-*object_x, -*object_y, 0.0f));

			// Rotate Each Individual Vertex
			for (int i = 0; i < 3; i++)
			{
				// Move Data Into Vertices Object
				glm::vec4 vertex = glm::vec4(vertices[i * 2], vertices[i * 2 + 1], 0.0f, 1.0f);

				vertex[0] -= pivot[0];
				vertex[1] -= pivot[1];

				// Convert to polar coords
				float distance = (float)glm::sqrt(glm::pow(vertex[0], 2) + glm::pow(vertex[1], 2));
				double angle = atan(vertex[1] / vertex[0]);
				if (vertex[0] < 0)
					angle += 3.141593;

				// Apply Angle Manipulation
				angle -= Global::deltaMouse.x * Constant::ROTATION_CONSTANT;

				// Convert back to cartecian coords
				vertex[0] = distance * (float)cos(angle) + pivot[0];
				vertex[1] = distance * (float)sin(angle) + pivot[1];

				// Perform Rotation
				//vertex = rotation_matrix * vertex;

				// Move Data Back Into Array
				vertices[i * 2] = vertex.x;
				vertices[i * 2 + 1] = vertex.y;
			}
		}

		// Sort Vertices
		coords1 = glm::vec2(vertices[0], vertices[1]);
		coords2 = glm::vec2(vertices[2], vertices[3]);
		coords3 = glm::vec2(vertices[4], vertices[5]);
		Source::Collisions::Point::arrangeTriVertices(coords1, coords2, coords3);

		// Store Vertices in Object Data
		object_data.position = coords1;
		triangle_data = Shape::Triangle(coords2, coords3);
		should_sort = true;
	}

	// Update VAO and VBO
	if (enable_rotation)
	{
		genSelectorVertices();
	}
}

void Editor::Selector::addChild()
{
	// SpringMass Node
	if (springmass_node_modified)
		addSpringMassNode();

	// SpringMass Spring
	else if (springmass_spring_modified)
		addSpringMassSpring();

	// Hinge Object
	else if (hinge_object_modified)
		addHingeObject();
}

void Editor::Selector::addSpringMassNode()
{
	// Temp Node Object
	node_data.mass = 1.0f;
	node_data.position = glm::vec2(1.0f, -1.0f);
	node_data.health = 1.0f;
	node_data.material = 1;
	node_data.radius = 1.0f;

	// Determine if the SpringMass File Exists. If So, Establish First Node
	std::filesystem::path temp_path = "../Resources/Models/Soft Bodies/" + file_name;
	std::error_code ec;
	if ((int)std::filesystem::file_size(temp_path, ec) == 0 || ec)
	{
		node_data.name = 0;
	}

	// Determine the Name of the New Node
	else
	{
		// Open File
		std::ifstream temp_file;
		temp_file.open(temp_path, std::ios::binary);

		// Determine the Number of Nodes
		uint8_t temp_byte = 0;
		uint8_t node_iterator = 0;
		temp_file.read((char*)&node_count, 1);
		temp_file.read((char*)&temp_byte, 1);

		// Use a Heap to Keep Track of Node Names
		Struct::MinHeap<Object::Physics::Soft::NodeData> heap(node_count);

		// Temp Data to Read To
		Object::Physics::Soft::NodeData temp_node_data;
		Object::Physics::Soft::Spring temp_spring_data;

		// The Highest Name Value
		int max_name = 0;

		// Reset the Node Count to Count Nodes Read
		node_count = 0;

		// Iterate Through File and Read Nodes and Springs
		while (!temp_file.eof())
		{
			// Read Type of Next Object
			temp_file.read((char*)&temp_byte, 1);

			// If File is Finished Reading, Stop Iterating
			if (temp_file.eof())
				break;

			// Read Node Into Heap
			if (temp_byte == 0)
			{
				temp_file.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
				heap.enqueue(temp_node_data);
				max_name = (max_name > temp_node_data.name) ? max_name : temp_node_data.name;
				node_count++;
			}

			// Read Spring Into Dummy Var
			else
			{
				temp_file.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
			}
		}

		// If The Highest Name is Equal to the Node Count - 1, It is Certain All Previous Names Are Filled. Increment Name
		if (max_name == node_count - 1)
			node_data.name = max_name + 1;

		// There is a Gap Within the Names
		else
		{
			// Dequeue the Names Until The Gap is Found
			int temp_compare_value = 0;
			bool should_dequeue = true;
			while (!heap.empty() && should_dequeue)
			{
				// Dequeue Next Node From Heap
				temp_node_data = heap.dequeue();

				// If The Current Name is Greater Than the Next Value, The Gap is Found. Store Name as Value of Gap
				if (temp_node_data.name > temp_compare_value)
				{
					node_data.name = temp_compare_value;
					should_dequeue = false;
				}

				// Gap is Not Yet Found, Increment Compare Value
				else
					temp_compare_value++;
			}

			// In the Event the Gap Was Not Found, Increment Name
			if (should_dequeue)
				node_data.name = temp_compare_value + 1;
		}
	}

	// Write Data
	deselectNode();
	deselectObject();
}

void Editor::Selector::addSpringMassSpring()
{
	// Determine if the SpringMass File Exists. If Not, Throw Error Since There Are No Nodes to Connect To
	std::filesystem::path temp_path = "../Resources/Models/Soft Bodies/" + file_name;
	std::error_code ec;
	if ((int)std::filesystem::file_size(temp_path, ec) == 0 || ec)
	{
		// Throw Error
		std::string message = "MISSING FILE DETECTED\n\nThe SpringMass File Does Not Yet Exist\n\nPlease Create At Least Two Nodes\nFirst or Select a Different File";
		notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
		return;
	}

	// Open SpringMass File and Create Spring
	else
	{
		// Open File
		std::ifstream temp_file;
		temp_file.open("../Resources/Models/Soft Bodies/" + file_name, std::ios::binary);

		// Determine the Number of Nodes
		uint8_t temp_byte = 0;
		uint8_t node_iterator = 0;
		temp_file.read((char*)&node_count, 1);
		temp_file.read((char*)&temp_byte, 1);

		// If There are Less Than Two Nodes, Throw Error
		if (node_count < 2)
		{
			// Close File
			temp_file.close();

			// Throw Error
			std::string message = "INVALID FILE DETECTED\n\nThe SpringMass File Does Not Have Enough\nValid Nodes\n\nPlease Create At Least Two Nodes\nFirst or Select a Different File";
			notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
			return;
		}

		// Read Nodes From File
		else
		{
			// Allocate Memory for Nodes
			node_list = new Object::Physics::Soft::NodeData[node_count];

			// Temp Data to Read To
			Object::Physics::Soft::NodeData temp_node_data;
			Object::Physics::Soft::Spring temp_spring_data;

			// Reset Node Count
			node_count = 0;

			// Read Rest of File
			while (!temp_file.eof())
			{
				// Read Type of Next Object
				temp_file.read((char*)&temp_byte, 1);

				// If File is Finished Reading, Stop Iterating
				if (temp_file.eof())
					break;

				// Read Node Into Array
				if (temp_byte == 0)
				{
					temp_file.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
					temp_node_data.position += object_data.position;
					node_list[node_count] = temp_node_data;
					node_count++;
				}

				// Read Spring Into Dummy Var
				else
				{
					temp_file.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
				}
			}

			// Close File
			temp_file.close();

			// Establish Spring
			spring_data.Node1 = node_list[0].name;
			spring_data.Node2 = node_list[1].name;
			spring_data.RestLength = glm::distance(node_list[0].position, node_list[1].position);
			spring_data.MaxLength = spring_data.RestLength * 2.0f;
			spring_data.Dampening = 1.0f;
			spring_data.Stiffness = 1.0f;

			// Write Data
			deselectSpring();
			deselectObject();
		}
	}
}

void Editor::Selector::addHingeObject()
{
}

void Editor::Selector::clampBase()
{
	// Clamp Object, if Enabled
	if (editor_data.clamp)
	{
		// Clamp Collision Masks
		if (object_identifier[0] == Object::MASK)
		{
			// Parse Mask Type
			switch (object_identifier[1])
			{

				// Horizontal Masks
			case Object::Mask::FLOOR:
			case Object::Mask::CEILING:
			{
				// Parse Horizontal Shape
				switch (object_identifier[2])
				{

				// Horizontal Line
				case Object::Mask::HORIZONTAL_LINE:
				{
					// Determine Endpoints of Object
					float half_width = *object_width * 0.5f;
					float endpoints[8] = { *object_x - half_width, *object_y, *object_x + half_width, *object_y };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(editor_data.clamp, endpoints, 0, 2, NULL, c_floor);
					else
						clampObjects(editor_data.clamp, endpoints, 0, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*object_width = abs(endpoints[2] - endpoints[0]);
					*object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				// Slant
				case Object::Mask::HORIZONTAL_SLANT:
				{
					// Determine Endpoints of Object
					float endpoints[8] = { *object_x, *object_y, *object_opposite_x, *object_opposite_y };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(editor_data.clamp, endpoints, 1, 2, NULL, c_floor);
					else
						clampObjects(editor_data.clamp, endpoints, 1, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*object_x = endpoints[0];
					*object_y = endpoints[1];
					*object_opposite_x = endpoints[2];
					*object_opposite_y = endpoints[3];

					break;
				}

				// Slope
				case Object::Mask::HORIZONTAL_SLOPE:
				{
					// Determine Endpoints of Object
					float half_width = *object_width * 0.5f;
					float half_height = *object_height * 0.5f;
					float endpoints[8] = { *object_x - half_width, *object_y - half_height, *object_x + half_width, *object_y + half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(editor_data.clamp, endpoints, 2, 2, NULL, c_floor);
					else
						clampObjects(editor_data.clamp, endpoints, 2, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*object_width = (endpoints[2] - endpoints[0]);
					*object_height = (endpoints[3] - endpoints[1]);
					*object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				}

				break;
			}

			// Vertical Masks
			case Object::Mask::LEFT_WALL:
			case Object::Mask::RIGHT_WALL:
			{
				// Parse Vertical Shape
				switch (object_identifier[2])
				{

				// Vertical Line
				case Object::Mask::VERTICAL_LINE:
				{
					// Determine Endpoints of Object
					float half_height = *object_height * 0.5f;
					float endpoints[8] = { *object_x, *object_y - half_height, *object_x, *object_y + half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::LEFT_WALL)
						clampObjects(editor_data.clamp, endpoints, object_identifier[1], 2, NULL, c_left);
					else
						clampObjects(editor_data.clamp, endpoints, object_identifier[1], 2, NULL, c_right);

					// Unpack Endpoints
					*object_height = endpoints[3] - endpoints[1];
					*object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				// Curve
				case Object::Mask::VERTICAL_CURVE:
				{
					// Determine Endpoints of Object
					int curve_direction = -1 + 2 * (object_identifier[1] == Object::Mask::RIGHT_WALL);
					float half_width = *object_width * 0.5f;
					float half_height = *object_height * 0.5f;
					float endpoints[8] = { *object_x + half_width * curve_direction, *object_y + half_height, *object_x - half_width * curve_direction, *object_y - half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::LEFT_WALL)
						clampObjects(editor_data.clamp, endpoints, object_identifier[1] + 2, 2, Algorithms::Math::getSign(*object_height), c_left);
					else
						clampObjects(editor_data.clamp, endpoints, object_identifier[1] + 2, 2, Algorithms::Math::getSign(*object_height), c_right);

					// Unpack Endpoints
					*object_width = abs(endpoints[0] - endpoints[2]);
					*object_height = endpoints[1] - endpoints[3];
					*object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				}

				break;
			}

			}
		}

		// Clamp Terrain
		else if (object_identifier[0] == Object::TERRAIN)
		{
			clampTerrain(object_identifier[2], object_identifier[1]);
		}
	}
}

void Editor::Selector::clampObjects(bool enabled, float(&endpoints)[8], int Type, int endpoint_count, int extraValue, func function)
{
	// Map to Get All Levels
	const glm::vec2 level_map[9] =
	{
		glm::vec2(-1, 1),
		glm::vec2(0, 1),
		glm::vec2(1, 1),
		glm::vec2(-1, 0),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(-1, -1),
		glm::vec2(0, -1),
		glm::vec2(1, -1)
	};

	// Only Clamp if Boolean is True
	if (enabled)
	{
		// Clamp For Both Endpoints
		for (int i = 0; i < 2 * endpoint_count; i += 2)
		{
			// Determine Level Location of Endpoint
			glm::vec2 endpoint_level;
			level->updateLevelPos(glm::vec2(endpoints[i], endpoints[i + 1]), endpoint_level);

			// Iterate Through Level Endpoint is In And All 8 Levels Surrounding It
			for (int j = 0; j < 9; j++)
			{
				glm::vec2 level_index = endpoint_level + level_map[j];

				// Get the Unsaved Level Endpoint is At
				Render::Objects::UnsavedLevel* unsaved_level = change_controller->getUnsavedLevel((int)level_index.x, (int)level_index.y, 0);

				// Perform Requested Clamp
				std::invoke(function, this, endpoints, unsaved_level, Type, i, extraValue);
			}
		}
	}
}

void Editor::Selector::clampFloorMasks(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue)
{
	// Return the Collision Mask Floor Objects From Level
	Object::Mask::Floor::FloorMask** floor_masks = nullptr;
	int floor_masks_size = 0;
	unsaved_level->returnFloorMasks(&floor_masks, floor_masks_size, object_index);

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < floor_masks_size; j++)
	{
		// Get Current Object
		Object::Mask::Floor::FloorMask* object = floor_masks[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Get Endpoints of Test Object
		glm::vec2 vertex_left, vertex_right;
		float testing_endpoints[4];
		object->getLeftRightEdgeVertices(vertex_left, vertex_right);
		testing_endpoints[0] = vertex_left.x;
		testing_endpoints[1] = vertex_left.y;
		testing_endpoints[2] = vertex_right.x;
		testing_endpoints[3] = vertex_right.y;

		//Vertices::Visualizer::visualizePoint(endpoints[0], endpoints[1], 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		//Vertices::Visualizer::visualizePoint(endpoints[2], endpoints[3], 1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

		//Vertices::Visualizer::visualizePoint(testing_endpoints[0], testing_endpoints[1], 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		//Vertices::Visualizer::visualizePoint(testing_endpoints[2], testing_endpoints[3], 1.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

		Global::activate_elusive_breakpoint = true;

		// Perform Endpoint Detection
		for (int k = 0; k < 4; k += 2)
		{
			// Calculate Distance Between Endpoints
			float distance = (float)glm::sqrt(glm::pow(endpoints[i] - testing_endpoints[k], 2) + glm::pow(endpoints[i + 1] - testing_endpoints[k + 1], 2));

			// Only Clamp if Objects Are Close Enough
			if (distance < Constant::CLAMP_CONSTANT)
			{
				// X Poisition
				endpoints[i] = testing_endpoints[k];

				// Y Positions
				if (Type == Object::Mask::HORIZONTAL_LINE)
				{
					Vertices::Visualizer::visualizePoint(testing_endpoints[k], testing_endpoints[k + 1], 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
					endpoints[1] = testing_endpoints[k + 1];
					endpoints[3] = testing_endpoints[k + 1];
				}

				else
				{
					endpoints[i + 1] = testing_endpoints[k + 1];
				}
			}
		}

		// Delete Object
		delete object;
	}

	// Delete Floor Masks Array
	delete[] floor_masks;

	// Search Through All in Collision Mask Walls to Determine if Object Should Clamp
}

void Editor::Selector::clampWallMasksLeft(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue)
{
	// Only Compare One Curve Endpoint if Curve
	if (Type > 1 && i == 2)
	{
		return;
	}

	// Return the Collision Mask Left Wall Objects From Level
	Object::Mask::Left::LeftMask** left_masks = nullptr;
	int left_masks_size = 0;
	unsaved_level->returnLeftMasks(&left_masks, left_masks_size, object_index);

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < left_masks_size; j++)
	{
		// Get Current Object
		int iterations = 2;
		Object::Mask::Left::LeftMask* object = left_masks[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Get Endpoints of Test Object
		glm::vec2 vertex_top, vertex_bottom;
		float testing_endpoints[4];
		object->getTopAndBottomEdgeVertices(vertex_top, vertex_bottom);
		testing_endpoints[0] = vertex_top.x;
		testing_endpoints[1] = vertex_top.y;
		testing_endpoints[2] = vertex_bottom.x;
		testing_endpoints[3] = vertex_bottom.y;
		if (object->type == 1)
			iterations = 1;

		// Perform Endpoint Detection
		for (int k = 0; k < 2 * iterations; k += 2)
		{
			// Calculate Distance Between Endpoints
			float distance = (float)glm::sqrt(glm::pow(endpoints[i] - testing_endpoints[k], 2) + glm::pow(endpoints[i + 1] - testing_endpoints[k + 1], 2));

			// Only Clamp if Objects Are Close Enough
			if (distance < Constant::CLAMP_CONSTANT)
			{
				// Y Position
				endpoints[i + 1] = testing_endpoints[k + 1];

				// X Positions
				if (Type < 2)
				{
					endpoints[0] = testing_endpoints[k];
					endpoints[2] = testing_endpoints[k];
				}

				else
				{
					endpoints[i] = testing_endpoints[k];
				}
			}
		}

		// Delete Object
		delete object;
	}

	// Delete Floor Masks Array
	delete[] left_masks;

	// Search Through All in Collision Mask Floors to Determine if Object Should Clamp

	// Search Through All in Collision Mask Reverse Floors to Determine if Object Should Clamp
}

void Editor::Selector::clampWallMasksRight(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue)
{
	// Only Compare One Curve Endpoint if Curve
	if (Type > 1 && i == 2)
	{
		return;
	}

	// Return the Collision Mask Left Wall Objects From Level
	Object::Mask::Right::RightMask** right_masks = nullptr;
	int right_masks_size = 0;
	unsaved_level->returnRightMasks(&right_masks, right_masks_size, object_index);

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < right_masks_size; j++)
	{
		// Get Current Object
		int iterations = 2;
		Object::Mask::Right::RightMask* object = right_masks[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Get Endpoints of Test Object
		glm::vec2 vertex_top, vertex_bottom;
		float testing_endpoints[4];
		object->getTopAndBottomEdgeVertices(vertex_top, vertex_bottom);
		testing_endpoints[0] = vertex_top.x;
		testing_endpoints[1] = vertex_top.y;
		testing_endpoints[2] = vertex_bottom.x;
		testing_endpoints[3] = vertex_bottom.y;
		if (object->type == 1)
			iterations = 1;

		// Perform Endpoint Detection
		for (int k = 0; k < 2 * iterations; k += 2)
		{
			// Calculate Distance Between Endpoints
			float distance = (float)glm::sqrt(glm::pow(endpoints[i] - testing_endpoints[k], 2) + glm::pow(endpoints[i + 1] - testing_endpoints[k + 1], 2));

			// Only Clamp if Objects Are Close Enough
			if (distance < Constant::CLAMP_CONSTANT)
			{
				// Y Position
				endpoints[i + 1] = testing_endpoints[k + 1];

				// X Positions
				if (Type < 2)
				{
					endpoints[0] = testing_endpoints[k];
					endpoints[2] = testing_endpoints[k];
				}

				else
				{
					endpoints[i] = testing_endpoints[k];
				}
			}
		}

		// Delete Object
		delete object;
	}

	// Delete Floor Masks Array
	delete[] right_masks;

	// Search Through All in Collision Mask Floors to Determine if Object Should Clamp

	// Search Through All in Collision Mask Reverse Floors to Determine if Object Should Clamp
}

void Editor::Selector::clampCeilingMasks(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue)
{
	// Return the Collision Mask Floor Objects From Level
	Object::Mask::Ceiling::CeilingMask** ceiling_masks = nullptr;
	int ceiling_masks_size = 0;
	unsaved_level->returnCeilingMasks(&ceiling_masks, ceiling_masks_size, object_index);

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < ceiling_masks_size; j++)
	{
		// Get Current Object
		Object::Mask::Ceiling::CeilingMask* object = ceiling_masks[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Get Endpoints of Test Object
		glm::vec2 vertex_left, vertex_right;
		float testing_endpoints[4];
		object->getLeftRightEdgeVertices(vertex_left, vertex_right);
		testing_endpoints[0] = vertex_left.x;
		testing_endpoints[1] = vertex_left.y;
		testing_endpoints[2] = vertex_right.x;
		testing_endpoints[3] = vertex_right.y;

		// Perform Endpoint Detection
		for (int k = 0; k < 4; k += 2)
		{
			// Calculate Distance Between Endpoints
			float distance = (float)glm::sqrt(glm::pow(endpoints[i] - testing_endpoints[k], 2) + glm::pow(endpoints[i + 1] - testing_endpoints[k + 1], 2));

			// Only Clamp if Objects Are Close Enough
			if (distance < Constant::CLAMP_CONSTANT)
			{
				// X Poisition
				endpoints[i] = testing_endpoints[k];

				// Y Positions
				if (Type == 0)
				{
					endpoints[1] = testing_endpoints[k + 1];
					endpoints[3] = testing_endpoints[k + 1];
				}

				else
				{
					endpoints[i + 1] = testing_endpoints[k + 1];
				}
			}
		}

		// Delete Object
		delete object;
	}

	// Delete Floor Masks Array
	delete[] ceiling_masks;

	// Search Through All in Collision Mask Walls to Determine if Object Should Clamp
}

void Editor::Selector::clampTerrain(int Shape, int Object)
{
	switch (Shape)
	{
	
	// Rectangle
	case Shape::RECTANGLE:
	{
		// Create Endpoints
		float half_width = *object_width * 0.5f;
		float half_height = *object_height * 0.5f;
		float endpoints[8] = {
			*object_x - half_width, *object_y - half_height, *object_x + half_width, *object_y - half_height,
			*object_x + half_width, *object_y + half_height, *object_x - half_width, *object_y + half_height
		};

		// Clamp
		clampObjects(editor_data.clamp, endpoints, Shape, 4, Object, c_terrain);

		// Finsish Endpoints
		*object_x = (endpoints[0] + endpoints[2] + endpoints[4] + endpoints[6]) * 0.25f;
		*object_y = (endpoints[1] + endpoints[3] + endpoints[5] + endpoints[7]) * 0.25f;
		*object_width = abs(endpoints[2] - endpoints[0]);
		*object_height = abs(endpoints[5] - endpoints[3]);

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Create Endpoints
		float half_width = *object_width * 0.5f;
		float half_height = *object_height * 0.5f;
		float endpoints[8] = {
			*object_x - half_width,                          *object_y - half_height,
			*object_x + half_width,                          *object_y - half_height + *object_height_modifier,
			*object_x + half_width + *object_width_modifier, *object_y + half_height + *object_height_modifier,
			*object_x - half_width + *object_width_modifier, *object_y + half_height
		};

		// Clamp
		clampObjects(editor_data.clamp, endpoints, Shape, 4, Object, c_terrain);

		// Finsish Endpoints
		*object_width = abs(endpoints[2] - endpoints[0]);
		*object_height = abs(endpoints[7] - endpoints[1]);
		*object_width_modifier = endpoints[4] - endpoints[2];
		*object_height_modifier = endpoints[5] - endpoints[7];
		*object_x = (endpoints[2] + endpoints[0]) * 0.5f;
		*object_y = (endpoints[7] + endpoints[1]) * 0.5f;

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Create Endpoints
		float endpoints[8] = {
			coords1.x, coords1.y,
			coords2.x, coords2.y,
			coords3.x, coords3.y,
			0.0f, 0.0f
		};

		// Clamp
		clampObjects(editor_data.clamp, endpoints, Shape, 3, Object, c_terrain);

		// Sort Endpoints
		coords1 = glm::vec2(endpoints[0], endpoints[1]);
		coords2 = glm::vec2(endpoints[2], endpoints[3]);
		coords3 = glm::vec2(endpoints[4], endpoints[5]);
		Source::Collisions::Point::arrangeTriVertices(coords1, coords2, coords3);

		break;
	}
	}
}

void Editor::Selector::clampTerrainHelper(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue)
{
	// Calculate Midpoints of Object
	float midpoints[8] =
	{
		(endpoints[0] + endpoints[2]) * 0.5f, (endpoints[1] + endpoints[3]) * 0.5f,
		(endpoints[2] + endpoints[4]) * 0.5f, (endpoints[3] + endpoints[5]) * 0.5f,
		(endpoints[4] + endpoints[6]) * 0.5f, (endpoints[5] + endpoints[7]) * 0.5f,
		(endpoints[6] + endpoints[0]) * 0.5f, (endpoints[7] + endpoints[1]) * 0.5f
	};

	Vertices::Visualizer::visualizePoint(endpoints[0], endpoints[1], 1.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(endpoints[2], endpoints[3], 1.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(endpoints[4], endpoints[5], 1.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	Vertices::Visualizer::visualizePoint(endpoints[6], endpoints[7], 1.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	//Global::activate_elusive_breakpoint = true;

	// Get the Terrain Objects to Compare to Based on Layer
	Object::Terrain::TerrainBase** terrain_objects;
	int terrain_objects_size = 0;
	unsaved_level->returnTerrainObjects(&terrain_objects, terrain_objects_size, object_identifier[1], object_index);

	// Perform Actual Clamping
	abstractedClampTerrain(endpoints, midpoints, Type, i, terrain_objects_size, terrain_objects);

	// Delete Terrain Objects
	for (int i = 0; i < terrain_objects_size; i++)
		delete terrain_objects[i];
	delete[] terrain_objects;
}

void Editor::Selector::abstractedClampTerrain(float(&endpoints)[8], float(&midpoints)[8], int Type, int i, int max, Object::Terrain::TerrainBase** data)
{
	// Transform Current Endpoint Into a Line
	float slope1, intercept1;
	glm::vec2 midpoint1 = glm::vec2(0.0f, 0.0f);
	try
	{
		if ((Type == Shape::TRIANGLE && i == 4) || (Type < Shape::TRIANGLE && i == 6)) { throw "OutOfRange"; }
		slope1 = (endpoints[i + 1] - endpoints[i + 3]) / (endpoints[i] - endpoints[i + 2]);
		//midpoint1 = glm::vec2((endpoints[i] + endpoints[i + 2]) / 2, (endpoints[i + 1] + endpoints[i + 3]) / 2);
		//std::cout << "1 " << endpoints[i] << " " << endpoints[i + 1] << "  " << endpoints[i + 2] << " " << endpoints[i + 3] << "\n";
	}

	catch (const char* e)
	{
		e = nullptr;
		slope1 = (endpoints[i + 1] - endpoints[1]) / (endpoints[i] - endpoints[0]);
		//midpoint1 = glm::vec2((endpoints[i] + endpoints[0]) / 2, (endpoints[i + 1] + endpoints[1]) / 2);
		//std::cout << "2 " << endpoints[i] << " " << endpoints[i + 1] << "  " << endpoints[0] << " " << endpoints[1] << "\n";
	}
	intercept1 = endpoints[i + 1] - slope1 * endpoints[i];
	midpoint1 = glm::vec2(midpoints[i], midpoints[i + 1]);
	//std::cout << slope1 << " n\n";
	//std::cout << slope1 << " " << midpoint1.x << " " << midpoint1.y << "  v\n";

	for (int j = 0; j < max; j++)
	{
		float* testing_endpoints = 0;
		int iterations = 0;
		int second_type = 0;
		Object::Terrain::TerrainBase* object = data[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Parse Object Shape
		Shape::Shape* shape = object->returnShapePointer();
		switch (shape->shape)
		{

		// Rectangle
		case Shape::RECTANGLE:
		{
			Shape::Rectangle& temp_rect = *static_cast<Shape::Rectangle*>(shape);
			iterations = 4;
			second_type = Shape::RECTANGLE;
			float half_width = *temp_rect.pointerToWidth() * 0.5f;
			float half_height = *temp_rect.pointerToHeight() * 0.5f;
			testing_endpoints = new float[8] {
				object->returnPosition().x - half_width, object->returnPosition().y - half_height,
				object->returnPosition().x + half_width, object->returnPosition().y - half_height,
				object->returnPosition().x + half_width, object->returnPosition().y + half_height,
				object->returnPosition().x - half_width, object->returnPosition().y + half_height
			};

			Vertices::Visualizer::visualizePoint(testing_endpoints[0], testing_endpoints[1], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[2], testing_endpoints[3], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[4], testing_endpoints[5], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[6], testing_endpoints[7], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(object->returnPosition(), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			//Global::activate_elusive_breakpoint = true;

			break;
		}

		// Trapezoid
		case Shape::TRAPEZOID:
		{
			Shape::Trapezoid& temp_trap = *static_cast<Shape::Trapezoid*>(shape);
			iterations = 4;
			second_type = Shape::TRAPEZOID;
			float half_width = *temp_trap.pointerToWidth() * 0.5f;
			float half_height = *temp_trap.pointerToHeight() * 0.5f;
			testing_endpoints = new float[8] {
				object->returnPosition().x - half_width,                                     object->returnPosition().y - half_height,
				object->returnPosition().x + half_width,                                     object->returnPosition().y - half_height + *temp_trap.pointerToHeightOffset(),
				object->returnPosition().x + half_width + *temp_trap.pointerToWidthOffset(), object->returnPosition().y + half_height + *temp_trap.pointerToHeightOffset(),
				object->returnPosition().x - half_width + *temp_trap.pointerToWidthOffset(), object->returnPosition().y + half_height
			};

			break;
		}

		// Triangle
		case Shape::TRIANGLE:
		{
			Shape::Triangle& temp_tri = *static_cast<Shape::Triangle*>(shape);
			iterations = 3;
			second_type = Shape::TRIANGLE;
			testing_endpoints = new float[6] {
				object->returnPosition().x, object->returnPosition().y,
				temp_tri.pointerToSecondPosition()->x, temp_tri.pointerToSecondPosition()->y,
				temp_tri.pointerToThirdPosition()->x, temp_tri.pointerToThirdPosition()->y,
			};

			break;
		}

		}

		// Perform Linear Detection
		for (int k = 0; k < 2 * iterations; k += 2)
		{
			// First Test if Endpoints Are Close
			if (glm::sqrt(glm::pow(endpoints[i] - testing_endpoints[k], 2) + glm::pow(endpoints[i + 1] - testing_endpoints[k + 1], 2)) < Constant::CLAMP_CONSTANT)
			{
				// Make Lines in Rectangle Stay Vertical/Horizontal
				if (Type == Shape::RECTANGLE)
				{
					// Move Current Endpoint
					endpoints[i] = testing_endpoints[k];
					endpoints[i + 1] = testing_endpoints[k + 1];

					// Change Y of Next Endpoint, Change X of Previous Endpoint
					if (i % 4 == 0)
					{
						// Change Y
						if (i == 6) { endpoints[1] = testing_endpoints[k + 1]; }
						else { endpoints[i + 3] = testing_endpoints[k + 1]; }

						// Change X
						if (i == 0) { endpoints[6 - 2 * (Type == Shape::TRAPEZOID)] = testing_endpoints[k]; }
						else { endpoints[i - 2] = testing_endpoints[k]; }
					}

					// Change X of Next Endpoint, Change Y of Previous Endpoint
					else
					{
						// Change X
						if (i == 6) { endpoints[0] = testing_endpoints[k]; }
						else { endpoints[i + 2] = testing_endpoints[k]; }

						// Change Y
						if (i == 0) { endpoints[7 - 2 * (Type == Shape::TRAPEZOID)] = testing_endpoints[k + 1]; }
						else { endpoints[i - 1] = testing_endpoints[k + 1]; }
					}
				}

				// Change Endpoints to Make them Match
				else
				{
					endpoints[i] = testing_endpoints[k];
					endpoints[i + 1] = testing_endpoints[k + 1];
				}

				break;
			}

			// Second, Test Lines
			else if (true)
			{
				// Calculate Testing Line
				float slope2, intercept2;
				glm::vec2 midpoint2 = glm::vec2(0.0f, 0.0f);
				try
				{
					if ((second_type == Shape::TRIANGLE && k == 4) || (second_type < Shape::TRIANGLE && k == 6)) { throw "OutOfRange"; }
					slope2 = (testing_endpoints[k + 1] - testing_endpoints[k + 3]) / (testing_endpoints[k] - testing_endpoints[k + 2]);
					midpoint2 = glm::vec2((testing_endpoints[k] + testing_endpoints[k + 2]) * 0.5f, (testing_endpoints[k + 1] + testing_endpoints[k + 3]) * 0.5f);
				}

				catch (const char* e)
				{
					e = nullptr;
					slope2 = (testing_endpoints[k + 1] - testing_endpoints[1]) / (testing_endpoints[k] - testing_endpoints[0]);
					midpoint2 = glm::vec2((testing_endpoints[k] + testing_endpoints[0]) * 0.5f, (testing_endpoints[k + 1] + testing_endpoints[1]) * 0.5f);
				}
				intercept2 = testing_endpoints[k + 1] - slope2 * testing_endpoints[k];

				// Test if Direction Of Clamp Lines Matches
				bool matching_sides = Type == Shape::TRIANGLE;
				if (second_type == Shape::TRAPEZOID)
				{
					// Test if Line is the Closest to Test Line
					float test_distance = glm::distance(midpoint1, midpoint2);
					float differences[4] = { glm::distance(midpoint2, glm::vec2(midpoints[0], midpoints[1])), glm::distance(midpoint2, glm::vec2(midpoints[2], midpoints[3])), glm::distance(midpoint2, glm::vec2(midpoints[4], midpoints[5])), glm::distance(midpoint2, glm::vec2(midpoints[6], midpoints[7])) };
					if (test_distance <= differences[0] && test_distance <= differences[1] && test_distance <= differences[2] && test_distance <= differences[3])
					{
						matching_sides = true;
					}
				}

				else if (second_type == Shape::RECTANGLE)
				{
					if ((abs(midpoint1.y - midpoint2.y) < Constant::CLAMP_CONSTANT && k % 4 == 0) || (abs(midpoint1.x - midpoint2.x) < Constant::CLAMP_CONSTANT && k % 4 == 2))
					{

						matching_sides = true;
					}
				}

				// Slopes Are Vertical
				if (abs(atan(slope2)) > 1.35f)
				{
					if (abs(atan(slope1)) > 1.35f)
					{
						//continue;
					}

					if (abs(atan(slope1)) > 1.35f && ((abs(midpoint1.x - midpoint2.x) < Constant::CLAMP_CONSTANT) || (abs(midpoint1.y - midpoint2.y) < Constant::CLAMP_CONSTANT)) && matching_sides)
					{
						// Move X Endpoints To Meet Object
						endpoints[i] = testing_endpoints[k];
						try
						{
							if ((Type == Shape::TRIANGLE && i == 4) || (Type < Shape::TRIANGLE && i == 6)) { throw "OutOfRange"; }
							endpoints[i] = testing_endpoints[k];
						}

						catch (const char* e)
						{
							e = nullptr;
							endpoints[0] = testing_endpoints[k];
						}
					}

					//break;
				}

				// Test If Slopes and Intercepts are in Acceptable Ranges
				else if (abs(atan(slope1) - atan(slope2)) < 0.05f && ((abs(midpoint1.x - midpoint2.x) < Constant::CLAMP_CONSTANT) || (abs(midpoint1.y - midpoint2.y) < Constant::CLAMP_CONSTANT)) && matching_sides)
				{
					// Rectangle 
					if (Type == Shape::RECTANGLE)
					{
						// Move Rectangle Y to Average of Values
						if (i % 4 == 0)
						{
							endpoints[i + 1] = midpoint2.y;
							try
							{
								if (i == 6) { throw "OutOfRange"; }
								endpoints[i + 3] = midpoint2.y;
							}

							catch (const char* e)
							{
								e = nullptr;
								endpoints[1] = midpoint2.y;
							}
						}

						// Move Rectangle X to Average of Values
						else
						{
							endpoints[i] = midpoint2.x;
							try
							{
								if (i == 6) { throw "OutOfRange"; }
								endpoints[i + 2] = midpoint2.x;
							}

							catch (const char* e)
							{
								e = nullptr;
								endpoints[0] = midpoint2.x;
							}
						}
					}

					// Triangle and Trapezoid
					else
					{
						// Slope 2 Is Horizontal
						if (slope2 == 0)
						{
							// Move Y Endpoints To Meet Object
							endpoints[i + 1] = testing_endpoints[k + 1];
							try
							{
								if ((Type == Shape::TRIANGLE && i == 4) || (Type < Shape::TRIANGLE && i == 6)) { throw "OutOfRange"; }
								endpoints[i + 3] = testing_endpoints[k + 1];
							}

							catch (const char* e)
							{
								e = nullptr;
								endpoints[1] = testing_endpoints[k + 1];
							}
						}

						// Slope 2 Is Not Horizontal
						else
						{
							// Calculte Two Slopes Perpendicular to Testing Slope That Pass Through Endpoints
							float perpendicular_slope = -1 / slope2;
							float perpendicular_intercept1 = endpoints[i + 1] - perpendicular_slope * endpoints[i];
							float perpendicular_intercept2;
							try
							{
								if ((Type == Shape::TRIANGLE && i == 4) || (Type < Shape::TRIANGLE && i == 6)) { throw "OutOfRange"; }
								perpendicular_intercept2 = endpoints[i + 3] - perpendicular_slope * endpoints[i + 2];
							}

							catch (const char* e)
							{
								e = nullptr;
								perpendicular_intercept2 = endpoints[1] - perpendicular_slope * endpoints[0];
							}

							// Calculate Intersection Points On The Lines
							glm::vec2 intersect1, intersect2 = glm::vec2(0.0f, 0.0f);
							intersect1.x = (intercept2 - perpendicular_intercept1) / (perpendicular_slope - slope2);
							intersect1.y = perpendicular_slope * intersect1.x + perpendicular_intercept1;
							intersect2.x = (intercept2 - perpendicular_intercept2) / (perpendicular_slope - slope2);
							intersect2.y = perpendicular_slope * intersect2.x + perpendicular_intercept2;

							// Move Endpoints in a Line Perpendicular to Testing Line
							endpoints[i] = intersect1.x;
							endpoints[i + 1] = intersect1.y;
							try
							{
								if ((Type == Shape::TRIANGLE && i == 4) || (Type < Shape::TRIANGLE && i == 6)) { throw "OutOfRange"; }
								endpoints[i + 2] = intersect2.x;
								endpoints[i + 3] = intersect2.y;
							}

							catch (const char* e)
							{
								e = nullptr;
								endpoints[0] = intersect2.x;
								endpoints[1] = intersect2.y;
							}
						}

					}

					//break;
				}
			}
		}
	}
}

