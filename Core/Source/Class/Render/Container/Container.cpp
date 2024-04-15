#include "Container.h"
#include "Globals.h"
#include "Render\Objects\Level.h"
#include "Render\GUI\GUI.h"
#include "Algorithms\Quick Math\QuickMath.h"
#include "Render\Editor\Selector.h"
#include "Render\Editor\ObjectInfo.h"
#include "Render\Struct\DataClasses.h"
#include "Render\Objects\ChangeController.h"
#include "Render\Objects\UnsavedLevel.h"
#include "Render\Objects\UnsavedGroup.h"
#include "Source/Vertices/Buffer/ObjectBuffers.h"
#include "Class/Object/Terrain/TerrainBase.h"
#include "Algorithms/Sorting/SortingAlgorithms.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Vertices/Visualizer/Visualizer.h"
#include "Render\Objects\UnsavedCollection.h"
#include "Render\Objects\UnsavedComplex.h"
#include "Render\Editor\Debugger.h"
#include "Render\Shader\Shader.h"

void Render::Container::initContainer()
{

}

void Render::Container::constructTerrainHelper(int vertex_count, uint16_t terrain_size, uint16_t group_size, Object::Terrain::TerrainBase** terrain_start, Object::Group::GroupObject** group_start)
{
	// Allocate Memory
	Vertices::Buffer::clearObjectVAO(terrainVAO, terrainVBO, vertex_count);
	Vertices::Buffer::clearObjectDataBuffer(Global::InstanceBuffer, vertex_count);

	// Bind Buffer Objects
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::InstanceBuffer);

	// Construct and Store Each Vertex and Instance
	// Store Offset and Instance in Object
	int instance = Constant::INSTANCE_SIZE;
	int instance_index = 1;
	constructTerrainAlgorithm(instance, instance_index);

	// Unbind Buffer Object
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Update Model Matrix for Group Visualizers
	for (int i = 0; i < group_size; i++)
		group_start[i]->updateModelMatrix();
}

void Render::Container::reallocateTextures()
{
	
}

void Render::Container::segregateHelper(ObjectContainer& container, uint16_t** count_map, void** pointer_map)
{
	// The Map for Container Index to Container Storage Type
	const uint8_t storage_map[STORAGE_TYPE_COUNT] = { Object::NULL_TEMP, Object::FLOOR_COUNT, Object::LEFT_COUNT, Object::RIGHT_COUNT, Object::CEILING_COUNT, Object::TRIGGER_COUNT, Object::TERRAIN_COUNT, Object::DIRECTIONAL_COUNT, Object::POINT_COUNT, Object::SPOT_COUNT, Object::BEAM_COUNT, Object::GROUP_COUNT, Object::ELEMENT_COUNT };

	// The Pointer of the Current Object Being Iterated in Container
	Object::Object** object_array_pointer = container.object_array;

	// The Pointer of the Final Object in Container
	Object::Object** object_array_end_pointer = object_array_pointer + container.total_object_count;

	// The Current Object Being Iterated Through
	Object::Object* current_object = nullptr;

	// The Pointer of the Current Object Size
	uint16_t* current_object_size = nullptr;

	// Determines if Index Has Increased
	bool index_increased = false;

	// The Current Index in the Pointer Map
	uint8_t pointer_map_index = 0;

	// Null Initialize All Counts to 0
	for (int i = 0; i < STORAGE_TYPE_COUNT; i++)
	{
		if (count_map[i] != nullptr)
			*count_map[i] = 0;
	}

	// Pass Through All Temp Objects
	while (object_array_pointer < object_array_end_pointer && (*object_array_pointer)->storage_type == Object::NULL_TEMP)
		object_array_pointer++;

	// Segregate Object Pointers
	while (object_array_pointer < object_array_end_pointer)
	{
		// Get the Current Object
		current_object = *object_array_pointer;

		// Iterate Until the Index in Storage Map Equals the Storage Type of Current Object
		while (current_object->storage_type != storage_map[pointer_map_index])
		{
			index_increased = true;
			pointer_map_index++;

			// If Storage Type Count is Reached, Throw Error
			if (pointer_map_index == STORAGE_TYPE_COUNT)
				throw "Y";
		}

		// If Index Increased, Set Start of Current Pointer
		if (index_increased)
		{
			// If Current Pointer is Null, Throw Error
			if (count_map[pointer_map_index] == nullptr)
				throw "X";

			// Setup Pointers
			index_increased = false;
			*((Object::Object***)pointer_map[pointer_map_index]) = object_array_pointer;
			current_object_size = count_map[pointer_map_index];
		}

		// Increment Count of Current Storage Type
		(*current_object_size)++;

		// Increment Object Array Pointer
		object_array_pointer++;
	}

	// Store Current Object Count in Debugger
	debugger->storeObjectCount(container.total_object_count);
}

