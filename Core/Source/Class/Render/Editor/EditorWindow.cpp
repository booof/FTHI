#include "EditorWindow.h"

// Functions
#include "Source/Algorithms/Common/Common.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Trapezoid/TrapezoidVertices.h"
#include "Source/Vertices/Triangle/TriangleVertices.h"
#include "Source/Vertices/Circle/CircleVertices.h"
#include "Source/Vertices/Line/LineVertices.h"

// Externals
#include "Globals.h"
#include "Constants.h"
#include "Macros.h"

// Other
#include "Class/Render/Shader/Shader.h"
#include "Selector.h"
#include "Class/Object/Terrain/TerrainBase.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Events/EventListeners.h"
#include "Notification.h"
#include "Source/Collisions/Point Collisions/PointCollisions.h"
#include "Source/Loaders/Fonts.h"
#include "Render/GUI/AdvancedString.h"

#include "Render/Struct/DataClasses.h"
#include "Render/GUI/SelectedText.h"
#include "Render/Objects/UnsavedComplex.h"
#include "Render/Objects/ChangeController.h"

void Editor::EditorWindow::initializeWindow()
{
	// Create Width and Height
	width = Global::halfScalarX * 2 - 20.0f;
	height = 80.0f;

	// Create Scale Factor
	scale = width / 170.0f;

	// Create Model Matrix
	window_position = glm::vec2(0.0f, 0.0f);
	model = glm::translate(model, glm::vec3(0, 0, 0));
	editing_model = model;
	selected_model = model;

	// Background Object

	// Generate Vertex Objects
	glGenVertexArrays(1, &backgroundVAO);
	glGenBuffers(1, &backgroundVBO);

	// Bind Array Object
	glBindVertexArray(backgroundVAO);

	// Enable VBO and Bind Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * BACKGROUND_VERTICES_COUNT * 7, NULL, GL_DYNAMIC_DRAW);

	// Send Position Data to Object
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Object
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Array Object
	glBindVertexArray(0);

	// Create Background Vertices
	genBackground();

	// Highlighter Object

	// Generate Highlighter Vertex Objects
	glGenVertexArrays(1, &highlighterVAO);
	glGenBuffers(1, &highlighterVBO);

	// Bind highlighter VAO
	glBindVertexArray(highlighterVAO);

	// Bind highlighter VBO and Nullify it
	glBindBuffer(GL_ARRAY_BUFFER, highlighterVBO);
	glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	// Send Postion Data to Object
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Object
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Highlighter VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Highlighter VAO
	glBindVertexArray(0);

	// Nullified Selected Object 

	// Generate Selected Object Vertex Objects
	glGenVertexArrays(1, &object_visualizerVAO);
	glGenBuffers(1, &object_visualzierVBO);

	// Editing Screen

	// Generate Editing Screen Object
	glGenVertexArrays(1, &editing_screenVAO);
	glGenBuffers(1, &editing_screenVBO);

	// Bind the Array Object of the Editing Screen
	glBindVertexArray(editing_screenVAO);

	// Bind and Clear the Editing Screen Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_DYNAMIC_DRAW);

	// Send Position Data to Shaders
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Shaders
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Array Object
	glBindVertexArray(0);

	/*
	
	// This Code Might be Removed, Depends on If List of Sublevels is Still Needed

	// Load List of Files
	std::string filePath = Global::level_data_path;

	// Initialize File Data
	filesSize = Source::Algorithms::Common::getDirectoryFileNumber(filePath);
	files = new std::string[filesSize];
	int index = 0;

	// Iterate Between Each File in Directory and Append it to File List
	for (const auto& entry : std::filesystem::directory_iterator(filePath))
	{
		// Save File Path to String
		std::string test = entry.path().string();

		// Remove Path of File Object
		test.replace(0, filePath.length() + 1, "");

		// Assign File Object At Index
		files[index] = test;

		// Inrease Index
		index++;
	}
	*/

	// Generate the Master Element
	GUI::DefaultElements* default_elements = new GUI::DefaultElements();
	default_elements->vertical_bar = &bar1;
	default_elements->default_bar = &bar1;
	master = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 100.0f, 100.0f, default_elements);
}

void Editor::EditorWindow::genBackground()
{
	// Vertices of Object
	float vertices[42];

	// Keeps Track of the Offset of the Vertices
	int offset = 0;

	border1X = -width / 3 + 1;
	border2X = width / 4 - 1;

	// Bind Background VAO
	glBindVertexArray(backgroundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);

	// Solid Backdrop Outline 1
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.5f, width + 0.3f, height + 0.3f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices); // Retrieve Vertices From Function
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices); // Attach Vertices to Buffer Object
	offset += 42; // Update Offset

	// Solid Backdrop 2
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.4f, width, height, glm::vec4(.13f, .13f, .13f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Left Border 3
	Vertices::Rectangle::genRectColor(-width / 2 + 1 * scale, 0.0f, -1.0f, 2.0f * scale, height, glm::vec4(.25f, .25f, .25f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Left Divider 4
	Vertices::Rectangle::genRectColor(border1X, 0.0f, -1.0f, 2.0f * scale, height, glm::vec4(.25f, .25f, .25f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Right Border 5
	Vertices::Rectangle::genRectColor(width / 2 - 1 * scale, 0.0f, -1.0f, 2.0f * scale, height, glm::vec4(.25f, .25f, .25f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Right Divider 6
	Vertices::Rectangle::genRectColor(border2X, 0.0f, -1.0f, 2.0f * scale, height, glm::vec4(.25f, .25f, .25f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Top Border 7
	Vertices::Rectangle::genRectColor(0.0f, height / 2 - (5 * scale), -1.0f, width, 10.0f * scale, glm::vec4(.25f, .25f, .25f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;

	// Bottom Border 8
	Vertices::Rectangle::genRectColor(0.0f, -height / 2 + (1 * scale), -1.0f, width, 2.0f * scale, glm::vec4(.25, .25, .25, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);

	// Unbind Background VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Create Scroll Bars
	bar1 = GUI::VerticalScrollBar();
}

void Editor::EditorWindow::genSegregators()
{
	const int EDITING_VERTICES_COUNT = 210;
	EditorVertexCount = 0;

	// Bind the Array Object of the Editing Screen
	glBindVertexArray(editing_screenVAO);

	// Bind and Clear the Editing Screen Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * EDITING_VERTICES_COUNT, NULL, GL_DYNAMIC_DRAW);

	// Keeps Track of Vertex Offset
	int offset = 0;

	// The Top of Editor Window (!= to top of GUI)
	windowTop = (height / 2) - 10 * scale + (position.y - 50);
	editingOffset = height / 2 + (position.y);

	// Used for General Rectangle Shapes
	float vertices_rect[42];

	// Verticle Divider
	Vertices::Rectangle::genRectColor(10.0f * scale, windowTop - 22.5f * scale, -1.2f, 0.5f * scale, 45 * scale, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices_rect);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, 42 * sizeof(GLfloat), vertices_rect);
	offset += 42;

	// Upper Horizontal Divider
	Vertices::Rectangle::genRectColor((border1X + 10.0f * scale) / 2, windowTop - 30.0f * scale, -1.2f, 10.0f * scale - border1X, 0.5f * scale, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices_rect);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, 42 * sizeof(GLfloat), vertices_rect);
	offset += 42;

	// Lower Horizontal Divider
	Vertices::Rectangle::genRectColor((border1X + border2X) / 2, windowTop - 45.0f * scale, -1.2f, border2X - border1X, 0.5f * scale, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices_rect);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, 42 * sizeof(GLfloat), vertices_rect);
	offset += 42;

	// Right Horizontal Divider
	Vertices::Rectangle::genRectColor((border2X + 10.0f * scale) / 2, windowTop - 30.0f * scale, -1.2f, border2X - 10.0f * scale, 0.5f * scale, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices_rect);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, 42 * sizeof(GLfloat), vertices_rect);
	offset += 42;

	// Final Horizontal Divider
	Vertices::Rectangle::genRectColor((border1X + border2X) / 2, windowTop - 60.0f * scale, -1.2f, border2X - border1X, 0.5f * scale, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices_rect);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, 42 * sizeof(GLfloat), vertices_rect);
	offset += 42;

	// Unbind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Editor::EditorWindow::getStringFromStringMap(STRING_MAPS map_type, int& key)
{
	// The File of the Map
	std::ifstream file;

	// Open Correct File and Store Pointer to Key
	std::string* string_pointer = nullptr;
	switch (map_type)
	{

	// Script Map
	case STRING_MAPS::SCRIPT:
	{
		file.open(Global::project_map_path + "ScriptMap.lst");
		script_pointer = &key;
		string_pointer = &script_path;
		break;
	}

	// Texture Map
	case STRING_MAPS::TEXTURE:
	{
		file.open(Global::project_map_path + "TextureMap.lst");
		texture_pointer = &key;
		string_pointer = &texture_path;
		break;
	}

	// Material Map
	case STRING_MAPS::MATERIAL:
	{
		file.open(Global::project_map_path + "MaterialMap.lst");
		material_pointer = &key;
		string_pointer = &material_path;
		break;
	}

	// Physics Material Map
	case STRING_MAPS::PHYSICS_MATERIAL:
	{
		file.open(Global::project_map_path + "PhysicsMaterialMap.lst");
		physics_material_pointer = &key;
		string_pointer = &physics_material_name;
		break;
	}

	}

	// Iterate Through File Until Line Equals Key
	int line_index = 0;
	std::string line = "";
	while (std::getline(file, line))
	{
		// Test if Line Index Equals Key
		if (line_index == key)
		{
			*string_pointer = line;
			break;
		}

		// Else, Continue to Next Line
		line_index++;
	}

	// Close File
	file.close();
}

void Editor::EditorWindow::getKeyFromStringMap(STRING_MAPS map_type)
{
	// The File of the Map
	std::ifstream file;

	// Open Correct File and Store Pointer to String and Key
	int* key_pointer = nullptr;
	std::string* string_pointer = nullptr;
	switch (map_type)
	{

	// Script Map
	case STRING_MAPS::SCRIPT:
	{
		file.open(Global::project_map_path + "ScriptMap.lst");
		key_pointer = script_pointer;
		string_pointer = &script_path;
		break;
	}

	// Texture Map
	case STRING_MAPS::TEXTURE:
	{
		file.open(Global::project_map_path + "TextureMap.lst");
		key_pointer = texture_pointer;
		string_pointer = &texture_path;
		break;
	}

	// Material Map
	case STRING_MAPS::MATERIAL:
	{
		file.open(Global::project_map_path + "MaterialMap.lst");
		key_pointer = material_pointer;
		string_pointer = &material_path;
		break;
	}

	// Physics Material Map
	case STRING_MAPS::PHYSICS_MATERIAL:
	{
		file.open(Global::project_map_path + "PhysicsMaterialMap.lst");
		key_pointer = physics_material_pointer;
		string_pointer = &physics_material_name;
		break;
	}

	}

	// If String Pointer is NULL, Return
	if (string_pointer == nullptr)
		return;

	// Iterate Through File Until String Equals Value
	int line_index = 0;
	std::string& string = *string_pointer;
	std::string line = "";
	bool string_found = false;
	while (std::getline(file, line))
	{
		// Test if Line Equals Value
		if (line == string)
		{
			*key_pointer = line_index;
			string_found = true;
			break;
		}

		// Else, Continue to Next Line
		line_index++;
	}

	// Close File
	file.close();

	// If A Match Was Not Found, Throw Notification and Revert to Previous String
	if (!string_found)
	{
		std::string map_strings[4] = { "Script", "Texture", "Material", "Physics Material" };
		std::string message = "UNKOWN IDENTIFIER ERROR DETECTED\n" + map_strings[(int)map_type] + " Does Not Have Matching String\n\nEither the Identifier is Not Linked Yet or the\nIdentifier is Not Spelled Correctly\n\nPlease Check Spelling And/Or Identifier Linker";
		notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
		if (key_pointer != nullptr)
			getStringFromStringMap(map_type, *key_pointer);
	}
}

void Editor::EditorWindow::resetBoxes(uint8_t new_box_size, uint8_t new_text_size)
{
	// If Boxes Are Already Allocated, Delete Boxes
	if (boxes_size)
	{
		// Delete Individual Box Objects
		for (int i = 0; i < boxes_size; i++)
			delete boxes[i];

		// Delete Box Array
		delete[] boxes;
	}

	// If Text is Already Allocated, Delete Text
	if (texts_size)
	{
		// Delete Individual Text Objects
		for (int i = 0; i < texts_size; i++)
			delete texts[i];

		// Delete Text Array
		delete[] texts;
	}

	// Store New Sizes
	boxes_size = new_box_size;
	texts_size = new_text_size;

	// Allocate Memory for Array
	boxes = new GUI::Box*[boxes_size];
	texts = new GUI::TextObject*[texts_size];
}

void Editor::EditorWindow::genBoxesCommon(uint8_t& box_offset, uint8_t& text_offset, float* posX, float* posY, DataClass::Data_Object* data_object)
{
	// Set Lock/Clamp Boxes Sizes
	temp_box_data.width = 3.0f;
	temp_box_data.height = 3.0f;
	temp_box_data.zpos = -1.0f;
	temp_box_data.centered = true;

	// Clamp Box
	temp_box_data.position = glm::vec2(32.0f * scale, windowTop - 5.0f);
	temp_box_data.button_text = GUI::AdvancedString("");
	temp_box_data.mode = GUI::TOGGLE_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_object->getEditorData().clamp);
	box_offset++;

	// Clamp Text
	temp_text_data.position = glm::vec2(15.0f * scale, windowTop - 6.0);
	temp_text_data.text = "Clamp:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Lock Box
	temp_box_data.position = glm::vec2(32.0f * scale, windowTop - 9.0f);
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_object->getEditorData().lock);
	box_offset++;

	// Lock Text
	temp_text_data.position = glm::vec2(15.0f * scale, windowTop - 10.0f);
	temp_text_data.text = "Lock:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// X-Pos Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-18.0f * scale, windowTop - 32.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*posX));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(posX);
	box_offset++;

	// X-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 33.0f);
	temp_text_data.text = "xPos:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Y-Pos Box
	temp_box_data.position = glm::vec2(-18.0f * scale, windowTop - 39.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*posY));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(posY);
	box_offset++;

	// Y-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 40.0f);
	temp_text_data.text = "yPos:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Name Box
	temp_box_data.width = 70.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-2.0f * scale, windowTop - 46.0f);
	temp_box_data.button_text = data_object->getName();
	temp_box_data.mode = GUI::ALPHABETICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_object->getName());
	box_offset++;

	// Name Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 47.0f);
	temp_text_data.text = "Name:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Script Box
	temp_box_data.position = glm::vec2(-2.0f * scale, windowTop - 53.0f);
	temp_box_data.button_text = script_path;
	temp_box_data.mode = GUI::GENERAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&script_path);
	update_script = box_offset;
	box_offset++;

	// Script Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 54.0f);
	temp_text_data.text = "Script:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesHorizontalLine(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Horizontal Line
	DataClass::Data_HorizontalLine& data_horizontal_line = *static_cast<DataClass::Data_HorizontalLine*>(data_object);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_horizontal_line.getHorizontalLineData().width));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_horizontal_line.getHorizontalLineData().width);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesSlant(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Horizontal Slant
	DataClass::Data_Slant& data_slant = *static_cast<DataClass::Data_Slant*>(data_object);

	// X-Pos Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_slant.getSlantData().position2.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_slant.getSlantData().position2.x);
	box_offset++;

	// X-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "xPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Y-Pos Box
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_slant.getSlantData().position2.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_slant.getSlantData().position2.y);
	box_offset++;

	// Y-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "yPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesSlope(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Horizontal Slope
	DataClass::Data_Slope& data_slope = *static_cast<DataClass::Data_Slope*>(data_object);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_slope.getSlopeData().width));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_slope.getSlopeData().width);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Box
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_slope.getSlopeData().height));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_slope.getSlopeData().height);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesPlatform(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Get Platform from Correct Data Class
	bool* floor_mask_platform;
	switch (data_object->getObjectIdentifier()[2])
	{
	case Object::Mask::HORIZONTAL_SLANT: floor_mask_platform = &static_cast<DataClass::Data_FloorMaskSlant*>(data_object)->getPlatform(); break;
	case Object::Mask::HORIZONTAL_SLOPE: floor_mask_platform = &static_cast<DataClass::Data_FloorMaskSlope*>(data_object)->getPlatform(); break;
	default: floor_mask_platform = &static_cast<DataClass::Data_FloorMaskHorizontalLine*>(data_object)->getPlatform();
	}

	// Platform Box
	temp_box_data.width = 20.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-42.0f * scale, height_offset);
	temp_box_data.button_text = GUI::AdvancedString("Platform?");
	temp_box_data.mode = GUI::TOGGLE_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(floor_mask_platform);
	box_offset++;
}

