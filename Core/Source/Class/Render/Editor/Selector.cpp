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
#include "Object/Object.h"
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
#include "Object/Physics/SoftBody/SpringMass.h"
#include "Object/Physics/SoftBody/Wire.h"
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

#include "Render/Struct/DataClasses.h"

#include "Render/Objects/UnsavedGroup.h"
#include "Render/Objects/UnsavedCollection.h"
#include "Render/Objects/UnsavedComplex.h"

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
	glGenVertexArrays(1, &highlighted_vertex_objects.objectVAO);
	glGenBuffers(1, &highlighted_vertex_objects.objectVBO);

	// Generate Vertex Object for Outline
	glGenVertexArrays(1, &highlighted_vertex_objects.outlineVAO);
	glGenBuffers(1, &highlighted_vertex_objects.outlineVBO);

	// Generate Vertex Object for Pivot
	genPivotVertices();

	// Generate the Copy Buffer
	glGenBuffers(1, &copyBuffer);

	// Initialize Window
	initializeWindow();
	genBackground();

	// Generate Vertex Object for Group Selector
	glGenVertexArrays(1, &group_selector.outlineVAO);
	glGenBuffers(1, &group_selector.outlineVBO);

	// Bind Group Vertex Objects
	glBindVertexArray(group_selector.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, group_selector.outlineVBO);

	// Enable Position Vertices for Group VAO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Vertices for Group VBO
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Store Group Selector Resizing Pointer
	Selected_Object::group_selector = &group_selector;

	// Null-Initialize Global Position Pointers
	first_position_global = &Global::dummy_vec2;
	second_position_global = &Global::dummy_vec2;
	third_position_global = &Global::dummy_vec2;

	// Set Selected Object Initial Values
	Selected_Object::moving = false;
	Selected_Object::resizing = false;
	Selected_Object::mouse_intersects_object = false;
	Selected_Object::genSelectorVertices = [this](DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)->void { genSelectorVertices(data_object, vertex_objects); };
	Selected_Object::sortVertices = [this](bool enable_rotation, Selected_Object* selected_object)->void { sortVertices(enable_rotation, selected_object); };
	Selected_Object::storeTempConnectionPos = [this](glm::vec2& left, glm::vec2& right)->void {setTempConnectionPos(left, right); };
	Selected_Object::updateSelectedPositions = [this](DataClass::Data_Object* data_object, float deltaX, float deltaY)->void {updateSelectedPositions(data_object, deltaX, deltaY); };
}

void Editor::Selector::activateHighlighter(glm::vec2 offset, SelectedHighlight mode)
{
	// Allocate Memory for Selector Vertices
	allocateSelectorVertices(highlighted_object, highlighted_vertex_objects);

	// Generate Selector Vertices 
	genSelectorVertices(highlighted_object, highlighted_vertex_objects);

	// Apply Offset, If Needed
	highlighted_vertex_objects.model = glm::translate(highlighted_vertex_objects.model, glm::vec3(offset.x, offset.y, 0.0f));

	// Bind Outline Vertex Objects
	glBindVertexArray(highlighted_vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, highlighted_vertex_objects.outlineVBO);

	// Apply the Highlighter Color, If it is Different
	if (mode != SelectedHighlight::SELECTABLE)
	{
		for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
			glBufferSubData(GL_ARRAY_BUFFER, i, 3 * sizeof(GL_FLOAT), glm::value_ptr(highlight_colors[mode]));
	}

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
	// If Objects are Queued for Selection, Integrate into Selector
	if (!unadded_data_objects.empty())
		initializeSelector();

	// If Editing, Edit Object
	else if (editing)
		editObject();

	// Update Object Info
	updateInfoPositions();
}

void Editor::Selector::blitzSelector()
{
	// Draw Object
	for (Selected_Object* selected_object : selected_objects)
	{
		// Get Rendering Data
		Selected_VertexObjects& vertex_objects = selected_object->vertex_objects;

		// Determine Delta Pos, If Needed
		glm::vec3 delta_pos = glm::vec3(0.0f, 0.0f, 0.0f);
		if (selected_object->complex_root != nullptr)
			delta_pos = glm::vec3(*selected_object->object_x - selected_object->complex_root->pointerToPosition()->x, *selected_object->object_y - selected_object->complex_root->pointerToPosition()->y, 0.0f);

		// Object is a Object With Color and Texture
		if (selected_object->vertex_objects.visualize_object)
		{
			// Bind Object Shader
			Global::objectShaderStatic.Use();

			// Send Matrix to Shader
			//glm::mat4 matrix = level->returnProjectionViewMatrix(4) * model;
			//glUniformMatrix4fv(Global::objectStaticMatrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));
			Global::modelLocObjectStatic = glGetUniformLocation(Global::objectShaderStatic.Program, "model");
			glUniformMatrix4fv(Global::modelLocObjectStatic, 1, GL_FALSE, glm::value_ptr(vertex_objects.model));
			glUniform4f(glGetUniformLocation(Global::objectShaderStatic.Program, "view_pos"), level->camera->Position.x, level->camera->Position.y, 0.0f, 0.0f);
			//glUniform1f(glGetUniformLocation(Global::objectShaderStatic.Program, "material.shininess"), 1.0);

			// Draw Object
			glBindVertexArray(vertex_objects.objectVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertex_objects.object_vertex_count);

			// If Belonging to a Complex Root, Draw Other Instances
			if (selected_object->complex_root != nullptr)
				static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelected(vertex_objects.object_vertex_count, GL_TRIANGLES, Global::modelLocObjectStatic, delta_pos, selected_object->complex_root);

			// Unbind Object
			glBindVertexArray(0);
		}

		// Object is Composed of Lines
		else if (vertex_objects.visualize_lines)
		{
			// Bind Static Color Shader
			Global::colorShaderStatic.Use();
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(vertex_objects.model));

			// Draw Object
			glBindVertexArray(vertex_objects.objectVAO);
			glDrawArrays(GL_LINES, 0, vertex_objects.object_vertex_count);

			// If Belonging to a Complex Root, Draw Other Instances
			if (selected_object->complex_root != nullptr)
				static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelected(vertex_objects.object_vertex_count, GL_LINES, Global::modelLocColorStatic, delta_pos, selected_object->complex_root);

			// Unbind Object
			glBindVertexArray(0);
		}

		// Object Only Has a Texture
		else if (vertex_objects.visualize_texture)
		{
			// Bind Static Texture Shader
			Global::texShaderStatic.Use();
			glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(vertex_objects.model));

			// Load Texture
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, texture.texture);

			// Draw Object
			glBindVertexArray(vertex_objects.objectVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertex_objects.object_vertex_count);

			// If Belonging to a Complex Root, Draw Other Instances
			if (selected_object->complex_root != nullptr)
				static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelected(vertex_objects.object_vertex_count, GL_TRIANGLES, Global::modelLocTextureStatic, delta_pos, selected_object->complex_root);

			// Unbind Object
			glBindVertexArray(0);
		}

		// Object Only Has a Color
		else
		{
			// Bind Static Color Shader
			Global::colorShaderStatic.Use();
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(vertex_objects.model));

			// Draw Object
			glBindVertexArray(vertex_objects.objectVAO);
			glDrawArrays(GL_TRIANGLES, 0, vertex_objects.object_vertex_count);

			// If Belonging to a Complex Root, Draw Other Instances
			if (selected_object->complex_root != nullptr)
				static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelected(vertex_objects.object_vertex_count, GL_TRIANGLES, Global::modelLocColorStatic, delta_pos, selected_object->complex_root);

			// Unbind Object
			glBindVertexArray(0);
		}

		// Draw Highlighter

		// Bind Highlighter Shader
		Global::colorShaderStatic.Use();
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(vertex_objects.model));

		// Draw Highllighter
		glBindVertexArray(vertex_objects.outlineVAO);
		glDrawArrays(GL_LINES, 0, vertex_objects.outline_vertex_count);
		glBindVertexArray(0);
	}

	// Draw Pivot, if Enabled
	if (Selected_Object::rotating)
	{
		glm::mat4 pivot_model = glm::translate(glm::mat4(1.0f), glm::vec3(pivot.x, pivot.y, 0.0f));
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(pivot_model));
		glBindVertexArray(pivotVAO);
		glDrawArrays(GL_TRIANGLES, 0, 30);
		glBindVertexArray(0);
	}

	// Draw Group Selector, If Enabled
	if (selected_objects.size() > 1)
	{
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(group_selector.model));
		glBindVertexArray(group_selector.outlineVAO);
		glDrawArrays(GL_LINES, 0, 8);
		glBindVertexArray(0);
	}

	// Draw Visualizers for Groups
	for (Selected_Object* selected_object : selected_objects)
	{
		// If This is a Complex Object, Draw As if From Complex Object
		if (selected_object->data_object->getGroup() != nullptr && selected_object->data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		{
			// If This Object Has a Nested Complex Parent, Draw to All Instances
			if (selected_object->complex_root != nullptr)
			{
				// Determine the Real Object That Was Selected
				for (Object::Object* object : selected_object->data_object->getObjects())
				{
					if (static_cast<Object::TempObject*>(object)->isOriginal())
					{
						// Determine the Offset of the Real Object
						glm::vec2 offset = glm::vec2(*selected_object->object_x, *selected_object->object_y) - *object->pointerToPosition();

						// Draw Connections To All Children
						for (Object::Object* object : selected_object->data_object->getObjects())
						{
							glm::vec2 combined_offest = *object->pointerToPosition() + offset;
							selected_object->data_object->drawSelectedGroupVisualizerOffset(combined_offest, combined_offest - selected_object->data_object->getPosition());
						}

						break;
					}
				}
			}

			// Else, Draw Only Connections For This Object
			else
				selected_object->data_object->drawSelectedGroupVisualizer(glm::vec2(*selected_object->object_x, *selected_object->object_y));

			// If Object Has a Parent, Draw Parent Connection
			if (selected_object->complex_root == nullptr)
				selected_object->data_object->drawParentConnection();
			else
			{
				// Get Delta Position of Selected Object
				glm::vec2 delta_pos = *selected_object->complex_root->pointerToPosition();

				// Draw Connection ONLY to Parent
				static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelectedConnectionParentOnly(selected_object->data_object, delta_pos);
			}
		}

		// For Children of Normal Group Objects, Draw Parent Connection
		else if (selected_object->complex_root == nullptr)
		{
			// Draw Visualizers to Children
			selected_object->data_object->drawSelectedGroupVisualizer(glm::vec2(0.0f, 0.0f));

			// Draw Visualizer to Parent
			selected_object->data_object->drawParentConnection();
		}
			
		// For Children of Complex Objects, Draw Connection for All Instances
		else
		{
			// Get Delta Position of Selected Object
			glm::vec2 delta_pos = *selected_object->complex_root->pointerToPosition();

			// Draw Connection for All Instances
			static_cast<Render::Objects::UnsavedComplex*>(selected_object->complex_root->group_object)->drawSelectedConnection(selected_object->data_object, delta_pos);
		}
	}
}

void Editor::Selector::blitzHighlighter()
{
	// Bind Highlighter Shader
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highlighted_vertex_objects.model));

	// Draw Highllighter
	glBindVertexArray(highlighted_vertex_objects.outlineVAO);
	glDrawArrays(GL_LINES, 0, highlighted_vertex_objects.outline_vertex_count);
	glBindVertexArray(0);
}

void Editor::Selector::deselectObject()
{
	// Selector is Not Loaded
	active = false;
	editing = false;
	highlighting = false;

	// Clamp Objects, if Needed
	for (Selected_Object* selected_object : selected_objects)
		clampBase(selected_object);

	// Handle the Desselection of Any Nodes
	for (int i = 0; i < selected_objects.size(); i++)
	{
		if (selected_objects.at(i)->editing_shape == SPRINGMASS_NODE)
		{
			deselectNode(static_cast<Selected_SpringMassNode*>(selected_objects.at(i)));
			selected_objects.erase(selected_objects.begin() + i);
			data_objects.erase(data_objects.begin() + i);
			i--;
		}

		else if (selected_objects.at(i)->editing_shape == SPRINGMASS_SPRING)
		{
			deselectSpring(static_cast<Selected_SpringMassSpring*>(selected_objects.at(i)));
			selected_objects.erase(selected_objects.begin() + i);
			data_objects.erase(data_objects.begin() + i);
			i--;
		}
	}

	// Return the Object
	change_controller->handleSelectorReturn(this);

	// Delete the Selected Object Container
	for (Selected_Object* selected_object : selected_objects)
		delete selected_object;
	selected_objects.clear();

	// Make All Object Selectable Again
	makeSelectable();

	// Reset Some Variables
	originated_from_level = false;
	add_child_object = CHILD_OBJECT_TYPES::NONE;
	add_child = false;
	//object_index = 0;
	moused_object = nullptr;

	// Erase Data Objects
	data_objects.clear();
}

void Editor::Selector::deselectNode(Selected_SpringMassNode* selected_object)
{
	// Reset Connected List if Needed
	if (connected_limbs_count)
	{
		delete[] connected_limbs;
		connected_limbs_count = 0;
	}

	// Get SpringMass File Name
	std::string& file_name = static_cast<DataClass::Data_SpringMassNode*>(selected_object->data_object)->getParent()->getFile();

	// Get Node Data
	Object::Physics::Soft::NodeData& node_data = static_cast<DataClass::Data_SpringMassNode*>(selected_object->data_object)->getNodeData();

	// Make Position of Node Relative to Object
	static_cast<DataClass::Data_SpringMassNode*>(selected_object->data_object)->getNodeData().position -= static_cast<DataClass::Data_SpringMassNode*>(selected_object->data_object)->getParent()->getPosition();

	// Copy File Data Into Stream
	std::stringstream file_stream;
	std::ifstream in_file;
	in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);
	file_stream << in_file.rdbuf();
	in_file.close();

	// Open File for Writing
	std::ofstream out_file;
	out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);

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
	add_child_object = CHILD_OBJECT_TYPES::NONE;

	// Reset Some Variables
	active = false;
	editing = false;
	highlighting = false;
	originated_from_level = false;
	//object_index = 0;
	moused_object = nullptr;
	uuid = 0;

	// Erase Data Objects
	//data_objects.clear();
}

void Editor::Selector::deselectSpring(Selected_SpringMassSpring* selected_object)
{
	// Get SpringMass File Name
	std::string& file_name = static_cast<DataClass::Data_SpringMassSpring*>(selected_object->data_object)->getParent()->getFile();

	// Get Node Data
	Object::Physics::Soft::Spring& spring_data = static_cast<DataClass::Data_SpringMassSpring*>(selected_object->data_object)->getSpringData();

	// Copy File Data Into Stream
	std::stringstream file_stream;
	std::ifstream in_file;
	in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);
	file_stream << in_file.rdbuf();
	in_file.close();

	// Open File for Writing
	std::ofstream out_file;
	out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);

	// Copy the Number of Nodes
	char temp_byte;
	file_stream.read(&temp_byte, 1);
	out_file.put(temp_byte);

	// Copy the Number of Springs Incremented by 1
	file_stream.read(&temp_byte, 1);
	temp_byte++;
	out_file.put(temp_byte);

	// Write Rest of File
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
	add_child_object = CHILD_OBJECT_TYPES::NONE;

	// Reset Some Variables
	active = false;
	editing = false;
	highlighting = false;
	originated_from_level = false;
	//object_index = 0;
	moused_object = nullptr;
	uuid = 0;

	// Delete the Node Array
	delete[] selected_object->node_list;
}

void Editor::Selector::readSpringMassFile(Selected_SpringMassSpring* selected_object)
{
	// Get SpringMass File Name
	std::string& file_name = static_cast<DataClass::Data_SpringMassSpring*>(selected_object->data_object)->getParent()->getFile();

	// Open File
	std::ifstream temp_file;
	temp_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);

	// Determine the Number of Nodes
	uint8_t temp_byte = 0;
	uint8_t node_iterator = 0;
	temp_file.read((char*)&selected_object->node_count, 1);
	temp_file.read((char*)&temp_byte, 1);

	// Allocate Memory for Nodes
	selected_object->node_list = new Object::Physics::Soft::NodeData[selected_object->node_count];

	// Temp Data to Read To
	Object::Physics::Soft::NodeData temp_node_data;
	Object::Physics::Soft::Spring temp_spring_data;

	// Reset Node Count
	selected_object->node_count = 0;

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
			temp_node_data.position += static_cast<DataClass::Data_SpringMassSpring*>(selected_object->data_object)->getParent()->getObjectData().position;
			selected_object->node_list[selected_object->node_count] = temp_node_data;
			selected_object->node_count++;
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
	if (Selected_Object::resizing || !Selected_Object::moving)
		return;

	// The Distance to Move the Object
	float distance = Constant::SPEED * Global::deltaTime;

	// Old Positions
	float old_x = 0.0f;
	float old_y = 0.0f;

	// Update for Each Selected Object
	for (Selected_Object* selected_object : selected_objects)
	{
		// Store Old Positions
		old_x = *selected_object->object_x;
		old_y = *selected_object->object_y;

		// Process Movements
		if (direction == NORTH) { *selected_object->object_y += distance * camera.accelerationY; }
		if (direction == SOUTH) { *selected_object->object_y -= distance * camera.accelerationY; }
		if (direction == EAST) { *selected_object->object_x -= distance * camera.accelerationL; }
		if (direction == WEST) { *selected_object->object_x += distance * camera.accelerationR; }

		// Update Model Matrix
		selected_object->vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*selected_object->object_x, *selected_object->object_y, 0.0f));

		// Update Children Positions
		updateSelectedPositions(selected_object->data_object, *selected_object->object_x - old_x, *selected_object->object_y - old_y);
	}
}

void Editor::Selector::moveWithArrowKeys(uint8_t direction)
{
	// If Inactive, Don't Do Anything
	if (!active)
		return;

	// If Resizing, Don't Move Object
	if (Selected_Object::resizing)
		return;

	// The Distance to Move the Object
	float distance = Global::editor_options->option_shift_speed * Global::deltaTime;

	// Old Positions
	float old_x = 0.0f;
	float old_y = 0.0f;

	// Update for Each Selected Object
	for (Selected_Object* selected_object : selected_objects)
	{
		// Store Old Positions
		old_x = *selected_object->object_x;
		old_y = *selected_object->object_y;

		// Process Movements
		if (direction == NORTH) { *selected_object->object_y += distance; }
		if (direction == SOUTH) { *selected_object->object_y -= distance; }
		if (direction == EAST) { *selected_object->object_x += distance; }
		if (direction == WEST) { *selected_object->object_x -= distance; }

		// Update Model Matrix
		selected_object->vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*selected_object->object_x, *selected_object->object_y, 0.0f));

		// Update Children Positions
		updateSelectedPositions(selected_object->data_object, *selected_object->object_x - old_x, *selected_object->object_y - old_y);
	}
}

void Editor::Selector::stopResizing()
{
	Selected_Object::moving = false;
	Global::LeftClick = false;
}