void Render::Container::drawCommonVisualizers(ObjectContainer& container)
{
	// Draw Visualizer for Group Objects
	for (int i = 0; i < container.group_size; i++)
		container.group_start[i]->drawObject();

	// Bind Texture Shader
	Global::texShaderStatic.Use();
	glUniform1i(Global::staticLocTexture, 0);
	glUniform1i(Global::directionLoc, 1);

	// Draw Directional Lights
	for (int i = 0; i < container.directional_size; i++)
		container.directional_start[i]->blitzObject();

	// Draw Point Lights
	for (int i = 0; i < container.point_size; i++)
		container.point_start[i]->blitzObject();

	// Draw Spot Lights
	for (int i = 0; i < container.spot_size; i++)
		container.spot_start[i]->blitzObject();

	// Draw Beam Lights
	for (int i = 0; i < container.beam_size; i++)
		container.beam_start[i]->blitzObject();

	// Draw Visualizers for Groups
	for (int i = 0; i < container.total_object_count; i++)
		container.object_array[i]->drawGroupVisualizer();
}

uint8_t Render::Container::testSelectorTerrain(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	for (int i = container.terrain_seperators[index]; i < container.terrain_seperators[index + 1]; i++)
	{
		uint8_t returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.terrain_start), container.terrain_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				constructTerrain();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Container::testSelectorTerrainAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container)
{
	for (int i = 0; i < container.terrain_count; i++)
	{
		uint8_t returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.dynamic_start), container.terrain_count, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				constructTerrain();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Container::testSelectorLights(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	uint8_t returned_value = 0;

	// Directional Light
	for (int i = container.directional_seperators[index]; i < container.directional_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.directional_start), container.directional_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Point Light
	for (int i = container.point_seperators[index]; i < container.point_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.point_start), container.point_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Spot Light
	for (int i = container.spot_seperators[index]; i < container.spot_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.spot_start), container.spot_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Beam Light
	for (int i = container.beam_seperators[index]; i < container.beam_seperators[index + 1]; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.beam_start), container.beam_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Container::testSelectorLightsAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container)
{
	uint8_t returned_value = 0;

	// Directional Light
	for (int i = 0; i < container.directional_size; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.directional_start), container.directional_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Point Light
	for (int i = 0; i < container.point_size; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.point_start), container.point_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Spot Light
	for (int i = 0; i < container.spot_size; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.spot_start), container.spot_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	// Beam Light
	for (int i = 0; i < container.beam_size; i++)
	{
		returned_value = testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.beam_start), container.beam_size, selector, i, object_info, container);
		if (returned_value)
		{
			if (returned_value == 2)
				loadLights();
			return returned_value;
		}
	}

	return 0;
}

uint8_t Render::Container::testSelectorEffects(short index, Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	return 0;
}

uint8_t Render::Container::testSelectorEffectsAll(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::GUI::ObjectContainer_GUI& container)
{
	return 0;
}