void Editor::EditorWindow::genBoxesVerticalLine(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Vertical Line
	DataClass::Data_VerticalLine& data_vertical_line = *static_cast<DataClass::Data_VerticalLine*>(data_object);

	// Height Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_vertical_line.getVerticalLineData().height));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_vertical_line.getVerticalLineData().height);
	box_offset++;

	// Height Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesCurve(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Vertical Curve
	DataClass::Data_Curve& data_curve = *static_cast<DataClass::Data_Curve*>(data_object);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_curve.getCurveData().width));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_curve.getCurveData().width);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Box
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_curve.getCurveData().height));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_curve.getCurveData().height);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesMaterial(uint8_t& box_offset, uint8_t& text_offset, float height_offset)
{
	// Physics Material Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1);
	temp_text_data.text = "Physics Material";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Physics Material Box
	temp_box_data.width = 60.0f;
	temp_box_data.position = glm::vec2(-20.0f * scale, height_offset - 6.0f);
	temp_box_data.button_text = texture_path;
	temp_box_data.mode = GUI::ALPHABETICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&physics_material_name);
	box_offset++;
}

void Editor::EditorWindow::genBoxesTrigger(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Trigger Mask
	DataClass::Data_TriggerMask& data_trigger = *static_cast<DataClass::Data_TriggerMask*>(data_object);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_trigger.getTriggerData().width));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_trigger.getTriggerData().width);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_trigger.getTriggerData().height));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_trigger.getTriggerData().height);
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesObject(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Trigger Mask
	Object::ObjectData& object_data = (static_cast<DataClass::Data_SubObject*>(data_object))->getObjectData();

	// Enable Color Wheel
	wheel_active = true;

	// Set Un-Normalized Wheel Color Data
	wheel_color[0] = (unsigned int)(object_data.colors.r * 255.0f);
	wheel_color[1] = (unsigned int)(object_data.colors.g * 255.0f);
	wheel_color[2] = (unsigned int)(object_data.colors.b * 255.0f);
	wheel_color[3] = (unsigned int)(object_data.colors.a * 255.0f);

	// Assign Color Wheel
	wheel_box_start = box_offset;
	assignColorWheel(&wheel, box_offset, text_offset, wheel_color, height_offset);

	// Normals Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 25.0f);
	temp_text_data.text = "Normals";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Normal X Box
	temp_box_data.width = 20.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-35.0f * scale, height_offset - 30.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(object_data.normals.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&object_data.normals.x);
	box_offset++;

	// Normal X Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 31.0f);
	temp_text_data.text = "X:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Normal Y Box
	temp_box_data.position.x = -8.0f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(object_data.normals.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&object_data.normals.y);
	box_offset++;

	// Normal Y Text
	temp_text_data.position.x = -23.0f * scale;
	temp_text_data.text = "Y:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Normal Z Box
	temp_box_data.position.x = 19.0f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(object_data.normals.z));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&object_data.normals.z);
	box_offset++;

	// Normal Z Text
	temp_text_data.position.x = 4.0f * scale;
	temp_text_data.text = "Z:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Texture Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 40.0f);
	temp_text_data.text = "Texture";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Texture Box
	temp_box_data.width = 60.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 45.0f);
	temp_box_data.button_text = texture_path;
	temp_box_data.mode = GUI::GENERAL_TEXT_BOX;
	temp_box_data.text_color = glm::vec4(0.88f, 0.42f, 0.0f, 1.0f);
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&texture_path);
	box_offset++;

	// Material Text
	temp_text_data.position.y = height_offset - 55.0f;
	temp_text_data.text = "Material";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Material Box
	temp_box_data.position.y = height_offset - 60.0f;
	temp_box_data.button_text = material_path;
	temp_box_data.text_color = glm::vec4(0.62f, 0.13f, 0.49f, 1.0f);
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&material_path);
	box_offset++;

	// Z-Percent Text
	temp_text_data.position.y = height_offset - 70.0f;
	temp_text_data.text = "Z-Percent";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Z-Percent Box
	temp_box_data.width = 20.0f;
	temp_box_data.position = glm::vec2(-35.0f * scale, height_offset - 75.0f);
	temp_box_data.button_text = std::to_string(object_data.zpos);
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	temp_box_data.text_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&object_data.zpos);
	box_offset++;
}

void Editor::EditorWindow::genBoxesRectangle(uint8_t& box_offset, uint8_t& text_offset, float height_offset, Shape::Shape* shape)
{
	// Get Rectangle Shape
	Shape::Rectangle& rectangle_data = *static_cast<Shape::Rectangle*>(shape);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*rectangle_data.pointerToWidth()));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(rectangle_data.pointerToWidth());
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*rectangle_data.pointerToHeight()));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(rectangle_data.pointerToHeight());
	box_offset++;

	// Height Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesTrapezoid(uint8_t& box_offset, uint8_t& text_offset, float height_offset, Shape::Shape* shape)
{
	// Get Trapezoid Shape
	Shape::Trapezoid& trapezoid_data = *static_cast<Shape::Trapezoid*>(shape);

	// Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*trapezoid_data.pointerToWidth()));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(trapezoid_data.pointerToWidth());
	box_offset++;

	// Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*trapezoid_data.pointerToHeight()));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(trapezoid_data.pointerToHeight());
	box_offset++;

	// Height Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Width Offset Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 14.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*trapezoid_data.pointerToWidthOffset()));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(trapezoid_data.pointerToWidthOffset());
	box_offset++;

	// Width Offset Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 15.0f);
	temp_text_data.text = "Width Offset:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Height Offset Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 21.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*trapezoid_data.pointerToHeightOffset()));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(trapezoid_data.pointerToHeightOffset());
	box_offset++;

	// Height Offset Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 22.0f);
	temp_text_data.text = "Height Offset:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesTriangle(uint8_t& box_offset, uint8_t& text_offset, float height_offset, Shape::Shape* shape)
{
	// Get Triangle Shape
	Shape::Triangle& triangle_data = *static_cast<Shape::Triangle*>(shape);

	// xPos 2 Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(triangle_data.pointerToSecondPosition()->x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&triangle_data.pointerToSecondPosition()->x);
	box_offset++;

	// xPos 2 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "xPos 2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// yPos 2 Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(triangle_data.pointerToSecondPosition()->y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&triangle_data.pointerToSecondPosition()->y);
	box_offset++;

	// yPos 2 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "yPos 2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// xPos 3 Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 14.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(triangle_data.pointerToThirdPosition()->x));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&triangle_data.pointerToThirdPosition()->x);
	box_offset++;

	// xPos 3 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 15.0f);
	temp_text_data.text = "xPos 3:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// yPos 3 Box
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 21.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(triangle_data.pointerToThirdPosition()->y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&triangle_data.pointerToThirdPosition()->y);
	box_offset++;

	// yPos 3 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 22.0f);
	temp_text_data.text = "yPos 3:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesCircle(uint8_t& box_offset, uint8_t& text_offset, float height_offset, Shape::Shape* shape)
{
	// Get Circle Shape
	Shape::Circle& circle_data = *static_cast<Shape::Circle*>(shape);

	// Radius Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*circle_data.pointerToRadius()));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(circle_data.pointerToRadius());
	box_offset++;

	// Radius Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Radius:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesPolygon(uint8_t& box_offset, uint8_t& text_offset, float height_offset, Shape::Shape* shape)
{
	// Get Polygon Shape
	Shape::Polygon& polygon_data = *static_cast<Shape::Polygon*>(shape);

	// Radius Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*polygon_data.pointerToRadius()));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(polygon_data.pointerToRadius());
	box_offset++;

	// Radius Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Radius:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Offset Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-10.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*polygon_data.pointerToAngleOffset()));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(polygon_data.pointerToAngleOffset());
	box_offset++;

	// Offset Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Offset:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Number of Sides Box
	temp_box_data.width = 12.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 14.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(*polygon_data.pointerToNumberOfSides()));
	temp_box_data.mode = GUI::ABSOLUTE_INTEGER_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(polygon_data.pointerToNumberOfSides());
	box_offset++;

	// Number of Sides Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 15.0f);
	temp_text_data.text = "Number of Sides:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesLight(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Get Light Data
	Object::Light::LightData& light_data = static_cast<DataClass::Data_Light*>(data_object)->getLightData();

	// Enable Light Wheels
	light_active = true;

	// Ambient Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset);
	temp_text_data.text = "Ambient";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Diffuse Text
	temp_text_data.position.y = height_offset - 37.0f;
	temp_text_data.text = "Diffuse";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Ambient Text
	temp_text_data.position.y = height_offset - 74.0f;
	temp_text_data.text = "Specular";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Intensity Text
	temp_text_data.position.y = height_offset - 111.0f;
	temp_text_data.text = "Intensity";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Intensity Box
	temp_box_data.width = 20.0f;
	temp_box_data.position = glm::vec2(-35.0f * scale, height_offset - 116.0f);
	temp_box_data.button_text = std::to_string(light_data.intensity);
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&light_data.intensity);
	box_offset++;

	// Set Un-Normalized Ambient Wheel Color Data
	ambient_color[0] = (unsigned int)(light_data.ambient.r * 255.0f);
	ambient_color[1] = (unsigned int)(light_data.ambient.g * 255.0f);
	ambient_color[2] = (unsigned int)(light_data.ambient.b * 255.0f);
	ambient_color[3] = (unsigned int)(light_data.ambient.a * 255.0f);

	// Assign Ambient Color Wheel
	light_wheel_box_start = box_offset;
	assignColorWheel(&wheelAmbient, box_offset, text_offset, ambient_color, height_offset - 12.0f);

	// Set Un-Normalized Diffuse Wheel Color Data
	diffuse_color[0] = (unsigned int)(light_data.diffuse.r * 255.0f);
	diffuse_color[1] = (unsigned int)(light_data.diffuse.g * 255.0f);
	diffuse_color[2] = (unsigned int)(light_data.diffuse.b * 255.0f);
	diffuse_color[3] = (unsigned int)(light_data.diffuse.a * 255.0f);

	// Assign Ambient Color Wheel
	assignColorWheel(&wheelDiffuse, box_offset, text_offset, diffuse_color, height_offset - 49.0f);

	// Set Un-Normalized Specular Wheel Color Data
	specular_color[0] = (unsigned int)(light_data.specular.r * 255.0f);
	specular_color[1] = (unsigned int)(light_data.specular.g * 255.0f);
	specular_color[2] = (unsigned int)(light_data.specular.b * 255.0f);
	specular_color[3] = (unsigned int)(light_data.specular.a * 255.0f);

	// Assign Ambient Color Wheel
	assignColorWheel(&wheelSpecular, box_offset, text_offset, specular_color, height_offset - 86.0f);
}

void Editor::EditorWindow::genBoxesDirectional(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Directional Light
	DataClass::Data_Directional& data_directional = *static_cast<DataClass::Data_Directional*>(data_object);

	// X-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "xPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// X-Pos Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_directional.getDirectionalData().position2.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_directional.getDirectionalData().position2.x);
	box_offset++;

	// Y-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "yPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Y-Pos Box
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_directional.getDirectionalData().position2.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_directional.getDirectionalData().position2.y);
	box_offset++;
}

void Editor::EditorWindow::genBoxesPoint(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Point Light
	DataClass::Data_Point& data_point = *static_cast<DataClass::Data_Point*>(data_object);

	// Linear Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Linear:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Linear Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_point.getPointData().linear));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_point.getPointData().linear);
	box_offset++;

	// Quadratic Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Quadratic:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Quadratic Box
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_point.getPointData().quadratic));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_point.getPointData().quadratic);
	box_offset++;
}