void Editor::Selector::clear()
{
	// Set State to Inactive
	editing = false;
	active = false;
	highlighting = false;

	// Remove Objects from Unsaved Objects
	change_controller->handleSelectorDelete(this);

	// Reset Arrays
	data_objects.clear();
	for (Selected_Object* selected_object : selected_objects)
		delete selected_object;
	selected_objects.clear();

	// Reset Some Variables
	originated_from_level = false;
	//object_index = 0;
	moused_object = nullptr;
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

bool& Editor::Selector::retrieveRotation()
{
	return Selected_Object::rotating;
}

bool Editor::Selector::selectedOnlyOne()
{
	return selected_objects.size() == 1;
}

void Editor::Selector::addChildToOnlyOne(DataClass::Data_Object* data_object, Object::Object& origin_object)
{
	// Get the Selected Object
	DataClass::Data_Object* only_one = selected_objects.at(0)->data_object;

	// Determine if Object is Leaving a Complex Object
	if (origin_object.parent != nullptr)
	{
		Object::Object* root_parent = origin_object.parent;
		while (root_parent != nullptr)
		{
			if (root_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			{
				// Add Offset to Objects and Children
				glm::vec2& offset = *root_parent->pointerToPosition();
				data_object->updateSelectedPosition(offset.x, offset.y, false);
				break;
			}
			root_parent = root_parent->parent;
		}
	}

	// Determine if Data Object is Being Added to a Complex Object
	Object::Object* root_object = selected_objects.at(0)->complex_root;
	DataClass::Data_Object* root_data_object = nullptr;
	if (only_one->getGroup() != nullptr && only_one->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
		root_data_object = static_cast<Render::Objects::UnsavedComplex*>(only_one->getGroup())->getComplexParent();

	// Object is Being Added Directly
	if (root_data_object != nullptr)
	{
		// Remove Offset From Objects and Children
		glm::vec2* offset = static_cast<Render::Objects::UnsavedComplex*>(root_data_object->getGroup())->getSelectedPosition();
		if (offset == nullptr)
			offset = &root_data_object->getPosition();
		data_object->updateSelectedPosition(-offset->x, -offset->y, false);

		// Remove Children From Level
		level->removeMarkedChildrenFromList(data_object);
	}

	// Object is Being Added to a Lower Object
	else if (root_object != nullptr)
	{
		// Remove Offset From Objects and Children
		glm::vec2& offset = *root_object->pointerToPosition();
		data_object->updateSelectedPosition(-offset.x, -offset.y, true);

		// Remove Children From Level
		level->removeMarkedChildrenFromList(data_object);
	}

	// Add Child to Selected Object
	only_one->addChildViaSelection(data_object, Render::Objects::MOVE_WITH_PARENT::MOVE_DISSABLED);

	// Update Number of Children Object Has
	only_one->getObjectIdentifier()[3] = only_one->getGroup()->getNumberOfChildren();

	// Link Selected Object as Parent
	// If Selected Object is Complex, Set Complex Version as Parent
	if (root_data_object != nullptr)
		data_object->setParent(root_data_object);
	else
		data_object->setParent(only_one);

	// Make Data Object and Children Unselectable
	addUnselectableRecursive(data_object);

	// Set Group Layer
	int8_t new_layer = selected_objects.at(0)->data_object->getGroupLayer() + 1;
	data_object->setGroupLayer(new_layer);

	// Set Group Layer Recursively
	Render::Objects::UnsavedCollection* data_group = data_object->getGroup();
	if (data_group != nullptr)
		data_group->recursiveSetGroupLayer(new_layer + 1);

	// If Going to a Complex Object, Add Children to Each Individual 
	if (root_object != nullptr || root_data_object != nullptr)
	{
		// Get List of Parent Instances
		std::vector<Object::Object*> parents; 
		if (root_data_object != nullptr)
			parents = static_cast<Render::Objects::UnsavedComplex*>(root_data_object->getGroup())->getInstances();
		else
			parents = data_object->getParent()->getObjects();

		// Allocate to Store All Object Instances and Their Offsets, No Children
		Object::Object** object_list = new Object::Object*[parents.size()];
		glm::vec2* offsets = new glm::vec2[parents.size()];
		int list_size = 0;

		// Iterate Through Each Parent Instance to Link Parents Correctly and Get Correct Offset
		for (Object::Object* parent : parents)
		{
			// Find the Parent's Complex Offset
			Object::Object* parent_parent = parent;
			offsets[list_size] = glm::vec2(0.0f, 0.0f);
			while (parent_parent != nullptr)
			{
				if (parent_parent->group_object != nullptr && parent_parent->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				{
					if (parent_parent->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
					{
						if (parent_parent->data_object->getObjects().size() == 1)
							offsets[list_size] += *static_cast<Object::TempObject*>(parent_parent)->pointerToSelectedPosition();

						else
						{
							// Determine the Real Object That Was Selected
							for (Object::Object* object : parent_parent->data_object->getObjects())
							{
								if (static_cast<Object::TempObject*>(object)->isOriginal())
								{
									// Determine the Offset of the Real Object
									glm::vec2 offset = glm::vec2(*selected_objects.at(0)->object_x, *selected_objects.at(0)->object_y) - *object->pointerToPosition();
									offsets[list_size] += parent_parent->returnPosition() + offset;

									break;
								}
							}
						}
					}
					else
						offsets[list_size] += parent_parent->returnPosition();
					break;
				}
				parent_parent = parent_parent->parent;
			}

			// If the Parent Was Assigned a New Group Object, Store it Here
			parent->group_object = only_one->getGroup();

			// Generate Object With Correct Offset
			object_list[list_size] = data_object->generateObject(offsets[list_size]);
			object_list[list_size]->parent = parent;
			list_size++;
		}

		// Generate Children for Each Object With the Correct Offset
		data_object->enableSelectionNonRecursive();
		for (int i = 0; i < parents.size(); i++)
			data_object->genChildrenRecursive(&object_list, list_size, object_list[i], offsets[i], this, true);
		addUnselectableRecursive(data_object);

		// Free the Offsets List
		delete[] offsets;

		// Store New Objects in Level
		level->incorperatNewObjects(object_list, list_size);
		delete[] object_list;
	}
}

DataClass::Data_Object* Editor::Selector::getOnlyOne()
{
	return selected_objects.at(0)->data_object;
}

void Editor::Selector::copyOnlyOne()
{
	data_objects[0] = data_objects[0]->makeCopy();
	selected_objects.at(0)->data_object = data_objects[0];
}

void Editor::Selector::updateParentofSelected(DataClass::Data_Object* new_parent)
{
	// Get Object Index of Parent
	uint32_t parent_index = new_parent->getObjectIndex();

	for (DataClass::Data_Object* child : data_objects)
	{
		// If Index of Child Parent Matches New Parent, Set Parent to New Parent
		DataClass::Data_Object* test_parent = child->getParent();
		if (test_parent != nullptr && test_parent->getObjectIndex() == parent_index)
			child->setParent(new_parent);
	}
}

void Editor::Selector::clearOnlyOneComplexParent()
{
	// Get the Selected Object
	Selected_Object& selected_object = *selected_objects.at(0);

	// If Complex Parent is Not Null, Perform Offset Correction on All Children
	if (selected_object.complex_root)
	{
		// Retrieve the Data Object
		DataClass::Data_Object* data_object = selected_object.data_object;

		// Get the Offset of the Complex Root
		glm::vec2 offset = selected_object.complex_root->returnPosition();

		// Update All Children of Object
		if (data_object->getGroup() != nullptr)
		{
			std::vector<DataClass::Data_Object*>& children = data_object->getGroup()->getChildren();
			for (DataClass::Data_Object* child : children)
				child->updateSelectedPosition(offset.x, offset.y, false);
		}

		// Remove Children From Level
		level->removeMarkedChildrenFromList(data_object);

		// Get the Temp Object Representing the Selected Object
		Object::Object* temp_object = nullptr;
		for (Object::Object* possible_temp : data_object->getObjects())
		{
			temp_object = possible_temp->parent;
			while (temp_object->group_object->getCollectionType() != Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				temp_object = temp_object->parent;
			if (temp_object == selected_object.complex_root)
			{
				temp_object = possible_temp;
				break;
			}
		}

		// Recursively Generate the New Children for Object
		Object::Object** object_list = new Object::Object*[1];
		int list_size = 0;
		offset = glm::vec2(0.0f, 0.0f);
		data_object->enableSelection();
		data_object->genChildrenRecursive(&object_list, list_size, temp_object, offset, this, true);
		addUnselectableRecursive(data_object);

		// Add Children Into Level
		level->incorperatNewObjects(object_list, list_size);
		delete[] object_list;
	}

	// Clear the Complex Root
	selected_object.complex_root = nullptr;
}

void Editor::Selector::makeSelectable()
{
	// Iterate Throught Previously Selected Objects and Make Them and Their Children Selectable
	for (DataClass::Data_Object* data_object : previously_selected)
		data_object->enableSelection();

	// Clear the List
	previously_selected.clear();
}

void Editor::Selector::addUnselectable(DataClass::Data_Object* data_object)
{
	// Mark Object as Being Unselectable
	data_object->disableSelecting();

	// Add Object to Previously Selected Objects
	previously_selected.push_back(data_object);
}

void Editor::Selector::addUnselectableRecursive(DataClass::Data_Object* data_object)
{
	// Disable Selection for Object
	addUnselectable(data_object);

	// Check Children of Object
	if (data_object->getGroup() != nullptr)
	{
		for (DataClass::Data_Object* child : data_object->getGroup()->getChildren())
			addUnselectableRecursiveHelper(child);
	}
}

void Editor::Selector::deleteSelectedObjects()
{
	// Selector is Not Loaded
	active = false;
	editing = false;
	highlighting = false;

	// Delete Selected Data Objects
	for (DataClass::Data_Object* data_object : data_objects)
		delete data_object;

	// Delete Selected Objects
	for (Selected_Object* selected_object : selected_objects)
		delete selected_object;

	// Clear the Vectors
	data_objects.clear();
	selected_objects.clear();

	// Make All Object Selectable Again
	makeSelectable();

	// Reset Some Variables
	originated_from_level = false;
	add_child_object = CHILD_OBJECT_TYPES::NONE;
	add_child = false;
	//object_index = 0;
	moused_object = nullptr;
}

void Editor::Selector::initializeSelector()
{
	// Disable Left Click to Prevent Accidental Desselections
	Global::LeftClick = false;

	for (DataClass::Data_Object* data_object : unadded_data_objects)
	{
		// Move Data Object to Data Object Array
		data_objects.push_back(data_object);

		// Generate Vertex Objects
		Selected_VertexObjects vertex_objects;
		glGenVertexArrays(1, &vertex_objects.objectVAO);
		glGenBuffers(1, &vertex_objects.objectVBO);
		glGenVertexArrays(1, &vertex_objects.outlineVAO);
		glGenBuffers(1, &vertex_objects.outlineVBO);

		// Allocate Memory for Selector Vertices
		allocateSelectorVertices(data_object, vertex_objects);

		// Generate Selector Vertices
		genSelectorVertices(data_object, vertex_objects);

		// Store Object Data
		Selected_Object* selected_object = storeSelectorData(data_object);
		selected_object->vertex_objects = vertex_objects;
		selected_objects.push_back(selected_object);

		// Test if Object Belongs to a Parent to Test for Complex Origin
		if (data_object->getParent() != nullptr)
		{
			// Get the Root Parent of Objects
			DataClass::Data_Object* root_parent = data_object->getParent();
			while (root_parent->getParent() != nullptr)
				root_parent = root_parent->getParent();

			// Test if Root Parent of Group is A Complex Object. If So, Store Root Parent Object
			if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			{
				// Store Complex Root Parent
				selected_object->complex_root = static_cast<DataClass::Data_ComplexParent*>(root_parent)->getRootParent();
			}
		}

		// Bind Object Vertex Objects
		glBindVertexArray(vertex_objects.objectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

		// Object with Color and Texture
		if (selected_object->vertex_objects.visualize_object)
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
		else if (selected_object->vertex_objects.visualize_texture)
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
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

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

	// Set Editing Flag to True
	editing = true;

	// Erase the Unadded Data Objects
	unadded_data_objects.clear();

	// Determine the Boundaries of the Group Selector
	updateGroupSelector();
}

void Editor::Selector::editInfoPositions(DataClass::Data_Object* data_object)
{
	// Indicies to be Replaced in the Object Info Object
	int index1 = 0, index2 = 0, index3 = 0;

	// Root Parent
	DataClass::Data_Object* root_parent = data_object->getParent();

	// Determine The Offset to Make Positions Relative
	// If Not Complex, Offset Will be 0
	relative_offset = glm::vec2(0.0f, 0.0f);
	while (root_parent != nullptr)
	{
		if (root_parent->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
			relative_offset -= root_parent->getPosition();
		root_parent = root_parent->getParent();
	}

	// Get Pointers and Indicies from Data Object
	data_object->setInfoPointers(index1, index2, index3, &first_position_global, &second_position_global, &third_position_global);

	// Replace First Position
	if (index1 != -1)
		info->editDoubleValue(index1 - 1, &first_position_relative.x, &first_position_relative.y);

	// Replace Second Position
	if (index2 != -1)
		info->editDoubleValue(index2 - 1, &second_position_relative.x, &second_position_relative.y);

	// Replace Third Position
	if (index3 != -1)
		info->editDoubleValue(index3 - 1, &third_position_relative.x, &third_position_relative.y);

	// Set the Initial Values
	updateInfoPositions();
}

void Editor::Selector::updateInfoPositions()
{
	// Update All Three Positions, Even if Only 1 is Used
	// Should be More Efficient Than an If Statement
	first_position_relative = *first_position_global + relative_offset;
	second_position_relative = *second_position_global + relative_offset;
	third_position_relative = *third_position_global + relative_offset;

	// Update the Object Info Object
	info->forceResize();
}

void Editor::Selector::allocateSelectorVertices(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Vertex Array Object
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Object
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			allocateSelectorVerticesHorizontalMasks(data_object, vertex_objects);
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			allocateSelectorVerticesVerticalMasks(data_object, vertex_objects);
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			allocateSelectorVerticesTriggerMasks(data_object, vertex_objects);
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		allocateSelectorVerticesShapes(object_identifier[2], data_object, vertex_objects);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		allocateSelectorVerticesLights(data_object, vertex_objects);
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
			allocateSelectorVerticesShapes(object_identifier[2], data_object, vertex_objects);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			allocateSelectorVerticesSoftBody(data_object, vertex_objects);
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			allocateSelectorVerticesHinge(data_object, vertex_objects);
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		alocateSelectorVerticesEntity(data_object, vertex_objects);
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	// Groups
	case Object::GROUP:
	{
		alocateSelectorVerticesGroup(data_object, vertex_objects);
		break;
	}

	}

	// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::Selector::genSelectorVertices(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Vertex Array Object
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Object
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			genSelectorVerticesHorizontalMasks(data_object, vertex_objects);
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			genSelectorVerticesVerticalMasks(data_object, vertex_objects);
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			genSelectorVerticesTriggerMasks(data_object, vertex_objects);
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		genSelectorVerticesShapes(object_identifier[2], data_object, vertex_objects);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		genSelectorVerticesLights(data_object, vertex_objects);
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
			genSelectorVerticesShapes(object_identifier[2], data_object, vertex_objects);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			genSelectorVerticesSoftBody(data_object, vertex_objects);
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			genSelectorVerticesHinge(data_object, vertex_objects);
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		genSelectorVerticesEntity(data_object, vertex_objects);
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	// Groups
	case Object::GROUP:
	{
		genSelectorVerticesGroup(data_object, vertex_objects);
		break;
	}

	}

	// Unbind VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorData(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Parse Object
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		// Parse Mask Type

		// Horizontal Mask
		if (object_identifier[1] == Object::Mask::FLOOR || object_identifier[1] == Object::Mask::CEILING)
		{
			selected_object = storeSelectorDataHorizontalMasks(data_object);
		}

		// Vertical Mask
		else if (object_identifier[1] == Object::Mask::LEFT_WALL || object_identifier[1] == Object::Mask::RIGHT_WALL)
		{
			selected_object = storeSelectorDataVerticalMasks(data_object);
		}

		// Trigger Object
		else if (object_identifier[1] == Object::Mask::TRIGGER)
		{
			selected_object = storeSelectorDataTriggerMasks(data_object);
		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		selected_object = storeSelectorDataShapes(object_identifier[2], data_object);
		break;
	}

	// Lights
	case Object::LIGHT:
	{
		selected_object = storeSelectorDataLights(data_object);
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
			selected_object = storeSelectorDataShapes(object_identifier[2], data_object);
			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			selected_object = storeSelectorDataSoftBody(data_object);
			break;
		}

		// Hinge Base
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			selected_object = storeSelectorDataHinge(data_object);
			break;
		}

		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		selected_object = storeSelectorDataEntity(data_object);
		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	// Groups
	case Object::GROUP:
	{
		selected_object = storeSelectorDataGroup(data_object);
		break;
	}

	}

	// Store Object Info
	data_object->info(*info);

	// Edit Object Info
	editInfoPositions(data_object);

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesHorizontalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Horizontal Mask Shapes
	switch (data_object->getObjectIdentifier()[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 616, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	}

	// Object Does Not Consist of Color and Texture
	vertex_objects.visualize_object = false;

	// Object is Composed of Lines
	vertex_objects.visualize_lines = true;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesHorizontalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Horizontal Mask Shapes
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Get the Horizontal Line Data of the Data Class
		Object::Mask::HorizontalLineData& horizontal_line_data = static_cast<DataClass::Data_HorizontalLine*>(data_object)->getHorizontalLineData();

		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::FLOOR ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 1.0f);
		float half_width = horizontal_line_data.width * 0.5f;
		float object_vertices[] = {
			0.0f - half_width, 0.0f, -1.6f,  temp_color.x, temp_color.y, temp_color.z, 1.0f,
			0.0f + half_width, 0.0f, -1.6f,  temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, horizontal_line_data.width, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(horizontal_line_data.position.x, horizontal_line_data.position.y, 0.0f));

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Get the Horizontal Slant Data of the Data Class
		Object::Mask::SlantData& slant_data = static_cast<DataClass::Data_Slant*>(data_object)->getSlantData();

		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::FLOOR ? glm::vec3(0.0f, 1.0f, 1.0f) : glm::vec3(0.28f, 0.0f, 0.34f);
		float object_vertices[] = {
			0.0f,                                           0.0f,				                          	-1.6f,  temp_color.x, temp_color.y, temp_color.z, 1.0f,
			slant_data.position2.x - slant_data.position.x, slant_data.position2.y - slant_data.position.y, -1.6f,  temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, slant_data.position2.x - slant_data.position.x, 0.0f, slant_data.position2.y - slant_data.position.y, -1.1f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(slant_data.position.x, slant_data.position.y, 0.0f));

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Get the Horizontal Slant Data of the Data Class
		Object::Mask::SlopeData& slope_data = static_cast<DataClass::Data_Slope*>(data_object)->getSlopeData();

		// Generate and Store Object Vertices
		float object_vertices[154];
		Vertices::Line::genLineSimplifiedCurve1(0.0f, 0.0f, -1.6f, slope_data.height, slope_data.width, object_identifier[1] == Object::Mask::FLOOR ? glm::vec4(0.04f, 0.24f, 1.0f, 1.0f) : glm::vec4(0.0f, 0.0f, 0.45f, 1.0f), 11, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 616, object_vertices);
		vertex_objects.object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -0.9f, slope_data.width, slope_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(slope_data.position.x, slope_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataHorizontalMasks(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Parse Horizontal Mask Shapes
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[2])
	{

	// Horizontal Line
	case Object::Mask::HORIZONTAL_LINE:
	{
		// Transform Data Class into Horizontal Line and Get Horizontal Line Data
		DataClass::Data_HorizontalLine& data_horizontal_line = *static_cast<DataClass::Data_HorizontalLine*>(data_object);
		Object::Mask::HorizontalLineData& horizontal_line_data = data_horizontal_line.getHorizontalLineData();

		// Shape is a Horizontal Line
		selected_object = new Selected_Horizontal_Line();
		Selected_Horizontal_Line& new_selected_horizontal_line = *static_cast<Selected_Horizontal_Line*>(selected_object);
		selected_object->editing_shape = HORIZONTAL_LINE;

		// Store Pointers to Data
		new_selected_horizontal_line.object_x = &horizontal_line_data.position.x;
		new_selected_horizontal_line.object_y = &horizontal_line_data.position.y;
		new_selected_horizontal_line.object_width = &horizontal_line_data.width;
		new_selected_horizontal_line.data_object = data_object;

		// Enable Resize
		new_selected_horizontal_line.enable_resize = true;

		break;
	}

	// Horizontal Slant
	case Object::Mask::HORIZONTAL_SLANT:
	{
		// Transform Data Class into Horizontal Slant and Get Horizontal Slant Data
		DataClass::Data_Slant& data_slant = *static_cast<DataClass::Data_Slant*>(data_object);
		Object::Mask::SlantData& slant_data = data_slant.getSlantData();

		// Shape is a Line
		selected_object = new Selected_Line();
		Selected_Line& new_selected_line = *static_cast<Selected_Line*>(selected_object);
		selected_object->editing_shape = LINE;

		// Store Pointers to Data
		new_selected_line.object_x = &slant_data.position.x;
		new_selected_line.object_y = &slant_data.position.y;
		new_selected_line.object_opposite_x = &slant_data.position2.x;
		new_selected_line.object_opposite_y = &slant_data.position2.y;
		new_selected_line.slope = (slant_data.position2.y - slant_data.position.y) / (slant_data.position2.x - slant_data.position.x);
		new_selected_line.intercept = slant_data.position.y - (new_selected_line.slope * slant_data.position.x);
		new_selected_line.data_object = data_object;

		break;
	}

	// Horizontal Slope
	case Object::Mask::HORIZONTAL_SLOPE:
	{
		// Transform Data Class into Horizontal Slope and Get Horizontal Slope Data
		DataClass::Data_Slope& data_slope = *static_cast<DataClass::Data_Slope*>(data_object);
		Object::Mask::SlopeData& slope_data = data_slope.getSlopeData();

		// Shape is a Rectangle
		selected_object = new Selected_Rectangle();
		Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
		selected_object->editing_shape = RECTANGLE;

		// Store Pointers to Data
		new_selected_rectangle.object_x = &slope_data.position.x;
		new_selected_rectangle.object_y = &slope_data.position.y;
		new_selected_rectangle.object_width = &slope_data.width;
		new_selected_rectangle.object_height = &slope_data.height;
		new_selected_rectangle.data_object = data_object;

		// Enable Resize
		new_selected_rectangle.enable_resize = true;

		break;
	}

	}

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesVerticalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Vertical Mask Shapes
	switch (data_object->getObjectIdentifier()[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 616, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 22;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	}

	// Object Does Not Consist of Color and Texture
	vertex_objects.visualize_object = false;

	// Object is Composed of Lines
	vertex_objects.visualize_lines = true;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesVerticalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Vertical Mask Shapes
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Get the Vertical Line Data of the Data Class
		Object::Mask::VerticalLineData& vertical_line_data = static_cast<DataClass::Data_VerticalLine*>(data_object)->getVerticalLineData();

		// Generate and Store Object Vertices
		glm::vec3 temp_color = object_identifier[1] == Object::Mask::LEFT_WALL ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 0.0f, 1.0f);
		float half_height = vertical_line_data.height * 0.5f;
		float object_vertices[] = {
			0.0f, 0.0f + half_height, -1.6f, temp_color.x, temp_color.y, temp_color.z, 1.0f,
			0.0f, 0.0f - half_height, -1.6f, temp_color.x, temp_color.y, temp_color.z, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 1.0f, vertical_line_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(vertical_line_data.position.x, vertical_line_data.position.y, 0.0f));

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Get the Vertical Curve Data of the Data Class
		Object::Mask::CurveData& curve_data = static_cast<DataClass::Data_Curve*>(data_object)->getCurveData();

		// Generate and Store Object Vertices
		float slope = curve_data.height / 6.0f;
		float amplitude = (1.0f + (6.0f / curve_data.width));
		float x_offset = (21.0f * curve_data.width - 25.0f) / 30.0f;
		int8_t sign = (object_identifier[1] == 1) ? -1 : 1;
		float object_vertices[154];
		Vertices::Line::genLineSimplifiedCurve2(-curve_data.width / 2 * sign, -curve_data.height / 2, -1.7f, curve_data.width, slope, amplitude, x_offset, sign, object_identifier[1] == Object::Mask::LEFT_WALL ? glm::vec4(1.0f, 0.4f, 0.0f, 1.0f) : glm::vec4(0.04f, 0.0f, 0.27f, 1.0f), 11, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 616, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, curve_data.width, curve_data.height, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(curve_data.position.x, curve_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataVerticalMasks(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Parse Vertical Mask Shapes
	uint8_t* object_identifier = data_object->getObjectIdentifier();
	switch (object_identifier[2])
	{

	// Vertical Line
	case Object::Mask::VERTICAL_LINE:
	{
		// Transform Data Class into Vertical Line and Get Vertical Line Data
		DataClass::Data_VerticalLine& data_vertical_line = *static_cast<DataClass::Data_VerticalLine*>(data_object);
		Object::Mask::VerticalLineData& vertical_line_data = data_vertical_line.getVerticalLineData();

		// Shape is a Vertical Line
		selected_object = new Selected_Vertical_Line();
		Selected_Vertical_Line& new_selected_vertical_line = *static_cast<Selected_Vertical_Line*>(selected_object);
		selected_object->editing_shape = VERTICAL_LINE;

		// Store Pointers to Data
		new_selected_vertical_line.object_x = &vertical_line_data.position.x;
		new_selected_vertical_line.object_y = &vertical_line_data.position.y;
		new_selected_vertical_line.object_height = &vertical_line_data.height;
		new_selected_vertical_line.data_object = data_object;

		// Enable Resize
		new_selected_vertical_line.enable_resize = true;

		break;
	}

	// Vertical Curve
	case Object::Mask::VERTICAL_CURVE:
	{
		// Transform Data Class into Vertical Curve and Get Vertical Curve Data
		DataClass::Data_Curve& data_curve = *static_cast<DataClass::Data_Curve*>(data_object);
		Object::Mask::CurveData& curve_data = data_curve.getCurveData();

		// Shape is a Rectangle
		selected_object = new Selected_Rectangle();
		Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
		selected_object->editing_shape = RECTANGLE;

		// Store Pointers to Data
		new_selected_rectangle.object_x = &curve_data.position.x;
		new_selected_rectangle.object_y = &curve_data.position.y;
		new_selected_rectangle.object_width = &curve_data.width;
		new_selected_rectangle.object_height = &curve_data.height;
		new_selected_rectangle.data_object = data_object;

		// Enable Resize
		new_selected_rectangle.enable_resize = true;

		break;
	}

	}

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesTriggerMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Allocate Memory Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.outline_vertex_count = 8;

	// Object Does Not Consist of Color and Texture
	vertex_objects.visualize_object = false;

	// Object is Compose of Triangles
	vertex_objects.visualize_lines = false;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesTriggerMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Get the Trigger Line Data of the Data Class
	Object::Mask::Trigger::TriggerData& trigger_data = static_cast<DataClass::Data_TriggerMask*>(data_object)->getTriggerData();

	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Generate and Store Object Vertices
	float object_vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.7f, trigger_data.width, trigger_data.height, glm::vec4(0.4f, 0.0f, 0.0f, 0.5f), object_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, trigger_data.width, trigger_data.height, outline_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

	// Set Initial Position Model Matrix
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(trigger_data.position.x, trigger_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataTriggerMasks(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Transform Data Class into Trigger Mask and Get Trigger Data
	DataClass::Data_TriggerMask& data_trigger = *static_cast<DataClass::Data_TriggerMask*>(data_object);
	Object::Mask::Trigger::TriggerData& trigger_data = data_trigger.getTriggerData();

	// Shape is a Rectangle
	selected_object = new Selected_Rectangle();
	Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
	selected_object->editing_shape = RECTANGLE;

	// Store Object Data
	new_selected_rectangle.object_x = &trigger_data.position.x;
	new_selected_rectangle.object_y = &trigger_data.position.y;

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesShapes(int index, DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 144, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 3;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 6;

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		//Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 2880, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 60;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 1120, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 40;

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Allocate Memory for Object Vertices
		unsigned char number_of_sides = *static_cast<Shape::Polygon*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape())->pointerToNumberOfSides();
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)144 * number_of_sides, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = number_of_sides * 3;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)56 * number_of_sides, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = number_of_sides * 2;

		break;
	}

	}

	// Object Consists of Color and Texture
	vertex_objects.visualize_object = true;

	// Object is Compose of Triangles
	vertex_objects.visualize_lines = false;

	// Object Also Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesShapes(int index, DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Get the Object Data
	Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();

	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Get the Rectangle Shape
		Shape::Rectangle& rectangle_data = *static_cast<Shape::Rectangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Generate and Store Object Vertices
		float object_vertices[72];
		Vertices::Rectangle::genRectObjectFull(glm::vec2(0.0f), -2.0f, *rectangle_data.pointerToWidth(), *rectangle_data.pointerToHeight(), object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, *rectangle_data.pointerToWidth(), *rectangle_data.pointerToHeight(), outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Get the Trapezoid Shape
		Shape::Trapezoid& trapezoid_data = *static_cast<Shape::Trapezoid*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Generate and Store Object Vertices
		float object_vertices[72];
		Vertices::Trapezoid::genTrapObjectFull(glm::vec2(0.0f), -2.0f, *trapezoid_data.pointerToWidth(), *trapezoid_data.pointerToHeight(), *trapezoid_data.pointerToWidthOffset(), *trapezoid_data.pointerToHeightOffset(), object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Trapezoid::genTrapHighlighter(0.0f, 0.0f, -1.1f, *trapezoid_data.pointerToWidth(), *trapezoid_data.pointerToHeight(), *trapezoid_data.pointerToWidthOffset(), *trapezoid_data.pointerToHeightOffset(), outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Get the Triangle Shape
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Store Object Data
		glm::vec2 temp_coords1 = object_data.position;
		glm::vec2 temp_coords2 = *triangle_data.pointerToSecondPosition();
		glm::vec2 temp_coords3 = *triangle_data.pointerToThirdPosition();

		// Generate and Store Object Vertices
		float object_vertices[36];
		Vertices::Triangle::genTriObjectFull(glm::vec2(0.0f), temp_coords2 - temp_coords1, temp_coords3 - temp_coords1, -2.0f, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 144, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[42];
		Vertices::Triangle::genTriHighlighter(glm::vec2(0.0f), temp_coords2 - temp_coords1, temp_coords3 - temp_coords1, -1.1f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, outline_vertices);

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		// Get the Circle Shape
		Shape::Circle& circle_data = *static_cast<Shape::Circle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Generate and Store Object Vertices
		float object_vertices[720];
		Vertices::Circle::genCircleObjectFull(glm::vec2(0.0f), -2.0f, *circle_data.pointerToRadius(), 20, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 2880, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[280];
		Vertices::Circle::genCircleHighlighter(0.0f, 0.0f, -1.1f, *circle_data.pointerToRadius(), 20, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 1120, outline_vertices);

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Get the Polygon Shape
		Shape::Polygon& polygon_data = *static_cast<Shape::Polygon*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Generate and Store Object Vertices
		unsigned char number_of_sides = *polygon_data.pointerToNumberOfSides();
		float* object_vertices = new float[(int)number_of_sides * 36];
		Vertices::Circle::genCircleObjectFull(glm::vec2(0.0f), -2.0f, *polygon_data.pointerToRadius(), (int)number_of_sides, object_data.colors, object_data.normals, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)number_of_sides * 144, object_vertices);
		delete[] object_vertices;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float* outline_vertices = new float[number_of_sides * 14];
		Vertices::Circle::genCircleHighlighter(0.0f, 0.0f, -1.1f, *polygon_data.pointerToRadius(), number_of_sides, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56 * number_of_sides, outline_vertices);
		delete[] outline_vertices;

		break;
	}

	}

	// Set Initial Position Model Matrix
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataShapes(int index, DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Parse Shape
	switch (index)
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		// Get the Rectangle Shape
		Shape::Rectangle& rectangle_data = *static_cast<Shape::Rectangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Shape is a Rectangle
		selected_object = new Selected_Rectangle();
		Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
		selected_object->editing_shape = RECTANGLE;

		// Store Object Data
		new_selected_rectangle.object_width = rectangle_data.pointerToWidth();
		new_selected_rectangle.object_height = rectangle_data.pointerToHeight();
		new_selected_rectangle.data_object = data_object;

		// Enable Resize
		new_selected_rectangle.enable_resize = true;

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Get the Trapezoid Shape
		Shape::Trapezoid& trapezoid_data = *static_cast<Shape::Trapezoid*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Shape is a Trapezoid
		selected_object = new Selected_Trapezoid();
		Selected_Trapezoid& new_selected_trapezoid = *static_cast<Selected_Trapezoid*>(selected_object);
		selected_object->editing_shape = TRAPEZOID;

		// Store Object Data
		new_selected_trapezoid.object_width = trapezoid_data.pointerToWidth();
		new_selected_trapezoid.object_height = trapezoid_data.pointerToHeight();
		new_selected_trapezoid.object_width_modifier = trapezoid_data.pointerToWidthOffset();
		new_selected_trapezoid.object_height_modifier = trapezoid_data.pointerToHeightOffset();
		new_selected_trapezoid.data_object = data_object;

		// Enable Resize
		new_selected_trapezoid.enable_resize = true;

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Get the Triangle Shape
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Shape is a Triangle
		selected_object = new Selected_Triangle();
		Selected_Triangle& new_selected_triangle = *static_cast<Selected_Triangle*>(selected_object);
		selected_object->editing_shape = TRIANGLE;

		// Store Object Data
		new_selected_triangle.coords1 = data_object->getPosition();
		new_selected_triangle.coords2 = *triangle_data.pointerToSecondPosition();
		new_selected_triangle.coords3 = *triangle_data.pointerToThirdPosition();
		new_selected_triangle.data_object = data_object;

		break;
	}

	// Circle
	case Shape::CIRCLE:
	{
		// Get the Circle Shape
		Shape::Circle& circle_data = *static_cast<Shape::Circle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Shape is a Circle
		selected_object = new Selected_Circle();
		Selected_Circle& new_selected_circle = *static_cast<Selected_Circle*>(selected_object);
		selected_object->editing_shape = CIRCLE;

		// Store Object Data
		new_selected_circle.object_radius = circle_data.pointerToRadius();
		new_selected_circle.object_inner_radius = circle_data.pointerToRadiusInner();
		new_selected_circle.data_object = data_object;

		break;
	}

	// Polygon
	case Shape::POLYGON:
	{
		// Get the Polygon Shape
		Shape::Polygon& polygon_data = *static_cast<Shape::Polygon*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());

		// Shape is a Circle
		selected_object = new Selected_Circle();
		Selected_Circle& new_selected_circle = *static_cast<Selected_Circle*>(selected_object);
		selected_object->editing_shape = CIRCLE;

		// Store Object Data
		new_selected_circle.object_radius = polygon_data.pointerToRadius();
		new_selected_circle.object_inner_radius = polygon_data.pointerToRaidusInner();
		new_selected_circle.data_object = data_object;

		break;
	}

	}

	// Store Initial Position Data
	selected_object->object_x = &data_object->getPosition().x;
	selected_object->object_y = &data_object->getPosition().y;

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesLights(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Light Types
	switch (data_object->getObjectIdentifier()[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 120, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		break;
	}

	}

	// Object Only Has Texture
	vertex_objects.visualize_object = false;

	// Object Only Has Texture
	vertex_objects.visualize_texture = true;

	// Object is Composed of Triangles
	vertex_objects.visualize_lines = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesLights(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Get Light Data
	Object::Light::LightData& light_data = static_cast<DataClass::Data_Light*>(data_object)->getLightData();

	// Parse Light Types
	switch (data_object->getObjectIdentifier()[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Get Directional Data
		Object::Light::Directional::DirectionalData& directional_data = static_cast<DataClass::Data_Directional*>(data_object)->getDirectionalData();

		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Line::genLineTexture(0.0f, directional_data.position2.x - light_data.position.x, 0.0f, directional_data.position2.y - light_data.position.y, -1.5f, 0.4f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, directional_data.position2.x - light_data.position.x, 0.0f, directional_data.position2.y - light_data.position.y, -1.1f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -1.5f, 2.0f, 3.0f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 2.0f, 3.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -1.5f, 2.0f, 4.0f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 2.0f, 4.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Get Directional Data
		Object::Light::Beam::BeamData& beam_data = static_cast<DataClass::Data_Beam*>(data_object)->getBeamData();

		// Generate and Store Object Vertices
		float object_vertices[30];
		Vertices::Line::genLineTexture(0.0f, beam_data.position2.x - light_data.position.x, 0.0f, beam_data.position2.y - light_data.position.y, -1.5f, 0.4f, object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 120, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, beam_data.position2.x - light_data.position.x, 0.0f, beam_data.position2.y - light_data.position.y, -1.1f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		break;
	}

	}

	// Set Initial Position Model Matrix
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(light_data.position.x, light_data.position.y, 0.0f));

	// Object is a Light
	vertex_objects.lighting_object = true;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataLights(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Constant, Unchanging Values for the Width and Height of Point Lights and Spot Lights
	static float temp_width = 5.0f;
	static float temp_height = 5.0f;

	// Get Light Data
	Object::Light::LightData& light_data = static_cast<DataClass::Data_Light*>(data_object)->getLightData();

	// Determine the Number of Instances the Real Data Object Appears
	int real_instance_count = data_object->getObjects().size();

	// Parse Light Types
	switch (data_object->getObjectIdentifier()[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Transform Data Class into Directional Light and Get Directional Light Data
		DataClass::Data_Directional& data_directional = *static_cast<DataClass::Data_Directional*>(data_object);
		Object::Light::Directional::DirectionalData& directional_data = data_directional.getDirectionalData();

		// Shape is a Line
		selected_object = new Selected_Line();
		Selected_Line& new_selected_line = *static_cast<Selected_Line*>(selected_object);
		selected_object->editing_shape = LINE;

		// Store Pointers to Opposite Position Data
		new_selected_line.object_opposite_x = &directional_data.position2.x;
		new_selected_line.object_opposite_y = &directional_data.position2.y;
		new_selected_line.slope = (directional_data.position2.y - light_data.position.y) / (directional_data.position2.x - light_data.position.x);
		new_selected_line.intercept = light_data.position.y - (new_selected_line.slope * light_data.position.x);
		new_selected_line.data_object = data_object;

		// Store Texture
		texture = Global::Visual_Textures.find("DirectionalLight.png")->second;

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Transform Data Class into Point Light and Get Point Light Data
		DataClass::Data_Point& data_point = *static_cast<DataClass::Data_Point*>(data_object);
		Object::Light::Point::PointData& point_data = data_point.getPointData();

		// Shape is a Rectangle
		selected_object = new Selected_Rectangle();
		Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
		selected_object->editing_shape = RECTANGLE;

		// Store Static Sizes
		new_selected_rectangle.object_width = &temp_width;
		new_selected_rectangle.object_height = &temp_height;
		new_selected_rectangle.enable_resize = false;
		new_selected_rectangle.data_object = data_object;

		// Store Texture
		texture = Global::Visual_Textures.find("PointLight.png")->second;

		// Disable Resize
		new_selected_rectangle.enable_resize = false;

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Transform Data Class into Spot Light and Get Spot Light Data
		DataClass::Data_Spot& data_spot = *static_cast<DataClass::Data_Spot*>(data_object);
		Object::Light::Spot::SpotData& spot_data = data_spot.getSpotData();

		// Shape is a Rectangle
		selected_object = new Selected_Rectangle();
		Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
		selected_object->editing_shape = RECTANGLE;

		// Store Static Sizes
		new_selected_rectangle.object_width = &temp_width;
		new_selected_rectangle.object_height = &temp_height;
		new_selected_rectangle.enable_resize = false;
		new_selected_rectangle.data_object = data_object;

		// Store Texture
		texture = Global::Visual_Textures.find("SpotLight.png")->second;

		// Disable Resize
		new_selected_rectangle.enable_resize = false;

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Transform Data Class into Directional Light and Get Directional Light Data
		DataClass::Data_Beam& data_beam = *static_cast<DataClass::Data_Beam*>(data_object);
		Object::Light::Beam::BeamData& beam_data = data_beam.getBeamData();

		// Shape is a Line
		selected_object = new Selected_Line();
		Selected_Line& new_selected_line = *static_cast<Selected_Line*>(selected_object);
		selected_object->editing_shape = LINE;

		// Store Pointers to Opposite Position Data
		new_selected_line.object_opposite_x = &beam_data.position2.x;
		new_selected_line.object_opposite_y = &beam_data.position2.y;
		new_selected_line.slope = (beam_data.position2.y - light_data.position.y) / (beam_data.position2.x - light_data.position.x);
		new_selected_line.intercept = light_data.position.y - (new_selected_line.slope * light_data.position.x);
		new_selected_line.data_object = data_object;

		// Store Texture
		texture = Global::Visual_Textures.find("BeamLight.png")->second;

		break;
	}

	}

	// Set Object Position
	selected_object->object_x = &light_data.position.x;
	selected_object->object_y = &light_data.position.y;

	// Allocate Shader Data
	allocateSelectorShaderDataLights(data_object);

	// Store Shader Data
	storeSelectorShaderDataLights(data_object);

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorShaderDataLights(DataClass::Data_Object* data_object)
{
	// Determine the Number of Instances the Real Data Object Appears
	int real_instance_count = data_object->getObjects().size();

	// Parse Light Types
	switch (data_object->getObjectIdentifier()[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Get Size of Directional Light Buffer
		light_buffer_offset = level->returnDirectionalBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::DirectionalBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)light_buffer_offset + 96 * real_instance_count, NULL, GL_STREAM_COPY);

		// Copy Contents of Directional Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::DirectionalBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Directional Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, (GLsizeiptr)light_buffer_offset + 96 * real_instance_count, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count += real_instance_count;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Get Size of Point Light Buffer
		light_buffer_offset = level->returnPointBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 80 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::PointBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 80 * real_instance_count, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::PointBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 80 * real_instance_count, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count += real_instance_count;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Get Size of Spot Light Buffer
		light_buffer_offset = level->returnSpotBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::SpotBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96 * real_instance_count, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::SpotBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96 * real_instance_count, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count += real_instance_count;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Get Size of Beam Light Buffer
		light_buffer_offset = level->returnBeamBufferSize();
		int light_buffer_count = (light_buffer_offset - 16) / 96 + 1;

		// Bind Buffers
		glBindBuffer(GL_COPY_READ_BUFFER, Global::BeamBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, copyBuffer);

		// Allocate Memory for Copy Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96 * real_instance_count, NULL, GL_STREAM_COPY);

		// Copy Contents of Point Buffer to the Copy Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Swap Buffer Enums
		glBindBuffer(GL_COPY_WRITE_BUFFER, Global::BeamBuffer);
		glBindBuffer(GL_COPY_READ_BUFFER, copyBuffer);

		// Allocate Memory for Point Buffer
		glBufferData(GL_COPY_WRITE_BUFFER, light_buffer_offset + 96 * real_instance_count, NULL, GL_DYNAMIC_DRAW);

		// Copy Contents of Copy Buffer Back Into Point Buffer
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, light_buffer_offset);

		// Increment the Header
		light_buffer_count += real_instance_count;
		glBufferSubData(GL_COPY_WRITE_BUFFER, 0, 4, &light_buffer_count);

		// Unbind Buffers
		glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
		glBindBuffer(GL_COPY_READ_BUFFER, 0);

		break;
	}

	}
}

void Editor::Selector::storeSelectorShaderDataLights(DataClass::Data_Object* data_object)
{
	// Get Light Data
	Object::Light::LightData& light_data = static_cast<DataClass::Data_Light*>(data_object)->getLightData();

	// Determine the Original Position from the Original Real Object
	glm::vec2 original_position;
	for (Object::Object* temp_real : data_object->getObjects())
	{
		if (temp_real->storage_type == Object::STORAGE_TYPES::NULL_TEMP && static_cast<Object::TempObject*>(temp_real)->isOriginal())
		{
			original_position = temp_real->returnPosition();
			break;
		}
	}

	// Get the Offset From the Original Light Position
	glm::vec2 original_offset = data_object->getPosition() - original_position;

	// Parse Light Types
	switch (data_object->getObjectIdentifier()[1])
	{

	// Directional Light
	case Object::Light::DIRECTIONAL:
	{
		// Get Directional Data
		Object::Light::Directional::DirectionalData& directional_data = static_cast<DataClass::Data_Directional*>(data_object)->getDirectionalData();

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
		glm::vec2 second_position_offset = directional_data.position2 - light_data.position;

		// Add Each Instance of Selected Directional Light to Shaders
		int real_instance_count = data_object->getObjects().size();
		for (int i = 0, j = light_buffer_offset; i < real_instance_count; i++, j += 96)
		{
			// Add Light and Line Direction Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j, 16, glm::value_ptr(light_direction));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 16, 16, glm::value_ptr(line_direction));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 32, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 48, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 64, 16, glm::value_ptr(light_data.specular * light_data.intensity));

			// Get the Real Instance to be Shaded
			Object::Light::Directional::Directional* directional_real = static_cast<Object::Light::Directional::Directional*>(data_object->getObjects().at(i));

			// Add Endpoint Data
			glm::vec2 first_real_position = directional_real->returnPosition() + original_offset;
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 80, 8, glm::value_ptr(first_real_position));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 88, 8, glm::value_ptr(first_real_position + second_position_offset));
		}

		// Unbind Directional Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Point Light
	case Object::Light::POINT:
	{
		// Get Point Data
		Object::Light::Point::PointData& point_data = static_cast<DataClass::Data_Point*>(data_object)->getPointData();

		// Bind Point Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);

		// Add Each Instance of Selected Point Light to Shaders
		int real_instance_count = data_object->getObjects().size();
		for (int i = 0, j = light_buffer_offset; i < real_instance_count; i++, j += 80)
		{
			// Get the Real Instance to be Shaded
			Object::Light::Point::Point* point_real = static_cast<Object::Light::Point::Point*>(data_object->getObjects().at(i));

			// Add Position Data
			glm::vec2 real_position = *point_real->pointerToPosition() + original_offset;
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j, 16, glm::value_ptr(glm::vec4(real_position.x, real_position.y, 2.0f, 1.0f)));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 16, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 32, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 48, 16, glm::value_ptr(light_data.specular * light_data.intensity));

			// Add Attenuation Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 64, 4, &point_data.linear);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 68, 4, &point_data.quadratic);
		}

		// Unbind Point Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Spot Light
	case Object::Light::SPOT:
	{
		// Get Spot Data
		Object::Light::Spot::SpotData& spot_data = static_cast<DataClass::Data_Spot*>(data_object)->getSpotData();

		// Bind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::SpotBuffer);

		// Add Each Instance of Selected Spot Light to Shaders
		int real_instance_count = data_object->getObjects().size();
		for (int i = 0, j = light_buffer_offset; i < real_instance_count; i++, j += 96)
		{
			// Get the Real Instance to be Shaded
			Object::Light::Spot::Spot* spot_real = static_cast<Object::Light::Spot::Spot*>(data_object->getObjects().at(i));

			// Add Position Data
			glm::vec2 real_position = *spot_real->pointerToPosition() + original_offset;
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j, 16, glm::value_ptr(glm::vec4(real_position.x, real_position.y, 2.0f, 0.0f)));

			// Add Direction Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 16, 16, glm::value_ptr(spot_data.direction));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 32, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 48, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 64, 16, glm::value_ptr(light_data.specular * light_data.intensity));

			// Add Attenuation Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 80, 4, &spot_data.linear);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 84, 4, &spot_data.quadratic);

			// Add Angle Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 88, 4, &spot_data.angle1);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 92, 4, &spot_data.angle2);
		}

		// Unbind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	// Beam Light
	case Object::Light::BEAM:
	{
		// Get Beam Data
		Object::Light::Beam::BeamData& beam_data = static_cast<DataClass::Data_Beam*>(data_object)->getBeamData();

		// Bind Beam Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);

		// Calculate Directional Vectors
		float slope = (beam_data.position2.y - light_data.position.y) / (beam_data.position2.x - light_data.position.x);
		float perpendicular_slope = -1.0f / slope;
		glm::vec4 direction = glm::vec4(1.0f, slope, 0, 0);
		direction = glm::normalize(direction);
		glm::vec2 second_position_offset = beam_data.position2 - light_data.position;

		// Add Each Instance of Selected Spot Light to Shaders
		int real_instance_count = data_object->getObjects().size();
		for (int i = 0, j = light_buffer_offset; i < real_instance_count; i++, j += 96)
		{
			// Add Light and Line Direction Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j, 16, glm::value_ptr(direction));

			// Add Ambient, Diffuse, and Specular Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 16, 16, glm::value_ptr(light_data.ambient * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 32, 16, glm::value_ptr(light_data.diffuse * light_data.intensity));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 48, 16, glm::value_ptr(light_data.specular * light_data.intensity));

			// Add Attenuation Data
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 64, 4, &beam_data.linear);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 68, 4, &beam_data.quadratic);

			// Get the Real Instance to be Shaded
			Object::Light::Beam::Beam* beam_real = static_cast<Object::Light::Beam::Beam*>(data_object->getObjects().at(i));

			// Add Endpoint Data
			glm::vec2 first_real_position = *beam_real->pointerToPosition() + original_offset;
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 72, 8, glm::value_ptr(first_real_position));
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, j + 80, 8, glm::value_ptr(first_real_position + second_position_offset));
		}

		// Unbind Spot Light Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		break;
	}

	}
}

void Editor::Selector::allocateSelectorVerticesSoftBody(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Soft Body Types
	switch (data_object->getObjectIdentifier()[2])
	{

	// SpringMass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 6;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		// Object is Not Composed of Lines
		vertex_objects.visualize_lines = false;

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Allocate Memory for Object Vertices
		glBufferData(GL_ARRAY_BUFFER, 56, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.object_vertex_count = 2;

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Allocate Memory for Outline Vertices
		glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
		vertex_objects.outline_vertex_count = 8;

		// Object is Composed of Lines
		vertex_objects.visualize_lines = true;

		break;
	}

	}

	// Object is a Static Color
	vertex_objects.visualize_object = false;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesSoftBody(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Soft Body Types
	switch (data_object->getObjectIdentifier()[2])
	{

	// Spring Mass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Edit Node
		if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_NODE)
		{
			// Get Node Data
			Object::Physics::Soft::NodeData node_data = static_cast<DataClass::Data_SpringMassNode*>(data_object)->getNodeData();

			// Generate and Store Object Vertices
			float object_vertices[42];
			float double_radius = node_data.radius * 2.0f;
			Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.7f, double_radius, double_radius, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(vertex_objects.outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

			// Generate and Store Outline Vertices
			double_radius *= 1.2f;
			float outline_vertices[56];
			Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, double_radius, double_radius, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(node_data.position.x, node_data.position.y, 0.0f));
		}

		// Edit Spring
		else if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_SPRING)
		{
			// Generate and Store Object Vertices
			float object_vertices[42];
			Vertices::Line::genLineColor(temp_connection_pos_left.x, temp_connection_pos_right.x, temp_connection_pos_left.y, temp_connection_pos_right.y, -1.7f, 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(vertex_objects.outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

			// Generate and Store Outline Vertices
			float outline_vertices[56];
			Vertices::Line::genLineHighlighterWidth(temp_connection_pos_left.x, temp_connection_pos_right.x, temp_connection_pos_left.y, temp_connection_pos_right.y, -1.1f, 0.3f, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		}

		// Edit Core
		else
		{
			// Get Object Data
			Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();

			// Generate and Store Object Vertices
			float object_vertices[42];
			Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 2.0f, 2.0f, object_data.colors, object_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

			// Bind Outline VAO
			glBindVertexArray(vertex_objects.outlineVAO);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

			// Generate and Store Outline Vertices
			float outline_vertices[56];
			Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 2.0f, 2.0f, outline_vertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

			// Set Initial Position Model Matrix
			vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));
		}

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Get Wire and Object Data
		Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();
		Object::Physics::Soft::WireData& wire_data = static_cast<DataClass::Data_Wire*>(data_object)->getWireData();

		// Generate and Store Object Vertices
		float object_vertices[] = {
			0.0f,                                           0.0f,				                          	-1.0f,  0.0f, 1.0f, 1.0f, 1.0f,
			wire_data.position2.x - object_data.position.x, wire_data.position2.y - object_data.position.y, -1.0f,  0.0f, 1.0f, 1.0f, 1.0f };
		glBufferSubData(GL_ARRAY_BUFFER, 0, 56, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Line::genLineHighlighter(0.0f, wire_data.position2.x - object_data.position.x, 0.0f, wire_data.position2.y - object_data.position.y, -0.9f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataSoftBody(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	// Parse Soft Body Types
	switch (data_object->getObjectIdentifier()[2])
	{

	// SpringMass
	case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
	{
		// Edit Node
		if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_NODE)
		{
			// Get Node Data
			Object::Physics::Soft::NodeData& node_data = static_cast<DataClass::Data_SpringMassNode*>(data_object)->getNodeData();

			// Shape is a SpringMass Node
			selected_object = new Selected_SpringMassNode();
			Selected_SpringMassNode& new_selected_springmass_node = *static_cast<Selected_SpringMassNode*>(selected_object);
			selected_object->editing_shape = SPRINGMASS_NODE;

			// Set Object Position
			new_selected_springmass_node.object_x = &node_data.position.x;
			new_selected_springmass_node.object_y = &node_data.position.y;
			new_selected_springmass_node.object_radius = &node_data.radius;
			new_selected_springmass_node.data_object = data_object;

			// Get Object Info
			info->clearAll();
			info->setObjectType("SpringMass Node", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			info->addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &node_data.name, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
			info->addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &node_data.position.x, &node_data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
		}

		// Edit Spring
		else if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_SPRING)
		{
			// Get Spring Data
			Object::Physics::Soft::Spring& spring_data = static_cast<DataClass::Data_SpringMassSpring*>(data_object)->getSpringData();

			// Shape is a SpringMass Spring
			selected_object = new Selected_SpringMassSpring();
			Selected_SpringMassSpring& new_selected_springmass_spring = *static_cast<Selected_SpringMassSpring*>(selected_object);
			selected_object->editing_shape = SPRINGMASS_SPRING;

			// Set Object Position
			// Idea: For When Moving Spring, Position is the Mean of the Two Node Positions (End Points)
			new_selected_springmass_spring.data_object = data_object;
			new_selected_springmass_spring.connection_pos_left = temp_connection_pos_left;
			new_selected_springmass_spring.connection_pos_right = temp_connection_pos_right;

			// Get Object Info
			info->clearAll();
			info->setObjectType("SpringMass Spring", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			info->addDoubleValue("Nodes: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "L: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " R: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &spring_data.Node1, &spring_data.Node2, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), true);
			info->addSingleValue("Rest Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.RestLength, glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), false);
			info->addSingleValue("Max Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.MaxLength, glm::vec4(0.0f, 0.9f, 0.0f, 1.0f), false);
			info->addSingleValue("Spring Constant: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.Stiffness, glm::vec4(0.0f, 0.0f, 0.9f, 1.0f), false);
			info->addSingleValue("Dampening: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &spring_data.Dampening, glm::vec4(0.9f, 0.0f, 0.9f, 1.0f), false);

			// Read SpringMass File
			readSpringMassFile(&new_selected_springmass_spring);
		}

		// Edit Core
		else
		{
			// Transform Data Class into SpringMass and Get Object Data
			DataClass::Data_SpringMass& data_springmass = *static_cast<DataClass::Data_SpringMass*>(data_object);
			Object::ObjectData& object_data = data_springmass.getObjectData();

			// Shape is a SpringMass Object
			selected_object = new Selected_SpringMassObject();
			Selected_SpringMassObject& new_selected_springmass_object = *static_cast<Selected_SpringMassObject*>(selected_object);
			selected_object->editing_shape = SPRINGMASS_OBJECT;

			// Set Object Position
			new_selected_springmass_object.object_x = &object_data.position.x;
			new_selected_springmass_object.object_y = &object_data.position.y;
			new_selected_springmass_object.data_object = data_object;

			// Get Object Info
			//Object::Physics::Soft::SpringMass::info(*info, editor_data.name, object_data, file_name);
		}

		break;
	}

	// Wire
	case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
	{
		// Transform Data Class into Wire and Get Wire Data
		DataClass::Data_Wire& data_wire = *static_cast<DataClass::Data_Wire*>(data_object);
		Object::Physics::Soft::WireData& wire_data = data_wire.getWireData();

		// Shape is a Line
		selected_object = new Selected_Line();
		Selected_Line& new_selected_line = *static_cast<Selected_Line*>(selected_object);
		selected_object->editing_shape = LINE;

		// Store Pointers to Data
		new_selected_line.object_opposite_x = &wire_data.position2.x;
		new_selected_line.object_opposite_y = &wire_data.position2.y;
		new_selected_line.slope = (wire_data.position2.y - data_wire.getPosition().y) / (wire_data.position2.x - data_wire.getPosition().x);
		new_selected_line.intercept = data_wire.getPosition().y - (new_selected_line.slope * data_wire.getPosition().x);
		new_selected_line.data_object = data_object;

		// Set Object Position
		new_selected_line.object_x = &data_object->getPosition().x;
		new_selected_line.object_y = &data_object->getPosition().y;

		// Get Object Info
		//Object::Physics::Soft::Wire::info(*info, data_wire.getName(), wire_data, wire_data);

		break;
	}

	}

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::allocateSelectorVerticesHinge(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Allocate Memory for Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.outline_vertex_count = 8;

	// Object is a Static Color
	vertex_objects.visualize_object = false;

	// Object is Not Composed of Lines
	vertex_objects.visualize_lines = false;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesHinge(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Parse Hinge Types
	switch (data_object->getObjectIdentifier()[2])
	{

	// Anchor
	case (int)Object::Physics::HINGES::ANCHOR:
	{
		// Get Anchor Data
		Object::Physics::Hinge::AnchorData& anchor_data = static_cast<DataClass::Data_Anchor*>(data_object)->getAnchorData();

		// Generate and Store Object Vertices
		float object_vertices[42];
		Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 1.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 1.0f, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(anchor_data.position.x, anchor_data.position.y, 0.0f));

		break;
	}

	// Hinge
	case (int)Object::Physics::HINGES::HINGE:
	{
		// Get Hinge Data
		Object::Physics::Hinge::HingeData& hinge_data = static_cast<DataClass::Data_Hinge*>(data_object)->getHingeData();

		// Generate and Store Object Vertices
		float object_vertices[42];
		Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 1.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), object_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

		// Bind Outline VAO
		glBindVertexArray(vertex_objects.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

		// Generate and Store Outline Vertices
		float outline_vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 1.0f, 1.0f, outline_vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

		// Set Initial Position Model Matrix
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(hinge_data.position.x, hinge_data.position.y, 0.0f));

		break;
	}

	}

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataHinge(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	static float temp_width = 2.0f;
	static float temp_height = 2.0f;

	// Object is a Rectangle
	selected_object = new Selected_Rectangle();
	Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
	selected_object->editing_shape = RECTANGLE;

	// Parse Hinge Types
	switch (data_object->getObjectIdentifier()[2])
	{

	// Anchor
	case (int)Object::Physics::HINGES::ANCHOR:
	{
		// Get Anchor Data
		Object::Physics::Hinge::AnchorData& anchor_data = static_cast<DataClass::Data_Anchor*>(data_object)->getAnchorData();

		// Set Object Position
		new_selected_rectangle.object_x = &anchor_data.position.x;
		new_selected_rectangle.object_y = &anchor_data.position.y;

		// Disable Resize
		new_selected_rectangle.enable_resize = false;

		break;
	}

	// Hinge
	case (int)Object::Physics::HINGES::HINGE:
	{
		// Get Hinge Data
		Object::Physics::Hinge::HingeData& hinge_data = static_cast<DataClass::Data_Hinge*>(data_object)->getHingeData();

		// Set Object Position
		new_selected_rectangle.object_x = &hinge_data.position.x;
		new_selected_rectangle.object_y = &hinge_data.position.y;

		// Disable Resize
		new_selected_rectangle.enable_resize = false;

		break;
	}

	}

	// Store Pointers to Data
	new_selected_rectangle.object_width = &temp_width;
	new_selected_rectangle.object_height = &temp_height;
	new_selected_rectangle.data_object = data_object;

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::alocateSelectorVerticesEntity(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 288, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Allocate Memory for Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.outline_vertex_count = 8;

	// Object Consists of Color and Texture
	vertex_objects.visualize_object = true;

	// Object is Compose of Triangles
	vertex_objects.visualize_lines = false;

	// Object Also Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesEntity(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Get Object and Entity Data
	Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();
	Object::Entity::EntityData& entity_data = static_cast<DataClass::Data_Entity*>(data_object)->getEntityData();

	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);
	
	// Generate and Store Object Vertices
	float object_vertices[72];
	object_data.colors = glm::vec4(1.0f);
	Vertices::Rectangle::genRectObjectFull(glm::vec2(0.0f), -1.8f, entity_data.half_width * 2.0f, entity_data.half_height * 2.0f, object_data.colors, object_data.normals, object_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 288, object_vertices);

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, entity_data.half_collision_width * 2.0f, entity_data.half_collision_height * 2.0f, outline_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

	// Set Initial Position Model Matrix
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(object_data.position.x, object_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataEntity(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	static float full_width, full_height;

	// Get Object and Entity Data
	Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();
	Object::Entity::EntityData& entity_data = static_cast<DataClass::Data_Entity*>(data_object)->getEntityData();

	// Shape is a Rectangle
	selected_object = new Selected_Rectangle();
	Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
	selected_object->editing_shape = RECTANGLE;

	// Store Object Data
	full_width = entity_data.half_collision_width * 2.0f;
	full_height = entity_data.half_collision_height * 2.0f;
	new_selected_rectangle.object_width = &full_width;
	new_selected_rectangle.object_height = &full_height;
	new_selected_rectangle.data_object = data_object;

	// Store Initial Position Data
	new_selected_rectangle.object_x = &object_data.position.x;
	new_selected_rectangle.object_y = &object_data.position.y;

	// Disable Resize
	new_selected_rectangle.enable_resize = false;

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::alocateSelectorVerticesGroup(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Allocate Memory for Object Vertices
	glBufferData(GL_ARRAY_BUFFER, 168, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.object_vertex_count = 6;

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Allocate Memory for Outline Vertices
	glBufferData(GL_ARRAY_BUFFER, 224, NULL, GL_DYNAMIC_DRAW);
	vertex_objects.outline_vertex_count = 8;

	// Object Consists of Color Only
	vertex_objects.visualize_object = false;

	// Object is Compose of Triangles
	vertex_objects.visualize_lines = false;

	// Object Only Has Color
	vertex_objects.visualize_texture = false;

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Editor::Selector::genSelectorVerticesGroup(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects)
{
	// Get Group Data
	Object::Group::GroupData& group_data = static_cast<DataClass::Data_GroupObject*>(data_object)->getGroupData();

	// Bind Object VAO
	glBindVertexArray(vertex_objects.objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.objectVBO);

	// Generate and Store Object Vertices
	float object_vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, 2.0f, 2.0f, glm::vec4(0.0f, 0.8f, 0.6f, 0.9f), object_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, object_vertices);

	// Bind Outline VAO
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Generate and Store Outline Vertices
	float outline_vertices[56];
	Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.1f, 2.0f, 2.0f, outline_vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 224, outline_vertices);

	// Set Initial Position Model Matrix
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(group_data.position.x, group_data.position.y, 0.0f));

	// Unbind Highlighter VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Editor::Selector::Selected_Object* Editor::Selector::storeSelectorDataGroup(DataClass::Data_Object* data_object)
{
	// Selected Object
	Selected_Object* selected_object = nullptr;

	static float full_width, full_height;

	// Get Group Data
	Object::Group::GroupData& group_data = static_cast<DataClass::Data_GroupObject*>(data_object)->getGroupData();

	// Shape is a Rectangle
	selected_object = new Selected_Rectangle();
	Selected_Rectangle& new_selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);
	selected_object->editing_shape = RECTANGLE;

	// Store Object Data
	full_width = 2.0f;
	full_height = 2.0f;
	new_selected_rectangle.object_width = &full_width;
	new_selected_rectangle.object_height = &full_height;
	new_selected_rectangle.data_object = data_object;

	// Store Initial Position Data
	new_selected_rectangle.object_x = &group_data.position.x;
	new_selected_rectangle.object_y = &group_data.position.y;

	// Disable Resize
	new_selected_rectangle.enable_resize = false;

	// Return Selected Object
	return selected_object;
}

void Editor::Selector::uninitializeSelector()
{
	glDeleteVertexArrays(1, &pivotVAO);
	glDeleteBuffers(1, &pivotVBO);
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

void Editor::Selector::updateGroupSelector()
{
	// Only Update Group Selector if a Group is Selected
	if (selected_objects.size() > 1)
	{
		// Reset the Group Selector
		group_selector.position = glm::vec2(*selected_objects.at(0)->object_x, *selected_objects.at(0)->object_y);
		group_selector.extreme_value_north = group_selector.position.y;
		group_selector.extreme_value_south = group_selector.position.y;
		group_selector.extreme_value_east = group_selector.position.x;
		group_selector.extreme_value_west = group_selector.position.x;

		// Update the Extreme Vertices of Group Selector
		for (Selected_Object* selected_object : selected_objects)
			selected_object->updateGroup(group_selector);

		// Update the Vertex Objects
		group_selector.position = glm::vec2((group_selector.extreme_value_east + group_selector.extreme_value_west) * 0.5f, (group_selector.extreme_value_north + group_selector.extreme_value_south) * 0.5f);

		// Bind the Vertex Object
		glBindVertexArray(group_selector.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, group_selector.outlineVBO);

		// Generate and Bind Vertex Data
		float vertices[56];
		Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.0f, group_selector.extreme_value_east - group_selector.extreme_value_west, group_selector.extreme_value_north - group_selector.extreme_value_south, vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Unbind the Vertex Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Set Model Matrix
		group_selector.model = glm::translate(glm::mat4(1.0f), glm::vec3(group_selector.position.x, group_selector.position.y, 0.0f));
	}
}

void Editor::Selector::updateSelectedPositions(DataClass::Data_Object* data_object, float deltaX, float deltaY)
{
	// Only Execute if Group Object is Not NULL
	if (data_object->getGroup() != nullptr)
	{
		// If Group Object, Update Selected Positions of Child Data Objects
		if (data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::GROUP)
		{
			for (DataClass::Data_Object* child : data_object->getGroup()->getChildren())
				child->updateSelectedPosition(deltaX, deltaY, true);
		}

		// If Complex Object, Only Update Visual Positions of Children
		else
		{
			for (DataClass::Data_Object* child : data_object->getGroup()->getChildren())
			{
				for (Object::Object* instance : child->getObjects())
				{
					if (instance->parent->object_index == data_object->getObjectIndex())
						instance->updateSelectedComplexPosition(deltaX, deltaY);
				}
			}
		}
	}
}

void Editor::Selector::editObject()
{
	// Idea: update_functions should return an int that determinses if a Selected Object
	// Was Interacted With. If it returns 1, nothing happened. If it returns 2, the Object
	// Was Moused Over But Nothing Happened. If it returns 3, an Object
	// is being Moved. If it returns 4, an Object is being Resized. If it returns 0, an
	// Object was Deselected. All Objects will be Modified Based on the First Non-Zero
	// Return Value Based on the Changes to the Object Being Modified. An Object That Has
	// Been Individual Deselected With CTRL Will Return 1

	// Idea: To allow the User to Determine the First Return Value, the "Tab to Skip"
	// Button Will Be Used, But it will Send the Selected Object to the Back of the Array
	// Instead of Skipping It

	// Idea: For Resizing, Each Object Type Will Have Its Own Resizing Function for Each
	// Other Object Type. When Another Selected Object is Being Resized, That Function Will
	// Be Executed. Ex: A rectangle Will Have a Trapezoid Resizing Function If a Trapezoid
	// Is Being Resized. This Function Will be Similar to the Trapezoid Resizing, However, The
	// Rectangle Will Only Resize in the Same Cardinal Directions of the Trapezoid. Ex 2: A
	// Resizing Circle Will Expand a Trapezoid, Rectangle, and Triangle In All Directions.

	// Idea: For this Same Resizing Idea, The Other Objects Should Expand Way/Towards the 
	// Modified Object Based on the Amount it is Being Resized

	// Idea: In Addition to the Resizing Idea, There Should be a Highlighted Box Surrounding 
	// All Selected Objects, As Long as the Number of Selected Objects is Greater Than 1. This
	// Box Should Be Tangent to the Farthest Vertices In Each of the Four Cardinal Directions.
	// This Box is Only Able to Resize Objects, and It Will Resize Like a Rectangle, However,
	// It Cannot be Moved and Will Resize Around the Center of the Box.

	// Test if Child Object Should be Added
	if (add_child_object != CHILD_OBJECT_TYPES::NONE && add_child)
	{
		addChild();
		return;
	}

	// If the Flag to Update Object Vertices is Enabled, Force Update Vertices
	if (force_reload_vertices)
	{
		force_reload_vertices = false;
		for (Selected_Object* selected_object : selected_objects)
			genSelectorVertices(selected_object->data_object, selected_object->vertex_objects);
	}

	// If The Level Object Reloaded the Lights, Reload the Selected Lights
	if (Global::reload_lights)
	{
		Global::reload_lights = false;
		for (DataClass::Data_Object* data_object : data_objects)
		{
			if (data_object->getObjectIdentifier()[0] == Object::ObjectList::LIGHT)
			{
				allocateSelectorShaderDataLights(data_object);
				storeSelectorShaderDataLights(data_object);
			}
		}
	}

	// If the CTRL Modifier is Active, Attempt to Deselect a Singular Object
	if (Global::Keys[GLFW_KEY_LEFT_CONTROL] || Global::Keys[GLFW_KEY_RIGHT_CONTROL])
	{
		for (int i = 0; i < selected_objects.size(); i++)
		{
			if (selected_objects.at(i)->updateObject() >= MOUSED_OVER)
			{
				// Desselect the Object if Left-Click is Held
				if (Global::LeftClick)
				{
					// If There is Only 1 Object, Desselect the Entire Vector
					if (selected_objects.size() == 1)
						deselectObject();

					// Else, Modify Vector to Only Affect the Singular Object
					else
					{
						// Return SpringMass Node
						if (selected_objects.at(i)->editing_shape == SPRINGMASS_NODE)
							deselectNode(static_cast<Selected_SpringMassNode*>(selected_objects.at(i)));

						// Return SpringMass Spring
						else if (selected_objects.at(i)->editing_shape == SPRINGMASS_SPRING)
							deselectSpring(static_cast<Selected_SpringMassSpring*>(selected_objects.at(i)));

						// Return Normal Object
						else
							change_controller->handleSelectorRealReturn(selected_objects.at(i)->data_object, this);

						// Make the Object Unselectable
						addUnselectable(selected_objects.at(i)->data_object);

						// Remove Object From the DataClass Vector
						for (int j = 0; j < data_objects.size(); j++)
						{
							if (selected_objects.at(i)->data_object->getObjectIndex() == data_objects.at(j)->getObjectIndex())
							{
								data_objects.erase(data_objects.begin() + j);
								break;
							}
						}

						// Delete the Selected Object
						delete selected_objects.at(i);

						// Remove Object From the Selected Object Vector
						selected_objects.erase(selected_objects.begin() + i);

						// If A Group is Still Selected, Update Group Selector
						if (selected_objects.size() > 1)
							updateGroupSelector();
					}

					Global::LeftClick = false;
				}

				return;
			}
		}

		return;
	}

	// Test the Group Selector Before Testing Other Objects
	if (selected_objects.size() > 1 && !Selected_Object::moving && !Selected_Object::resizing || group_selector.resizing)
	{
		// If Group Selector is Resizing, Resize All
		if (group_selector.resizing)
		{
			if (!Global::LeftClick)
				group_selector.resizing = false;

			else
			{
				for (Selected_Object* selected_object : selected_objects)
				{
					selected_object->moveObject();
					genSelectorVertices(selected_object->data_object, selected_object->vertex_objects);
					if (selected_object->vertex_objects.lighting_object)
						storeSelectorShaderDataLights(selected_object->data_object);
				}

				// Reset the Group Selector
				group_selector.position = glm::vec2(*selected_objects.at(0)->object_x, *selected_objects.at(0)->object_y);
				group_selector.extreme_value_north = group_selector.position.y;
				group_selector.extreme_value_south = group_selector.position.y;
				group_selector.extreme_value_east = group_selector.position.x;
				group_selector.extreme_value_west = group_selector.position.x;

				// Update the Extreme Vertices of Group Selector
				for (Selected_Object* selected_object : selected_objects)
					selected_object->updateGroup(group_selector);

				// Update the Vertex Objects
				group_selector.position = glm::vec2((group_selector.extreme_value_east + group_selector.extreme_value_west) * 0.5f, (group_selector.extreme_value_north + group_selector.extreme_value_south) * 0.5f);

				// Bind the Vertex Object
				glBindVertexArray(group_selector.outlineVAO);
				glBindBuffer(GL_ARRAY_BUFFER, group_selector.outlineVBO);

				// Generate and Bind Vertex Data
				float vertices[56];
				Vertices::Rectangle::genRectHilighter(0.0f, 0.0f, -1.0f, group_selector.extreme_value_east - group_selector.extreme_value_west, group_selector.extreme_value_north - group_selector.extreme_value_south, vertices);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

				// Unbind the Vertex Object
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);

				// Set Model Matrix
				group_selector.model = glm::translate(glm::mat4(1.0f), glm::vec3(group_selector.position.x, group_selector.position.y, 0.0f));
			}

			return;
		}

		// Bind Highlighter Object
		glBindVertexArray(group_selector.outlineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, group_selector.outlineVBO);

		if (!group_selector.resizing && Global::mouseRelativeX > group_selector.extreme_value_west - 1.0f && Global::mouseRelativeX < group_selector.extreme_value_east + 1.0f && Global::mouseRelativeY > group_selector.extreme_value_south - 1.0f && Global::mouseRelativeY < group_selector.extreme_value_north + 1.0f)
		{
			// Resize Variables
			bool resize_horizontal = false;
			bool resize_vertical = false;

			// Test if Group Should Resize Horizontally
			if (Global::mouseRelativeX < group_selector.extreme_value_west || Global::mouseRelativeX > group_selector.extreme_value_east)
			{
				resize_horizontal = true;
				Global::Selected_Cursor = Global::CURSORS::HORIZONTAL_RESIZE;
			}

			// Test if Group Should Resize Vertically
			if (Global::mouseRelativeY < group_selector.extreme_value_south || Global::mouseRelativeY > group_selector.extreme_value_north)
			{
				resize_vertical = true;
				Global::Selected_Cursor = Global::CURSORS::VERTICAL_RESIZE;
			}

			// Prepare for Resizing of Groups
			if (resize_vertical || resize_horizontal)
			{
				// Iterate Through Each Vertex and Change Color
				for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
					glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.85f, 1.0f)));

				// Set Mouse Icon to Resize

				if (Global::LeftClick)
				{
					group_selector.resizing = true;

					for (Selected_Object* selected_object : selected_objects)
					{
						selected_object->offset_x = 0.0f;
						selected_object->offset_y = 0.0f;
						selected_object->change_horizontal = 0.0f;
						selected_object->change_vertical = 0.0f;
						if (resize_horizontal)
							selected_object->setHorizontalGroupResize();
						if (resize_vertical)
							selected_object->setVerticalGroupResize();
					}
				}

				return;
			}
		}

		else
		{
			// Iterate Through Each Vertex and Change Color
			for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < 8; i += 7 * sizeof(GL_FLOAT), j++)
				glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), glm::value_ptr(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)));
		}

		// Unbind Highlighter Object
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// If Objects are Being Rotated, Rotate All
	if (Selected_Object::rotating)
	{
		for (Selected_Object* selected_object : selected_objects)
			sortVertices(true, selected_object);
		return;
	}

	// If Left Click is Not Held, Objects Should Not be Moved Nor Resized
	else if (!Global::LeftClick)
	{
		Selected_Object::moving = false;
		Selected_Object::resizing = false;
		for (Selected_Object* selected_object : selected_objects)
		{
			selected_object->change_horizontal = 0.0f;
			selected_object->change_vertical = 0.0f;
			if (selected_object->editing_shape == CIRCLE)
				static_cast<Selected_Circle*>(selected_object)->selected_vertex = 0;
			else if (selected_object->editing_shape == TRIANGLE)
				static_cast<Selected_Triangle*>(selected_object)->selected_vertex = 0;
		}
	}

	// If Objects are Being Moved, Move All
	else if (Selected_Object::moving)
	{
		for (Selected_Object* selected_object : selected_objects)
		{
			// Move Object
			selected_object->moveObject();	

			// If Object is a Lighting Object, Update Shader Data
			if (selected_object->vertex_objects.lighting_object)
				storeSelectorShaderDataLights(selected_object->data_object);
		}
		updateGroupSelector();
		return;
	}

	// Edit Object
	bool should_deselect = true;
	for (int i = 0; i < selected_objects.size(); i++)
	{
		Selected_Object* selected_object = selected_objects.at(i);
		uint8_t result = selected_object->updateObject();
		should_deselect &= result == DESELECTED;
		if (result >= MOUSED_OVER)
		{
			// Code to Make Other Objects Resize
			if (result == RESIZING)
			{
				for (Selected_Object* selected_object2 : selected_objects)
				{
					if (selected_object2 != selected_object)
					{
						// Note: Possible Have Individual Resize Returns for Horizontal, Vertical, or Both

						if (selected_object->change_horizontal)
							selected_object2->setHorizontalGroupResize();
						if (selected_object->change_vertical)
							selected_object2->setVerticalGroupResize();
						selected_object2->offset_x = *selected_object2->object_x - *selected_object->object_x;
						selected_object2->offset_y = *selected_object2->object_y - *selected_object->object_y;
					}
				}
			}

			// Code to Make Other Objects Move
			else if (result == MOVING)
			{
				for (Selected_Object* selected_object2 : selected_objects)
				{
					if (selected_object2 != selected_object)
						selected_object2->setMouseOffset();
				}
			}

			// Send Object to Front of List
			if (result == MOVING || result == RESIZING)
			{
				selected_objects[i] = selected_objects[0];
				selected_objects[0] = selected_object;
			}

			// If Tab is Pressed, Send Object to Back of the List
			else if (Global::Keys[GLFW_KEY_TAB])
			{
				Global::Keys[GLFW_KEY_TAB] = false;
				selected_objects[i] = selected_objects[selected_objects.size() - 1];
				selected_objects[selected_objects.size() - 1] = selected_object;
			}

			break;
		}

		// Else, Reset Some Values
		if (result <= MOUSED_OVER && !Selected_Object::moving && !Selected_Object::resizing)
		{
			selected_object->offset_x = 0.0f;
			selected_object->offset_y = 0.0f;
			selected_object->change_horizontal = 0.0f;
			selected_object->change_vertical = 0.0f;
		}
	}

	if (should_deselect)
		deselectObject();
}

uint8_t Editor::Selector::updateHinge(Selected_Object* selected_object)
{
	return false;
}

void Editor::Selector::sortVertices(bool enable_rotation, Selected_Object* selected_object)
{
	// Rectangle
	if (selected_object->editing_shape == RECTANGLE)
	{
		// Get Selected Rectangle Data
		Selected_Rectangle& selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);

		// Transform Data into Vertices
		float half_width = *selected_rectangle.object_width * 0.5f;
		float half_height = *selected_rectangle.object_height * 0.5f;
		float vertices[8] = {
			*selected_rectangle.object_x - half_width, * selected_rectangle.object_y - half_height, * selected_rectangle.object_x + half_width, * selected_rectangle.object_height - half_height,
			* selected_rectangle.object_x + half_width, * selected_rectangle.object_y + half_height, * selected_rectangle.object_x - half_width, * selected_rectangle.object_height + half_height
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
		*selected_rectangle.object_x = (vertices[0] + vertices[2] + vertices[4] + vertices[6]) / 4;
		*selected_rectangle.object_y = (vertices[1] + vertices[3] + vertices[5] + vertices[7]) / 4;
		*selected_rectangle.object_width = abs(vertices[2] - vertices[0]);
		if (*selected_rectangle.object_width < 0.1f) { *selected_rectangle.object_width = 0.2f; }
		*selected_rectangle.object_height = abs(vertices[5] - vertices[3]);
		if (*selected_rectangle.object_height < 0.1f) { *selected_rectangle.object_height = 0.2f; }
	}

	// Trapezoid
	else if (selected_object->editing_shape == TRAPEZOID)
	{
		// Get Selected Trapezoid Data
		Selected_Trapezoid& selected_trapezoid = *static_cast<Selected_Trapezoid*>(selected_object);

		// Transform Data into Vertices
		float half_width = *selected_trapezoid.object_width * 0.5f;
		float half_height = *selected_trapezoid.object_height * 0.5f;
		float vertices[8] = {
			*selected_trapezoid.object_x - half_width,                          *selected_trapezoid.object_y - half_height,
			*selected_trapezoid.object_x + half_width,                          *selected_trapezoid.object_y - half_height + *selected_trapezoid.object_height_modifier,
			*selected_trapezoid.object_x + half_width + *selected_trapezoid.object_width_modifier, * selected_trapezoid.object_y + half_height + *selected_trapezoid.object_height_modifier,
			*selected_trapezoid.object_x - half_width + *selected_trapezoid.object_width_modifier, * selected_trapezoid.object_y + half_height
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
		*selected_trapezoid.object_width = abs(vertices[2] - vertices[0]);
		if (*selected_trapezoid.object_width < 0.1f) { *selected_trapezoid.object_width = 0.2f; }
		*selected_trapezoid.object_height = abs(vertices[7] - vertices[1]);
		if (*selected_trapezoid.object_height < 0.1f) { *selected_trapezoid.object_height = 0.2f; }
		*selected_trapezoid.object_width_modifier = vertices[4] - vertices[2];
		*selected_trapezoid.object_height_modifier = vertices[5] - vertices[7];
		*selected_trapezoid.object_x = (vertices[2] + vertices[0]) / 2;
		*selected_trapezoid.object_y = (vertices[7] + vertices[1]) / 2;
	}

	// Triangle
	else if (selected_object->editing_shape == TRIANGLE)
	{
		// Get Selected Triangle Data
		Selected_Triangle& selected_triangle = *static_cast<Selected_Triangle*>(selected_object);

		// Transform Data into Vertices
		float vertices[6] = {
			selected_triangle.coords1.x, selected_triangle.coords1.y,
			selected_triangle.coords2.x, selected_triangle.coords2.y,
			selected_triangle.coords3.x, selected_triangle.coords3.y
			//*object_x + *object_width * cos(*object_width_modifier), *object_y + *object_width * sin(*object_width_modifier),
			//*object_x + *object_height * cos(*object_height_modifier), *object_y + *object_height * sin(*object_height_modifier),
		};

		// Rotate Object if Enabled
		if (enable_rotation)
		{
			// Calculate Rotation Matrix
			glm::mat4 rotation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(*selected_triangle.object_x, *selected_triangle.object_y, 0.0f));
			rotation_matrix = glm::rotate(rotation_matrix, -Global::deltaMouse.x * Constant::ROTATION_CONSTANT, glm::vec3(0.0f, 0.0f, 1.0f));
			rotation_matrix = glm::translate(rotation_matrix, glm::vec3(-*selected_triangle.object_x, -*selected_triangle.object_y, 0.0f));

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
		selected_triangle.coords1 = glm::vec2(vertices[0], vertices[1]);
		selected_triangle.coords2 = glm::vec2(vertices[2], vertices[3]);
		selected_triangle.coords3 = glm::vec2(vertices[4], vertices[5]);
		Source::Collisions::Point::arrangeTriVertices(selected_triangle.coords1, selected_triangle.coords2, selected_triangle.coords3);

		// Get the Triangle Shape
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_objects.at(0))->getShape());

		// Store Vertices in Object Data
		temp_position = selected_triangle.coords1;
		triangle_data = Shape::Triangle(selected_triangle.coords2, selected_triangle.coords3);
		static_cast<DataClass::Data_SubObject*>(data_objects.at(0))->getObjectData().position = selected_triangle.coords1;
		selected_triangle.coords2 = *triangle_data.pointerToSecondPosition();
		selected_triangle.coords3 = *triangle_data.pointerToThirdPosition();
		selected_triangle.should_sort = true;
	}

	// Update VAO and VBO
	if (enable_rotation)
	{
		genSelectorVertices(selected_object->data_object, selected_object->vertex_objects);
	}
}

void Editor::Selector::setTempConnectionPos(glm::vec2& left, glm::vec2& right)
{
	temp_connection_pos_left = left;
	temp_connection_pos_right = right;
}

void Editor::Selector::addUnselectableRecursiveHelper(DataClass::Data_Object* data_object)
{
	// Disable Selection for Object
	data_object->disableSelecting();

	// Check Children of Object
	if (data_object->getGroup() != nullptr)
	{
		for (DataClass::Data_Object* child : data_object->getGroup()->getChildren())
			addUnselectableRecursiveHelper(child);
	}
}

void Editor::Selector::addChild()
{
	// NOTE: When Multiple Objects Are Selected, The Object Currently Edited by the Editor Window is
	// Going to be the Moved to the First Index of the Selected Objects List. This Allows for The
	// Currently Edited Object by the Editor Window to Have Changes Applied to Them Easily. Useful for
	// When SpringMass Objects and Hinges are Being Edited and Child-Objects are Created
	// 
	// WARNING: NOT YET IMPLEMENTED

	// SpringMass Node
	if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_NODE)
		addSpringMassNode(static_cast<Selected_SpringMassObject*>(selected_objects.at(0)));

	// SpringMass Spring
	else if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_SPRING)
		addSpringMassSpring(static_cast<Selected_SpringMassObject*>(selected_objects.at(0)));

	// Hinge Object
	else if (add_child_object == CHILD_OBJECT_TYPES::HINGE_SUBOBJECT)
		addHingeObject();
}

void Editor::Selector::addSpringMassNode(Selected_SpringMassObject* selected_object)
{
	// Get SpringMass File Name
	std::string& file_name = static_cast<DataClass::Data_SpringMass*>(selected_object->data_object)->getFile();

	// Generate New Node Object
	Selected_SpringMassNode* new_selected_node = new Selected_SpringMassNode();
	DataClass::Data_SpringMassNode* new_data_node = new DataClass::Data_SpringMassNode(0);
	Object::Physics::Soft::NodeData& node_data = new_data_node->getNodeData();
	int node_count = 0;
	node_data.mass = 1.0f;
	node_data.position = glm::vec2(1.0f, -1.0f);
	node_data.health = 1.0f;
	node_data.material = 1;
	node_data.radius = 1.0f;
	new_data_node->storeParent(static_cast<DataClass::Data_SpringMass*>(selected_object->data_object));
	new_selected_node->data_object = new_data_node;

	// Determine if the SpringMass File Exists. If So, Establish First Node
	std::filesystem::path temp_path = Global::project_resources_path + "/Models/SoftBodies/" + file_name;
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
	deselectNode(new_selected_node);
	deselectObject();
}

void Editor::Selector::addSpringMassSpring(Selected_SpringMassObject* selected_object)
{
	// Get SpringMass File Name
	std::string& file_name = static_cast<DataClass::Data_SpringMass*>(selected_object->data_object)->getFile();

	// Determine if the SpringMass File Exists. If Not, Throw Error Since There Are No Nodes to Connect To
	std::filesystem::path temp_path = Global::project_resources_path + "/Models/SoftBodies/" + file_name;
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
		temp_file.open(Global::project_resources_path + "/Models/SoftBodies/" + file_name, std::ios::binary);

		// Generate the New SpringMass Spring Selected Object
		Selected_SpringMassSpring* new_selected_spring = new Selected_SpringMassSpring();

		// Determine the Number of Nodes
		uint8_t temp_byte = 0;
		uint8_t node_iterator = 0;
		temp_file.read((char*)&new_selected_spring->node_count, 1);
		temp_file.read((char*)&temp_byte, 1);

		// If There are Less Than Two Nodes, Throw Error
		if (new_selected_spring->node_count < 2)
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
			new_selected_spring->node_list = new Object::Physics::Soft::NodeData[new_selected_spring->node_count];

			// Temp Data to Read To
			Object::Physics::Soft::NodeData temp_node_data;
			Object::Physics::Soft::Spring temp_spring_data;

			// Reset Node Count
			new_selected_spring->node_count = 0;

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
					temp_node_data.position += static_cast<DataClass::Data_SpringMass*>(selected_object->data_object)->getObjectData().position;
					new_selected_spring->node_list[new_selected_spring->node_count] = temp_node_data;
					new_selected_spring->node_count++;
				}

				// Read Spring Into Dummy Var
				else
				{
					temp_file.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
				}
			}

			// Close File
			temp_file.close();

			// Establish Spring Data
			DataClass::Data_SpringMassSpring* new_data_spring = new DataClass::Data_SpringMassSpring(0);
			Object::Physics::Soft::Spring& spring_data = new_data_spring->getSpringData();
			spring_data.Node1 = new_selected_spring->node_list[0].name;
			spring_data.Node2 = new_selected_spring->node_list[1].name;
			spring_data.RestLength = glm::distance(new_selected_spring->node_list[0].position, new_selected_spring->node_list[1].position);
			spring_data.MaxLength = spring_data.RestLength * 2.0f;
			spring_data.Dampening = 1.0f;
			spring_data.Stiffness = 1.0f;
			new_data_spring->storeParent(static_cast<DataClass::Data_SpringMass*>(selected_object->data_object));
			new_selected_spring->data_object = new_data_spring;

			// Write Data
			deselectSpring(new_selected_spring);
			deselectObject();
		}
	}
}

void Editor::Selector::addHingeObject()
{
}

void Editor::Selector::clampBase(Selected_Object* selected_object)
{
	// Get Object Identifier and Clamp Flag
	//uint8_t* object_identifier = data_objects.at(0)->getObjectIdentifier();
	//bool& clamp = data_objects.at(0)->getEditorData().clamp;
	uint8_t* object_identifier = selected_object->data_object->getObjectIdentifier();
	bool& clamp = selected_object->data_object->getEditorData().clamp;

	// Clamp Object, if Enabled
	if (clamp)
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
					// Get Selected Horizontal Line Data
					Selected_Horizontal_Line& selected_horizontal_line = *static_cast<Selected_Horizontal_Line*>(selected_object);

					// Determine Endpoints of Object
					float half_width = *selected_horizontal_line.object_width * 0.5f;
					float endpoints[8] = { *selected_horizontal_line.object_x - half_width, *selected_horizontal_line.object_y, *selected_horizontal_line.object_x + half_width, *selected_horizontal_line.object_y };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(clamp, endpoints, 0, 2, NULL, c_floor);
					else
						clampObjects(clamp, endpoints, 0, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*selected_horizontal_line.object_width = abs(endpoints[2] - endpoints[0]);
					*selected_horizontal_line.object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*selected_horizontal_line.object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				// Slant
				case Object::Mask::HORIZONTAL_SLANT:
				{
					// Get Selected Line Data
					Selected_Line& selected_line = *static_cast<Selected_Line*>(selected_object);

					// Determine Endpoints of Object
					float endpoints[8] = { *selected_line.object_x, *selected_line.object_y, *selected_line.object_opposite_x, *selected_line.object_opposite_y };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(clamp, endpoints, 1, 2, NULL, c_floor);
					else
						clampObjects(clamp, endpoints, 1, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*selected_line.object_x = endpoints[0];
					*selected_line.object_y = endpoints[1];
					*selected_line.object_opposite_x = endpoints[2];
					*selected_line.object_opposite_y = endpoints[3];

					break;
				}

				// Slope
				case Object::Mask::HORIZONTAL_SLOPE:
				{
					// Get Selected Rectangle Data
					Selected_Rectangle& selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);

					// Determine Endpoints of Object
					float half_width = *selected_rectangle.object_width * 0.5f;
					float half_height = *selected_rectangle.object_height * 0.5f;
					float endpoints[8] = { *selected_rectangle.object_x - half_width, *selected_rectangle.object_y - half_height, *selected_rectangle.object_x + half_width, *selected_rectangle.object_y + half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::FLOOR)
						clampObjects(clamp, endpoints, 2, 2, NULL, c_floor);
					else
						clampObjects(clamp, endpoints, 2, 2, NULL, c_ceiling);

					// Unpack Endpoints
					*selected_rectangle.object_width = (endpoints[2] - endpoints[0]);
					*selected_rectangle.object_height = (endpoints[3] - endpoints[1]);
					*selected_rectangle.object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*selected_rectangle.object_y = (endpoints[1] + endpoints[3]) * 0.5f;

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
					// Get Selected Vertical Line Data
					Selected_Vertical_Line& selected_vertical_line = *static_cast<Selected_Vertical_Line*>(selected_object);

					// Determine Endpoints of Object
					float half_height = *selected_vertical_line.object_height * 0.5f;
					float endpoints[8] = { *selected_vertical_line.object_x, *selected_vertical_line.object_y - half_height, *selected_vertical_line.object_x, *selected_vertical_line.object_y + half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::LEFT_WALL)
						clampObjects(clamp, endpoints, object_identifier[1], 2, NULL, c_left);
					else
						clampObjects(clamp, endpoints, object_identifier[1], 2, NULL, c_right);

					// Unpack Endpoints
					*selected_vertical_line.object_height = endpoints[3] - endpoints[1];
					*selected_vertical_line.object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*selected_vertical_line.object_y = (endpoints[1] + endpoints[3]) * 0.5f;

					break;
				}

				// Curve
				case Object::Mask::VERTICAL_CURVE:
				{
					// Get Selected Rectangle Data
					Selected_Rectangle& selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);

					// Determine Endpoints of Object
					int curve_direction = -1 + 2 * (object_identifier[1] == Object::Mask::RIGHT_WALL);
					float half_width = *selected_rectangle.object_width * 0.5f;
					float half_height = *selected_rectangle.object_height * 0.5f;
					float endpoints[8] = { *selected_rectangle.object_x + half_width * curve_direction, *selected_rectangle.object_y + half_height, *selected_rectangle.object_x - half_width * curve_direction, *selected_rectangle.object_y - half_height };

					// Clamp Object
					if (object_identifier[1] == Object::Mask::LEFT_WALL)
						clampObjects(clamp, endpoints, object_identifier[1] + 2, 2, Algorithms::Math::getSign(*selected_rectangle.object_height), c_left);
					else
						clampObjects(clamp, endpoints, object_identifier[1] + 2, 2, Algorithms::Math::getSign(*selected_rectangle.object_height), c_right);

					// Unpack Endpoints
					*selected_rectangle.object_width = abs(endpoints[0] - endpoints[2]);
					*selected_rectangle.object_height = endpoints[1] - endpoints[3];
					*selected_rectangle.object_x = (endpoints[0] + endpoints[2]) * 0.5f;
					*selected_rectangle.object_y = (endpoints[1] + endpoints[3]) * 0.5f;

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
			clampTerrain(object_identifier[2], object_identifier[1], selected_object);
		}
	}
}

void Editor::Selector::clampObjects(bool enabled, float(&endpoints)[8], int Type, int endpoint_count, int extraValue, func function)
{
	// Map to Get All Levels
	const glm::i16vec2 level_map[9] =
	{
		glm::i16vec2(-1, 1),
		glm::i16vec2(0, 1),
		glm::i16vec2(1, 1),
		glm::i16vec2(-1, 0),
		glm::i16vec2(0, 0),
		glm::i16vec2(1, 0),
		glm::i16vec2(-1, -1),
		glm::i16vec2(0, -1),
		glm::i16vec2(1, -1)
	};

	// Only Clamp if Boolean is True
	if (enabled)
	{
		// Clamp For Both Endpoints
		for (int i = 0; i < 2 * endpoint_count; i += 2)
		{
			// Determine Level Location of Endpoint
			glm::i16vec2 endpoint_level;
			level->updateLevelPos(glm::vec2(endpoints[i], endpoints[i + 1]), endpoint_level);

			// Iterate Through Level Endpoint is In And All 8 Levels Surrounding It
			for (int j = 0; j < 9; j++)
			{
				glm::i16vec2 level_index = endpoint_level + level_map[j];

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
	DataClass::Data_Object** floor_masks = nullptr;
	int floor_masks_size = 0;
	unsaved_level->returnMasks(&floor_masks, floor_masks_size, Object::Mask::FLOOR, data_objects.at(0));

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < floor_masks_size; j++)
	{
		// Get Current Object
		glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);
		Object::Mask::Floor::FloorMask* object = static_cast<Object::Mask::Floor::FloorMask*>(floor_masks[j]->generateObject(null_vec));

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
	DataClass::Data_Object** left_masks = nullptr;
	int left_masks_size = 0;
	unsaved_level->returnMasks(&left_masks, left_masks_size, Object::Mask::LEFT_WALL, data_objects.at(0));

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < left_masks_size; j++)
	{
		// Get Current Object
		int iterations = 2;
		glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);
		Object::Mask::Left::LeftMask* object = static_cast<Object::Mask::Left::LeftMask*>(left_masks[j]->generateObject(null_vec));

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
	DataClass::Data_Object** right_masks = nullptr;
	int right_masks_size = 0;
	unsaved_level->returnMasks(&right_masks, right_masks_size, Object::Mask::RIGHT_WALL, data_objects.at(0));

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < right_masks_size; j++)
	{
		// Get Current Object
		int iterations = 2;
		glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);
		Object::Mask::Right::RightMask* object = static_cast<Object::Mask::Right::RightMask*>(right_masks[j]->generateObject(null_vec));

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
	DataClass::Data_Object** ceiling_masks = nullptr;
	int ceiling_masks_size = 0;
	unsaved_level->returnMasks(&ceiling_masks, ceiling_masks_size, Object::Mask::CEILING, data_objects.at(0));

	// Search Through All Collision Mask Floors in Level to Determine if Object Should Clamp
	for (int j = 0; j < ceiling_masks_size; j++)
	{
		// Get Current Object
		glm::vec2 null_vec = glm::vec2(0.0f, 0.0f);
		Object::Mask::Ceiling::CeilingMask* object = static_cast<Object::Mask::Ceiling::CeilingMask*>(ceiling_masks[j]->generateObject(null_vec));

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

void Editor::Selector::clampTerrain(int Shape, int Object, Selected_Object* selected_object)
{
	switch (Shape)
	{
	
	// Rectangle
	case Shape::RECTANGLE:
	{
		// Get Selected Rectangle Data
		Selected_Rectangle& selected_rectangle = *static_cast<Selected_Rectangle*>(selected_object);

		// Create Endpoints
		float half_width = *selected_rectangle.object_width * 0.5f;
		float half_height = *selected_rectangle.object_height * 0.5f;
		float endpoints[8] = {
			*selected_rectangle.object_x - half_width, *selected_rectangle.object_y - half_height, *selected_rectangle.object_x + half_width, *selected_rectangle.object_y - half_height,
			*selected_rectangle.object_x + half_width, *selected_rectangle.object_y + half_height, *selected_rectangle.object_x - half_width, *selected_rectangle.object_y + half_height
		};

		// Clamp
		clampObjects(selected_object->data_object->getEditorData().clamp, endpoints, Shape, 4, Object, c_terrain);

		// Finsish Endpoints
		*selected_rectangle.object_x = (endpoints[0] + endpoints[2] + endpoints[4] + endpoints[6]) * 0.25f;
		*selected_rectangle.object_y = (endpoints[1] + endpoints[3] + endpoints[5] + endpoints[7]) * 0.25f;
		*selected_rectangle.object_width = abs(endpoints[2] - endpoints[0]);
		*selected_rectangle.object_height = abs(endpoints[5] - endpoints[3]);

		break;
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		// Get Selected Trapezoid Data
		Selected_Trapezoid& selected_trapezoid = *static_cast<Selected_Trapezoid*>(selected_object);

		// Create Endpoints
		float half_width = *selected_trapezoid.object_width * 0.5f;
		float half_height = *selected_trapezoid.object_height * 0.5f;
		float endpoints[8] = {
			*selected_trapezoid.object_x - half_width,                          *selected_trapezoid.object_y - half_height,
			*selected_trapezoid.object_x + half_width,                          *selected_trapezoid.object_y - half_height + *selected_trapezoid.object_height_modifier,
			*selected_trapezoid.object_x + half_width + *selected_trapezoid.object_width_modifier, *selected_trapezoid.object_y + half_height + *selected_trapezoid.object_height_modifier,
			*selected_trapezoid.object_x - half_width + *selected_trapezoid.object_width_modifier, *selected_trapezoid.object_y + half_height
		};

		// Clamp
		clampObjects(selected_object->data_object->getEditorData().clamp, endpoints, Shape, 4, Object, c_terrain);

		// Finsish Endpoints
		*selected_trapezoid.object_width = abs(endpoints[2] - endpoints[0]);
		*selected_trapezoid.object_height = abs(endpoints[7] - endpoints[1]);
		*selected_trapezoid.object_width_modifier = endpoints[4] - endpoints[2];
		*selected_trapezoid.object_height_modifier = endpoints[5] - endpoints[7];
		*selected_trapezoid.object_x = (endpoints[2] + endpoints[0]) * 0.5f;
		*selected_trapezoid.object_y = (endpoints[7] + endpoints[1]) * 0.5f;

		break;
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		// Get Selected Triangle Data
		Selected_Triangle& selected_triangle = *static_cast<Selected_Triangle*>(selected_object);

		// Create Endpoints
		float endpoints[8] = {
			selected_triangle.coords1.x, selected_triangle.coords1.y,
			selected_triangle.coords2.x, selected_triangle.coords2.y,
			selected_triangle.coords3.x, selected_triangle.coords3.y,
			0.0f, 0.0f
		};

		// Clamp
		clampObjects(selected_object->data_object->getEditorData().clamp, endpoints, Shape, 3, Object, c_terrain);

		// Sort Endpoints
		selected_triangle.coords1 = glm::vec2(endpoints[0], endpoints[1]);
		selected_triangle.coords2 = glm::vec2(endpoints[2], endpoints[3]);
		selected_triangle.coords3 = glm::vec2(endpoints[4], endpoints[5]);
		Source::Collisions::Point::arrangeTriVertices(selected_triangle.coords1, selected_triangle.coords2, selected_triangle.coords3);

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
	//Object::Terrain::TerrainBase** terrain_objects = nullptr;
	DataClass::Data_Terrain** terrain_objects = nullptr;
	int terrain_objects_size = 0;
	unsaved_level->returnTerrainObjects(&terrain_objects, terrain_objects_size, static_cast<DataClass::Data_Terrain*>(data_objects.at(0))->getLayer(), data_objects.at(0));

	// Perform Actual Clamping
	abstractedClampTerrain(endpoints, midpoints, Type, i, terrain_objects_size, terrain_objects);

	// Delete Terrain Objects Array
	delete[] terrain_objects;
}

void Editor::Selector::abstractedClampTerrain(float(&endpoints)[8], float(&midpoints)[8], int Type, int i, int max, DataClass::Data_Terrain** data)
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
		DataClass::Data_Terrain* object = data[j];

		// Skip Object if Clamp to Clamp is Enabled and Object is Not Clampable
		if (!(!Global::editor_options->option_clamp_to_clamp || (object->getEditorData().clamp && Global::editor_options->option_clamp_to_clamp)))
			continue;

		// Parse Object Shape
		Shape::Shape* shape = object->getShape();
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
				object->getPosition().x - half_width, object->getPosition().y - half_height,
				object->getPosition().x + half_width, object->getPosition().y - half_height,
				object->getPosition().x + half_width, object->getPosition().y + half_height,
				object->getPosition().x - half_width, object->getPosition().y + half_height
			};

			Vertices::Visualizer::visualizePoint(testing_endpoints[0], testing_endpoints[1], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[2], testing_endpoints[3], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[4], testing_endpoints[5], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(testing_endpoints[6], testing_endpoints[7], 1.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			Vertices::Visualizer::visualizePoint(object->getPosition(), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
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
				object->getPosition().x - half_width,                                     object->getPosition().y - half_height,
				object->getPosition().x + half_width,                                     object->getPosition().y - half_height + *temp_trap.pointerToHeightOffset(),
				object->getPosition().x + half_width + *temp_trap.pointerToWidthOffset(), object->getPosition().y + half_height + *temp_trap.pointerToHeightOffset(),
				object->getPosition().x - half_width + *temp_trap.pointerToWidthOffset(), object->getPosition().y + half_height
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
				object->getPosition().x, object->getPosition().y,
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

void Editor::Selector::Selected_Object::setMouseOffset()
{
	offset_x = *object_x - Global::mouseRelativeX;
	offset_y = *object_y - Global::mouseRelativeY;
	change_horizontal = 0;
	change_vertical = 0;
}

void Editor::Selector::Selected_Object::outlineForResize()
{
	float colors[4] = { 0.0f, 0.0f, 0.85f, 1.0f };
	outlineChangeColor(colors);
	resizing = true;
	mouse_intersects_object = true;
}

void Editor::Selector::Selected_Object::outlineForMove()
{
	float colors[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	outlineChangeColor(colors);
	resizing = false;
	mouse_intersects_object = true;
}

void Editor::Selector::Selected_Object::outlineForNotSelected()
{
	float colors[4] = { 0.55f, 0.55f, 0.0f, 0.95f };
	outlineChangeColor(colors);
	resizing = false;
	mouse_intersects_object = false;
}

void Editor::Selector::Selected_Object::outlineChangeColor(float* colors)
{
	// Bind Highlighter Object
	glBindVertexArray(vertex_objects.outlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_objects.outlineVBO);

	// Iterate Through Each Vertex and Change Color
	for (int i = 3 * sizeof(GL_FLOAT), j = 0; j < vertex_objects.outline_vertex_count; i += 7 * sizeof(GL_FLOAT), j++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, i, 4 * sizeof(GL_FLOAT), colors);
	}

	// Unbind Highlighter Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

uint8_t Editor::Selector::Selected_Rectangle::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

	// Test if Mouse is Inside Object
	if ((*object_x - (*object_width * 0.5f) < Global::mouseRelativeX) && (*object_x + (*object_width * 0.5f) > Global::mouseRelativeX) && (*object_y - (*object_height * 0.5f) < Global::mouseRelativeY) && (*object_y + (*object_height * 0.5f) > Global::mouseRelativeY))
	{
		// If Object is Currently Not Being Moved, Test If It Should
		if (!moving)
		{
			// Reset Variables
			change_horizontal = 0;
			change_vertical = 0;
			offset_x = 0;
			offset_y = 0;
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// Test if Rectangle Should Resize
			if (enable_resize)
				testResizeRectangle(true, true);

			// Test if Color of Outline Should Change to Moving
			if (!mouse_intersects_object || (resizing && !(change_vertical || change_horizontal)))
				outlineForMove();

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				if (change_horizontal || change_vertical)
					return RESIZING;
				else
					setMouseOffset();
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Rectangle::updateGroup(Group_Selector& group_selector)
{
	float temp_compare_value = 0.0f;

	// Test the North Extreme Value
	temp_compare_value = *object_y + *object_height * 0.5f;
	if (temp_compare_value > group_selector.extreme_value_north)
		group_selector.extreme_value_north = temp_compare_value;

	// Test the South Extreme Value
	temp_compare_value = *object_y - *object_height * 0.5f;
	if (temp_compare_value < group_selector.extreme_value_south)
		group_selector.extreme_value_south = temp_compare_value;

	// Test the East Extreme Value
	temp_compare_value = *object_x + *object_width * 0.5f;
	if (temp_compare_value > group_selector.extreme_value_east)
		group_selector.extreme_value_east = temp_compare_value;

	// Test the West Extreme Value
	temp_compare_value = *object_x - *object_width * 0.5f;
	if (temp_compare_value < group_selector.extreme_value_west)
		group_selector.extreme_value_west = temp_compare_value;
}

void Editor::Selector::Selected_Rectangle::moveObject()
{
	bool enable_negative = false;
	float new_x = *object_x;
	float new_y = *object_y;

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
		if (!group_selector->resizing)
		{
			new_x = (Global::mouseRelativeX + offset_x + (*object_x - ((*object_width * 0.5f) * change_horizontal))) * 0.5f;

			// Calculate New Width by Multiplying the Distance Between the Mouse and xPos by 2
			*object_width = 2 * (Global::mouseRelativeX + offset_x - new_x) * change_horizontal;

			// Prevent Size from Going Negative
			if (*object_width <= 0.1f && !enable_negative)
				*object_width = 0.1f;
		}

		else
		{
			float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);
			if (*object_x < group_selector->position.x)
				*object_x += f;
			else
				*object_x -= f;
			*object_width -= f;

			// Prevent Size from Going Negative
			if (*object_width <= 0.1f)
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
		if (!group_selector->resizing)
		{
			new_y = (Global::mouseRelativeY + offset_y + (*object_y - ((*object_height * 0.5f) * change_vertical))) * 0.5f;

			// Calculate New Height by Multiplying the Distance Between the Mouse and yPos by 2
			*object_height = 2 * (Global::mouseRelativeY + offset_y - new_y) * change_vertical;

			// Prevent Size from Going Negative
			if (*object_height <= 0.1f && !enable_negative)
				*object_height = 0.1f;
		}

		else
		{
			float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);
			if (*object_y < group_selector->position.y)
				*object_y += f;
			else
				*object_y -= f;
			 *object_height -= f;

			// Prevent Size from Going Negative
			if (*object_height <= 0.1f)
				*object_height = 0.1f;
		}
	}

	// Move if Size Doesn't Change
	if (moving && !(change_vertical || change_horizontal))
	{
		new_x = (float)(Global::mouseRelativeX + offset_x);
		new_y = (float)(Global::mouseRelativeY + offset_y);
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		*object_x = new_x;
		*object_y = new_y;
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	}

	// Else Update Vertices and Buffer Objects
	else
	{
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		*object_x = new_x;
		*object_y = new_y;
		genSelectorVertices(data_object, vertex_objects);
	}
}

void Editor::Selector::Selected_Rectangle::testResizeRectangle(bool enable_horizontal, bool enable_vertical)
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

void Editor::Selector::Selected_Rectangle::setHorizontalGroupResize()
{
	// Resize Left
	if (Global::mouseRelativeX < *object_x)
	{
		offset_x = *object_x - abs(*object_width) * 0.5f - Global::mouseRelativeX;
		change_horizontal = -Algorithms::Math::getSign(*object_width);
	}

	// Resize Right
	else
	{
		offset_x = *object_x + abs(*object_width) * 0.5f - Global::mouseRelativeX;
		change_horizontal = Algorithms::Math::getSign(*object_width);
	}
}

void Editor::Selector::Selected_Rectangle::setVerticalGroupResize()
{
	// Resize South
	if (Global::mouseRelativeY < *object_y)
	{
		offset_y = *object_y - abs(*object_height) * 0.5f - Global::mouseRelativeY;
		change_vertical = -Algorithms::Math::getSign(*object_height);
	}

	// Resize North
	else
	{
		offset_y = *object_y + abs(*object_height) * 0.5f - Global::mouseRelativeY;
		change_vertical = Algorithms::Math::getSign(*object_height);
	}
}

uint8_t Editor::Selector::Selected_Trapezoid::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

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
				if (change_horizontal || change_vertical)
					return RESIZING;
				else
					setMouseOffset();
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Trapezoid::updateGroup(Group_Selector& group_selector)
{
	float temp_compare_value1 = 0.0f; // Higher Value (N/E)
	float temp_compare_value2 = 0.0f; // Lower Value (S/W)

	// The Vertical Offset is Positive
	if (*object_height_modifier > 0)
	{
		temp_compare_value1 = *object_y + abs(*object_height) * 0.5f + *object_height_modifier;
		temp_compare_value2 = *object_y - abs(*object_height) * 0.5f;
	}

	// The Vertical Offset is Negative
	else
	{
		temp_compare_value1 = *object_y + abs(*object_height) * 0.5f;
		temp_compare_value2 = *object_y - abs(*object_height) * 0.5f + *object_height_modifier;
	}

	// Test the North Extreme Value
	if (temp_compare_value1 > group_selector.extreme_value_north)
		group_selector.extreme_value_north = temp_compare_value1;

	// Test the South Extreme Value
	if (temp_compare_value2 < group_selector.extreme_value_south)
		group_selector.extreme_value_south = temp_compare_value2;

	// The Vertical Offset is Positive
	if (*object_width_modifier > 0)
	{
		temp_compare_value1 = *object_x + *object_width * 0.5f + *object_width_modifier;
		temp_compare_value2 = *object_x - *object_width * 0.5f;

	}

	// The Vertical Offset is Negative
	else
	{
		temp_compare_value1 = *object_x + *object_width * 0.5f;
		temp_compare_value2 = *object_x - *object_width * 0.5f + *object_width_modifier;
	}

	// Test the East Extreme Value
	if (temp_compare_value1 > group_selector.extreme_value_east)
		group_selector.extreme_value_east = temp_compare_value1;

	// Test the West Extreme Value
	if (temp_compare_value2 < group_selector.extreme_value_west)
		group_selector.extreme_value_west = temp_compare_value2;
}

void Editor::Selector::Selected_Trapezoid::moveObject()
{
	float new_x = *object_x;
	float new_y = *object_y;

	// Shift Horizontal
	if (change_horizontal)
	{
		// Shift Bottom
		if (change_horizontal == 1)
		{
			if (!group_selector->resizing)
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
					new_x -= change;
					*object_width_modifier += change;

					// Apply Calculated Size Change
					new_y = temp_yPos;
					*object_height = temp_Size2;
				}
			}

			else
			{
				float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);
				if (*object_x < group_selector->position.x)
					*object_x += f;
				else
					*object_x -= f;
				*object_width -= f;

				// Prevent Size from Going Negative
				if (*object_width <= 0.1f)
					*object_width = 0.1f;
			}
		}

		// Shift Top
		else if (change_horizontal == 2)
		{
			if (!group_selector->resizing)
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
					new_y = temp_yPos;
					*object_height = temp_Size2;
				}
			}

			else
			{
				float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);
				if (*object_x < group_selector->position.x)
					*object_x += f;
				else
					*object_x -= f;
				*object_width -= f;

				// Prevent Size from Going Negative
				if (*object_width <= 0.1f)
					*object_width = 0.1f;
			}
		}

		// Prevent Size from Going Negative
		if (*object_height <= 0.1f)
			*object_height = 0.1f;
	}

	// Shift Vertical
	else if (change_vertical)
	{
		// Shift Right
		if (change_vertical == 1)
		{
			if (!group_selector->resizing)
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
					new_x = temp_xPos;
					*object_width = temp_Size1;
				}
			}

			else
			{
				float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);
				if (*object_y < group_selector->position.y)
					*object_y += f;
				else
					*object_y -= f;
				*object_height -= f;

				// Prevent Size from Going Negative
				if (*object_height <= 0.1f)
					*object_height = 0.1f;
			}
		}

		// Shift Left
		else if (change_vertical == 2)
		{
			if (!group_selector->resizing)
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
					new_y -= change;
					*object_height_modifier += change;

					// Apply Calculated Size Change
					new_x = temp_xPos;
					*object_width = temp_Size1;
				}
			}

			else
			{
				float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);
				if (*object_y < group_selector->position.y)
					*object_y += f;
				else
					*object_y -= f;
				*object_height -= f;

				// Prevent Size from Going Negative
				if (*object_height <= 0.1f)
					*object_height = 0.1f;
			}
		}

		// Prevent Size from Going Negative
		if (*object_width <= 0.1f)
			*object_width = 0.1f;
	}

	// Move if Size Doesn't Change
	if (!(change_vertical || change_horizontal))
	{
		new_x = (float)(Global::mouseRelativeX + offset_x);
		new_y = (float)(Global::mouseRelativeY + offset_y);
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		*object_x = new_x;
		*object_y = new_y;
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	}

	// Else Update Vertices and Buffer Objects
	else
	{
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		*object_x = new_x;
		*object_y = new_y;
		genSelectorVertices(data_object, vertex_objects);
	}
}