uint8_t Render::Container::testSelectorPhysics(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	// Test Selector on Base Objects
	if (testSelectorOnList(container.physics_list, selector, object_info))
		return 2;

	// Test Selector on Hinge Objects
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
	{
		// Get Pointer to Object
		Object::Physics::PhysicsBase* object_pointer = &*(container.physics_list.it);

		// Test if Object is a SpringMass
		if (object_pointer->type == Object::Physics::PHYSICS_TYPES::TYPE_HINGE)
		{

		}
	}

	// Test Selector on SpringMass Objects
	for (container.physics_list.it = container.physics_list.beginStatic(); container.physics_list.it != container.physics_list.endStatic(); container.physics_list.it++)
	{
		// Get Pointer to Object
		Object::Physics::PhysicsBase* object_pointer = &*(container.physics_list.it);

		// Test if Object is a SpringMass
		if (object_pointer->type == Object::Physics::PHYSICS_TYPES::TYPE_SPRING_MASS)
		{
			// Get Reference to Object
			Object::Physics::Soft::SpringMass& object = *static_cast<Object::Physics::Soft::SpringMass*>(&*(container.physics_list.it));
			selector.add_child_object = Editor::CHILD_OBJECT_TYPES::NONE;

			// Test if Object is Locked or Marked to Pass Over
			if (object.lock || object.skip_selection)
			{
				Global::Selected_Cursor = Global::CURSORS::LOCK;
				continue;
			}

			// Iterate Through Springs
			for (int i = 0; i < object.spring_count; i++)
			{
				// Test if Spring is to be Skipped
				if (object.skip_springs[i])
					continue;

				// Calculate the Angle Between Nodes
				glm::vec2& node_pos_1 = object.nodes[object.springs[i].Node1].Position;
				glm::vec2& node_pos_2 = object.nodes[object.springs[i].Node2].Position;
				float node_angle = Algorithms::Math::angle_from_vector(node_pos_1 - node_pos_2);

				// Get Angle Between Mouse and Node1
				float mouse_angle = Algorithms::Math::angle_from_vector(node_pos_1 - glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));

				// Get Upper and Lower Bounds of Node Angle to Compare to
				float upper_bound = node_angle + 0.01f;
				float lower_bound = node_angle - 0.01f;

				// Variable That Determines if Angles Match
				bool matching_angles = false;

				// Lower Bound is Negative
				if (lower_bound < 0.0f)
				{
					// Mouse is in Quadrant IV
					if (mouse_angle > 3.14159f)
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle - 6.2832f > lower_bound)
							matching_angles = true;
					}

					// Mouse is in Quadrant I
					else
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle > lower_bound)
							matching_angles = true;
					}
				}

				// Upper Bound is Greater Than 2*pi
				else if (upper_bound > 6.2832f)
				{
					// Mouse is in Quadrant IV
					if (mouse_angle > 3.14159f)
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound && mouse_angle > lower_bound)
							matching_angles = true;
					}

					// Mouse is in Quadrant I
					else
					{
						// Test if Mouse Angle is Within Node Angle
						if (mouse_angle < upper_bound - 6.2832f && mouse_angle > lower_bound)
							matching_angles = true;
					}
				}

				// Both Bounds are in Respectable Values
				else
				{
					// Test if Mouse Angle is Within Node Angle
					if (mouse_angle < node_angle + 0.01f && mouse_angle > node_angle - 0.01f)
						matching_angles = true;
				}

				// If Angles Match, Move On to Test Distances
				if (matching_angles)
				{
					// Calculate Distances
					float max_distance = glm::distance(node_pos_1, node_pos_2);
					float mouse_distance = glm::distance(node_pos_1, glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY));

					// If Mouse Distance is Less Than Max Distance, There is a Collision
					if (mouse_distance < max_distance)
					{
						// Test if User is Going to Skip Spring
						if (Global::Keys[GLFW_KEY_TAB])
						{
							Global::Keys[GLFW_KEY_TAB] = false;
							object.skip_springs[i] = true;
							continue;
						}

						// Enable Highlighter
						selector.highlighting = true;
						object_info.active = true;

						// Set Cursor to Hand
						Global::Selected_Cursor = Global::CURSORS::HAND;

						// Select Spring
						selector.add_child_object = Editor::CHILD_OBJECT_TYPES::SPRINGMASS_SPRING;
						//selector.spring_data.Node1 = object.nodes[object.springs[i].Node1].Name;
						//selector.spring_data.Node2 = object.nodes[object.springs[i].Node2].Name;
						//selector.object_index = 0;
						selector.moused_object = nullptr;
						//selector.data_objects.push_back(object_pointer->data_object);
						selector.highlighted_object = object.data_springs[i];
						selector.temp_connection_pos_left = node_pos_1;
						selector.temp_connection_pos_right = node_pos_2;
						selector.activateHighlighter(glm::vec2(0.0f, 0.0f), Editor::SelectedHighlight::SELECTABLE);
						selector.highlighting = true;
						object_info.clearAll();
						object_info.setObjectType("SpringMass Spring", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
						object_info.addDoubleValue("Nodes: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "L: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " R: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &object.nodes[object.springs[i].Node1].Name, &object.nodes[object.springs[i].Node2].Name, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), true);
						object_info.addSingleValue("Rest Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].RestLength, glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), false);
						object_info.addSingleValue("Max Length: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].MaxLength, glm::vec4(0.0f, 0.9f, 0.0f, 1.0f), false);
						object_info.addSingleValue("Spring Constant: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].Stiffness, glm::vec4(0.0f, 0.0f, 0.9f, 1.0f), false);
						object_info.addSingleValue("Dampening: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.springs[i].Dampening, glm::vec4(0.9f, 0.0f, 0.9f, 1.0f), false);

						// IDEA: Draw Red Circle at Left Node and Blue Circle at Right Node When Selecting
						Vertices::Visualizer::visualizePoint(object.nodes[object.springs[i].Node1].Position, 0.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
						Vertices::Visualizer::visualizePoint(object.nodes[object.springs[i].Node2].Position, 0.5f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

						// If Left Click, Select Object
						if (Global::LeftClick)
						{
							Global::LeftClick = false;

							// Select Spring
							selector.unadded_data_objects.push_back(object.data_springs[i]);

							// Copy File Data Into Stream
							std::stringstream file_stream;
							std::ifstream in_file;
							in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);
							file_stream << in_file.rdbuf();
							in_file.close();

							// Open File for Writing
							std::ofstream out_file;
							out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);

							// Copy the Number of Nodes
							char temp_byte;
							file_stream.read(&temp_byte, 1);
							out_file.put(temp_byte);

							// Copy the Number of Springs Decremented by 1
							file_stream.read(&temp_byte, 1);
							temp_byte--;
							out_file.put(temp_byte);

							// Copy Data Until Match is Found. Skip Match, then Continue Copying
							Object::Physics::Soft::NodeData temp_node_data;
							Object::Physics::Soft::Spring temp_spring_data;
							int spring_count = 0;
							while (!file_stream.eof())
							{
								// Read Identifier
								file_stream.read(&temp_byte, 1);

								if (file_stream.eof())
									break;

								// Node
								if (temp_byte == 0)
								{
									out_file.put(temp_byte);
									file_stream.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
									out_file.write((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								}

								// Spring
								else
								{
									file_stream.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
									if (i != spring_count)
									{
										out_file.put(temp_byte);
										out_file.write((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
									}
									spring_count++;
								}
							}

							// Close File
							out_file.close();

							object.read();

							// Reload All SpringMass Objects With Matching File

							// Enable Selector for Editing
							selector.active = true;
							selector.editing = false;
							//selector.data_objects.push_back(object.data_object);
							//selector.unadded_data_objects.push_back(selector.highlighted_object);
							//selector.readSpringMassFile();

							//return true;
						}

						return 2;
					}
				}
			}

			// Iterate Through Nodes
			for (int i = 0; i < object.node_count; i++)
			{
				// Test if Node is to be Skipped
				if (object.skip_nodes[i])
					continue;

				// Test if Mouse Intersects Node
				if (glm::distance(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), object.nodes[i].Position) < object.nodes[i].Radius)
				{
					// Test if User is Going to Skip Node
					if (Global::Keys[GLFW_KEY_TAB])
					{
						Global::Keys[GLFW_KEY_TAB] = false;
						object.skip_nodes[i] = true;
						continue;
					}

					// Enable Highlighter
					selector.highlighting = true;
					object_info.active = true;

					// Set Cursor to Hand
					Global::Selected_Cursor = Global::CURSORS::HAND;

					// Select Node
					selector.add_child_object = Editor::CHILD_OBJECT_TYPES::SPRINGMASS_NODE;
					//selector.node_data.position = object.nodes[i].Position;
					//selector.node_data.mass = object.nodes[i].Mass;
					//selector.node_data.health = object.nodes[i].Health;
					//selector.node_data.material = object.nodes[i].material;
					//selector.node_data.radius = object.nodes[i].Radius;
					//selector.node_data.name = object.nodes[i].Name;
					//selector.object_index = 0;
					selector.moused_object = nullptr;
					selector.highlighted_object = object.data_nodes[i];
					selector.activateHighlighter(glm::vec2(0.0f, 0.0f), Editor::SelectedHighlight::SELECTABLE);
					selector.highlighting = true;
					object_info.clearAll();
					object_info.setObjectType("SpringMass Node", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
					object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object.nodes[i].Name, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
					object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &object.nodes[i].Position.x, &object.nodes[i].Position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

					// If Left Click, Select Object
					if (Global::LeftClick)
					{
						Global::LeftClick = false;

						// Select Node
						selector.unadded_data_objects.push_back(object.data_nodes[i]);

						//return true;

						// Copy File Data Into Stream
						std::stringstream file_stream;
						std::ifstream in_file;
						in_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);
						file_stream << in_file.rdbuf();
						in_file.close();

						// Open File for Writing
						std::ofstream out_file;
						out_file.open(Global::project_resources_path + "/Models/SoftBodies/" + object.file_name, std::ios::binary);

						// Copy the Number of Nodes Decremented by 1
						char temp_byte;
						file_stream.read(&temp_byte, 1);
						temp_byte--;
						out_file.put(temp_byte);

						// Copy the Number of Springs 
						file_stream.read(&temp_byte, 1);
						out_file.put(temp_byte);

						// Copy Data Until Match is Found. Skip Match, then Continue Copying
						Object::Physics::Soft::NodeData temp_node_data;
						Object::Physics::Soft::Spring temp_spring_data;
						while (!file_stream.eof())
						{
							// Read Identifier
							file_stream.read(&temp_byte, 1);

							if (file_stream.eof())
								break;

							// Node
							if (temp_byte == 0)
							{
								file_stream.read((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								if (temp_node_data.name != object.nodes[i].Name)
								{
									out_file.put(temp_byte);
									out_file.write((char*)&temp_node_data, sizeof(Object::Physics::Soft::NodeData));
								}
							}

							// Spring
							else
							{
								out_file.put(temp_byte);
								file_stream.read((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
								out_file.write((char*)&temp_spring_data, sizeof(Object::Physics::Soft::Spring));
							}
						}

						// Close File
						out_file.close();

						// ReRead Data in SpringMass File
						object.read();

						// Perform Secondary Selection to Link Springs
						object.select3(selector);

						// Enable Selector for Editing
						selector.active = true;
						selector.editing = false;
						//selector.data_objects.push_back(object.data_object);
						//selector.unadded_data_objects.push_back(selector.highlighted_object);
					}

					return 2;
				}
			}
		}
	}

	return 0;
}

uint8_t Render::Container::testSelectorEntity(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	return testSelectorOnList(container.entity_list, selector, object_info);
}

uint8_t Render::Container::testSelectorGroup(Editor::Selector& selector, Editor::ObjectInfo& object_info, ObjectContainer& container)
{
	for (int i = 0; i < container.group_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.group_start), container.group_size, selector, i, object_info, container))
			return true;
	}

	return false;
}

uint8_t Render::Container::testSelectorMasks(Editor::Selector& selector, Editor::ObjectInfo& object_info, Render::Objects::ObjectContainer_Level& container)
{
	// Test Floor Masks
	for (int i = 0; i < container.floor_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.floor_start), container.floor_size, selector, i, object_info, container))
		{
			return true;
		}
	}

	// Test Left Wall Masks
	for (int i = 0; i < container.left_wall_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.left_wall_start), container.left_wall_size, selector, i, object_info, container))
		{
			return true;
		}
	}

	// Test Right Wall Masks
	for (int i = 0; i < container.right_wall_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.right_wall_start), container.right_wall_size, selector, i, object_info, container))
		{
			return true;
		}
	}

	// Test Ceiling Masks
	for (int i = 0; i < container.ceiling_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.ceiling_start), container.ceiling_size, selector, i, object_info, container))
		{
			return true;
		}
	}

	// Test Trigger Masks
	for (int i = 0; i < container.trigger_size; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.trigger_start), container.trigger_size, selector, i, object_info, container))
		{
			return true;
		}
	}

	return false;
}

