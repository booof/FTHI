#include "GUI.h"
#include "Globals.h"
#include "Algorithms\Sorting\SortingAlgorithms.h"
#include "Render\Editor\Selector.h"
#include "Render\Objects\ChangeController.h"
#include "Render\Objects\UnsavedLevel.h"
#include "Render\Editor\Debugger.h"
#include "Render\Camera\Camera.h"
#include "Render\Shader\Shader.h"
#include "Render\Editor\ObjectInfo.h"
#include "Source\Events\EventListeners.h"
#include "Source\Vertices\Buffer\ObjectBuffers.h"
#include "Render\Struct\DataClasses.h"

#include "Source\Loaders\Fonts.h"

void Render::GUI::GUI::constructTerrain()
{
	// TODO: Construct Verticies for Both Static and Dynamic Terrain
	// IDEA: Special Object Type for Static Terrain 
	// IDEA2: Create Another Shader With Same Vertex Shader But Different Fragment Shader for Fullbright
	// IDEA3: When A Terrain Object Is Hidden By a Master Element, Set Alpha to 0
	// IDEA4: Static Terrain Objects Will Have The Lowest Z-Pos for Sorting, But Will Have the Same Z-Pos as Dynamics When Rendering
	// (Foreground Static vs. Foreground Dynamic in Editor Window Object Creation)

	// TODO: Terrain is to be Constructed By Iterating Through Children of Master Elements
	// Children of Master Elements Will be Rendered by The Master Element
	// This Will Allow the Viewport of the Master Element to Function Correctly
	// Terrain With No Master Element Will be Rendered Seperately
	// Will No Longer Need to Set Alpha to 0 to Hide Terrain and Elements, Saves Performance

	// Sort Terrain Objects By Z-Position
	Algorithms::Sorting::quickZSort(container.dynamic_start, container.terrain_count);

	// Reset Number of Vertices
	number_of_vertices = 0;

	// Iterate Through Terrain Objects to Determine Size of Vertices
	for (int i = 0; i < container.terrain_count; i++)
	{
		// Reset Vertices of Terrain
		container.dynamic_start[i]->resetVertices();

		// If Terrain is Static, Start Iterating Static Objects
		if (static_cast<Object::Terrain::TerrainBase*>(container.dynamic_start[i])->layer == Object::Terrain::BACKDROP)
		{
			// Store Pointer to Static Starting Position
			container.static_start = container.dynamic_start + i;

			// Iterate Through Static Objects and Add Up Vertices
			for (; i < container.terrain_count; i++)
			{
				number_of_vertices += container.dynamic_start[i]->number_of_vertices;
				container.static_object_count++;
			}
		}

		// Else, Add Up Vertices of Dynamic Object
		else
		{
			number_of_vertices += container.dynamic_start[i]->number_of_vertices;
			container.dynamic_object_count++;
		}
	}

	// Recursively Generate Terrain Derived From Master Elements

	// Construct the Terrain Objects for the Rest of the Terrain
	constructTerrainHelper(number_of_vertices * 5, container.terrain_count, container.group_size, reinterpret_cast<Object::Terrain::TerrainBase**>(container.dynamic_start), container.group_start);
}

void Render::GUI::GUI::constructTerrainAlgorithm(int& instance, int& instance_index)
{
	int offset_static = 0, offset_dynamic = 0;

	// Recursively Construct Terrain Connected to Master Elements
	for (int i = 0; i < section_count; i++)
		element_sections[i]->genTerrain(offset_static, offset_dynamic, instance, instance_index);

	// Store Current Offsets of Vertices
	static_vertex_offset = offset_static * 0.05;
	dynamic_vertex_offset = offset_dynamic * 0.05;

	// Construct Terrain for the Rest of the Objects
	for (int i = 0; i < container.dynamic_object_count; i++)
		container.dynamic_start[i]->initializeTerrain(offset_dynamic, instance, instance_index);
	for (int i = 0; i < container.static_object_count; i++)
		container.static_start[i]->initializeTerrain(offset_static, instance, instance_index);

	// Store How Many Vertices Don't Correspond to a Master Element
	static_vertex_count = offset_static - static_vertex_offset * 0.05;
	dynamic_vertex_count = offset_dynamic - dynamic_vertex_offset * 0.05;
}