void Editor::Selector::Selected_Trapezoid::testResizeTrapezoid()
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

void Editor::Selector::Selected_Trapezoid::setHorizontalGroupResize()
{
	// Resize Left
	if (Global::mouseRelativeX < *object_x)
	{
		offset_x = *object_x - abs(*object_width) * 0.5f - Global::mouseRelativeX;
		change_horizontal = 2;
	}

	// Resize Right
	else
	{
		offset_x = *object_x + abs(*object_width) * 0.5f - Global::mouseRelativeX;
		change_horizontal = 1;
	}
}

void Editor::Selector::Selected_Trapezoid::setVerticalGroupResize()
{
	// Resize South
	if (Global::mouseRelativeY < *object_y)
	{
		offset_y = *object_y - abs(*object_height) * 0.5f - Global::mouseRelativeY;
		change_vertical = 2;
	}

	// Resize North
	else
	{
		offset_y = *object_y + abs(*object_height) * 0.5f - Global::mouseRelativeY;
		change_vertical = 1;
	}
}

uint8_t Editor::Selector::Selected_Triangle::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

	//Vertices::Visualizer::visualizePoint(coords1, 0.3f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	//Vertices::Visualizer::visualizePoint(coords2, 0.3f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	//Vertices::Visualizer::visualizePoint(coords3, 0.3f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

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
				setMouseOffset();
				if (selected_vertex)
					return RESIZING;
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Triangle::updateGroup(Group_Selector& group_selector)
{
	updateGroupHelper(group_selector, coords1);
	updateGroupHelper(group_selector, coords2);
	updateGroupHelper(group_selector, coords3);
}