uint8_t Render::Container::testSelectorElements(Editor::Selector& selector, Editor::ObjectInfo& objectinfo, Render::GUI::ObjectContainer_GUI& container)
{
	for (int i = 0; i < container.element_count; i++)
	{
		if (testSelectorOnObject(reinterpret_cast<Object::Object***>(&container.element_start), container.element_count, selector, i, objectinfo, container))
			return true;
	}

	return false;
}

uint8_t Render::Container::testSelectorOnObject(Object::Object*** object_list, uint16_t& count, Editor::Selector& selector, int index, Editor::ObjectInfo& object_info, ObjectContainer& container)
{
	// Get Reference of List and Object
	Object::Object** temp_list = *object_list;
	Object::Object& object = *(temp_list[index]);

	// Test if Object is Locked or Marked to Pass Over
	if (object.lock || object.skip_selection)
	{
		Global::Selected_Cursor = Global::CURSORS::LOCK;
		return 0;
	}

	// Test if Mouse Intersects Object
	if (object.testMouseCollisions(Global::mouseRelativeX, Global::mouseRelativeY))
	{
		// If Tab is Pressed, Mark Object for Pass Over
		if (Global::Keys[GLFW_KEY_TAB])
		{
			Global::Keys[GLFW_KEY_TAB] = false;
			object.skip_selection = true;
			return 0;
		}

		// Enable Highlighter
		selector.highlighting = true;
		object_info.active = true;

		// If Object is Not Currently Selected, Set Highlighter Visualizer
		if (selector.moused_object != &object)
		{
			selector.moused_object = &object;

			// If Select Was Unsuccessful, Test Next Object
			if (!temp_list[index]->select(selector, object_info, Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT]))
			{
				object_info.active = false;
				Global::Selected_Cursor = Global::CURSORS::FORBIDEN;
				return 1;
			}
		}

		// Set Cursor to Hand
		Global::Selected_Cursor = Global::CURSORS::HAND;

		// If Left Click, Select Object
		if (Global::LeftClick)
		{
			// Disable Left Click
			Global::LeftClick = false;

			// If Attempting to Add a Child Object, Attempt to Add Object as a Child
			if (selector.selectedOnlyOne() && (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT]))
			{
				// Get Selected Object
				DataClass::Data_Object* selected_object = selector.getOnlyOne();

				// Get Parent of Selector
				DataClass::Data_Object* selected_parent = selected_object->getParent();

				// If Attempting to Add a Parent to its Child, Prevent That From Happening
				// For Now, This Will Cause a Deselection, Might Do Something Else in the Future
				// For This object.data_object is calling the function, Test is the selected_parent->getObjectIndex();
				if (selected_parent != nullptr && object.data_object->testIsParent(selected_parent))
				{
					// If Selected Object Cannot be Returned to Level, Don't Deselect
					if (!change_controller->getUnsavedLevelObject(selected_object)->testValidSelection(object.data_object, selected_object))
						return 1;

					// Remove Child from Current Group
					object.data_object->getObjectIdentifier()[3]--;

					// Set Parent of Selected Object to Nothing
					selected_object->setParent(nullptr);

					// Clear Parent of Selected Object in Selector
					selector.clearOnlyOneComplexParent();

					// Set Group Layer to 0
					selected_object->setGroupLayer(0);

					// Recursively Set Group Layer
					Render::Objects::UnsavedCollection* data_group = selected_object->getGroup();
					if (data_group != nullptr)
						data_group->recursiveSetGroupLayer(1);
				}

				// Object Already Belongs to a Parent
				else if (object.data_object->getParent() != nullptr)
				{
					// Determine if the Specified Operation is Valid
					bool adding_to_current_parent = selected_object->testIsParent(object.data_object->getParent());
					if (adding_to_current_parent) {
						if (!change_controller->getUnsavedLevelObject(object.data_object)->testValidSelection(selected_object, object.data_object))
							return 1;
					}
					else {
						// Note: If Object Does Not Have a Group, It is a Standard Group Object
						if (selected_object->getGroup() == nullptr) {
							if (!Render::Objects::UnsavedGroup::testValidSelectionStatic(selected_object, object.data_object))
								return 1;
						}
						else if (!selected_object->getGroup()->testValidSelection(selected_object, object.data_object))
							return 1;
					}

					// Remove Child from Current Group
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object, Render::MOVE_WITH_PARENT::MOVE_DISSABLED);
					object.data_object->getParent()->getObjectIdentifier()[3]--;

					// If Attempting to Add To Its Current Parent, Remove Object From Being a Child
					// For This, Selected Object DataObject is Calling the Function, Test is the object.data_object->getParent()->getObjectIndex();
					if (adding_to_current_parent)
						change_controller->handleSingleSelectorReturn(object.data_object->makeCopySelected(selector), object.data_object, &selector, true, false);

					// Else, Swap Parents
					else
						selector.addChildToOnlyOne(object.data_object->makeCopySelected(selector), object);
				}

				// Else, Add Object as a New Child
				else
				{
					// Test if Object can be Placed in Group
					// Note: If Object Does Not Have a Group, It is a Standard Group Object
					if (selected_object->getGroup() == nullptr) {
						if (!Render::Objects::UnsavedGroup::testValidSelectionStatic(selected_object, object.data_object))
							return 1;
					}
					else if (!selected_object->getGroup()->testValidSelection(selected_object, object.data_object))
						return 1;

					// Remove Child From Level
					storeLevelOfOrigin(selector, object.returnPosition(), Render::MOVE_WITH_PARENT::MOVE_DISSABLED);

					// Add Child to Parent
					selector.addChildToOnlyOne(object.data_object->makeCopySelected(selector), object);
				}
			}

			// Else, Add to Selector
			else
			{
				// Activate Selector
				selector.active = true;

				// Possible Offset Created if From Complex Object
				glm::vec2 complex_offset = glm::vec2(0.0f, 0.0f);

				// If Object is a Complex Object, Store Values for Complex Parent
				// Note: Need to Find a Way to Do This for Children of Objects To Be Selected
				// This Is Because Complex Objects Can Now Become Children, And This Function Will be Skipped
				if (object.group_object != nullptr && object.data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
				{
					// Get the Root Parent Object of Object's Group
					DataClass::Data_Object* complex_version = static_cast<Render::Objects::UnsavedComplex*>(object.data_object->getGroup())->getComplexParent();

					// If Inactive, Set Position Offset
					if (!static_cast<DataClass::Data_ComplexParent*>(complex_version)->isActive())
					{
						// Store Root Parent in Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->storeRootParent(&object);

						// Store Offset in Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->setPositionOffset(object.returnPosition());

						// Activate Root Data Object
						static_cast<DataClass::Data_ComplexParent*>(complex_version)->setActive();
					}

					// If Not Active, DO NOT SELECT
					else
						return 1;
				}

				// Store Level of Origin if Originated From Level
				if (object.parent == nullptr)
					storeLevelOfOrigin(selector, object.returnPosition(), Render::MOVE_WITH_PARENT::MOVE_ENABLED);

				// If Originated From Group, Remove from Group
				else if (object.data_object->getParent() != nullptr)
				{
					// Look Through List of Objects to Determine If Object is From Complex Objects
					Object::Object* current_parent_object = object.parent;
					bool offseting = true;
					while (current_parent_object != nullptr)
					{
						// Test if Root Parent of Group is A Complex Object
						if (current_parent_object->group_object->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
						{
							// Get the Parent Object That Matches the DataClass Parent
							DataClass::Data_Object* current_parent = static_cast<Render::Objects::UnsavedComplex*>(current_parent_object->group_object)->getComplexParent();

							// If Root Parent Object is a Temp Object, DO NOT SELECT
							if (current_parent_object->storage_type == Object::STORAGE_TYPES::NULL_TEMP)
								return 1;

							// If Inactive, Set Position Offset
							if (!static_cast<DataClass::Data_ComplexParent*>(current_parent)->isActive())
							{
								// Store Root Parent in Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->storeRootParent(current_parent_object);

								// Store Offset in Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->setPositionOffset(current_parent_object->returnPosition());

								// Activate Root Data Object
								static_cast<DataClass::Data_ComplexParent*>(current_parent)->setActive();
							}

							// Remove Group Object Offset
							if (offseting)
								complex_offset = static_cast<DataClass::Data_ComplexParent*>(current_parent)->getPositionOffset();
							offseting = false;
						}

						// Check Out the Next Parent
						current_parent_object = current_parent_object->parent;
					}

					// For All Group Objects, Simply Pop From Current Object
					//object.data_object->getParent()->getGroup()->createChangePop(object.data_object, MOVE_WITH_PARENT::MOVE_ENABLED);
					object.data_object->getParent()->getGroup()->createChangePop(object.data_object, Render::MOVE_WITH_PARENT::MOVE_DISSABLED);
				}

				// Make a Copy of the Data Class
				DataClass::Data_Object* dataclass_copy = selector.highlighted_object->makeCopySelected(selector);

				// Remove Object From Global Objects List
				removeMarkedFromList(temp_list[index], &dataclass_copy->getPosition(), container);

				// Reset Object Info
				object_info.clearAll();

				// Apply Possible Complex Offset
				dataclass_copy->offsetPosition(complex_offset);

				// Select the Object
				selector.unadded_data_objects.push_back(dataclass_copy);
			}

			return 2;
		}

		return 1;
	}

	return 0;
}