void Render::GUI::GUI::loadLights()
{
	// Allocate Memory for Lights
	Vertices::Buffer::clearLightBuffer(Global::DirectionalBuffer, Global::PointBuffer, Global::SpotBuffer, Global::BeamBuffer, container.directional_size, container.point_size, container.spot_size, container.beam_size);

	// Write Directional Lights to Shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::DirectionalBuffer);
	for (int i = 0, j = 16; i < container.directional_size; i++, j += 96)
	{
		container.directional_start[i]->buffer_offset = j;
		container.directional_start[i]->loadLight();
	}

	// Write Point Lights to Shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);
	for (int i = 0, j = 16; i < container.point_size; i++, j += 80)
	{
		container.point_start[i]->buffer_offset = j;
		container.point_start[i]->loadLight();
	}

	// Write Spot Lights to Shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::SpotBuffer);
	for (int i = 0, j = 16; i < container.spot_size; i++, j += 96)
	{
		container.spot_start[i]->buffer_offset = j;
		container.spot_start[i]->loadLight();
	}

	// Write Beam Lights to Shader
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);
	for (int i = 0, j = 16; i < container.beam_size; i++, j += 96)
	{
		container.beam_start[i]->buffer_offset = j;
		container.beam_start[i]->loadLight();
	}

	// Unbind Buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// Set Flag to Reload Selected Lights
	Global::reload_lights = true;
}

void Render::GUI::GUI::genElements()
{
	// Determine the Number of Master Elements
	section_count = 0;
	element_sections = reinterpret_cast<Render::GUI::MasterElement**>(container.element_start);
	while (section_count < container.element_count && container.element_start[section_count]->getElementType() == Render::GUI::MASTER) {
		element_sections[section_count]->storeViewportStack(&viewport_stack);
		section_count++;
	}

	// Match Scroll Bars to Master Elements
	for (int i = section_count; i < container.element_count; i++)
	{
		if (container.element_start[i]->getElementType() == Render::GUI::SCROLL_BAR)
		{
			// Get Scroll Bar
			Render::GUI::ScrollBar& bar = *static_cast<Render::GUI::ScrollBar*>(container.element_start[i]);

			// Don't Match if Identifier is -1
			if (bar.getIdentifier() == -1)
				continue;

			// Test With Each Master Element
			for (int j = 0; j < section_count; j++)
				element_sections[j]->storeScrollBar(bar);
		}
	}

	// Add All Elements to Their Respective Master Element's Element List

	// Determine Which Elements Don't Have a Master Element
}

void Render::GUI::GUI::segregateObjects()
{
	// Sort the Objects Through Quick Sort
	Algorithms::Sorting::quickIdentifierSort(container.object_array, container.total_object_count);

	// The Map for Pointers in Object Container in Relation to Storage Map
	//                                          NULL,    FLOOR,   LEFT,    RIGHT,   CEILING, TRIGGER, TERRAIN,                  DIRECTIONAL,                  POINT,                  SPOT,                  BEAM,                  GROUP,                  ELEMENT                
	uint16_t* count_map[STORAGE_TYPE_COUNT] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &container.terrain_count, &container.directional_size,  &container.point_size,  &container.spot_size,  &container.beam_size,  &container.group_size,  &container.element_count };
	void* pointer_map[STORAGE_TYPE_COUNT] =   { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &container.dynamic_start, &container.directional_start, &container.point_start, &container.spot_start, &container.beam_start, &container.group_start, &container.element_start };

	// Perform the Segregation
	segregateHelper(container, count_map, pointer_map);

	// Generate Elements
	// IDEA: Vertex Objects for Each Master Element to Render Child Elements
	// TODO: Box Vertex Objects Currently Created on Startup
	genElements();
}