void Editor::Selector::Selected_Triangle::updateGroupHelper(Group_Selector& group_selector, glm::vec2 test_pos)
{
	// Test North
	if (test_pos.y > group_selector.extreme_value_north)
		group_selector.extreme_value_north = test_pos.y;

	// Test South
	else if (test_pos.y < group_selector.extreme_value_south)
		group_selector.extreme_value_south = test_pos.y;

	// Test East
	if (test_pos.x > group_selector.extreme_value_east)
		group_selector.extreme_value_east = test_pos.x;

	// Test West
	else if (test_pos.x < group_selector.extreme_value_west)
		group_selector.extreme_value_west = test_pos.x;
}

void Editor::Selector::Selected_Triangle::moveObject()
{
	float new_x = *object_x;
	float new_y = *object_y;

	if (group_selector->resizing)
	{
		if (change_horizontal > 0)
		{
			glm::vec2* coords_left = nullptr;
			glm::vec2* coords_right = nullptr;
			glm::vec2* coords_mid = nullptr;

			if (coords1.x < coords2.x && coords1.x < coords3.x)
			{
				coords_left = &coords1;
				if (coords2.x < coords3.x)
				{
					coords_mid = &coords2;
					coords_right = &coords3;
				}

				else
				{
					coords_mid = &coords3;
					coords_right = &coords2;
				}
			}

			else if (coords3.x < coords1.x && coords2.x < coords3.x)
			{
				coords_left = &coords2;
				if (coords1.x < coords3.x)
				{
					coords_mid = &coords1;
					coords_right = &coords3;
				}

				else
				{
					coords_mid = &coords3;
					coords_right = &coords1;
				}
			}

			else
			{
				coords_left = &coords3;
				if (coords1.x < coords2.x)
				{
					coords_mid = &coords1;
					coords_right = &coords2;
				}

				else
				{
					coords_mid = &coords2;
					coords_right = &coords1;
				}
			}

			float left_distance = coords_mid->x - coords_left->x;
			float right_distance = coords_right->x - coords_mid->x;

			float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);
			coords_left->x += f;
			coords_right->x -= f;

			if (left_distance != 0 && right_distance != 0)
			{
				float ratio = left_distance / right_distance;
				coords_mid->x = (coords_right->x * ratio + coords_left->x) / (ratio + 1);
			}
		}

		if (change_vertical > 0)
		{
			glm::vec2* coords_south = nullptr;
			glm::vec2* coords_north = nullptr;
			glm::vec2* coords_mid = nullptr;

			if (coords1.y < coords2.y && coords1.y < coords3.y)
			{
				coords_south = &coords1;
				if (coords2.y < coords3.y)
				{
					coords_mid = &coords2;
					coords_north = &coords3;
				}

				else
				{
					coords_mid = &coords3;
					coords_north = &coords2;
				}
			}

			else if (coords3.y < coords1.y && coords2.y < coords3.y)
			{
				coords_south = &coords2;
				if (coords1.y < coords3.y)
				{
					coords_mid = &coords1;
					coords_north = &coords3;
				}

				else
				{
					coords_mid = &coords3;
					coords_north = &coords1;
				}
			}

			else
			{
				coords_south = &coords3;
				if (coords1.y < coords2.y)
				{
					coords_mid = &coords1;
					coords_north = &coords2;
				}

				else
				{
					coords_mid = &coords2;
					coords_north = &coords1;
				}
			}

			float south_distance = coords_mid->x - coords_south->x;
			float north_distance = coords_north->x - coords_mid->x;

			float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);
			coords_south->y += f;
			coords_north->y -= f;

			if (south_distance != 0 && north_distance != 0)
			{
				float ratio = south_distance / north_distance;
				coords_mid->x = (coords_north->x * ratio + coords_south->x) / (ratio + 1);
			}
		}

		// Store Vertices in Object Data
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());
		triangle_data = Shape::Triangle(coords2, coords3);
		static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData().position = coords1;
		coords2 = *triangle_data.pointerToSecondPosition();
		coords3 = *triangle_data.pointerToThirdPosition();
		should_sort = true;
	}

	// Test if Triangle Should be Resized
	else if (selected_vertex)
	{
		// Move Highest Vertex
		if (selected_vertex == 1)
		{
			updateSelectedPositions(data_object, Global::mouseRelativeX - coords1.x, Global::mouseRelativeY - coords1.y);
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
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());
		triangle_data = Shape::Triangle(coords2, coords3);
		static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData().position = coords1;
		coords2 = *triangle_data.pointerToSecondPosition();
		coords3 = *triangle_data.pointerToThirdPosition();
		should_sort = true;
	}

	// Move if Size Doesn't Change
	if (!selected_vertex && !group_selector->resizing)
	{
		// Calclate New Coordinates
		glm::vec2 delta_coords2 = coords2 - coords1;
		glm::vec2 delta_coords3 = coords3 - coords1;
		new_x = (float)(Global::mouseRelativeX + offset_x);
		new_y = (float)(Global::mouseRelativeY + offset_y);
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		coords1.x = new_x;
		coords1.y = new_y;
		coords2 = delta_coords2 + coords1;
		coords3 = delta_coords3 + coords1;

		// Store New Position Data
		Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(static_cast<DataClass::Data_Shape*>(data_object)->getShape());
		triangle_data = Shape::Triangle(coords2, coords3);
		static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData().position = coords1;
		coords2 = *triangle_data.pointerToSecondPosition();
		coords3 = *triangle_data.pointerToThirdPosition();
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	}

	// Else Update Vertices and Buffer Objects
	else
		genSelectorVertices(data_object, vertex_objects);
}