template<class Type>
inline uint8_t Render::Container::testSelectorOnList(Struct::List<Type>& object_list, Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Iterate Through Object List
	for (object_list.it = object_list.beginStatic(); object_list.it != object_list.endStatic(); object_list.it++)
	{
		// Get Reference and Object
		Type& object = *(object_list.it);

		// Test if Object is Locked or Marked to Pass Over
		if (object.lock || object.skip_selection)
		{
			Global::Selected_Cursor = Global::CURSORS::LOCK;
			return 0;
		}

		// Test if Mouse Intersects Object
		if (object.testMouseCollisions(Global::mouseRelativeX, Global::mouseRelativeY))
		{
			// If Tab is Pressed, Mark Object for Pass Over
			if (Global::Keys[GLFW_KEY_TAB])
			{
				Global::Keys[GLFW_KEY_TAB] = false;
				object.skip_selection = true;
				return 0;
			}

			// Enable Highlighter
			selector.highlighting = true;
			object_info.active = true;

			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If Object is Not Currently Selected, Set Highlighter Visualizer
			if (selector.moused_object != &object)
				object.select(selector, object_info, false);
			selector.highlighting = true;

			// If Left Click, Select Object
			if (Global::LeftClick)
			{
				// Activate Selector
				selector.active = true;

				// Store Level of Origin
				storeLevelOfOrigin(selector, object.returnPosition(), MOVE_WITH_PARENT::MOVE_ENABLED);

				// Remove Object From List
				object_list.removeObject(object_list.it);

				// Reset Object Info
				object_info.clearAll();

				// Make a Copy of the Data Class
				selector.unadded_data_objects.push_back(selector.highlighted_object->makeCopySelected(selector));

				return 2;
			}

			return 1;
		}
	}

	return 0;
}