uint8_t Render::GUI::GUI::testSelector(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// If Mouse Moved, Reset Object Pass Over Flag
	if (Global::cursor_Move)
		resetObjectPassOver();

	// Set Highlighting to False
	selector.highlighting = false;
	object_info.active = false;

	// Reset the Scroll Callback
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);

	// The Result of Selection
	uint8_t result = 0;

	// Test Selector On Group Objects
	if (result = testSelectorGroup(selector, object_info, container)) { return result; }

	// Test Selector On Lighting
	if (result = testSelectorLightsAll(selector, object_info, container)) { return result; }

	// Test Selector On GUI Elements
	if (result = testSelectorElements(selector, object_info, container)) { return result; }

	// Test Selector On Effects
	if (result = testSelectorEffectsAll(selector, object_info, container)) { return result; }

	// Test Selector On All Terrain Types (Both Static and Dynamic)
	if (result = testSelectorTerrainAll(selector, object_info, container)) { return result; }

	// If We Ever Get To This Point, Reset the Pass Over Flag for All Objects
	resetObjectPassOver();

	// Return 0 Since There was no Selection
	return 0;
}

void Render::GUI::GUI::removeMarkedChildrenFromList(DataClass::Data_Object* parent)
{
	removeMarkedChildrenFromListHelper(parent, container);
}

void Render::GUI::GUI::incorperatNewObjects(Object::Object** new_objects, int new_objects_size)
{
	incorperatNewObjectsHelper(new_objects, new_objects_size, container);
}

void Render::GUI::GUI::resetObjectPassOver()
{
	// General Objects
	for (uint32_t i = 0; i < container.total_object_count; i++)
		container.object_array[i]->skip_selection = false;
}

void Render::GUI::GUI::storeLevelOfOrigin(Editor::Selector& selector, glm::vec2 position, Render::MOVE_WITH_PARENT disable_move)
{
	// Unsaved Level of Object in GUI is Always 0,0
	selector.level_of_origin = change_controller->getUnsavedLevel(0, 0, 0);

	// Remove Object from Unsaved Level
	selector.level_of_origin->createChangePop(selector.highlighted_object, disable_move);

	// Set Originated From Level Flag to True
	selector.originated_from_level = true;
}

void Render::GUI::GUI::reallocateAll(bool del, uint32_t size)
{
	// Test if Memory Has Previously Been Allocated
	if (del)
	{
		// Delete Objects in Container
		for (int i = 0; i < container.total_object_count; i++)
			delete container.object_array[i];

		// Delete Containers
		delete[] container.object_array;
	}

	// Null Initialize Container
	container = { 0 };

	// Set Size of Container
	container.total_object_count = size;

	// Allocate Memory
	container.object_array = new Object::Object*[size];
}

Render::GUI::GUI::GUI(std::string& gui_path, bool editing)
{
	// Reset Level Objects
	container = { 0 };
	temp_index_holder = 0;

	// Test Output
	std::cout << "Opening GUI at Path: " << gui_path << "\n";

#define READ2
#ifdef READ2

	// Open the GUI Data File for Reading
	std::ifstream gui_data_file = std::ifstream(gui_path + "GUIData.dat");

	// Read the GUI Data
	gui_data_file.read((char*)&gui_data, sizeof(gui_data));

	// Read the GUI Name
	container_name.resize(gui_data.name_size);
	gui_data_file.read(&container_name[0], gui_data.name_size);

#else

	container_name = "test";
	gui_data.name_size = 5;

	// Open the GUI Data File for Reading
	std::ofstream gui_data_file = std::ofstream(gui_path + "SceneData.dat");

	// Read the GUI Data
	gui_data_file.write((char*)&gui_data, sizeof(gui_data));

	// Read the Scene Name
	gui_data_file.write(container_name.c_str(), gui_data.name_size);

#endif

	// Close the File
	gui_data_file.close();

	// If Editing, Allow for Static Projection Matrix and Static Camera to be Used
	if (editing)
	{
		// Set the GUI Projection Matrix
		Global::projection = glm::ortho(-Global::halfScalarX * Global::zoom_scale, Global::halfScalarX * Global::zoom_scale, -50.0f * Global::zoom_scale, 50.0f * Global::zoom_scale, 0.1f, 100.0f);

		// Create the Camera
		camera = new Camera::Camera(0.0f, 0.0f, true, false, glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f));
	}

	// Store the Path to the GUI
	this->gui_path = gui_path;

	// Generate Terrain Buffer Object
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);

	// Bind Buffer
	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);

	// Enable Position Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Enable Instance Index
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(4 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// Unbind Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Initialize Change Controller
	change_controller->storeContainerPointer(this);

	// Store Level Width for Debugger
	debugger->storeCurrentLevel(this);
	camera->updateDebugPositions(true);

	// Dissable Zoom
	Global::zoom_scale = 0.5f;
	glfwSetScrollCallback(Global::window, NULL);
	Source::Listeners::ScrollCallback(Global::window, 0.0f, 0.0f);
	Global::force_dissable_zoom = true;

	// Initialize the Sublevel
	sublevel = Objects::SubLevel(gui_path);

	// Load All Objects for the First Time
	reloadAll();

	// Generate the Viewport Stack
	viewport_stack = ViewportStack(20);
}