void Editor::Selector::Selected_Triangle::testResizeTriangle()
{
	bool enable_resize = true;
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

void Editor::Selector::Selected_Triangle::setHorizontalGroupResize()
{
	change_horizontal = 1.0f;
}

void Editor::Selector::Selected_Triangle::setVerticalGroupResize()
{
	change_vertical = 1.0f;
}

uint8_t Editor::Selector::Selected_Circle::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

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
				setMouseOffset();
				if (selected_vertex)
					return RESIZING;
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Circle::updateGroup(Group_Selector& group_selector)
{
	// Test North
	if (*object_y + *object_radius > group_selector.extreme_value_north)
		group_selector.extreme_value_north = *object_y + *object_radius;

	// Test South
	if (*object_y - *object_radius < group_selector.extreme_value_south)
		group_selector.extreme_value_south = *object_y - *object_radius;

	// Test East
	if (*object_x + *object_radius > group_selector.extreme_value_east)
		group_selector.extreme_value_east = *object_x + *object_radius;

	// Test West
	if (*object_x - *object_radius < group_selector.extreme_value_west)
		group_selector.extreme_value_west = *object_x - *object_radius;
}

void Editor::Selector::Selected_Circle::moveObject()
{
	float new_x = *object_x;
	float new_y = *object_y;

	// Group Resizing
	if (group_selector->resizing)
	{
		// Horizontal Resize
		if (selected_vertex == 1)
		{
			float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);
			if (*object_x < group_selector->position.x)
				*object_x += f;
			else
				*object_x -= f;
			*object_radius -= f;
		}

		// Vertical Resize
		else if (selected_vertex == 2)
		{
			float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);
			if (*object_y < group_selector->position.y)
				*object_y += f;
			else
				*object_y -= f;
			*object_radius -= f;
		}

		// Prevent Size from Going Negative
		if (*object_radius <= 0.1f)
			*object_radius = 0.1f;
	}

	else
	{
		// Test if Circle is Resizing
		if (selected_vertex)
		{
			// Radius is New Distance Between Mouse and Center
			*object_radius = glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), glm::vec2(*object_x, *object_y));
		}
	}

	// Move if Size Doesn't Change
	if (!selected_vertex)
	{

		new_x = (float)(Global::mouseRelativeX + offset_x);
		new_y = (float)(Global::mouseRelativeY + offset_y);
		updateSelectedPositions(data_object, new_x - *object_x, new_y - *object_y);
		*object_x = new_x;
		*object_y = new_y;
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
	}

	// Else Update Vertices and Buffer Objects
	else
		genSelectorVertices(data_object, vertex_objects);
}