void Editor::EditorWindow::genBoxesSpot(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Directional Light
	DataClass::Data_Spot& data_spot = *static_cast<DataClass::Data_Spot*>(data_object);

	// Linear Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Linear:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Linear Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().linear));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().linear);
	box_offset++;

	// Quadratic Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Quadratic:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Quadratic Box
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().quadratic));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().quadratic);
	box_offset++;

	// Direction Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 14.0f);
	temp_text_data.text = "Direction";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Direction X Text
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 20.0f);
	temp_text_data.text = "X:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Direction X Box
	temp_box_data.width = 20.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-35.0f * scale, height_offset - 19.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().direction.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().direction.x);
	box_offset++;

	// Direction Y Text
	temp_text_data.position.x = -23.0f * scale;
	temp_text_data.text = "Y:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Direction Y Box
	temp_box_data.position.x = -8.0f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().direction.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().direction.y);
	box_offset++;

	// Direction Z Text
	temp_text_data.position.x = 4.0f * scale;
	temp_text_data.text = "Z:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Direction Z Box
	temp_box_data.position.x = 19.0f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().direction.z));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().direction.z);
	box_offset++;
	
	// Angle1 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 28.0f);
	temp_text_data.text = "Angle1:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Angle1 Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 27.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().angle1));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().angle1);
	box_offset++;

	// Angle2 Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 35.0f);
	temp_text_data.text = "Angle2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Angle2 Box
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 34.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_spot.getSpotData().angle2));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spot.getSpotData().angle2);
	box_offset++;
}

void Editor::EditorWindow::genBoxesBeam(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Directional Light
	DataClass::Data_Beam& data_beam = *static_cast<DataClass::Data_Beam*>(data_object);

	// X-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "xPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// X-Pos Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_beam.getBeamData().position2.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_beam.getBeamData().position2.x);
	box_offset++;

	// Y-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "yPos2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Y-Pos Box
	temp_box_data.position = glm::vec2(-16.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_beam.getBeamData().position2.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_beam.getBeamData().position2.y);
	box_offset++;

	// Linear Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 15.0f);
	temp_text_data.text = "Linear:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Linear Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 14.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_beam.getBeamData().linear));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_beam.getBeamData().linear);
	box_offset++;

	// Quadratic Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 22.0f);
	temp_text_data.text = "Quadratic:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Quadratic Box
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 21.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_beam.getBeamData().quadratic));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_beam.getBeamData().quadratic);
	box_offset++;
}

void Editor::EditorWindow::genBoxesRigidBody(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Rigid Body
	DataClass::Data_RigidBody& data_rigid_body = *static_cast<DataClass::Data_RigidBody*>(data_object);

	// Mass Box
	temp_box_data.width = 25.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-26.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_rigid_body.getRigidData().mass));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().mass);
	box_offset++;

	// Mass Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Mass:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Health Box
	temp_box_data.position = glm::vec2(16.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_rigid_body.getRigidData().max_health));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().max_health);
	box_offset++;

	// Health Text
	temp_text_data.position = glm::vec2(-10.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Health:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Center of Gravity Text
	temp_text_data.position = glm::vec2(-49.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Center of Gravity";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Center of Gravity X Box
	temp_box_data.position = glm::vec2(-26.0f * scale, height_offset - 13.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_rigid_body.getRigidData().center_of_gravity.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().center_of_gravity.x);
	box_offset++;

	// Center of Gravity X Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 14.0f);
	temp_text_data.text = " X:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Center of Gravity Y Box
	temp_box_data.position = glm::vec2(16.0f * scale, height_offset - 13.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_rigid_body.getRigidData().center_of_gravity.y));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().center_of_gravity.y);
	box_offset++;

	// Center of Gravity Y Text
	temp_text_data.position = glm::vec2(-10.0f * scale, height_offset - 14.0f);
	temp_text_data.text = " Y:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Offset Box
	temp_box_data.position = glm::vec2(-26.0f * scale, height_offset - 20.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_rigid_body.getRigidData().initial_rotation));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().initial_rotation);
	box_offset++;

	// Offset Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 21.0f);
	temp_text_data.text = "Offset:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Fluid Box
	temp_box_data.width = 5.0f * scale;
	temp_box_data.position = glm::vec2(5.0f * scale, height_offset - 20.0f);
	temp_box_data.button_text = GUI::AdvancedString("");
	temp_box_data.mode = GUI::TOGGLE_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_rigid_body.getRigidData().fluid);
	box_offset++;

	// Fluid Text
	temp_text_data.position = glm::vec2(-10.0f * scale, height_offset - 21.0f);
	temp_text_data.text = "Fluid:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Physics Material Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 28.0f);
	temp_text_data.text = "Physics Material";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Physics Material Box
	temp_box_data.width = 60.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, height_offset - 33.0f);
	temp_box_data.button_text = texture_path;
	temp_box_data.mode = GUI::ALPHABETICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&physics_material_name);
	box_offset++;
}

void Editor::EditorWindow::genBoxesSpringMass(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Spring Mass
	DataClass::Data_SpringMass& data_spring_mass = *static_cast<DataClass::Data_SpringMass*>(data_object);

	// Generate File Path Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "File:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Generate File Path Box
	temp_box_data.width = 70.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset);
	temp_box_data.button_text = data_spring_mass.getFile();
	temp_box_data.mode = GUI::GENERAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_spring_mass.getFile());
	update_script = box_offset;
	box_offset++;

	// Generate Generate New Component Box
	temp_box_data.width = 50.0f;
	temp_box_data.position = glm::vec2(0.0f, height_offset - 7.0f);
	temp_box_data.button_text = GUI::AdvancedString("Generate Object");
	temp_box_data.mode = GUI::FUNCTION_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setFunctionPointer([this]()->void {this->initializeSpringMassSelection(); });
	update_script = box_offset;
	box_offset++;
}

void Editor::EditorWindow::genBoxesSpringMassNode(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Get Node Data from Data Object
	Object::Physics::Soft::NodeData& node_data = static_cast<DataClass::Data_SpringMassNode*>(data_object)->getNodeData();

	// Position Offset is in Position Boxes in Common Vertices
	
	// Node Name is in the Name Location in Common Vertices

	// X-Pos Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-15.0f * scale, windowTop - 32.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(node_data.position.x));
	temp_box_data.mode = GUI::NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&node_data.position.x);
	box_offset++;

	// X-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 33.0f);
	temp_text_data.text = "OffsetX:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Y-Pos Box
	temp_box_data.position = glm::vec2(-15.0f * scale, windowTop - 39.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(node_data.position.y));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&node_data.position.y);
	box_offset++;

	// Y-Pos Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 40.0f);
	temp_text_data.text = "OffsetY:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Name Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 47.0f);
	temp_text_data.text = "Name:         " + std::to_string(node_data.name);
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// File Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 54.0f);
	temp_text_data.text = "File:         " + static_cast<DataClass::Data_SpringMass*>(data_object)->getFile();
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Radius Box
	temp_box_data.position.y = windowTop - 61.0f;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(node_data.radius));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&node_data.radius);
	box_offset++;

	// Radius Text
	temp_text_data.position.y = windowTop - 62.0f;
	temp_text_data.text = "Radius: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Mass Box
	temp_box_data.position.y = windowTop - 68.0f;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(node_data.mass));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&node_data.mass);
	box_offset++;

	// Mass Text
	temp_text_data.position.y = windowTop - 69.0f;
	temp_text_data.text = "Mass: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Health Box
	temp_box_data.position.y = windowTop - 75.0f;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(node_data.health));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&node_data.health);
	box_offset++;

	// Health Text
	temp_text_data.position.y = windowTop - 76.0f;
	temp_text_data.text = "Health: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesSpringMassSpring(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Get Spring Data from Data Object
	Object::Physics::Soft::Spring& spring_data = static_cast<DataClass::Data_SpringMassSpring*>(data_object)->getSpringData();

	// Node Indicies are in Location of Position Boxes in Common Vertices

	// Node1 Name Box
	temp_box_data.width = 45.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-16.0f * scale, windowTop - 32.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.Node1));
	temp_box_data.mode = GUI::ABSOLUTE_INTEGER_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.Node1);
	box_offset++;

	// Node1 Name Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 33.0f);
	temp_text_data.text = "Node1:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Node2 Name Box
	temp_box_data.position = glm::vec2(-16.0f * scale, windowTop - 39.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.Node2));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.Node2);
	box_offset++;

	// Node2 Name Text
	temp_text_data.position = glm::vec2(-52.0f * scale, windowTop - 40.0f);
	temp_text_data.text = "Node2:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Stiffness Box
	temp_box_data.position = glm::vec2(-10.0f * scale, windowTop - 46.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.Stiffness));
	temp_box_data.width = 40.0f;
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.Stiffness);
	box_offset++;

	// Stiffness Text
	temp_text_data.position.y = windowTop - 47.0f;
	temp_text_data.text = "Stiffness: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Dampening Box
	temp_box_data.position.y = windowTop - 53.0f;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.Dampening));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.Dampening);
	box_offset++;

	// Dampening Text
	temp_text_data.position.y = windowTop - 54.0f;
	temp_text_data.text = "Dampening: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Rest Length Box
	temp_box_data.position = glm::vec2(-10.0f * scale, windowTop - 61.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.RestLength));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.RestLength);
	box_offset++;

	// Rest Length Text
	temp_text_data.position.y = windowTop - 62.0f;
	temp_text_data.text = "Rest Length: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Max Length Box
	temp_box_data.position.y = windowTop - 68.0f;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(spring_data.MaxLength));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&spring_data.MaxLength);
	box_offset++;

	// Max Length Text
	temp_text_data.position.y = windowTop - 69.0f;
	temp_text_data.text = "Max Length: ";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::genBoxesWire(uint8_t& box_offset, uint8_t& text_offset, float height_offset)
{
}

void Editor::EditorWindow::genBoxesHinge(uint8_t& box_offset, uint8_t& text_offset, float height_offset)
{
}

void Editor::EditorWindow::genBoxesEntity(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// Convert Data to Entity
	DataClass::Data_Entity& data_entity = *static_cast<DataClass::Data_Entity*>(data_object);

	// Half Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 1.0f);
	temp_text_data.text = "Half Visual Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Half Width Box
	temp_box_data.width = 35.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_entity.getEntityData().half_width));
	temp_box_data.mode = GUI::ABSOLUTE_NUMERICAL_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_entity.getEntityData().half_width);
	box_offset++;

	// Half Height Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 8.0f);
	temp_text_data.text = "Half Visual Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Half Height Box
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset - 7.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_entity.getEntityData().half_height));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_entity.getEntityData().half_height);
	box_offset++;

	// Half Collision Width Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 15.0f);
	temp_text_data.text = "Half Collision Width:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Half Collision Width Box
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset - 14.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_entity.getEntityData().half_collision_width));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_entity.getEntityData().half_collision_width);
	box_offset++;

	// Half Collision Height Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 22.0f);
	temp_text_data.text = "Half Collision Height:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Half Collision Height Box
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset - 21.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(data_entity.getEntityData().half_collision_height));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&data_entity.getEntityData().half_collision_height);
	box_offset++;
}

void Editor::EditorWindow::genBoxesAI(uint8_t& box_offset, uint8_t& text_offset, float height_offset)
{
}

void Editor::EditorWindow::genBoxesGroup(uint8_t& box_offset, uint8_t& text_offset, float height_offset, DataClass::Data_Object* data_object)
{
	// File Box
	temp_box_data.width = 70.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-2.0f * scale, height_offset - 2);
	temp_box_data.button_text = static_cast<DataClass::Data_GroupObject*>(data_object)->getFilePath();
	temp_box_data.mode = GUI::FILE_PATH_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(&static_cast<DataClass::Data_GroupObject*>(data_object)->getFilePath());
	box_offset++;

	// File Text
	temp_text_data.position = glm::vec2(-52.0f * scale, height_offset - 3.0f);
	temp_text_data.text = " File:";
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;
}