Render::GUI::GUI::~GUI()
{
}

Render::CONTAINER_TYPES Render::GUI::GUI::getContainerType()
{
	return CONTAINER_TYPES::GUI;
}

void Render::GUI::GUI::updateLevelPos(glm::vec2 position, glm::i16vec2& level)
{
	// Level Is Always 0,0
	level = glm::i16vec2(0, 0);
}

void Render::GUI::GUI::updateCamera()
{
	// Update Camera to Get Mouse Positions
	camera->updateMousePosition();
}

void Render::GUI::GUI::updateContainer()
{
	// Begin Iteration at Master Elements
	for (int i = 0; i < section_count; i++)
	{
		if (element_sections[i]->updateElement())
			return;
	}
}

void Render::GUI::GUI::drawContainer()
{
	/**/
	glm::mat4 temp_matrix = glm::mat4(1.0f);

	// Rendering During Editing
	if (Global::editing)
	{
		// Refresh the Viewports and Models of the Master Elements
		for (int i = 0; i < section_count; i++)
			element_sections[i]->updateElementModel();
	}

	// Rendering When Not Editing
	else
	{
		// Refresh the Only Viewports of the Master Elements
		for (int i = 0; i < section_count; i++)
			element_sections[i]->updateViewport();
	}

	// Set Shader to Dynamic Objects
	Global::objectShader.Use();

	// Render Dynamic Objects With Lighting Calculations
	glBindVertexArray(terrainVAO);
	glDrawArrays(GL_TRIANGLES, dynamic_vertex_offset, dynamic_vertex_count);
	for (int i = 0; i < section_count; i++)
		element_sections[i]->renderMasterDynamicObjects();
	glBindVertexArray(0);

	// Set Shader to Static Objects
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, false);
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, false, glm::value_ptr(temp_matrix));

	// Render Static Objects With No Clipping of Z-Layer
	
	// Render Elements
	Element** element = container.element_start;
	for (int i = 0; i < container.element_count; i++, element++)
		(*element)->blitzElement();

	// Set Shader to Text
	Global::fontOffsetShader.Use();
	glUniformMatrix4fv(Global::modelLocRelativeFont, 1, false, glm::value_ptr(temp_matrix));
	glUniformMatrix4fv(Global::projectionLocRelativeFont, 1, false, glm::value_ptr(Global::projection));

	// Render Element Text
	element = container.element_start;
	for (int i = 0; i < container.element_count; i++, element++)
	{
		if ((*element)->data_object->getObjectIdentifier()[1] == Render::GUI::ElementList::BOX)
			static_cast<Box*>(*element)->blitzOffsetText();

		else if ((*element)->data_object->getObjectIdentifier()[1] == Render::GUI::ElementList::TEXT)
			static_cast<TextObject*>(*element)->blitzOffsetText();
	}

	/*

	// Temp Uniform Matrix
	glm::mat4 temp_matrix = glm::mat4(1.0f);

	// Set Shader to Static Objects
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, false);
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, false, glm::value_ptr(temp_matrix));

	// Render Static Objects With No Clipping of Z-Layer

	// Render Elements
	Element** element = container.element_start;
	for (int i = 0; i < container.element_count; i++, element++)
		(*element)->blitzElement();

	// Set Shader to Dynamic Objects
	Global::objectShader.Use();

	// Enable Clipping

	// Render Dynamic Objects With Lighting Calculations
	glBindVertexArray(terrainVAO);
	glDrawArrays(GL_TRIANGLES, 0, number_of_vertices);
	glBindVertexArray(0);

	// Dissable Clipping

	// Set Shader to Text
	Global::fontOffsetShader.Use();
	glUniformMatrix4fv(Global::modelLocRelativeFont, 1, false, glm::value_ptr(temp_matrix));
	glUniformMatrix4fv(Global::projectionLocRelativeFont, 1, false, glm::value_ptr(Global::projection));

	// Render Element Text
	element = container.element_start;
	for (int i = 0; i < container.element_count; i++, element++)
	{
		if ((*element)->data_object->getObjectIdentifier()[1] == Render::GUI::ElementList::BOX)
			static_cast<Box*>(*element)->blitzOffsetText();

		else if ((*element)->data_object->getObjectIdentifier()[1] == Render::GUI::ElementList::TEXT)
			static_cast<TextObject*>(*element)->blitzOffsetText();
	}
	*/
}