void Editor::Selector::Selected_Circle::setHorizontalGroupResize()
{
	selected_vertex = 1;
}

void Editor::Selector::Selected_Circle::setVerticalGroupResize()
{
	selected_vertex = 2;
}

void Editor::Selector::Selected_Circle::testResizeCircle(float& distance, float& delta_w, float& delta_h)
{
	bool enable_resize = true;
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

uint8_t Editor::Selector::Selected_Horizontal_Line::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

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
				if (change_horizontal)
				{
					offset_x = *object_x - Global::mouseRelativeX + *object_width * 0.5f * Algorithms::Math::getSign(change_horizontal);;
					offset_y = *object_y - Global::mouseRelativeY;
					return RESIZING;
				}
				setMouseOffset();
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Horizontal_Line::updateGroup(Group_Selector& group_selector)
{
	float temp_compare_value = 0.0f;

	// Test the North Extreme Value
	if (*object_y > group_selector.extreme_value_north)
		group_selector.extreme_value_north = *object_y;

	// Test the South Extreme Value
	if (*object_y < group_selector.extreme_value_south)
		group_selector.extreme_value_south = *object_y;

	// Test the East Extreme Value
	temp_compare_value = *object_x + *object_width * 0.5f;
	if (temp_compare_value > group_selector.extreme_value_east)
		group_selector.extreme_value_east = temp_compare_value;

	// Test the West Extreme Value
	temp_compare_value = *object_x - *object_width * 0.5f;
	if (temp_compare_value < group_selector.extreme_value_west)
		group_selector.extreme_value_west = temp_compare_value;
}

void Editor::Selector::Selected_Horizontal_Line::setVerticalGroupResize()
{
	offset_y = *object_y - Global::mouseRelativeY;
}

uint8_t Editor::Selector::Selected_Vertical_Line::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

	// Test if Mouse is Inside Object
	if ((*object_x - 0.5f < Global::mouseRelativeX) && (*object_x + 0.5f > Global::mouseRelativeX) && (*object_y - (*object_height / 2) < Global::mouseRelativeY) && (*object_y + (*object_height / 2) > Global::mouseRelativeY))
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
				if (change_vertical)
				{
					offset_x = *object_x - Global::mouseRelativeX;
					offset_y = *object_y - Global::mouseRelativeY + *object_height * 0.5f * Algorithms::Math::getSign(change_vertical);
					return RESIZING;
				}
				setMouseOffset();
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Vertical_Line::updateGroup(Group_Selector& group_selector)
{
	float temp_compare_value = 0.0f;

	// Test the North Extreme Value
	temp_compare_value = *object_y + *object_height * 0.5f;
	if (temp_compare_value > group_selector.extreme_value_north)
		group_selector.extreme_value_north = temp_compare_value;

	// Test the South Extreme Value
	temp_compare_value = *object_y - *object_height * 0.5f;
	if (temp_compare_value < group_selector.extreme_value_south)
		group_selector.extreme_value_south = temp_compare_value;

	// Test the East Extreme Value
	if (*object_x > group_selector.extreme_value_east)
		group_selector.extreme_value_east = *object_x;

	// Test the West Extreme Value
	if (*object_x < group_selector.extreme_value_west)
		group_selector.extreme_value_west = *object_x;
}

void Editor::Selector::Selected_Vertical_Line::setHorizontalGroupResize()
{
	offset_x = *object_x - Global::mouseRelativeX;
}

uint8_t Editor::Selector::Selected_Line::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

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
				setMouseOffset();
				if (selected_vertex)
					return RESIZING;
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

void Editor::Selector::Selected_Line::updateGroup(Group_Selector& group_selector)
{
	updateGroupHelper(group_selector, *object_x, *object_y);
	updateGroupHelper(group_selector, *object_opposite_x, *object_opposite_y);
}

void Editor::Selector::Selected_Line::updateGroupHelper(Group_Selector& group_selector, float& test_x, float& test_y)
{
	// Test North
	if (test_y > group_selector.extreme_value_north)
		group_selector.extreme_value_north = test_y;

	// Test South
	else if (test_y < group_selector.extreme_value_south)
		group_selector.extreme_value_south = test_y;

	// Test East
	if (test_x > group_selector.extreme_value_east)
		group_selector.extreme_value_east = test_x;

	// Test West
	else if (test_x < group_selector.extreme_value_west)
		group_selector.extreme_value_west = test_x;
}

void Editor::Selector::Selected_Line::moveObject()
{
	float new_x = *object_x;
	float new_y = *object_y;

	if (group_selector->resizing)
	{
		if (change_horizontal > 0.0f)
		{
			float f = abs(group_selector->extreme_value_west - group_selector->position.x) - abs(Global::mouseRelativeX - group_selector->position.x);

			if (*object_x < *object_opposite_x)
			{
				*object_x += f;
				*object_opposite_x -= f;
			}

			else
			{
				*object_x -= f;
				*object_opposite_x += f;
			}
		}

		if (change_vertical > 0.0f)
		{
			float f = abs(group_selector->extreme_value_south - group_selector->position.y) - abs(Global::mouseRelativeY - group_selector->position.y);

			if (*object_y < *object_opposite_y)
			{
				*object_y += f;
				*object_opposite_y -= f;
			}

			else
			{
				*object_y -= f;
				*object_opposite_y += f;
			}
		}
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
	}

	// Move if Size Doesn't Change
	if (!selected_vertex)
	{
		// Find Distance Between Positions
		glm::vec2 delta_pos = glm::vec2(*object_opposite_x, *object_opposite_y) - glm::vec2(*object_x, *object_y);

		// Get New Origin Position
		*object_x = (float)(Global::mouseRelativeX + offset_x);
		*object_y = (float)(Global::mouseRelativeY + offset_y);
		vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));

		// Generate Line Data
		intercept = *object_y - (slope * *object_x);

		// Find New Opposite Position
		*object_opposite_x = *object_x + delta_pos.x;
		*object_opposite_y = *object_y + delta_pos.y;
	}

	// Else Update Vertices and Buffer Objects
	else
	{
		slope = (*object_opposite_y - *object_y) / (*object_opposite_x - *object_x);
		intercept = *object_y - (slope * *object_x);
		genSelectorVertices(data_object, vertex_objects);
	}
}