void Render::Container::removeMarkedFromList(Object::Object* marked_object, glm::vec2* new_selected_position, ObjectContainer& container)
{
	// Note: This Function is Guarenteed to Remove 1 Object From the List, However,
// For the Case Where the Object is From a Complex Object, More Than 1 Objects
// With the Same Index May be Removed. The Final Array Size Will be Equal to the
// Old Object Count - 1, However, The Container Total Object Count May be Decremented
// Multiple Times for Any Repeated Indicies. The Excess and Unused Elements in the New
// Array Will Not be Used and Will be Ignored Until Next Build of the Array

// First, Reset DataClass Objects Array
	marked_object->data_object->clearObjects();

	// Retrieve the Object Index of the Marked Object
	uint32_t marked_index = marked_object->object_index;

	// Retrieve the Total Object Count and Array
	uint16_t old_object_count = container.total_object_count;
	Object::Object** old_object_array = container.object_array;

	// Reset Container
	container = { 0 };

	// Store Values Back in Container
	container.total_object_count = old_object_count;
	container.object_array = old_object_array;

	// Create New Array of Objects
	Object::Object** new_list = new Object::Object * [old_object_count - 1];

	// Coppy Objects From Old List to New List, Skipping Marked Object(s)
	short new_list_index = 0;
	for (uint32_t i = 0; i < old_object_count; i++)
	{
		// Test if Object is Marked
		if (container.object_array[i]->object_index == marked_index)
		{
			// Get the Object
			Object::Object* object = container.object_array[i];

			// Generate the Temp Object
			Object::TempObject* temp_object = new Object::TempObject(object, new_selected_position, object == marked_object);
			temp_objects.push_back(temp_object);

			// Store Temp Object in Place of Parent, If Object Has a Group
			if (object->group_object != nullptr)
			{
				// If Object is Complex, Add as an Instance to Complex Object
				if (object->group_object->getCollectionType() == Objects::UNSAVED_COLLECTIONS::COMPLEX)
					static_cast<Objects::UnsavedComplex*>(object->group_object)->addComplexInstance(temp_object);

				// Store Temp Object as Parent
				Object::Object** children = object->children;
				for (int i = 0; i < object->children_size; i++)
					children[i]->parent = temp_object;
			}

			// If Object Has a Parent, Replace the Original Child Instance With the Temp Object
			if (object->parent != nullptr)
			{
				Object::Object* test_child_object = nullptr;
				for (int i = 0; i < object->parent->children_size; i++)
				{
					test_child_object = object->parent->children[i];
					if (test_child_object == object)
					{
						object->parent->children[i] = temp_object;
						break;
					}
				}
			}

			// Delete Object
			delete object;

			// Also, Decrement Total Object Count
			container.total_object_count--;

			continue;
		}

		// Else, Copy Object
		new_list[new_list_index] = container.object_array[i];
		new_list_index++;
	}

	// Delete Old List
	delete[] container.object_array;

	// Swap Lists
	container.object_array = new_list;

	// Segregate Objects
	segregateObjects();
}