void Render::GUI::GUI::drawVisualizers()
{
	// Draw Everything Else
	drawCommonVisualizers(container);
}

void Render::GUI::GUI::loadObjects()
{
	// Segregate Some Objects Into Seperate Arrays
	segregateObjects();

	// Load Textures
	reallocateTextures();

	// Build Terrain Vertices
	constructTerrain();

	// Store Lights in Shader
	loadLights();

	// Hide Objects That are Children of All Currently Dissabled Master Elements
}

void Render::GUI::GUI::reloadAll()
{
	// Delete Old Objects Array
	if (!initialized)
		delete[] container.object_array;

	// Reset Container
	container = ObjectContainer_GUI{ 0 };

	// Re-read the Header of the Sublevel
	sublevel.readHeaders();

	// Retrieve the Number of Objects of Sublevel
	sublevel.addHeader(container.total_object_count);

	// Generate the Container
	container.object_array = new Object::Object*[container.total_object_count];
	initialized = true;

	// Read the GUI
	sublevel.readGUI(container.object_array);

	// Load the Objects
	loadObjects();
}

void Render::GUI::GUI::getSublevelSize(glm::vec2& sizes)
{
	// Size of GUI is Arbitrarily Large Values
	sizes.x = 10000.0f;
	sizes.y = 10000.0f;
}

void Render::GUI::GUI::getGUIInfo(GUIData** data, std::string** name)
{
	*data = &gui_data;
	*name = &container_name;
}

std::string Render::GUI::GUI::getGUIPath()
{
	return gui_path;
}

Render::GUI::ViewportStack::ViewportStack(int max_instances_count)
{
	// Temp Uniform Matrix
	static glm::mat4 temp = glm::mat4(1.0f);

	// Allocate Memory for Array
	instances = new ViewportInstance[max_instances_count];

	// Get the Initial Viewport
	ViewportInstance& instance = instances[0];

	// Store Initial Values
	instance.x = 0;
	instance.y = 0;
	instance.width = Global::screenWidth;
	instance.height = Global::screenHeight;
	instance.matrix_ptr = glm::value_ptr(temp);
}

Render::GUI::ViewportStack::~ViewportStack()
{
	// Delete the Array
	// Commented Out Because C++ Was Being a Bitch
	//delete instances;
}

void Render::GUI::ViewportStack::pushInstance(ViewportInstance& instance)
{
	// Increment Index
	instance_index++;

	// Add Viewport to Stack
	instances[instance_index] = instance;

	// Switch to Viewport
	//glScissor(instance.x, instance.y, instance.width, instance.height);
	//glUniformMatrix4fv(Global::modelLocRelativeFont, 1, GL_FALSE, instance.matrix_ptr);

	// Update Offset Matrix
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, instance.matrix_ptr);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Render::GUI::ViewportStack::popInstance()
{
	// Decrement Index
	instance_index--;

	// Get the Previous Viewport
	ViewportInstance& instance = instances[instance_index];

	// Switch to Viewport
	glScissor(instance.x, instance.y, instance.width, instance.height);
	//glUniformMatrix4fv(Global::modelLocRelativeFont, 1, GL_FALSE, instance.matrix_ptr);

	// Update Offset Matrix
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, instance.matrix_ptr);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