void Editor::EditorWindow::assignColorWheel(ColorWheel* wheel_, uint8_t& box_offset, uint8_t& text_offset, unsigned int* color, float height_offset)
{
	// Move Color Wheel
	glm::vec4 temp_color = glm::vec4(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, color[3] / 255.0f);
	*wheel_ = ColorWheel(20.0f * scale, height_offset, -1.2f, 20.0f * scale, -20.0f * scale, -30.0f * scale, -60 * scale, scale, temp_color);
	wheel_->FindColors(temp_color);

	// Red Box
	temp_box_data.width = 15.0f * scale;
	temp_box_data.height = 5.0f;
	temp_box_data.position = glm::vec2(-37.5f * scale, height_offset - 15.0f);
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(color[0]));
	temp_box_data.mode = GUI::ABSOLUTE_INTEGER_TEXT_BOX;
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(color);
	box_offset++;

	// Red Text
	temp_text_data.text = "R:";
	temp_text_data.position = glm::vec2(-50.0f * scale, height_offset - 16.0f);
	temp_text_data.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Green Box
	temp_box_data.position.x = -15.5f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(color[1]));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(color + 1);
	box_offset++;

	// Green Text
	temp_text_data.text = "G:";
	temp_text_data.position.x = -28.0f * scale;
	temp_text_data.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Blue Box
	temp_box_data.position.x = 6.5f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(color[2]));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(color + 2);
	box_offset++;

	// Blue Text
	temp_text_data.text = "B:";
	temp_text_data.position.x = -6.0f * scale;
	temp_text_data.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Alpha Box
	temp_box_data.position.x = 28.5f * scale;
	temp_box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(color[3]));
	boxes[box_offset] = new GUI::Box(temp_box_data);
	boxes[box_offset]->setDataPointer(color + 3);
	box_offset++;

	// Alpha Text
	temp_text_data.text = "A:";
	temp_text_data.position.x = 16.0f * scale;
	temp_text_data.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	texts[text_offset] = new GUI::TextObject(temp_text_data);
	text_offset++;

	// Reset Text Color
	temp_text_data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Editor::EditorWindow::displayText()
{
	// Bind Font Shader
	Global::fontShader.Use();

	// Calculate the Change in Height Between Labels
	float change_in_height = 15.0f * scale;
	float initial_height = 20.0f + bar1.BarOffset;

	// Parse Index in Object Identifier
	switch (object_identifier_index)
	{

	// Primary Object Types
	case 0:
	{
		// Draw Mask Label
		Source::Fonts::renderText("Collision Masks", -48.0f, initial_height, 0.13f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Terrain Label
		Source::Fonts::renderText("Terrain", -48.0f, initial_height, 0.13f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Light Lable
		Source::Fonts::renderText("Lights", -48.0f, initial_height, 0.13f, glm::vec4(0.9f, 0.9f, 0.0f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Physics Objects Label
		Source::Fonts::renderText("Physics Objects", -48.0f, initial_height, 0.13f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Entity Lable
		Source::Fonts::renderText("Entities", -48.0f, initial_height, 0.13f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Effect Label
		Source::Fonts::renderText("Effects", -48.0f, initial_height, 0.13f, glm::vec4(0.0f, 0.8f, 0.8f, 1.0f), true);
		initial_height -= change_in_height;

		// Draw Group Label
		Source::Fonts::renderText("Effects", -48.0f, initial_height, 0.13f, glm::vec4(0.0f, 0.8f, 0.6f, 1.0f), true);

		break;
	}

	// Secondary Object Types
	case 1:
	{
		// Parse Primary Object Types
		switch (new_object_identifier[0])
		{
			
		// Collision Masks
		case Object::MASK:
		{
			// Draw Floor Masks
			Source::Fonts::renderText("Floor Masks", -48.0f, initial_height, 0.13f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Left Wall Masks
			Source::Fonts::renderText("Left Wall Masks", -48.0f, initial_height, 0.13f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Right Wall Masks
			Source::Fonts::renderText("Right wall Masks", -48.0f, initial_height, 0.13f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Ceiling Masks
			Source::Fonts::renderText("Ceiling Masks", -48.0f, initial_height, 0.13f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Trigger Masks
			Source::Fonts::renderText("Trigger Masks", -48.0f, initial_height, 0.13f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);

			break;
		}

		// Terrain Layers
		case Object::TERRAIN:
		{
			// Draw Foreground Terrain Label
			Source::Fonts::renderText("Foreground", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[0], true);
			Source::Fonts::renderText(" Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[0], true);
			initial_height -= change_in_height;

			// Draw Formerground Terrain Label
			Source::Fonts::renderText("Formerground", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[1], true);
			Source::Fonts::renderText("  Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[1], true);
			initial_height -= change_in_height;

			// Draw Background 1 Terrain Label
			Source::Fonts::renderText("Background 1", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[2], true);
			Source::Fonts::renderText("  Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[2], true);
			initial_height -= change_in_height;

			// Draw Background 2 Terrain Label
			Source::Fonts::renderText("Background 2", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[3], true);
			Source::Fonts::renderText("  Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[3], true);
			initial_height -= change_in_height;

			// Draw Background 3 Terrain Label
			Source::Fonts::renderText("Background 3", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[4], true);
			Source::Fonts::renderText("  Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[4], true);
			initial_height -= change_in_height;

			// Draw Backdrop Terrain Label
			Source::Fonts::renderText("Backdrop", -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[5], true);
			Source::Fonts::renderText(" Terrain", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[5], true);
			initial_height -= change_in_height;

			break;
		}

		// Lights
		case Object::LIGHT:
		{
			// Draw Directional Light Label
			Source::Fonts::renderText("Directional", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("  Light", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Point Light Label
			Source::Fonts::renderText("Point", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("Light", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Spot Light Label
			Source::Fonts::renderText("Spot", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("Light", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Beam Light Label
			Source::Fonts::renderText("Beam", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("Light", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.0f, 1.0f), true);

			break;
		}

		// Physics Objects
		case Object::PHYSICS:
		{
			// Draw Rigid Body Label
			Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
			Source::Fonts::renderText("Physics Object", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Soft Body Label
			Source::Fonts::renderText("Soft Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
			Source::Fonts::renderText("Physics Object", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Hinge Label
			Source::Fonts::renderText("Hinge", -48.0f, initial_height, 0.13f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);

			break;
		}

		// Entities
		case Object::ENTITY:
		{
			// Draw Directional Light Label
			Source::Fonts::renderText("NPC", -48.0f, initial_height, 0.13f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Point Light Label
			Source::Fonts::renderText("Controllable", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("  Entity", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Spot Light Label
			Source::Fonts::renderText("Interactable", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			Source::Fonts::renderText("  Entity", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			initial_height -= change_in_height;

			// Draw Beam Light Label
			Source::Fonts::renderText("Dynamic", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);
			Source::Fonts::renderText(" Enity", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.5f, 0.0f, 0.0f, 1.0f), true);

			break;
		}

		// Effects
		case Object::EFFECT:
		{
			break;
		}
			
		}
		
		break;
	}

	// Unique Object Types
	case 2:
	{
		// Parse Primary Object Types
		switch (new_object_identifier[0])
		{

		// Collision Masks
		case Object::MASK:
		{
			// Parse Mask Type
			switch (new_object_identifier[1])
			{
			
			// Floor Masks
			case Object::Mask::FLOOR:
			{
				// Draw Horizontal Line Label
				Source::Fonts::renderText("Floor Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), true);
				Source::Fonts::renderText("Horizontal Line", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Slant Label
				Source::Fonts::renderText("Floor Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), true);
				Source::Fonts::renderText("  Slant", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Slope Label
				Source::Fonts::renderText("Floor Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.04f, 0.24f, 1.0f, 1.0f), true);
				Source::Fonts::renderText("  Slope", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.04f, 0.24f, 1.0f, 1.0f), true);

				break;
			}

			// Left Wall Masks
			case Object::Mask::LEFT_WALL:
			{
				// Draw Vertical Line Label
				Source::Fonts::renderText("Left Wall Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true);
				Source::Fonts::renderText("Vertical Line", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Curve Label
				Source::Fonts::renderText("Left Wall Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(1.0f, 0.4f, 0.0f, 1.0f), true);
				Source::Fonts::renderText("   Curve", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(1.0f, 0.4f, 0.0f, 1.0f), true);

				break;
			}

			// Right Wall Masks
			case Object::Mask::RIGHT_WALL:
			{
				// Draw Vertical Line Label
				Source::Fonts::renderText("Right Wall Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), true);
				Source::Fonts::renderText("Vertical Line", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Curve Label
				Source::Fonts::renderText("Right Wall Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.04f, 0.0f, 0.27f, 1.0f), true);
				Source::Fonts::renderText("   Curve", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.04f, 0.0f, 0.27f, 1.0f), true);

				break;
			}

			// Ceiling Masks
			case Object::Mask::CEILING:
			{
				// Draw Horizontal Line Label
				Source::Fonts::renderText("Ceiling Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
				Source::Fonts::renderText("Horizontal Line", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Slant Label
				Source::Fonts::renderText("Ceiling Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.28f, 0.0f, 0.34f, 1.0f), true);
				Source::Fonts::renderText("  Slant", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.28f, 0.0f, 0.34f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Slope Label
				Source::Fonts::renderText("Ceiling Mask", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.0f, 0.0f, 0.45f, 1.0f), true);
				Source::Fonts::renderText("  Slope", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.0f, 0.0f, 0.45f, 1.0f), true);

				break;
			}
			}

			break;
		}

		// Terrain Layers
		case Object::TERRAIN:
		{
			// Map for Terrain Layer Labels
			const std::string layer_map[6] = { "Foreground", "Formerground", "Background 1", "Background 2", "Background 3", "Backdrop" };

			// Draw Rectangle Label
			Source::Fonts::renderText(layer_map[new_object_identifier[1]], -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			Source::Fonts::renderText("Rectangle", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			initial_height -= change_in_height;

			// Draw Trapezoid Label
			Source::Fonts::renderText(layer_map[new_object_identifier[1]], -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			Source::Fonts::renderText("Trapezoid", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			initial_height -= change_in_height;

			// Draw Triangle Label
			Source::Fonts::renderText(layer_map[new_object_identifier[1]], -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			Source::Fonts::renderText("Triangle", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			initial_height -= change_in_height;

			// Draw Circle Label
			Source::Fonts::renderText(layer_map[new_object_identifier[1]], -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			Source::Fonts::renderText("Circle", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			initial_height -= change_in_height;

			// Draw Polygon Label
			Source::Fonts::renderText(layer_map[new_object_identifier[1]], -48.0f, initial_height + 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);
			Source::Fonts::renderText("Polygon", -48.0f, initial_height - 2.0f, 0.12f, terrain_layer_colors[new_object_identifier[1]], true);

			break;
		}

		// Physics Objects
		case Object::PHYSICS:
		{
			// Parse Physics Type
			switch (new_object_identifier[1])
			{

			// Rigid Body
			case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
			{
				// Draw Rectangle Label
				Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Rectangle", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Trapezoid Label
				Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Trapezoid", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Triangle Label
				Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Triangle", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Circle Label
				Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Circle", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Polygon Label
				Source::Fonts::renderText("Rigid Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Polygon", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);

				break;
			}

			// Soft Body
			case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
			{
				// Draw Spring Mass Label
				Source::Fonts::renderText("Soft Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("Spring Mass", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Wire Label
				Source::Fonts::renderText("Soft Body", -48.0f, initial_height + 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				Source::Fonts::renderText("  Wire", -48.0f, initial_height - 2.0f, 0.12f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);

				break;
			}

			// Hinge
			case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
			{
				// Draw Anchor Label
				Source::Fonts::renderText("Anchor", -48.0f, initial_height, 0.13f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);
				initial_height -= change_in_height;

				// Draw Hinge Label
				Source::Fonts::renderText("Hinge", -48.0f, initial_height, 0.13f, glm::vec4(0.8f, 0.8f, 0.4f, 1.0f), true);

				break;
			}

			}

			break;
		}

		// Effects
		case Object::EFFECT:
		{
			break;
		}

		}

		break;
	}

	}
}

void Editor::EditorWindow::updateScrollBars()
{
	// Location of Mouse in Window
	double mouseStaticX, mouseStaticY;
	mouseStaticX = Global::mouseX / Global::zoom_scale;
	mouseStaticY = Global::mouseY / Global::zoom_scale;

	// Test if Primary ScrollBar Should Scroll
	if (bar1.TestColloisions())
	{
		collision_type = 4;
		index = 0;
		Global::Selected_Cursor = Global::CURSORS::HAND;
	}

	// Test if Secondary ScrollBar Should Scroll
	//else if (false && bar2.TestColloisions())
	//{
	//	collision_type = 5;
	//	index = 0;
	//	Global::Selected_Cursor = Global::CURSORS::HAND;
	//}

	// Determine if Left Mouse Button is Also Being Held
	if (Global::LeftClick && !moving && collision_type > 1)
	{
		// Set Moving to True
		moving = (uint8_t)collision_type;

		// Set Mouse Offset
		offset_x = window_position.x - (float)mouseStaticX;
		offset_y = window_position.y - (float)mouseStaticY;

		// Change Offset For Scroll Bars
		if (collision_type > 3)
		{
			// Primary Editor Window Scroll Bar
			if (collision_type == 4)
			{
				offset_y = bar1.BarPosY - (float)mouseStaticY;
			}

			// Secondary Editor Window Scroll Bar
			else
			{
				offset_y = bar2.BarPosY - (float)mouseStaticY;
			}
		}
	}

	// If Moving, Move Objects
	if (moving)
	{
		// If No Longer Left Clicking, Stop Moving
		if (!Global::LeftClick)
			moving = false;

		// Move Object if it is Still Moving
		else
		{
			// Reset Models
			model = glm::mat4(1.0f);
			editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, bar1.BarOffset, 0.0f));
			//editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (editorHeightFull - editorHeight) * bar1.percent, 0.0f));
			//texture_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, (textureHeightFull - editorHeight) * bar2.percent, 0.0f));;

			switch (moving)
			{

				// Change Coordinates of Object
			case 1:
			{
				// Move Coordinates of Object
				window_position.x = (float)(mouseStaticX + offset_x);
				window_position.y = (float)(mouseStaticY + offset_y);

				break;
			}

			// Move Top of Window
			case 2:
			{
				// Calculate New yPos by Taking the Average of the y Position of the Unchanging (opposite) Side and the y Position of the Mouse
				window_position.y = ((float)mouseStaticY + (window_position.y - (height / 2))) / 2;

				// Calculate New Height by Multiplying the Distance Between the Mouse and yPos by 2
				height = 2 * (float)abs(mouseStaticY - window_position.y);

				// Prevent Height From Going Negative
				if (height <= 0)
				{
					height = 1;
				}

				//UpdateHeight();

				break;
			}

			// Move Bottom of Window
			case -2:
			{
				// Calculate New yPos by Taking the Average of the Position of the Unchanging (opposite) Side and the y Position of the Mouse
				window_position.y = ((float)mouseStaticY + (window_position.y + (height / 2))) / 2;

				// Calculate New Height by Multiplying the Distance Between the Mouse and yPos by 2
				height = 2 * (float)abs(mouseStaticY - window_position.y);

				// Prevent Height From Going Negtative
				if (height <= 0)
				{
					height = 1;
				}

				//UpdateHeight();

				break;
			}

			// Move Left of Window
			case -3:
			{
				// Calculate New xPos by Taking the Average of the Position of the Unchanging (opposite) Side and the x Position of the Mouse
				window_position.x = ((float)mouseStaticX + (window_position.x + (width / 2))) / 2;

				// Calculate New Width by Multiplying the Distance Between the Mouse and xPos by 2
				width = 2 * (float)abs(mouseStaticX - window_position.x);

				// Prevent Width From Going Negative
				if (width <= 0)
				{
					width = 1;
				}

				// Calculate Updated Scale Factor
				scale = width / 170;

				//UpdateWidth();

				break;
			}

			// Move Right of Window
			case 3:
			{
				// Calculate New xPos by Taking the Average of the Position of the Unchanging (oppposite) Size and the x Position of the Mouse
				window_position.x = ((float)mouseStaticX + (window_position.x - (width / 2))) / 2;

				// Calculate New Width by Multiplying the Distance Between the Mouse and xPos by 2
				width = 2 * (float)abs(mouseStaticX - window_position.x);

				// Prevent Width From Going Negative
				if (width <= 0)
				{
					width = 1;
				}

				// Calculate Updated Scale Factor
				scale = width / 170;

				//UpdateWidth();

				break;
			}

			// Move Primary ScrollBar
			case 4:
			{
				float test = bar1.Scroll((float)(mouseStaticY + offset_y));
				break;
			}

			// Move Secondary Scrollbar
			case 5:
			{
				float test = bar2.Scroll((float)(mouseStaticY + offset_y));
				break;
			}

			}

			// Update Model Matrix
			model = glm::translate(model, glm::vec3(window_position.x, window_position.y, 0.0f));
			editing_model = glm::translate(editing_model, glm::vec3(window_position.x, window_position.y + (height / 2 - editingOffset), 0.0f));

			// Update Scroll Bars
			bar1.moveElement(bar1X + window_position.x, window_position.y + height / 2 - 10 * scale);
			bar2.moveElement(bar2X + window_position.x, window_position.y + height / 2 - 10 * scale);
		}
	}

	else
	{
		// Reset Collision Type
		collision_type = 0;
	}

	editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, bar1.BarOffset, 0.0f));
}

void Editor::EditorWindow::updateNewObject()
{
	// Location of Mouse in Window
	double mouseStaticX, mouseStaticY;
	mouseStaticX = Global::mouseX / Global::zoom_scale;
	mouseStaticY = Global::mouseY / Global::zoom_scale;

	// The Offset Calculated by Scroll Bar
	float barOffset = (editorHeightFull - editorHeight) * bar1.percent;

	index = 0;
	if (mouseStaticY <= (double)window_position.y + ((double)height * 0.5) - 10.0 * (double)scale && mouseStaticY >= (double)window_position.y - ((double)height * 0.5) + 2.0 * (double)scale)
	{
		// Test if Mouse X is Inside Editor Window
		if (mouseStaticX >= (double)border1X + window_position.x + (double)scale && mouseStaticX <= (double)border2X + window_position.x - (double)scale)
		{
			Global::Selected_Cursor = Global::CURSORS::HAND;
			collision_type = 1;

			// Calculate the Index of Which Object the Mouse is Selecting
			index = (int)(((((double)window_position.y + height / 2 - 12 * (double)scale) - mouseStaticY) / (15 * (double)scale)) + (barOffset / (15 * scale)) + 1);

			// Generate Vertices of Highlighter
			float vertices[42];
			Vertices::Rectangle::genRectColor((border1X + border2X) / 2 + window_position.x, (window_position.y + (height / 2) - 5 * scale) - index * 15 * scale + barOffset, -1.3f, border2X - border1X, 15.0f * scale, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), vertices);

			// Bind Objects
			glBindVertexArray(highlighterVAO);
			glBindBuffer(GL_ARRAY_BUFFER, highlighterVBO);

			// Bind Vertices
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			// Unbind Objects
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			// If Left Click, Create New Object
			if (Global::LeftClick)
			{
				// Reset Some Values
				Global::LeftClick = false;
				bar1.percent = 0.0f;

				// Update Generalized New Object
				if (editing_mode == EDITING_MODES::NEW_OBJECT)
				{
					new_object_identifier[object_identifier_index] = index - 1;
					object_identifier_index++;
					changeNewObject();
				}

				// Update SpringMass New Object
				else if (editing_mode == EDITING_MODES::NEW_SPRINGMASS)
				{
					secondary_object_identifier[secondary_identifier_index] = index - 1;
					secondary_identifier_index++;
					changeNewObjectSpringMass();
				}

				// Update Hinge New Object
				else if (editing_mode == EDITING_MODES::NEW_HINGE)
				{
					secondary_object_identifier[secondary_identifier_index] = index - 1;
					secondary_identifier_index++;
					changeNewObjectHinge();
				}
			}
		}
	}
}

void Editor::EditorWindow::changeNewObject()
{
	// Keeps Track of the Offset of the Vertices
	int offset = 0;

	// Keeps Track of the Distance Between Objects
	float distance = height / 2 - 20 * scale;
	editingOffset = height / 2;

	// Parse Object Identifier Index
	switch (object_identifier_index)
	{

	// Base Object Type Object Identifier
	case 0:
	{
		// Vertices of Objects
		float vertices[42];

		// Bind Array Object
		glBindVertexArray(editing_screenVAO);

		// Enable VBO and Bind Nullified Vertices
		glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 294, NULL, GL_DYNAMIC_DRAW);

		// Collision Mask Vertices
		Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance, distance, -1.1f, 1 * scale, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Terrain Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Light Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Physics Object Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Entity Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(0.75f, 0.1f, 0.1f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Effect Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Group Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 10 * scale, 10 * scale, glm::vec4(0.0f, 0.8f, 0.6f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
		offset += 42;

		// Finalize Vertices
		finalizeNewObjectVertices(distance);

		break;
	}

	// Secondary Object Type Object Identifier
	case 1:
	{
		// Parse Initial Object Identifier Type
		switch (new_object_identifier[0])
		{

		// Mask Selections
		case Object::MASK:
		{
			// Vertices of Objects
			float vertices[42];

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 168, NULL, GL_DYNAMIC_DRAW);

			// Collision Mask Floor Vertices
			Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance - 2.0f * scale, distance - 2.0f * scale, -1.1f, 1 * scale, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Collision Mask left Wall Vertices
			Vertices::Line::genLineColor(0.0f, 0.0f, distance + 6.0f * scale, distance - 6 * scale, -1.1f, 1 * scale, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Collision Mask Right Wall Vertices
			Vertices::Line::genLineColor(20.0f * scale, 20.0f * scale, distance + 6.0f * scale, distance - 6 * scale, -1.1f, 1 * scale, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Collision Mask Ceiling Vertices
			Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance + 2.0f * scale, distance + 2.0f * scale, -1.1f, 1 * scale, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Terrain Layer Selections
		case Object::TERRAIN:
		{
			// Vertices of Objects
			float vertices[42];

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 252, NULL, GL_DYNAMIC_DRAW);

			// Draw Each Rectangle With Differing Colors
			for (int i = 0; i < 6; i++)
			{
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, terrain_layer_colors[i], vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
				offset += 42;
				distance -= 15 * scale;
			}

			// Counteract Previous Distance Change
			distance += 15 * scale;

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Light Selections
		case Object::LIGHT:
		{
			// Vertices of Objects
			float vertices[42];

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 168, NULL, GL_DYNAMIC_DRAW);

			// Directional Light Vertices
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 1.0f * scale, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Point Light Vertices
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Spot Light Vertices
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 4.0f * scale, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Beam Vertices
			Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance + 5.0f * scale, distance - 2 * scale, -1.1f, 1 * scale, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Physics Type Selections
		case Object::PHYSICS:
		{
			// Vertices of Objects
			float vertices[42];

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 168, NULL, GL_DYNAMIC_DRAW);

			// Rigid Body
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Soft Body
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 5.0f, 10.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Hinge
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 5.0f, 1.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * offset, sizeof(vertices), vertices);
			offset += 42;
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 1.0f, 5.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * offset, sizeof(vertices), vertices);
			offset += 42;

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Entity Base Selections
		case Object::ENTITY:
		{
			// Vertices of Objects
			float vertices[42];

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 168, NULL, GL_DYNAMIC_DRAW);

			// NPC
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(0.75f, 0.1f, 0.1f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Controllable
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(0.75f, 0.1f, 0.1f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Interactable
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(0.75f, 0.1f, 0.1f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			offset += 42;
			distance -= 15 * scale;

			// Dynamic
			Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f * scale, 3.0f * scale, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)* offset, sizeof(vertices), vertices);
			distance -= 15 * scale;

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Effect Selections
		case Object::EFFECT:
		{
			// Vertices of Objects
			float vertices[42] = {0};

			// Bind Array Object
			glBindVertexArray(editing_screenVAO);

			// Enable VBO and Bind Nullified Vertices
			glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 252, NULL, GL_DYNAMIC_DRAW);

			// Finalize Vertices
			finalizeNewObjectVertices(distance);

			break;
		}

		// Group Selections
		case Object::GROUP:
		{
			generateNewObject();
			break;
		}

		}

		break;
	}

	// Trinary Object Type Object Identifier
	case 2:
	{
		// Parse Initial Object Type
		switch (new_object_identifier[0])
		{

		// Mask Selections
		case Object::MASK:
		{
			switch (new_object_identifier[1])
			{
	
			// Floor Mask Selections
			case Object::Mask::FLOOR:
			{
				glm::vec4 floor_mask_colors[3] = { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.04f, 0.24f, 1.0f, 1.0f) };
				genNewObjectHorizontalMasks(floor_mask_colors, distance, offset);
				break;
			}

			// Left Mask Selections
			case Object::Mask::LEFT_WALL:
			{
				glm::vec4 left_wall_mask_colors[2] = { glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.4f, 0.0f, 1.0f) };
				genNewObjectVerticalMasks(left_wall_mask_colors, distance, offset, 1);
				break;
			}

			// Right Mask Selections
			case Object::Mask::RIGHT_WALL:
			{
				glm::vec4 right_wall_mask_colors[2] = { glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.04f, 0.0f, 0.27f, 1.0f) };
				genNewObjectVerticalMasks(right_wall_mask_colors, distance, offset, -1);
				break;
			}

			// Ceiling Mask Selections
			case Object::Mask::CEILING:
			{
				glm::vec4 ceiling_mask_colors[3] = { glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.28f, 0.0f, 0.34f, 1.0f), glm::vec4(0.0f, 0.0f, 0.45f, 1.0f) };
				genNewObjectHorizontalMasks(ceiling_mask_colors, distance, offset);
				break;
			}

			// Generate Trigger Object
			case Object::Mask::TRIGGER:
			{
				generateNewObject();
				break;
			}

			}

			break;
		}

		// Terrain Selections
		case Object::TERRAIN:
		{
			genNewObjectShapes(terrain_layer_colors[new_object_identifier[1]], distance, offset);
			break;
		}

		// Generate Light
		case Object::LIGHT:
		{
			generateNewObject();
			break;
		}

		// Physics Selections
		case Object::PHYSICS:
		{
			switch (new_object_identifier[1])
			{

			// Rigid Bodies
			case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
			{
				genNewObjectShapes(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), distance, offset);
				break;
			}

			// Soft Bodies
			case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
			{
				// Vertices of Objects
				float vertices[42];

				// Bind Array Object
				glBindVertexArray(editing_screenVAO);

				// Enable VBO and Bind Nullified Vertices
				glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 84, NULL, GL_DYNAMIC_DRAW);

				// Soft Body Vertices
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 5.0f, 10.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
				offset += 42;
				distance -= 15 * scale;

				// Wire Vertices
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f, 1.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
				offset += 42;

				// Finalize Vertices
				finalizeNewObjectVertices(distance);

				break;
			}

			// Hinge
			case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
			{
				// Vertices of Objects
				float vertices[42];

				// Bind Array Object
				glBindVertexArray(editing_screenVAO);

				// Enable VBO and Bind Nullified Vertices
				glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 126, NULL, GL_DYNAMIC_DRAW);

				// Anchor Vertices
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 2.0f, 2.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
				offset += 42;
				distance -= 15 * scale;

				// Hinge Vertieces
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 5.0f, 1.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
				offset += 42;
				Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 1.0f, 5.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
				glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT)* offset, sizeof(vertices), vertices);
				offset += 42;
				
				// Finalize Vertices
				finalizeNewObjectVertices(distance);

				break;
			}

			}

			break;
		}

		// Generate Entity
		case Object::ENTITY:
		{
			generateNewObject();
			break;
		}

		// Effect Selections
		case Object::EFFECT:
		{
			break;
		}

		}

		break;
	}

	// Finalized Object Identifier
	case 3:
	{
		// All Objects That End Up Here Will be an Individual Object
		generateNewObject();

		break;
	}

	}

	// Store Pointer to ScrollBar
	Global::scroll_bar = &bar1;
}

void Editor::EditorWindow::changeNewObjectSpringMass()
{
	// Generate Vertices for Selection
	if (secondary_identifier_index == 0)
	{
		// Keeps Track of the Offset of the Vertices
		int offset = 0;

		// Keeps Track of the Distance Between Objects
		float distance = height / 2 - 20 * scale;
		editingOffset = height / 2;

		// Vertices of Objects
		float vertices[42];

		// Bind Array Object
		glBindVertexArray(editing_screenVAO);

		// Enable VBO and Bind Nullified Vertices
		glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 84, NULL, GL_DYNAMIC_DRAW);

		// Soft Body Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 5.0f, 10.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * offset, sizeof(vertices), vertices);
		offset += 42;
		distance -= 15 * scale;

		// Wire Vertices
		Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f, 1.0f, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * offset, sizeof(vertices), vertices);
		offset += 42;

		// Finalize Vertices
		finalizeNewObjectVertices(distance);
	}

	// Generate New Object
	else
	{
		// New Object is a Node
		if (index == 1)
			add_child_object = CHILD_OBJECT_TYPES::SPRINGMASS_NODE;

		// New Object is a Spring
		else if (index == 2)
			add_child_object = CHILD_OBJECT_TYPES::SPRINGMASS_SPRING;

		// Deactivate Window
		active_window = false;

		// Set Flag to Add a Child Object
		add_child = true;
	}

	// Store Pointer to ScrollBar
	Global::scroll_bar = &bar1;
}

void Editor::EditorWindow::changeNewObjectHinge()
{

}

void Editor::EditorWindow::initializeSpringMassSelection()
{
	// Reset Second Object Identifier Index
	secondary_identifier_index = 0;

	// Set Mode to New SpringMass Object
	editing_mode = EDITING_MODES::NEW_SPRINGMASS;

	// Change New Object
	changeNewObjectSpringMass();

	// Update Vertex Count
	EditorVertexCount = 525;
}

void Editor::EditorWindow::initializeHingeSelection()
{
	// Reset Second Object Identifier Index
	secondary_identifier_index = 0;

	// Set Mode to New Hinge Object
	editing_mode = EDITING_MODES::NEW_HINGE;

	// Change New Object
	changeNewObjectHinge();

	// Update Vertex Count
	EditorVertexCount = 525;
}

void Editor::EditorWindow::genNewObjectShapes(glm::vec4 color, float& distance, int& offset)
{
	// Vertices of Quad Objects
	float vertices[42];
	
	// Vertices for Triangle Object
	float vertices_half[21];

	// Vertices for Circle Object
	float vertices2[420];

	// Vertices for Polygon Object
	float vertices3[105];

	// Bind Array Object
	glBindVertexArray(editing_screenVAO);

	// Enable VBO and Bind Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 630, NULL, GL_DYNAMIC_DRAW);

	// Rectangle Vertices
	Vertices::Rectangle::genRectColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;
	distance -= 15 * scale;

	// Trapezoid Vertices
	Vertices::Trapezoid::genTrapColor(10.0f * scale, distance, -1.1f, 30.0f * scale, 10.0f * scale, 3.0f, 0.0f, color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;
	distance -= 15 * scale;

	// Triangle Vertices
	glm::vec2 origin = glm::vec2(10.0f * scale, distance + 5.0f * scale);
	Vertices::Triangle::genTriColor(origin, origin + glm::vec2(-10.0f * scale, -10.0f * scale), origin + glm::vec2(10.0f * scale, -10.0f * scale), - 1.1f, color, vertices_half);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices_half), vertices_half);
	offset += 21;
	distance -= 15 * scale;

	// Circle Vertices
	Vertices::Circle::genCircleColorFull(10.0f * scale, distance, -1.1f, 5.0f, 20, color, vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices2), vertices2);
	offset += 420;
	distance -= 15 * scale;

	// Polygon Vertices
	Vertices::Circle::genCircleColorFull(10.0f * scale, distance, -1.1f, 5.0f, 5, color, vertices3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices3), vertices3);
	offset += 105;

	// Finalize Vertices
	finalizeNewObjectVertices(distance);
}

void Editor::EditorWindow::genNewObjectHorizontalMasks(glm::vec4 colors[3], float& distance, int& offset)
{
	// Vertices of Objects
	float vertices[42];

	// Vertices of Slope
	float largerVertices[462];

	// Bind Array Object
	glBindVertexArray(editing_screenVAO);

	// Enable VBO and Bind Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 546, NULL, GL_DYNAMIC_DRAW);

	// Collision Mask Line Vertices
	Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance, distance, -1.1f, 1 * scale, colors[0], vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;
	distance -= 15 * scale;

	// Collision Mask Slant Vertices
	Vertices::Line::genLineColor(-10.0f * scale, 30.0f * scale, distance + 5.0f * scale, distance - 2 * scale, -1.1f, 1 * scale, colors[1], vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;
	distance -= 15 * scale;

	// Collision Mask Slope Vertices
	Vertices::Line::genLineDetailedCurve1(10.0f * scale, distance, -1.1f, 5.0f * scale, 20.0f * scale, scale, colors[2], 11, largerVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(largerVertices), largerVertices);
	offset += 462;

	// Finalize Vertices
	finalizeNewObjectVertices(distance);
}

void Editor::EditorWindow::genNewObjectVerticalMasks(glm::vec4 colors[2], float& distance, int& offset, int8_t direction)
{
	// Vertices of Objects
	float vertices[42];

	// Vertices of Curve
	float largerVertices[462];

	// Bind Array Object
	glBindVertexArray(editing_screenVAO);

	// Enable VBO and Bind Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, editing_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 504, NULL, GL_DYNAMIC_DRAW);

	// Collision Mask Wall Vertices
	Vertices::Line::genLineColor((10.0f - 10.0f * direction) * scale, (10.0f - 10.0f * direction) * scale, distance + 6.0f * scale, distance - 6 * scale, -1.1f, 1 * scale, colors[0], vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(vertices), vertices);
	offset += 42;
	distance -= 15 * scale;

	// Collision Mask Curve Vertices
	Vertices::Line::genLineDetailedCurve2((10.0f + 20.0f * direction) * scale, distance - 6.0f * scale, -1.1f, 40.0f * scale, 2.0f * scale, (1 + (6.0f / (40.0f * scale))), ((21 * 40.0f * scale - 25) / 30), -direction, scale, colors[1], 11, largerVertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat) * offset, sizeof(largerVertices), largerVertices);
	offset += 462;

	// Finalize Vertices
	finalizeNewObjectVertices(distance);
}

void Editor::EditorWindow::finalizeNewObjectVertices(float distance)
{
	// Unbind Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Array Object
	glBindVertexArray(0);

	// Calculate Height of Window
	editorHeight = height - 12 * scale;
	editorHeightFull = 20 * scale - distance + (height / 2 - 20 * scale);

	// Create ScrollBar
	bar1X = border2X - 2 * scale;
	bar1 = GUI::VerticalScrollBar(bar1X, position.y + height / 2 - 10 * scale, 2.0f * scale, editorHeight, editorHeightFull, 0);

	if (!active_window && false)
	{
		editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y + (height / 2 - editingOffset), 0.0f));
	}

	bar1.moveElement(bar1X + window_position.x, window_position.y + height / 2 - 10 * scale);

	editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
}

void Editor::EditorWindow::generateNewObject()
{
	// Force Initialization of Selector
	force_selector_initialization = true;

	// Disable Window
	active_window = false;

	// Reset Scroll Callback
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);

	// Generate New Object Index
	//object_index = Global::object_index_counter;
	//Global::object_index_counter++;

	// Determine New Position of Object
	//glm::vec2 new_position = glm::vec2(0.0f, 0.0f);
	glm::vec2 new_position = Global::camera_pointer->Position;

	// Determine New Size of Object
	float new_size = 10.0f;

	// New Data Object
	DataClass::Data_Object* new_data_object;

	// Parse Object Identifier
	switch (new_object_identifier[0])
	{

	// Masks
	case Object::MASK:
	{
		switch (new_object_identifier[1])
		{

		// Horizontal Masks
		case Object::Mask::FLOOR:
		case Object::Mask::CEILING:
		{
			switch (new_object_identifier[2])
			{

			// Horizontal Line
			case Object::Mask::HORIZONTAL_LINE:
			{
				DataClass::Data_HorizontalLine* new_horizontal_line = (new_object_identifier[1] == 0) ? static_cast<DataClass::Data_HorizontalLine*>(new DataClass::Data_FloorMaskHorizontalLine(0)) : static_cast<DataClass::Data_HorizontalLine*>(new DataClass::Data_CeilingMaskHorizontalLine(0));
				new_horizontal_line->generateInitialValues(new_position, new_size);
				new_data_object = new_horizontal_line;

				break;
			}

			// Slant
			case Object::Mask::HORIZONTAL_SLANT:
			{
				DataClass::Data_Slant* new_slant = (new_object_identifier[1] == 0) ? static_cast<DataClass::Data_Slant*>(new DataClass::Data_FloorMaskSlant(0)) : static_cast<DataClass::Data_Slant*>(new DataClass::Data_CeilingMaskSlant(0));
				new_slant->generateInitialValues(new_position, new_size);
				new_data_object = new_slant;

				break;
			}

			// Slope
			case Object::Mask::HORIZONTAL_SLOPE:
			{
				DataClass::Data_Slope* new_slope = (new_object_identifier[1] == 0) ? static_cast<DataClass::Data_Slope*>(new DataClass::Data_FloorMaskSlope(0)) : static_cast<DataClass::Data_Slope*>(new DataClass::Data_CeilingMaskSlope(0));
				new_slope->generateInitialValues(new_position, new_size);
				new_data_object = new_slope;

				break;
			}

			}

			break;
		}

		// Vertical Masks
		case Object::Mask::LEFT_WALL:
		case Object::Mask::RIGHT_WALL:
		{
			switch (new_object_identifier[2])
			{

			// Vertical Line
			case Object::Mask::VERTICAL_LINE:
			{
				DataClass::Data_VerticalLine* new_vertical_line = (new_object_identifier[1] == 1) ? static_cast<DataClass::Data_VerticalLine*>(new DataClass::Data_LeftMaskVerticalLine(0)) : static_cast<DataClass::Data_VerticalLine*>(new DataClass::Data_RightMaskVerticalLine(0));
				new_vertical_line->generateInitialValues(new_position, new_size);
				new_data_object = new_vertical_line;

				break;
			}

			// Curve
			case Object::Mask::VERTICAL_CURVE:
			{
				DataClass::Data_Curve* new_curve = (new_object_identifier[1] == 1) ? static_cast<DataClass::Data_Curve*>(new DataClass::Data_LeftMaskCurve(0)) : static_cast<DataClass::Data_Curve*>(new DataClass::Data_RightMaskCurve(0));
				new_curve->generateInitialValues(new_position, new_size);
				new_data_object = new_curve;

				break;
			}

			}

			break;
		}

		// Trigger Masks
		case Object::Mask::TRIGGER:
		{
			DataClass::Data_TriggerMask* new_trigger = new DataClass::Data_TriggerMask(0);
			new_trigger->generateInitialValues(new_position, new_size);
			new_data_object = new_trigger;

			break;
		}

		}

		break;
	}

	// Terrain
	case Object::TERRAIN:
	{
		// Determine Actual Terrain Layer
		const uint8_t layer_map[6] = { 4, 5, 3, 2, 1, 0 };
		new_object_identifier[1] = layer_map[new_object_identifier[1]];

		// Generate Object Data
		DataClass::Data_Terrain* new_terrain = new DataClass::Data_Terrain(new_object_identifier[1], new_object_identifier[2], 0);
		new_terrain->generateInitialValues(new_position, terrain_layer_colors[new_object_identifier[1]], generateNewShape(new_size));
		new_data_object = new_terrain;

		break;
	}

	// Lights
	case Object::LIGHT:
	{
		switch (new_object_identifier[1])
		{
			
		// Directional Lights
		case Object::Light::DIRECTIONAL:
		{
			DataClass::Data_Directional* new_directional = new DataClass::Data_Directional(0);
			new_directional->generateInitialValues(new_position, new_size);
			new_data_object = new_directional;

			break;
		}

		// Point Lights
		case Object::Light::POINT:
		{
			DataClass::Data_Point* new_point = new DataClass::Data_Point(0);
			new_point->generateInitialValues(new_position);
			new_data_object = new_point;

			break;
		}

		// Spot Lights
		case Object::Light::SPOT:
		{
			DataClass::Data_Spot* new_spot = new DataClass::Data_Spot(0);
			new_spot->generateInitialValues(new_position);
			new_data_object = new_spot;

			break;
		}

		// Beam Lights
		case Object::Light::BEAM:
		{
			DataClass::Data_Beam* new_beam = new DataClass::Data_Beam(0);
			new_beam->generateInitialValues(new_position, new_size);
			new_data_object = new_beam;

			break;
		}

		}

		break;
	}

	// Physics
	case Object::PHYSICS:
	{
		switch (new_object_identifier[1])
		{

		// Rigid Body
		case (int)Object::Physics::PHYSICS_BASES::RIGID_BODY:
		{
			DataClass::Data_RigidBody* new_rigid = new DataClass::Data_RigidBody(new_object_identifier[2], 0);
			new_rigid->generateInitialValues(new_position, generateNewShape(new_size));
			new_data_object = new_rigid;

			break;
		}

		// Soft Body
		case (int)Object::Physics::PHYSICS_BASES::SOFT_BODY:
		{
			// Generate Object Data


			switch (new_object_identifier[2])
			{
			// SpringMass
			case (int)Object::Physics::SOFT_BODY_TYPES::SPRING_MASS:
			{
				DataClass::Data_SpringMass* new_springmass = new DataClass::Data_SpringMass(0);
				new_springmass->generateInitialValues(new_position);
				new_data_object = new_springmass;

				break;
			}

			// Wire
			case (int)Object::Physics::SOFT_BODY_TYPES::WIRE:
			{
				DataClass::Data_Wire* new_wire = new DataClass::Data_Wire(0);
				new_wire->generateInitialValues(new_position, new_size);
				new_data_object = new_wire;

				break;
			}
			}

			break;
		}

		// Hinge
		case (int)Object::Physics::PHYSICS_BASES::HINGE_BASE:
		{
			switch (new_object_identifier[2])
			{
			// Anchor
			case (int)Object::Physics::HINGES::ANCHOR:
			{
				DataClass::Data_Anchor* new_anchor = new DataClass::Data_Anchor(0);
				new_anchor->generateInitialValues(new_position);
				new_data_object = new_anchor;

				break;
			}

			// Hinge
			case (int)Object::Physics::HINGES::HINGE:
			{
				DataClass::Data_Hinge* new_hinge = new DataClass::Data_Hinge(0);
				new_hinge->generateInitialValues(new_position);
				new_data_object = new_hinge;

				break;
			}
			}

			break;
		}
		}

		break;
	}

	// Entity
	case Object::ENTITY:
	{
		switch (new_object_identifier[1])
		{
			
		// NPC
		case Object::Entity::ENTITY_NPC:
		{
			DataClass::Data_NPC* new_npc = new DataClass::Data_NPC(0);
			new_npc->generateInitialData(new_position);
			new_data_object = new_npc;

			break;
		}

		// Contollable Entity
		case Object::Entity::ENTITY_CONTROLLABLE:
		{
			DataClass::Data_Controllable* new_controllable = new DataClass::Data_Controllable(0);
			new_controllable->generateInitialData(new_position);
			new_data_object = new_controllable;

			break;
		}

		// Interactable Entity
		case Object::Entity::ENTITY_INTERACTABLE:
		{
			DataClass::Data_Interactable* new_interactable = new DataClass::Data_Interactable(0);
			new_interactable->generateInitialData(new_position);
			new_data_object = new_interactable;

			break;
		}

		// Dynamic Entity
		case Object::Entity::ENTITY_DYNAMIC:
		{
			DataClass::Data_Dynamic* new_dynamic = new DataClass::Data_Dynamic(0);
			new_dynamic->generateInitialData(new_position);
			new_data_object = new_dynamic;

			break;
		}

		}

		break;
	}

	// Effects
	case Object::EFFECT:
	{
		break;
	}

	// Group Objects
	case Object::GROUP:
	{
		DataClass::Data_GroupObject* new_group = new DataClass::Data_GroupObject();
		new_group->generateInitialData(new_position);
		new_data_object = new_group;

		break;
	}

	}

	// Store New Data Object
	unadded_data_objects.push_back(new_data_object);
}

Shape::Shape* Editor::EditorWindow::generateNewShape(float new_size)
{
	// Parse Shape Types
	switch (new_object_identifier[2])
	{

	// Rectangle
	case Shape::RECTANGLE:
	{
		return new Shape::Rectangle(new_size * 2.0f, new_size);
	}

	// Trapezoid
	case Shape::TRAPEZOID:
	{
		return new Shape::Trapezoid(new_size * 2.0f, new_size, 0.0f, 0.0f);
	}

	// Triangle
	case Shape::TRIANGLE:
	{
		glm::vec2 origin = glm::vec2(Global::camera_pointer->Position.x, Global::camera_pointer->Position.y);
		return new Shape::Triangle(origin + glm::vec2(new_size, 0.0f), origin + glm::vec2(0.0f, new_size));
	}

	// Circle
	case Shape::CIRCLE:
	{
		return new Shape::Circle(new_size, 0.0f);
	}

	// Polygon
	case Shape::POLYGON:
	{
		return new Shape::Polygon(new_size, 0.0f, 5, 0.0f);
	}

	}
}

void Editor::EditorWindow::updateEditorMode()
{
	// Location of Mouse in Window
	double mouseStaticX, mouseStaticY;
	mouseStaticX = (float)Global::mouseX / Global::zoom_scale;
	mouseStaticY = (float)Global::mouseY / Global::zoom_scale - (editorHeightFull - editorHeight) * bar1.percent;

	// Set Closing Function of Selected Text
	selected_text->assignCloser([this]()->void { textCloser(); });

	// Update Index
	if (!Global::LeftClick)
		index = 0;

	// Test Collisions of Every Box
	if (!index)
	{
		for (int i = 0; i < boxes_size; i++)
		{
			if (boxes[i]->updateElement())
			{
				// If Active Wheel and Index is a Color Box, Update Color Wheel
				if (wheel_active && i >= wheel_box_start && i < wheel_box_start + 4)
					update_wheel = i;

				// If Light Wheel Active and Index is a Color Box, Update Light Wheel
				if (light_active && i >= light_wheel_box_start && i < light_wheel_box_start + 12)
				{
					// Update Ambient
					if (i - light_wheel_box_start < 4)
						update_ambient = i;

					// Update Diffuse
					else if (i - light_wheel_box_start < 8)
						update_diffuse = i;

					// Update Specular
					else
						update_specular = i;
				}

				// Test String Maps
				if (i)
				{
					// If Script is Active and Box is Script Box, Set Script Flag
					if (i == update_script)
					{
						script_active = true;
					}

					// If Texture is Active and Box is Texture Box, Set Texture Flag
					else if (i == update_texture)
					{
						texture_active = true;
					}

					// If Material is Active and Box is Material Box, Set Material Flag
					else if (i == update_material)
					{
						material_active = true;
					}

					// If Physics Material is Active and Box is Material Box, Set Physics Material Flag
					else if (i == update_physics_material)
					{
						physics_material_active = true;
					}
				}
			}
		}
	}

	// Clear the Selected Text Closer
	selected_text->assignCloser(nullptr);

	// If Editing Mode Changed, Return
	if (editing_mode == EDITING_MODES::NEW_SPRINGMASS || editing_mode == EDITING_MODES::NEW_HINGE)
		return;

	// If Main Color Wheel is Active, Compute Color Wheel Collisions and Button Text
	if (wheel_active)
	{
		DataClass::Data_Object* data_object = data_objects.at(0);
		Object::ObjectData& object_data = static_cast<DataClass::Data_SubObject*>(data_object)->getObjectData();
		if (update_wheel && !boxes[update_wheel]->texting) {
			updateColorWheels(wheel, object_data.colors, wheel_color, mouseStaticX, mouseStaticY, 1, true);
			update_wheel = 0;
		} else
			updateColorWheels(wheel, object_data.colors, wheel_color, mouseStaticX, mouseStaticY, 1, false);
	}

	// Same but for Light Wheels
	if (light_active)
	{
		DataClass::Data_Object* data_object = data_objects.at(0);
		Object::Light::LightData& light_data = static_cast<DataClass::Data_Light*>(data_object)->getLightData();

		// Update Ambient Wheel
		if (update_ambient && !boxes[update_ambient]->texting) {
			updateColorWheels(wheelAmbient, light_data.ambient, ambient_color, mouseStaticX, mouseStaticY, 1, true);
			update_ambient = 0;
		} else
			updateColorWheels(wheelAmbient, light_data.ambient, ambient_color, mouseStaticX, mouseStaticY, 1, false);

		// Update Diffuse Wheel
		if (update_diffuse && !boxes[update_diffuse]->texting) {
			updateColorWheels(wheelDiffuse, light_data.diffuse, diffuse_color, mouseStaticX, mouseStaticY, 4, true);
			update_diffuse = 0;
		} else
			updateColorWheels(wheelDiffuse, light_data.diffuse, diffuse_color, mouseStaticX, mouseStaticY, 4, false);

		// Update Specular
		if (update_specular && !boxes[update_specular]->texting) {
			updateColorWheels(wheelSpecular, light_data.specular, specular_color, mouseStaticX, mouseStaticY, 7, true);
			update_specular = 0;
		} else
			updateColorWheels(wheelSpecular, light_data.specular, specular_color, mouseStaticX, mouseStaticY, 7, false);
	}

	// If Script is Active, Find Script From Script Map
	if (script_active && !boxes[update_script]->texting)
		getKeyFromStringMap(STRING_MAPS::SCRIPT);

	// If Texture is Active, Find Texture From Texture Map
	if (texture_active && !boxes[update_texture]->texting)
		getKeyFromStringMap(STRING_MAPS::TEXTURE);

	// If Material is Active, Find Material From Material Map
	if (material_active && !boxes[update_material]->texting)
		getKeyFromStringMap(STRING_MAPS::MATERIAL);

	// If Physics Material is Active, Find Physics Material From Physics Material Map
	if (physics_material_active && !boxes[update_physics_material]->texting)
		getKeyFromStringMap(STRING_MAPS::PHYSICS_MATERIAL);
}

void Editor::EditorWindow::updateColorWheels(ColorWheel& wheel_, glm::vec4& color, unsigned int* wheel_color_, double mouseStaticX, double mouseStaticY, int offset, bool update)
{
	// Test Collisions With Color Wheel
	if (index == 0 && Global::LeftClick)
		index = wheel_.TestCollisions((float)mouseStaticX, (float)mouseStaticY, offset);

	// If Collision, Update Values on Color Wheel
	if (index)
	{
		switch (index - offset)
		{

		// Update Sample From Color Wheel
		case 0:
		{
			wheel_.UpdateWheel(mouseStaticX, mouseStaticY);
			break;
		}

		// Update Brightness Bar
		case 1:
		{
			wheel_.UpdateBrightness(mouseStaticY);
			break;
		}

		// Update Alpha Bar
		case 2:
		{
			wheel_.UpdateAlpha(mouseStaticY);
			break;
		}
		}

		// Update Color Boxes if Wheel was Modified
		color = wheel_.Color;
		wheel_color_[0] = (unsigned int)(color.r * 255);
		wheel_color_[1] = (unsigned int)(color.g * 255);
		wheel_color_[2] = (unsigned int)(color.b * 255);
		wheel_color_[3] = (unsigned int)(color.a * 255);
	}

	// If Update Flag is True, Update Wheel Values Based on Text Boxes
	else if (update)
	{
		color.r = wheel_color_[0] / 255.0f;
		color.g = wheel_color_[1] / 255.0f;
		color.b = wheel_color_[2] / 255.0f;
		color.a = wheel_color_[3] / 255.0f;
		wheel_.FindColors(color);
	}
}

void Editor::EditorWindow::textCloser()
{
	// Get the Current Data Object
	DataClass::Data_Object* data_object = data_objects.at(0);

	// If Object is a Complex Object, Reload the Complex Group if File Changed
	if (data_object->getGroup() != nullptr && data_object->getGroup()->getCollectionType() == Render::Objects::UNSAVED_COLLECTIONS::COMPLEX)
	{
		// Get the Group's File
		std::string& group_file = static_cast<Render::Objects::UnsavedComplex*>(data_object->getGroup())->getFilePath();

		// Get the Data Object's File
		std::string& object_file = static_cast<DataClass::Data_Complex*>(data_object)->getFilePath();

		// Erase File Extension
		Source::Algorithms::Common::eraseFileExtension(object_file);

		// Compare the File Paths
		if (group_file.compare(object_file))
		{
			// If File Name is Null, Don't Change the File
			if (!Source::Algorithms::Common::getFileName(object_file, false).compare("NULL"))
			{
				// Revert Path to Original Value
				object_file = group_file;
				selected_text->getString() = group_file;

				// Send Error Message
				std::string message = "ERROR: INVALID INPUT DETECTED\n\nThe Names of Files Cannot be \"NULL\"\n\nPlease Change the File Name To Something\nOther Than \"NULL\"";
				notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);

				return;
			}

			// Test if the File name Exists
			try 
			{
				// If Exists Returns 0, File Does Not Exist
				if (!std::filesystem::exists(object_file + ".dat"))
				{
					// Prompt User If They Want to Create the File
					std::string message = "File Does Not Yet Exist\n\nWould You Like to Create It?";
					bool result = notification_->notificationCancelOption(NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message);

					// If User Does Not Want to Create the File, Revert Path
					if (!result)
					{
						object_file = group_file;
						selected_text->getString() = group_file;
						return;
					}

					// Create Both the Dat and Edt Files Here
					std::ofstream fstream;
					fstream.open(object_file + ".dat");
					fstream.close();
					fstream.open(object_file + ".edt");
					fstream.close();
				}
			}

			// Catch Illegal File Names Here
			catch (std::filesystem::filesystem_error const& ex)
			{
				// Revert Path to Original Value
				object_file = group_file;
				selected_text->getString() = group_file;

				// Send Error Message
				std::string message = "ERROR: ILLEGAL FILE PATH DETECTED\n\nThe Requested File Path Contains a File Name\nor a Directory Name That is Reserved by The\nOperating System\n\nPlease Select A Different Path";
				notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);

				return;
			}

			// Store New Complex Group Object
			static_cast<DataClass::Data_Complex*>(data_object)->setGroup(change_controller->getUnsavedComplex(object_file));
		}

		return;
	}
}

void Editor::EditorWindow::closerInvalidFile(std::string& file_good, std::string& file_bad)
{

}

void Editor::EditorWindow::genNewObjectWindow()
{
	const int EDITING_VERTICES_COUNT = 525;

	// Set Mode to New Object Mode
	editing_mode = EDITING_MODES::NEW_OBJECT;

	// Update Vertex Count
	EditorVertexCount = 525;

	// Generate Background
	genBackground();

	// Set Object Identifier Index to Base Object Types
	object_identifier_index = 0;

	// Generate Object Vertices
	changeNewObject();
}

void Editor::EditorWindow::genObjectEditorWindow()
{
	// Reset Some Variables
	wheel_active = false;
	light_active = false;
	update_script = 0;
	script_active = false;
	update_texture = 0;
	texture_active = false;
	update_material = 0;
	material_active = false;
	update_physics_material = 0;
	physics_material_active = false;

	// Constant Number of Boxes Added Per Object Type
	const uint8_t shape_box_adders[5] = { 2, 4, 4, 1, 3 };
	const uint8_t shape_text_adders[5] = { 2, 4, 4, 1, 3 };

	// Set Mode to Editing Mode
	editing_mode = EDITING_MODES::EDIT_OBJECT;

	// Generate Segregators
	genSegregators();

	// Box Offset
	uint8_t box_offset = 0;

	// Text Offset
	uint8_t text_offset = 0;

	// Reset Box Colors
	temp_box_data.background_color = glm::vec4(0.35f, 0.35f, 0.35f, 1.0f);
	temp_box_data.outline_color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
	temp_box_data.highlight_color = glm::vec4(0.75f, 0.75f, 0.75f, 0.85f);
	temp_box_data.text_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Reset Text Data
	temp_text_data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	temp_text_data.scale = 0.1f;
	temp_text_data.static_ = true;

	// Parse Object Identifier
	DataClass::Data_Object* current_data_object = data_objects.at(0);
	uint8_t* object_identifier = current_data_object->getObjectIdentifier();
	int& script = current_data_object->getScript();
	glm::vec2& position = current_data_object->getPosition();

	// Collision Masks
	if (object_identifier[0] == Object::ObjectList::MASK)
	{
		// Floor Mask
		if (object_identifier[1] == Object::Mask::MASKS::FLOOR)
		{
			// Horizontal Line
			if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_LINE)
			{
				resetBoxes(9, 8);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesHorizontalLine(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 68.0f);
				genBoxesPlatform(box_offset, text_offset, windowTop - 83.0f, current_data_object);
				editorHeightFull = 90;
			}

			// Horizontal Slant
			else if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_SLANT)
			{
				resetBoxes(10, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesSlant(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				genBoxesPlatform(box_offset, text_offset, windowTop - 90.0f, current_data_object);
				editorHeightFull = 97;
			}

			// Horizontal Slope
			else if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_SLOPE)
			{
				resetBoxes(10, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesSlope(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				genBoxesPlatform(box_offset, text_offset, windowTop - 90.0f, current_data_object);
				editorHeightFull = 97;
			}
		}

		// Left Mask
		else if (object_identifier[1] == Object::Mask::MASKS::LEFT_WALL)
		{
			// Vertical Line
			if (object_identifier[2] == Object::Mask::VERTICAL_SHAPES::VERTICAL_LINE)
			{
				resetBoxes(8, 8);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesVerticalLine(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 68.0f);
				editorHeightFull = 81;
			}

			// Curve
			else if (object_identifier[2] == Object::Mask::VERTICAL_SHAPES::VERTICAL_CURVE)
			{
				resetBoxes(9, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesCurve(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				editorHeightFull = 88;
			}
		}

		// Right Mask
		else if (object_identifier[1] == Object::Mask::MASKS::RIGHT_WALL)
		{
			// Vertical Line
			if (object_identifier[2] == Object::Mask::VERTICAL_SHAPES::VERTICAL_LINE)
			{
				resetBoxes(8, 8);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesVerticalLine(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 68.0f);
				editorHeightFull = 81;
			}

			// Curve
			else if (object_identifier[2] == Object::Mask::VERTICAL_SHAPES::VERTICAL_CURVE)
			{
				resetBoxes(9, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesCurve(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				editorHeightFull = 88;
			}
		}

		// Ceiling Mask
		else if (object_identifier[1] == Object::Mask::MASKS::CEILING)
		{
			// Horizontal Line
			if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_LINE)
			{
				resetBoxes(8, 8);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesHorizontalLine(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 68.0f);
				editorHeightFull = 81;
			}

			// Horizontal Slant
			else if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_SLANT)
			{
				resetBoxes(9, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesSlant(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				editorHeightFull = 88;
			}

			// Horizontal Slope
			else if (object_identifier[2] == Object::Mask::HORIZONTAL_SHAPES::HORIZONTAL_SLOPE)
			{
				resetBoxes(9, 9);
				getStringFromStringMap(STRING_MAPS::SCRIPT, script);
				genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);
				genBoxesSlope(box_offset, text_offset, windowTop - 61.0f, current_data_object);
				genBoxesMaterial(box_offset, text_offset, windowTop - 75.0f);
				editorHeightFull = 88;
			}
		}

		// Trigger Mask
		else if (object_identifier[1] == Object::Mask::MASKS::TRIGGER)
		{

		}
	}

	// Terrain Object
	else if (object_identifier[0] == Object::ObjectList::TERRAIN)
	{
		// Allocate Memory
		resetBoxes(16 + shape_box_adders[object_identifier[2]], 17 + shape_text_adders[object_identifier[2]]);
		getStringFromStringMap(STRING_MAPS::SCRIPT, script);

		// Create Common Boxes
		genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

		// Generate Shape Boxes
		float object_data_offset = 0.0f;
		Shape::Shape* shape = static_cast<DataClass::Data_Terrain*>(current_data_object)->getShape();
		switch (object_identifier[2])
		{
		// Rectangle
		case Shape::RECTANGLE:
		{
			genBoxesRectangle(box_offset, text_offset, windowTop - 60.0f, shape);
			object_data_offset = 14.0f;
			break;
		}
		// Trapezoid
		case Shape::TRAPEZOID:
		{
			genBoxesTrapezoid(box_offset, text_offset, windowTop - 60.0f, shape);
			object_data_offset = 28.0f;
			break;
		}
		// Triangle
		case Shape::TRIANGLE:
		{
			genBoxesTriangle(box_offset, text_offset, windowTop - 60.0f, shape);
			object_data_offset = 28.0f;
			break;
		}
		// Circle
		case Shape::CIRCLE:
		{
			genBoxesCircle(box_offset, text_offset, windowTop - 60.0f, shape);
			object_data_offset = 7.0f;
			break;
		}
		// Polygon
		case Shape::POLYGON:
		{
			genBoxesPolygon(box_offset, text_offset, windowTop - 60.0f, shape);
			object_data_offset = 21.0f;
			break;
		}
		}

		// Generate Object Data
		genBoxesObject(box_offset, text_offset, windowTop - 69.0f - object_data_offset, current_data_object);

		editorHeightFull = 151 + object_data_offset;
	}

	// Lighting Object
	else if (object_identifier[0] == Object::ObjectList::LIGHT)
	{
		getStringFromStringMap(STRING_MAPS::SCRIPT, script);

		// Directional
		if (object_identifier[1] == Object::Light::DIRECTIONAL)
		{
			// Allocate Memory
			resetBoxes(21, 24);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Create Point Boxes
			genBoxesDirectional(box_offset, text_offset, windowTop - 61.0f, current_data_object);

			// Create Light Boxes
			genBoxesLight(box_offset, text_offset, windowTop - 76.0f, current_data_object);

			editorHeightFull = 196.0f;
		}

		// Point
		else if (object_identifier[1] == Object::Light::POINT)
		{
			// Allocate Memory
			resetBoxes(21, 24);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Create Directional Boxes
			genBoxesPoint(box_offset, text_offset, windowTop - 61.0f, current_data_object);

			// Create Light Boxes
			genBoxesLight(box_offset, text_offset, windowTop - 76.0f, current_data_object);

			editorHeightFull = 196.0f;
		}

		// Spot
		else if (object_identifier[1] == Object::Light::SPOT)
		{
			// Allocate Memory
			resetBoxes(26, 30);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Create Spot Boxes
			genBoxesSpot(box_offset, text_offset, windowTop - 61.0f, current_data_object);

			// Create Light Boxes
			genBoxesLight(box_offset, text_offset, windowTop - 103.0f, current_data_object);

			editorHeightFull = 223.0f;
		}

		// Beam
		else if (object_identifier[1] == Object::Light::BEAM)
		{
			// Allocate Memory
			resetBoxes(23, 26);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Create Beam Boxes
			genBoxesBeam(box_offset, text_offset, windowTop - 61.0f, current_data_object);

			// Create Light Boxes
			genBoxesLight(box_offset, text_offset, windowTop - 90.0f, current_data_object);

			editorHeightFull = 210.0f;
		}
	}

	// Physics Object
	else if (object_identifier[0] == Object::ObjectList::PHYSICS)
	{
		// Rigid Body
		if (object_identifier[1] == 0)
		{
			// Allocate Memory
			resetBoxes(23 + shape_box_adders[object_identifier[2]], 25 + shape_text_adders[object_identifier[2]]);
			getStringFromStringMap(STRING_MAPS::SCRIPT, script);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Generate Shape Boxes
			float object_data_offset = 0.0f;
			Shape::Shape* shape = static_cast<DataClass::Data_RigidBody*>(current_data_object)->getShape();
			switch (object_identifier[2])
			{
				// Rectangle
			case Shape::RECTANGLE:
			{
				genBoxesRectangle(box_offset, text_offset, windowTop - 60.0f, shape);
				object_data_offset = 14.0f;
				break;
			}
			// Trapezoid
			case Shape::TRAPEZOID:
			{
				genBoxesTrapezoid(box_offset, text_offset, windowTop - 60.0f, shape);
				object_data_offset = 28.0f;
				break;
			}
			// Triangle
			case Shape::TRIANGLE:
			{
				genBoxesTriangle(box_offset, text_offset, windowTop - 60.0f, shape);
				object_data_offset = 28.0f;
				break;
			}
			// Circle
			case Shape::CIRCLE:
			{
				genBoxesCircle(box_offset, text_offset, windowTop - 60.0f, shape);
				object_data_offset = 7.0f;
				break;
			}
			// Polygon
			case Shape::POLYGON:
			{
				genBoxesPolygon(box_offset, text_offset, windowTop - 60.0f, shape);
				object_data_offset = 21.0f;
				break;
			}
			}

			// Generate Rigid Body Data
			genBoxesRigidBody(box_offset, text_offset, windowTop - 61.0f - object_data_offset, current_data_object);

			// Generate Object Data
			genBoxesObject(box_offset, text_offset, windowTop - 109.0f - object_data_offset, current_data_object);

			editorHeightFull = 191 + object_data_offset;
		}

		// Soft Body
		else if (object_identifier[1] == 1)
		{
			// SpringMass
			if (object_identifier[2] == 0)
			{
				// SpringMass Node
				if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_NODE)
				{
					// Allocate Memory
					resetBoxes(5, 7);

					// Generate Node Boxes
					genBoxesSpringMassNode(box_offset, text_offset, windowTop, current_data_object);

					editorHeightFull = 80.0f;
				}

				// SpringMass Spring
				else if (add_child_object == CHILD_OBJECT_TYPES::SPRINGMASS_SPRING)
				{
					// Allocate Memory
					resetBoxes(6, 6);

					// Generate Spring Boxes
					genBoxesSpringMassSpring(box_offset, text_offset, windowTop, current_data_object);

					editorHeightFull = 75.0f;
				}

				else
				{
					// Allocate Memory
					resetBoxes(8, 7);

					// Generate Common Boxes
					genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

					// Generate SpringMass Data
					genBoxesSpringMass(box_offset, text_offset, windowTop - 61.0f, current_data_object);

					editorHeightFull = 75.0f;
				}
			}

			// Wire
			else if (object_identifier[2] == 1)
			{

			}
		}

		// Anchor
		else if (object_identifier[1] == 2)
		{

		}

		// Hinge
		else if (object_identifier[1] == 3)
		{

		}
	}

	// Entity Object
	else if (object_identifier[0] == Object::ObjectList::ENTITY)
	{
		getStringFromStringMap(STRING_MAPS::SCRIPT, script);

		switch (object_identifier[1])
		{

		// NPC
		case Object::Entity::ENTITY_NPC:
		{
			break;
		}

		// Controllable Entity
		case Object::Entity::ENTITY_CONTROLLABLE:
		{
			// Allocate Memory
			resetBoxes(20, 21);

			// Create Common Boxes
			genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

			// Generate Entity Data
			genBoxesEntity(box_offset, text_offset, windowTop - 60.0f, current_data_object);

			// Generate Object Data
			genBoxesObject(box_offset, text_offset, windowTop - 97.0f, current_data_object);

			editorHeightFull = 179.0f;

			break;
		}

		// Interactable Entity
		case Object::Entity::ENTITY_INTERACTABLE:
		{
			break;
		}

		// Dynamic Entity
		case Object::Entity::ENTITY_DYNAMIC:
		{
			break;
		}

		}
	}

	// Effect Object
	else if (object_identifier[0] == Object::ObjectList::EFFECT)
	{
		// Particle Generator
		if (object_identifier[1] == 0)
		{

		}
	}

	// Group Object
	else if (object_identifier[0] == Object::ObjectList::GROUP)
	{
		// Allocate Memory
		resetBoxes(7, 7);

		// Create Common Boxes
		genBoxesCommon(box_offset, text_offset, &position.x, &position.y, current_data_object);

		// Generate Group Data
		genBoxesGroup(box_offset, text_offset, windowTop - 60.0f, current_data_object);

		editorHeightFull = 65.0f;
	}

	// Create ScrollBar
	editingOffset = height / 2;
	editorHeight = height - 12 * scale;
	bar1X = border2X - 2 * scale;
	bar1 = GUI::VerticalScrollBar(bar1X, position.y + height / 2 - 10 * scale, 2.0f * scale, editorHeight, editorHeightFull, bar1.percent);
	bar1.moveElement(bar1X + window_position.x, window_position.y + height / 2 - 10 * scale);
	//editing_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	editing_model = glm::translate(editing_model, glm::vec3(window_position.x, window_position.y + (height / 2 - editingOffset), 0.0f));

	// Store Pointer to ScrollBar
	Global::scroll_bar = &bar1;
}

bool Editor::EditorWindow::traverseBackNewObject()
{
	// If Object Identifier Index is 0, Return Unsuccessful
	if (object_identifier_index == 0)
		return false;

	// Else, Decrement Index, Update Window, and Return True
	object_identifier_index--;
	bar1.percent = 0.0f;
	changeNewObject();
	return true;
}

void Editor::EditorWindow::updateWindow()
{
	// Update Object if Mouse Moved or Left Click
	if (Global::cursor_Move || Global::LeftClick)
	{
		// Move Object
		updateScrollBars();

		if (!moving)
		{
			switch (editing_mode)
			{

			// New Object
			case EDITING_MODES::NEW_OBJECT:
			case EDITING_MODES::NEW_SPRINGMASS:
			case EDITING_MODES::NEW_HINGE:
			{
				updateNewObject();
				break;
			}

			// Edit Object
			case EDITING_MODES::EDIT_OBJECT:
			{
				updateEditorMode();
				break;
			}

			}
		}
	}
}

void Editor::EditorWindow::drawWindow()
{
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// Set Shader
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, true);

	// Send View Matrix to Shader
	glUniform1i(Global::staticLocColor, 1);

	// Send Brightness Vector to Shader
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 0.0f);

	// Send Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Draw Background Object
	glBindVertexArray(backgroundVAO);
	glDrawArrays(GL_TRIANGLES, 0, BACKGROUND_VERTICES_COUNT);
	glBindVertexArray(0);

	// Draw ScrollBars
	bar1.blitzElement();

	// Scales Clipping Area
	double widthScalar = (double)Global::screenWidth / (2 * (double)Global::halfScalarX);
	double heightScalar = (double)Global::screenHeight / 100;

	// Set Clipping Area
	glScissor((GLint)(widthScalar * ((double)window_position.x - width / 2 + Global::halfScalarX)), (GLint)(heightScalar * ((double)window_position.y - height / 2 + 2 * (double)scale + 50)), (GLsizei)(widthScalar * width), (GLsizei)(heightScalar * ((double)height - 12.5 * (double)scale)));

	// Draw Highlighter Object if Mouse Hovers Over an Option
	if (editing_mode == EDITING_MODES::NEW_OBJECT || editing_mode == EDITING_MODES::NEW_SPRINGMASS || editing_mode == EDITING_MODES::NEW_HINGE)
	{
		if (index != 0)
		{
			// Draw Highlighter
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
			glBindVertexArray(highlighterVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		// Send Other Model Matrix to Shader
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(editing_model));

		// Draw Segregators
		glBindVertexArray(editing_screenVAO);
		glDrawArrays(GL_TRIANGLES, 0, EditorVertexCount);
		//glDrawArrays(GL_TRIANGLES, 0, 12);
		glBindVertexArray(0);

		glUniform1i(Global::staticLocColor, false);

		// Display Text
		displayText();
	}

	// Draw Object Editing Features
	if (editing_mode == EDITING_MODES::EDIT_OBJECT)
	{
		// Update Master Element
		master.updateElement();

		// Send Other Model Matrix to Shader
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(editing_model));

		// Draw Segregators
		glBindVertexArray(editing_screenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 30);
		glBindVertexArray(0);

		// Draw Boxes
		for (uint8_t i = 0; i < boxes_size; i++)
			boxes[i]->blitzElement();

		// Draw Color Wheel
		if (wheel_active)
		{
			wheel.Blitz(editing_model);
		}

		// Draw Light Wheels
		else if (light_active)
		{
			wheelAmbient.Blitz(editing_model);
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(editing_model));
			wheelDiffuse.Blitz(editing_model);
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(editing_model));
			wheelSpecular.Blitz(editing_model);
		}

		// Switch Shader to Texture Shader to Draw Selected Object if it Has a Texture
		//if (object_data.texture_name != 0)
		if (false)
		{
			// Activate Texture
			Global::texShader.Use();

			// Send Data to Shaders
			glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(selected_model));
			glUniform1i(Global::staticLocTexture, 1);

			// Bind Texture
			//glBindTexture(GL_TEXTURE_2D, object_data.texture_name);
		}

		// Only Send New Model Matrix if it is Colored
		else
		{
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(selected_model));
			//glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, object_data.colors.w);
			glUniform1i(Global::staticLocColor, false);
		}

		// Draw Selected Object
		//glBindVertexArray(object_visualizerVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glBindVertexArray(0);
		//if (object_data.texture_name == 0) { glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 1.0f); }

		// Make Texture Shader Unstatic
		glUniform1i(Global::staticLocTexture, 0);

		// Bind Font Shader
		Global::fontShader.Use();
		// Bind Global Font Shader
		Global::fontGlobalShader.Use();
		glUniformMatrix4fv(Global::viewLocGlobalFont, 1, GL_FALSE, glm::value_ptr(editing_model));

		// Bind Offset Font Shader
		Global::fontOffsetShader.Use();
		glUniformMatrix4fv(Global::modelLocRelativeFont, 1, GL_FALSE, glm::value_ptr(editing_model));

		// Draw Box Text
		for (uint8_t i = 0; i < boxes_size; i++)
			boxes[i]->blitzOffsetText();

		// Bind Font Shader
		//Global::fontShader.Use();

		// Draw Text
		for (uint8_t i = 0; i < texts_size; i++)
			texts[i]->blitzOffsetText();
	}

	// Reset Clipping Area
	glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

	glEnable(GL_DEPTH_TEST);
}