void Render::Container::removeMarkedChildrenFromListHelper(DataClass::Data_Object* marked_parent, ObjectContainer& container)
{	
	// Note: Array Will NOT be Reduced, Since it WILL be Reduced After Parent is Returned

	// Get the Array of Indicies to Remove
	uint32_t* indices = new uint32_t[1];
	indices[0] = marked_parent->getObjectIndex();
	int indices_size = 1;
	getObjectIndicies(marked_parent, &indices, indices_size);

	// If There Were No Children to Remove, No Need to Iterate
	if (indices_size == 0)
		return;

	// Remove Any Indices Found in the Array
	int old_object_count = container.total_object_count;
	int placement_index = 0;
	for (int i = 0; i < old_object_count; i++)
	{
		// Match Was Found
		if (searchObjectIndicies(indices, 0, indices_size, container.object_array[i]->object_index))
		{
			delete container.object_array[i];
			container.total_object_count--;
		}

		// No Match Was Found
		else
		{
			container.object_array[placement_index] = container.object_array[i];
			placement_index++;
		}
	}
}

void Render::Container::getObjectIndicies(DataClass::Data_Object* parent, uint32_t** indicies, int& indicies_size)
{
	// Only Execute if Group is Not Nullptr Not Complex
	if (parent->getGroup() != nullptr)
	{
		// Get Vector of Children
		std::vector<DataClass::Data_Object*>& children = parent->getGroup()->getChildren();

		// Only Execute if There Are Children
		if (children.size() > 0)
		{
			// Dereference the Old Array
			uint32_t* old_array = *indicies;

			// Generate an Array to Hold the New Children
			uint32_t* children_indicies = new uint32_t[children.size()];

			// Copy Children Indicies Into Children Indicies Array
			for (int i = 0; i < children.size(); i++)
				children_indicies[i] = children[i]->getObjectIndex();

			// Sort Children Indicies (Selection Sort)
			int min_index = 0;
			for (int i = 0; i < children.size() - 1; i++)
			{
				min_index = i;
				for (int j = i; j < children.size(); j++)
				{
					if (children_indicies[j] < children_indicies[min_index])
						min_index = j;
				}
				uint32_t temp = children_indicies[min_index];
				children_indicies[min_index] = children_indicies[i];
				children_indicies[i] = temp;
			}

			// Determine the New Size of the Array
			int new_size = indicies_size + children.size();

			// Allocate Memory for the New Array
			uint32_t* new_array = new uint32_t[new_size];

			// Perform a Merge on the Old Array With New Children
			int left = 0;
			int right = 0;
			int count = 0;
			while (count < new_size)
			{
				if (left < indicies_size && right < children.size())
				{
					if (old_array[left] < children_indicies[right])
					{
						new_array[count] = old_array[left];
						left++;
					}

					else
					{
						new_array[count] = children_indicies[right];
						right++;
					}
				}

				else if (left < indicies_size)
				{
					new_array[count] = old_array[left];
					left++;
				}

				else
				{
					new_array[count] = children_indicies[right];
					right++;
				}

				count++;
			}

			// Delete Old and Temp Arrays
			if (indicies_size != 0)
				delete[] old_array;
			delete[] children_indicies;

			// Store Updated Array Values
			(*indicies) = new_array;
			indicies_size = new_size;

			// Recursively Add Children Indicies
			for (DataClass::Data_Object* child : children)
				getObjectIndicies(child, indicies, indicies_size);
		}
	}
}