void Editor::Selector::Selected_Line::setHorizontalGroupResize()
{
	change_horizontal = 1.0f;
	selected_vertex = 1;
}

void Editor::Selector::Selected_Line::setVerticalGroupResize()
{
	change_vertical = 1.0f;
	selected_vertex = 1;
}

void Editor::Selector::Selected_Line::testResizeLine()
{
	bool enable_resize = true;
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
			Global::Selected_Cursor = Global::CURSORS::BIDIRECTIONAL_RESIZE;
		}

		// Test if Mouse is Close to Opposite Vertex
		else if (glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), glm::vec2(*object_opposite_x, *object_opposite_y)) < resize_width)
		{
			// Set Vertex to Second Vertex
			selected_vertex = 2;

			// Set Cursor
			Global::Selected_Cursor = Global::CURSORS::BIDIRECTIONAL_RESIZE;
		}

		// Test if Color of Outline Should Change
		if (!resizing && selected_vertex)
			outlineForResize();
	}
}

void Editor::Selector::Selected_Unsized::moveObject()
{
	Global::Selected_Cursor = Global::CURSORS::HAND;
	*object_x = (float)(Global::mouseRelativeX + offset_x);
	*object_y = (float)(Global::mouseRelativeY + offset_y);
	vertex_objects.model = glm::translate(glm::mat4(1.0f), glm::vec3(*object_x, *object_y, 0.0f));
}

void Editor::Selector::Selected_Unsized::updateGroupHelper(Group_Selector& group_selector, float half_width, float half_height)
{
	float temp_compare_value = 0.0f;

	// Test the North Extreme Value
	temp_compare_value = *object_y + half_height;
	if (temp_compare_value > group_selector.extreme_value_north)
		group_selector.extreme_value_north = temp_compare_value;

	// Test the South Extreme Value
	temp_compare_value = *object_y - half_height;
	if (temp_compare_value < group_selector.extreme_value_south)
		group_selector.extreme_value_south = temp_compare_value;

	// Test the East Extreme Value
	temp_compare_value = *object_x + half_width;
	if (temp_compare_value > group_selector.extreme_value_east)
		group_selector.extreme_value_east = temp_compare_value;

	// Test the West Extreme Value
	temp_compare_value = *object_x - half_width;
	if (temp_compare_value < group_selector.extreme_value_west)
		group_selector.extreme_value_west = temp_compare_value;
}

uint8_t Editor::Selector::Selected_SpringMassObject::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

	// Test if Mouse is Inside Object
	if (Global::mouseRelativeX > *object_x - object_half_width && Global::mouseRelativeX < *object_x + object_half_width && Global::mouseRelativeY > *object_y - object_half_height && Global::mouseRelativeY < *object_y + object_half_height)
	{
		Global::Selected_Cursor = Global::CURSORS::HAND;
		outlineForMove();

		// If Left Mouse Button is Held, Start Moving
		if (Global::LeftClick)
		{
			moving = true;
			setMouseOffset();
			return MOVING;
		}

		result = MOUSED_OVER;
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
			//deselectObject();
			return DESELECTED;
		}
	}

	return result;
}

void Editor::Selector::Selected_SpringMassObject::updateGroup(Group_Selector& group_selector)
{
	updateGroupHelper(group_selector, object_half_width, object_half_height);
}

uint8_t Editor::Selector::Selected_SpringMassNode::updateObject()
{
	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

	// Test if Mouse is Inside Node
	if (Global::mouseRelativeX > *object_x - *object_radius && Global::mouseRelativeX < *object_x + *object_radius && Global::mouseRelativeY > *object_y - *object_radius && Global::mouseRelativeY < *object_y + *object_radius)
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
				setMouseOffset();
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	// Update Node in Origin Object
	//*node_pointer = Object::Physics::Soft::Node(node_data);
	return result;
}

void Editor::Selector::Selected_SpringMassNode::updateGroup(Group_Selector& group_selector)
{
	updateGroupHelper(group_selector, *object_radius, *object_radius);
}

// Function to Update the Object
uint8_t Editor::Selector::Selected_SpringMassSpring::updateObject()
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

	// The Result of the Object Interaction
	uint8_t result = ABSOLUTLY_NOTHING;

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
			change_horizontal = 0;
			change_vertical = 0;

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				change_horizontal = 1;
				return MOVING;
			}
		}

		result = MOUSED_OVER;
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
			change_horizontal = 0;
			change_vertical = 0;

			// If Left Mouse Button is Held, Start Moving
			if (Global::LeftClick)
			{
				moving = true;
				change_vertical = 1;
				return MOVING;
			}
		}

		result = MOUSED_OVER;
	}

	// Else, Test if Object Should be Deselected
	else if (!moving)
	{
		// Update Outline Color
		if (mouse_intersects_object)
			outlineForNotSelected();

		// If Left Click is Pressed, Deselect Object
		if (Global::LeftClick)
			return DESELECTED;
	}

	return result;
}

// Function to Update the Group Selector
void Editor::Selector::Selected_SpringMassSpring::updateGroup(Group_Selector& group_selector)
{

}

void Editor::Selector::Selected_SpringMassSpring::moveObject()
{
	// Move Endpoints
	if (true)//resizing)
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

		// Get Spring Data
		Object::Physics::Soft::Spring& spring_data = static_cast<DataClass::Data_SpringMassSpring*>(data_object)->getSpringData();

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

			// Find Positions of Spring
			glm::vec2 left_pos = glm::vec2(0.0f);
			glm::vec2 right_pos = glm::vec2(0.0f);
			for (int i = 0; i < node_count; i++)
			{
				if (node_list[i].name == spring_data.Node1)
					left_pos = node_list[i].position;
				if (node_list[i].name == spring_data.Node2)
					right_pos = node_list[i].position;
			}

			// Change Vertices
			//storeTempConnectionPos(node_list[spring_data.Node1].position, node_list[spring_data.Node2].position);
			storeTempConnectionPos(left_pos, right_pos);
			genSelectorVertices(data_object, vertex_objects);
		}
	}
}

bool Editor::Selector::Selected_Object::moving;
bool Editor::Selector::Selected_Object::resizing;
bool Editor::Selector::Selected_Object::mouse_intersects_object;
bool Editor::Selector::Selected_Object::rotating;
std::function<void(DataClass::Data_Object*, Editor::Selector::Selected_VertexObjects&)> Editor::Selector::Selected_Object::genSelectorVertices;
std::function<void(bool, Editor::Selector::Selected_Object*)> Editor::Selector::Selected_Object::sortVertices;
std::function<void(glm::vec2&, glm::vec2&)> Editor::Selector::Selected_Object::storeTempConnectionPos;
std::function<void(DataClass::Data_Object*, float, float)> Editor::Selector::Selected_Object::updateSelectedPositions;
Editor::Selector::Group_Selector* Editor::Selector::Selected_Object::group_selector;