bool Render::Container::searchObjectIndicies(uint32_t* indicies, int left, int right, uint32_t test_value)
{
	// If Code Reaches Here, Value Was Not Found
	if (left > right)
		return false;

	// Calculate Midpoint in Array
	int midpoint = (left + right) >> 1;

	// If Test Value is at Midpoint, Return True
	if (indicies[midpoint] == test_value)
		return true;

	// If Test Value is Less Than Midpoint Value, Test Lower Half
	if (test_value < indicies[midpoint])
		return searchObjectIndicies(indicies, left, midpoint - 1, test_value);

	// If Test Value is Greater Than Midpoint Value, Test Upper Half
	return searchObjectIndicies(indicies, midpoint + 1, right, test_value);
}

void Render::Container::incorperatNewObjectsHelper(Object::Object** new_objects, int new_objects_size, ObjectContainer& container)
{
	// Generate a New Object Container Array That Can Fit New Objects
	int new_container_size = container.total_object_count + new_objects_size;
	Object::Object** new_object_array = new Object::Object * [new_container_size];

	// Copy Current Objects Into New Container Array
	for (int i = 0; i < container.total_object_count; i++)
		new_object_array[i] = container.object_array[i];

	// Copy New Objects Into New Container Array
	for (int i = container.total_object_count, j = 0; i < new_container_size; i++, j++)
		new_object_array[i] = new_objects[j];

	// Delete the Old Object Array
	delete[] container.object_array;

	// Reset the Container
	container = { 0 };

	// Store New Container Array and Size in Container Object
	container.total_object_count = new_container_size;
	container.object_array = new_object_array;

	// If Object is A Level, Incorporate the New Objects Into Their Corrisponding Sublevels
	// GUIs Don't Have Sublevels, No Need to Set Objects to Active
	if (getContainerType() == CONTAINER_TYPES::LEVEL)
		static_cast<Render::Objects::Level*>(this)->setActives(new_objects, new_objects_size);

	// Perform Object Loading
	loadObjects();
}

void Render::Container::clearTemps()
{
	// Delete ALL Temp Objects
	for (Object::Object* temp : temp_objects)
		delete temp;
	//delete static_cast<Object::TempObject*>(temp);

	// Clear Temp Object Vector
	temp_objects.clear();
}
