#include "EditorOptions.h"

#include "Source/Rendering/Initialize.h"
#include "Class/Render/Objects/SubLevel.h"
#include "Class/Render/Objects/Level.h"
#include "Globals.h"
#include "Class/Render/Shader/Shader.h"

// Functions
#include "Source/Algorithms/Common/Common.h"
#include "Source/Algorithms/Quick Math/QuickMath.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Events/EventListeners.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Vertices/Line/LineVertices.h"

Editor::EditorOptions::EditorOptions()
{
	// Read File
	read();
}

void Editor::EditorOptions::initialize()
{
	// Read File
	read();

	// Create GUI
	initialize_gui();
}

void Editor::EditorOptions::read()
{
	// Open Options File
	std::ifstream file("../Resources/Config/Editing.cfg");

	// Copy Lines From File to Memory
	std::string string;

	std::getline(file, string);
	option_double_click = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_default_clamp = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_default_lock = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_visualize_masks = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_visualize_lights = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_clamp_to_clamp = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_enable_lighting = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_disable_pass_through = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_resize = Source::Algorithms::Common::convertStringToBool(string);

	std::getline(file, string);
	option_fps = Source::Algorithms::Common::convertStringToInt(string);

	std::getline(file, string);
	option_default_to_window = Source::Algorithms::Common::convertStringToBool(string);

	for (int i = 0; i < 13; i++)
	{
		std::getline(file, string);
		option_interactable[i] = Source::Algorithms::Common::convertStringToInt(string);
	}

	std::getline(file, string);
	option_camera_speed = Source::Algorithms::Common::convertStringToFloat(string);

	std::getline(file, string);
	option_shift_speed = Source::Algorithms::Common::convertStringToFloat(string);

	std::getline(file, string);
	option_scroll_speed = Source::Algorithms::Common::convertStringToFloat(string);

	std::getline(file, string);
	option_object_info_max_width_percent = Source::Algorithms::Common::convertStringToFloat(string);

	std::getline(file, string);
	option_object_info_text_size_percent = Source::Algorithms::Common::convertStringToFloat(string);

	// Close File
	file.close();
}

void Editor::EditorOptions::write()
{
	// Open Options File
	std::ofstream file("..\\Resources\\Config\\Editing.cfg");

	// Write To File
	file << (int)option_double_click << "\n"
		<< (int)option_default_clamp << "\n"
		<< (int)option_default_lock << "\n"
		<< (int)option_visualize_masks << "\n"
		<< (int)option_visualize_lights << "\n"
		<< (int)option_clamp_to_clamp << "\n"
		<< (int)option_enable_lighting << "\n"
		<< (int)option_disable_pass_through << "\n"
		<< (int)option_resize << "\n"
		<< (int)option_fps << "\n"
		<< (int)option_default_to_window << "\n";
	for (int i = 0; i < 13; i++)
	{
		file << (int)option_interactable[i] << "\n";
	}
	file << option_camera_speed << "\n"
		<< option_shift_speed << "\n"
		<< option_scroll_speed << "\n"
		<< option_object_info_max_width_percent << "\n"
		<< option_object_info_text_size_percent << "\n";

	// Close File
	file.close();
}

void Editor::EditorOptions::initialize_gui()
{
	// Create Width and Height
	width = Global::halfScalarX * 2;
	height = 100.0f;

	// Create Scale Factor
	Scale = width / 170.0f;

	// Initialize Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	secondary_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	// Background Object

	// Create Vertex Objects
	glGenVertexArrays(1, &backgroundVAO);
	glGenBuffers(1, &backgroundVBO);

	// Bind Vertex Objects
	glBindVertexArray(backgroundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, backgroundVBO);

	// Vertices of Background of Window
	float vertices[210];
	int offset = 0;
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.5f, width, height, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices);
	offset += 42;

	// Vertices of Mode Divider
	Vertices::Rectangle::genRectColor(0.0f, 40.0f, -1.4f, width, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices + offset);
	offset += 42;

	// Divider Between Modes
	for (int i = 0; i < 3; i++)
	{
		Vertices::Rectangle::genRectColor(-Global::halfScalarX + (Global::halfScalarX / 2) * (i + 1), 45.0f, -1.4f, 1.0f, 10.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices + offset);
		offset += 42;
	}

	// Store Data in Object
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Lines

	// Create Vertex Object
	glGenVertexArrays(1, &linesVAO);
	glGenBuffers(1, &linesVBO);

	// Bind Vertex Object
	glBindVertexArray(linesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, linesVBO);

	// Nullify Data
	int boxes_count[4] = { 23, 39, 7, 0 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * boxes_count[mode] * 144, NULL, GL_STATIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Create Lines Vertices
	draw_vertices[mode]();

	// Initialize Scroll Bar
	bar = GUI::VerticalScrollBar(Global::halfScalarX - 1.0f * Scale, 40.0f, 2.0f * Scale, 90.0f, options_height, bar.percent);
	y_offset = height_difference * bar.percent;
	secondary_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, y_offset, 0.0f));

	// Selected Options

	// Create Vertex Object
	glGenVertexArrays(1, &selectedVAO);
	glGenBuffers(1, &selectedVBO);

	// Bind Vertex Object
	glBindVertexArray(selectedVAO);
	glBindBuffer(GL_ARRAY_BUFFER, selectedVBO);

	// Nullify Data
	int selected_count[4] = { 11, 13, 2, 0 };
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * selected_count[mode] * 36, NULL, GL_STATIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Highlighter

	// Create Vertex Object
	glGenVertexArrays(1, &highlighterVAO);
	glGenBuffers(1, &highlighterVBO);

	// Bind Vertex Object
	glBindVertexArray(highlighterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, highlighterVBO);

	// Nullify Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 36, NULL, GL_DYNAMIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Popup

	// Create Vertex Object
	glGenVertexArrays(1, &popupVAO);
	glGenBuffers(1, &popupVBO);

	// Bind Vertex Object
	glBindVertexArray(popupVAO);
	glBindBuffer(GL_ARRAY_BUFFER, popupVBO);

	// First Background Layer
	float vertices2[252];
	offset = 0;
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.35f, Global::halfScalarX * 1.02f, 50 * 1.02f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices2);
	offset += 42;

	// Second Background Layer
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.34f, Global::halfScalarX, 50, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices2 + offset);
	offset += 42;

	// Text Box
	Vertices::Line::genLineOutlineRect(-Global::halfScalarX / 2 + 10 * Scale, Global::halfScalarX / 2 - 10 * Scale, -18.0f, -23.0f, -1.3f, 0.5f, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), vertices2 + offset);

	// Store Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_DYNAMIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Color Data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	test_bar = GUI::HorizontalScrollBar(0.0f, -30.0f, 80.0f, 5.0f, 100.0f, option_object_info_max_width_percent);
	test_bar2 = GUI::HorizontalScrollBar(0.0f, -40.0f, 80.0f, 5.0f, 100.0f, option_object_info_text_size_percent);
	test_bar.linkValue(&option_object_info_max_width_percent);
	test_bar2.linkValue(&option_object_info_text_size_percent);
	master = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 100.0f, height);
	GUI::DefaultElements* temp = new GUI::DefaultElements;
	temp->vertical_bar = &bar;
	master.linkValue(temp);
}

void Editor::EditorOptions::Blitz()
{
	// Send Model Matrix to Shader
	Global::colorShaderStatic.Use();
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(Global::brightnessLoc, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	glUniform1i(Global::staticLocColor, 1);

	// Bind ScrollBar
	Global::scroll_bar = &bar;

	// Perform User-Input Handling
	if (Global::cursor_Move || Global::LeftClick)
	{
		Update();
	}

	// Determine Section of Window to Render
	secondary_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, bar.BarOffset, 0.0f));

	// Draw Object
	glBindVertexArray(backgroundVAO);
	glDrawArrays(GL_TRIANGLES, 0, 30);
	glBindVertexArray(0);

	// Set Scissor
	glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)(Global::screenHeight * 0.89f));

	// Send Second Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(secondary_model));

	// Draw Lines and Selectors (and Highlighter
	glBindVertexArray(linesVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)lines_triangles);
	glBindVertexArray(selectedVAO);
	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)select_triangles);
	if (Highlight)
	{
		glBindVertexArray(highlighterVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);

	// Render Boxes
	for (int i = 0; i < box_count; i++)
		box_array[i].blitzElement();

	// Update Master Element
	master.updateElement();

	// Update Scrollbar
	bar.blitzElement();

	if (mode == 3)
	{
		if (test_bar.updateElement() || test_bar2.updateElement())
			write();
		test_bar.blitzElement();
		test_bar2.blitzElement();
	}

	// Render Mode Specific Text
	Global::fontShader.Use();
	std::invoke(text_renderers[mode]);

	// Render Box Text
	for (int i = 0; i < box_count; i++)
		box_array[i].blitzText(2.0f + bar.BarOffset);

	// Reset Scissor
	glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

	// HUD Text
	DisplayText();

	// If Escape was Pressed, Deactivate Window
	if (Global::Keys[GLFW_KEY_ESCAPE])
	{
		// Write Changes
		write();

		// Deactivate Window
		Global::Keys[GLFW_KEY_ESCAPE] = false;
		Active = false;
		glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);
	}
}

void Editor::EditorOptions::getLevelPointer(Render::Objects::Level& level_)
{
	level = &level_;
}

void Editor::EditorOptions::DisplayText()
{
	// Render Header Text

	// Common
	Source::Fonts::renderTextAdvanced("Common", -3 * Global::halfScalarX / 4, 42.0f, 100.0f, Scale * 0.2f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), true);

	// Object
	Source::Fonts::renderTextAdvanced("Object", -1 * Global::halfScalarX / 4, 42.0f, 100.0f, Scale * 0.2f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), true);

	// Function
	Source::Fonts::renderTextAdvanced("Function", 1 * Global::halfScalarX / 4, 42.0f, 100.0f, Scale * 0.2f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), true);

	// Extra
	Source::Fonts::renderTextAdvanced("Extra", 3 * Global::halfScalarX / 4, 42.0f, 100.0f, Scale * 0.2f, glm::vec4(1.0f, 0.4f, 0.0f, 1.0f), true);
}

void Editor::EditorOptions::Render_Common_Text()
{
	// Index of Y Position
	float y_index = 26.0f + bar.BarOffset;

	// X Position of Option Text
	float x_index = -width / 2 + 5.0f * Scale;

	// Size of Text
	float size1 = 0.1f * Scale, size2 = 0.135f * Scale;

	// Double Click
	Source::Fonts::renderText("Requires A Second Mouse Click Before Object Can be Moved", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Double Click", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// AutoClamp
	Source::Fonts::renderText("New Objects Automatically Have Clamp Enabled", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Auto Clamp", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// AutoLock
	Source::Fonts::renderText("New Objects Are Automatically Locked", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Auto Lock", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Visualize Masks
	Source::Fonts::renderText("Determines if Collision Masks Should be Visible", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Visualize Masks", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Visualize Lights
	Source::Fonts::renderText("Determines if Lighting Identifiers Should be Visible", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Visualize Lights", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Clamp To Clamp
	Source::Fonts::renderText("Clampable Objects Can Only Clamp to Objects With Clamp Enabled", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Clamp To Clamp", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Enable Lighting
	Source::Fonts::renderText("Determines if Lighting Should Be Calculated While Editing (Fullbright Mode)", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Enable Lighting", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Disable Pass Through
	Source::Fonts::renderText("Disables Opposite Side of Selected Object From Moving if Selected Side Moves Past", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Disable Pass Through", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Resize
	Source::Fonts::renderText("Allows The Size (Length and Width) of Objects to be Changed", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Resize", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// FPS
	Source::Fonts::renderText("Toggle to Display FPS on Debug Consol and/or Screen", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("FPS", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Default Window
	Source::Fonts::renderText("When Creating a New Object, Editor Window will Reopen to Object", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Default to Window", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;
}

void Editor::EditorOptions::Render_Object_Text()
{
	// Index of Y Position
	float y_index = 26.0f + bar.BarOffset;

	// X Position of Option Text
	float x_index = -width / 2 + 5.0f * Scale;

	// Size of Text
	float size1 = 0.1f * Scale, size2 = 0.135f * Scale;

	// Comment for Object Hide / Ignore Option
	Source::Fonts::renderText("Ignore Prevents Mouse From Interacting With Object. Hide Stops Object From Being Displayed", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);

	// Display Name For Each Object For This Option
	Source::Fonts::renderText("CollisionMaskFloor", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("CollisionMaskWall", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("CollisionMaskReverseFloor", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("CollisionMaskTrigger", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Foreground", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Formerground", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Background", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Backdrop", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Directional", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Point", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Spot", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Particle", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
	Source::Fonts::renderText("Physics Object", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 10.0f;
}

void Editor::EditorOptions::Render_Function_Text()
{
	// Index of Y Position
	float y_index = 26.0f + bar.BarOffset;

	// X Position of Option Text
	float x_index = -width / 2 + 5.0f * Scale;

	// Size of Text
	float size1 = 0.1f * Scale, size2 = 0.135f * Scale;

	// Lock All
	Source::Fonts::renderText("Locks All Objects in All Levels or a Specific Level", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Lock All", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Unlock All
	Source::Fonts::renderText("Unlocks All Objects in All Levels or a Specific Level", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Unlock All", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Delete Level
	Source::Fonts::renderText("Deletes the Specified Level or Culls All Empty Levels", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Delete Level", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Reset to Defaults
	Source::Fonts::renderText("Resets All Options to Their Default Values", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Default", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;
}

void Editor::EditorOptions::Render_Extra_Text()
{
	// Index of Y Position
	float y_index = 26.0f + bar.BarOffset;

	// X Position of Option Text
	float x_index = -width / 2 + 5.0f * Scale;

	// Size of Text
	float size1 = 0.1f * Scale, size2 = 0.135f * Scale;

	// Camera Speed
	Source::Fonts::renderText("Multiplier of the Camera Move Speed", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Camera Speed", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Shift Speed
	Source::Fonts::renderText("Multiplier of the Speed Selected Objects Move by Arrow Keys", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Shift Speed", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;

	// Scroll Speed
	Source::Fonts::renderText("Multiplier of the Speed at Which the ScrollWheel Scrolls ScrollBars", x_index, y_index + 8.0f, size1, glm::vec4(0.0f, 4.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText("Scroll Speed", x_index, y_index, size2, glm::vec4(8.0f, 8.0f, 8.0f, 1.0f), true);
	y_index -= 18.0f;
}

void Editor::EditorOptions::Update()
{
	// Disable Highlight
	Highlight = false;

	// Reset Cursor
	glfwSetCursor(Global::window, Global::Mouse_Textures.find("Cursor")->second);

	// Calculate Positions of Static Mouse Cursor;
	double mouseStaticX, mouseStaticY;
	mouseStaticX = Global::mouseX / Global::zoom_scale;
	mouseStaticY = Global::mouseY / Global::zoom_scale;

	// Update Scroll Bar
	if (bar_offset && Global::LeftClick)
	{
		bar.Scroll((float)mouseStaticY + bar_offset);
		return;
	}

	// Mouse is In Mode Selection
	if (mouseStaticY > 40.0f)
	{
		// Change Mouse Cursor
		glfwSetCursor(Global::window, Global::Mouse_Textures.find("Hand")->second);

		// User Left Clicks
		if (Global::LeftClick)
		{
			// Disable Left Click
			Global::LeftClick = false;

			// Calculate Mode
			int temp_mode = 1 + (int)ceil(mouseStaticX / (Global::halfScalarX / 2));

			// If Mode is Different, Reinitialize GUI and Scrollbar
			if (temp_mode != mode)
			{
				// Set Mode
				mode = temp_mode;

				// Reset Scrollbar
				bar.percent = 0;

				// Re-Initialize GUI
				initialize_gui();
			}
		}

		return;
	}

	// Test if ScrollBar Should Start Moving
	if (bar.TestColloisions() && Global::LeftClick && bar_offset == 0.0f)
	{
		bar_offset = bar.BarPosY - (float)mouseStaticY;
		return;
	}

	// Reset Bar Offset
	bar_offset = 0.0f;

	// Mouse is In Body of GUI
	updateBoxes(glm::vec2(mouseStaticX, mouseStaticY - bar.BarOffset));
}

void Editor::EditorOptions::updateBoxes(glm::vec2 cursor)
{
	for (int i = 0; i < box_count; i++)
	{
		// Test if Mouse Toggled Box
		box_array[i].updateElement();
	}
}

void Editor::EditorOptions::Common_Vertices()
{
	// Index of Y Position
	float y_index = 26.0f;

	// Width and Height
	float temp_width = 20.0f, temp_height = 8.0f;

	// Pointer
	float* pointer = nullptr;

	// Vertices
	float vertices[144] = {0};

	// Vertices Offset in Data
	int offset = 0;

	// Box X Position Offest
	float x_offsets[11] = { -40.0f, -42.0f, -42.0f, -35.0f, -35.0f, -35.0f, -35.0f, -30.0f, -42.0f, -42.0f, -33.0f };

	// Free Memory
	if (box_count)
		delete[] box_array;
	if (group_count)
		delete[] group_array;

	// Allocate Memory for Groups and Boxes
	box_array = new GUI::Box[23];
	group_array = new GUI::ToggleGroup[11];
	box_count = 23;
	group_count = 11;

	// Object Index Holders
	int box_index = 0;
	int group_index = 0;

	// Draw First Group of Booleans Boxes
	generateBooleanBoxes(x_offsets, y_index, 18.0f, temp_width, temp_height, 0, 9, box_index, group_index);

	// Draw FPS Boxes
	std::string* fps_strings = new std::string[3]( "Off", "Debug", "Display" );
	generateMultipleBoxes(3, x_offsets, y_index, 18.0f, temp_width, temp_height, 9, 10, box_index, group_index, fps_strings);

	// Draw Second Group of Boolean Boxes
	generateBooleanBoxes(x_offsets, y_index, 18.0f, temp_width, temp_height, 10, 11, box_index, group_index);

	// Store Pointers to Group Data
	group_array[0].setDataPointer(&option_double_click);
	group_array[1].setDataPointer(&option_default_clamp);
	group_array[2].setDataPointer(&option_default_lock);
	group_array[3].setDataPointer(&option_visualize_masks);
	group_array[4].setDataPointer(&option_visualize_lights);
	group_array[5].setDataPointer(&option_clamp_to_clamp);
	group_array[6].setDataPointer(&option_enable_lighting);
	group_array[7].setDataPointer(&option_disable_pass_through);
	group_array[8].setDataPointer(&option_resize);
	group_array[9].setDataPointer(&option_fps);
	group_array[10].setDataPointer(&option_default_to_window);

	// Determine Full Height of Window
	y_index += 10.0f;
	options_height = 40.0f - y_index;
	height_difference = options_height - (height - 20.0f);
}

void Editor::EditorOptions::Object_Vertices()
{
	// Index of Y Position
	float y_index = 28.0f;

	// Width and Height
	float temp_width = 20.0f, temp_height = 6.0f;

	// Pointer
	float* pointer = nullptr;

	// Vertices
	float vertices[144] = {0};

	// Vertices Offset in Data
	int offset = 0;

	// Box X Position Offest
	float x_offsets[13] = { -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f };

	// Free Memory
	if (box_count)
		delete[] box_array;
	if (group_count)
		delete[] group_array;

	// Allocate Memory for Groups and Boxes
	box_array = new GUI::Box[39];
	group_array = new GUI::ToggleGroup[13];
	box_count = 39;
	group_count = 13;

	// Object Index Holders
	int box_index = 0;
	int group_index = 0;

	// Generate Hide/Ignore Boxes
	std::string* strings = new std::string[3]("None", "Ignore", "Hide");
	generateMultipleBoxes(3, x_offsets, y_index, 10.0f, temp_width, temp_height, 0, 13, box_index, group_index, strings);

	// Link Interactable Variables
	for (int i = 0; i < 13; i++)
		group_array[i].setDataPointer(&option_interactable[i]);

	// Determine Full Height of Window
	y_index += 2.0f;
	options_height = 40.0f - y_index;
	height_difference = options_height - (height - 20.0f);
}

void Editor::EditorOptions::Function_Vertices()
{
	// Colors
	const glm::vec4 background_colors(0.1f, 0.1f, 0.1f, 1.0f);
	const glm::vec4 outline_colors(0.6f, 0.6f, 0.6f, 1.0f);
	const glm::vec4 highlight_colors(0.0f, 1.0f, 0.0f, 1.0f);
	const glm::vec4 text_colors(1.0f, 0.0f, 0.0f, 1.0f);

	// Index of Y Position
	float y_index = 26.0f;

	// Width and Height
	float temp_width = 20.0f, temp_height = 8.0f;

	// Pointer
	float* pointer = nullptr;

	// Vertices
	float vertices[144] = {0};

	// Vertices Offset in Data
	int offset = 0;

	// Box X Position Offest
	float x_offsets[4] = { -40.0f, -40.0f, -42.0f, -50.0f };

	// Free Memory
	if (box_count)
		delete[] box_array;
	if (group_count)
		delete[] group_array;

	// Allocate Memory for Groups and Boxes
	box_array = new GUI::Box[7];
	box_count = 7;
	group_count = 0;

	// Lock All Box
	GUI::BoxData data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[0], y_index, -1.4f, temp_width, temp_height, true, "All", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[0] = GUI::Box(data);
	box_array[0].setFunctionPointer([this]()->void {this->lockAll(); });

	// Lock Select Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[0] + 30.0f, y_index, -1.4f, temp_width, temp_height, true, "Select", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[1] = GUI::Box(data);
	box_array[1].setFunctionPointer([this]()->void {this->lockSelect(); });
	y_index -= 18.0f;

	// Unlock All Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[1], y_index, -1.4f, temp_width, temp_height, true, "All", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[2] = GUI::Box(data);
	box_array[2].setFunctionPointer([this]()->void {this->unlockAll(); });

	// Unlock Select Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[1] + 30.0f, y_index, -1.4f, temp_width, temp_height, true, "Select", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[3] = GUI::Box(data);
	box_array[3].setFunctionPointer([this]()->void {this->unlockSelect(); });
	y_index -= 18.0f;

	// Delete Delete Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[2], y_index, -1.4f, temp_width, temp_height, true, "Delete", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[4] = GUI::Box(data);
	box_array[4].setFunctionPointer([this]()->void {this->deleteLevel(); });

	// Delete Cull Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[2] + 30.0f, y_index, -1.4f, temp_width, temp_height, true, "Cull", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[5] = GUI::Box(data);
	box_array[5].setFunctionPointer([this]()->void {this->cullEmptyLevels(); });
	y_index -= 18.0f;

	// Reset To Defaults Box
	data = Source::Render::Initialize::constrtuctBox(GUI::FUNCTION_BOX, x_offsets[3], y_index, -1.4f, temp_width, temp_height, true, "Reset", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[6] = GUI::Box(data);
	box_array[6].setFunctionPointer([this]()->void {this->resetToDefault(); });
	y_index -= 18.0f;

	// Determine Full Height of Window
	y_index += 10.0f;
	options_height = 40.0f - y_index;
	height_difference = options_height - (height - 20.0f);
}

void Editor::EditorOptions::Extra_Vertices()
{
	// Colors
	const glm::vec4 background_colors(0.1f, 0.1f, 0.1f, 1.0f);
	const glm::vec4 outline_colors(0.6f, 0.6f, 0.6f, 1.0f);
	const glm::vec4 highlight_colors(0.0f, 1.0f, 0.0f, 1.0f);
	const glm::vec4 text_colors(1.0f, 0.0f, 0.0f, 1.0f);

	// Index of Y Position
	float y_index = 26.0f;

	// Width and Height
	float temp_width = 40.0f, temp_height = 8.0f;

	// Pointer
	float* pointer = nullptr;

	// Vertices
	float vertices[144] = {0};

	// Vertices Offset in Data
	int offset = 0;

	// Box X Position Offest
	float x_offsets[3] = { -30.0f, -35.0f, -33.0f };

	// Free Memory
	if (box_count)
		delete[] box_array;
	if (group_count)
		delete[] group_array;

	// Allocate Memory for Groups and Boxes
	box_array = new GUI::Box[3];
	box_count = 3;
	group_count = 0;

	// Generate Camera Speed Box
	GUI::BoxData data = Source::Render::Initialize::constrtuctBox(GUI::ABSOLUTE_NUMERICAL_TEXT_BOX, x_offsets[0], y_index, -1.4f, temp_width, temp_height, false, "", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[0] = GUI::Box(data);
	box_array[0].setDataPointer(&option_camera_speed);
	y_index -= 18.0f;

	// Generate Shift Speed Box
	data = Source::Render::Initialize::constrtuctBox(GUI::ABSOLUTE_NUMERICAL_TEXT_BOX, x_offsets[1], y_index, -1.4f, temp_width, temp_height, false, "", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[1] = GUI::Box(data);
	box_array[1].setDataPointer(&option_shift_speed);
	y_index -= 18.0f;

	// Generate Scroll Speed Box
	data = Source::Render::Initialize::constrtuctBox(GUI::ABSOLUTE_NUMERICAL_TEXT_BOX, x_offsets[2], y_index, -1.4f, temp_width, temp_height, false, "", background_colors, outline_colors, highlight_colors, text_colors);
	box_array[2] = GUI::Box(data);
	box_array[2].setDataPointer(&option_scroll_speed);
	y_index -= 18.0f;

	// Determine Full Height of Window
	y_index += 10.0f;
	options_height = 40.0f - y_index;
	height_difference = options_height - (height - 20.0f);
}

void Editor::EditorOptions::generateBooleanBoxes(float* x_offsets, float& y_index, float y_difference, float temp_width, float temp_height, int begin, int end, int& box_index, int& group_index)
{
	// Colors
	const glm::vec4 background_colors(0.1f, 0.1f, 0.1f, 1.0f);
	const glm::vec4 outline_colors(0.6f, 0.6f, 0.6f, 1.0f);
	const glm::vec4 highlight_colors(0.0f, 1.0f, 0.0f, 1.0f);
	const glm::vec4 text_colors(1.0f, 0.0f, 0.0f, 1.0f);

	// Array of Two Boxes
	GUI::BoxData boxes[2];

	for (int i = begin; i < end; i++)
	{
		// False
		boxes[0] = Source::Render::Initialize::constrtuctBox(GUI::GROUPED_BOX, x_offsets[i] + 30.0f, y_index, -1.4f, temp_width, temp_height, true, "False", background_colors, outline_colors, highlight_colors, text_colors);

		// True
		boxes[1] = Source::Render::Initialize::constrtuctBox(GUI::GROUPED_BOX, x_offsets[i], y_index, -1.4f, temp_width, temp_height, true, "True", background_colors, outline_colors, highlight_colors, text_colors);

		// Grouper
		GUI::ToggleGroupData group_data = Source::Render::Initialize::constructGrouper(2, 0);
		group_array[group_index] = GUI::ToggleGroup(group_data, box_array, box_index, boxes);

		// Increment Values
		box_index += 2;
		group_index++;
		y_index -= y_difference;
	}
}

void Editor::EditorOptions::generateMultipleBoxes(uint8_t stride, float* x_offsets, float& y_index, float y_difference, float temp_width, float temp_height, int begin, int end, int& box_index, int& group_index, std::string* text)
{
	// Colors
	const glm::vec4 background_colors(0.1f, 0.1f, 0.1f, 1.0f);
	const glm::vec4 outline_colors(0.6f, 0.6f, 0.6f, 1.0f);
	const glm::vec4 highlight_colors(0.0f, 1.0f, 0.0f, 1.0f);
	const glm::vec4 text_colors(1.0f, 0.0f, 0.0f, 1.0f);

	// Array of Boxes
	GUI::BoxData* boxes = new GUI::BoxData[stride];

	for (int i = begin; i < end; i++)
	{
		// Generate Boxes
		for (int j = 0, offset = (int)x_offsets[i]; j < stride; j++, offset += 30)
		{
			// False
			boxes[j] = Source::Render::Initialize::constrtuctBox(GUI::GROUPED_BOX, (float)offset, y_index, -1.4f, temp_width, temp_height, true, text[j], background_colors, outline_colors, highlight_colors, text_colors);
		}

		// Grouper
		GUI::ToggleGroupData group_data = Source::Render::Initialize::constructGrouper(stride, 0);
		group_array[group_index] = GUI::ToggleGroup(group_data, box_array, box_index, boxes);

		// Increment Values
		box_index += stride;
		group_index++;
		y_index -= y_difference;
	}

	// Delete Temp Box Array
	delete[] boxes;
}

std::string& Editor::EditorOptions::querry(std::string message, std::string input, int typing_mode)
{
	// Reset User Input String
	Global::text = &input;

	// Enable Typing Mode
	Global::textModifier = typing_mode;
	glfwSetKeyCallback(Global::window, Source::Listeners::TypeCallback);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// While Loop That Propmts User for Input
	while (!Global::Keys[GLFW_KEY_ENTER] || input == "")
	{
		// Clear Window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Handle Inputs
		glfwPollEvents();

		// Draw Popup Window
		Global::colorShaderStatic.Use();
		glBindVertexArray(popupVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// Draw Text
		Global::fontShader.Use();
		Source::Fonts::renderText(message, -Global::halfScalarX / 2 + 5.0f * Scale, 60.0f, 0.15f * Scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
		Source::Fonts::renderText(input, -Global::halfScalarX / 2 + 12.0f * Scale, 28.0f, 0.15f * Scale, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);

		// Update Window
		glfwSwapBuffers(Global::window);
	}

	// Disable Typing Mode
	glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
	Global::Keys[GLFW_KEY_ENTER] = false;

	// Return Text
	return input;
}

void Editor::EditorOptions::lockAll()
{
	lockUnlockFunction(true, false);
}

void Editor::EditorOptions::lockSelect()
{
	lockUnlockFunction(true, true);
}

void Editor::EditorOptions::unlockAll()
{
	lockUnlockFunction(false, false);
}

void Editor::EditorOptions::unlockSelect()
{
	lockUnlockFunction(false, true);
}

void Editor::EditorOptions::lockUnlockFunction(bool lock, bool specific)
{
	// If Specific, Ask User for X and Y of Level
	if (specific)
	{
		// Specific Level
		glm::vec2 specific_level = glm::vec2(0.0f, 0.0f);

		// Collect User Information
		specific_level.x = (float)Source::Algorithms::Common::convertStringToInt(querry("Enter the X Position of Level", "", 4));
		specific_level.y = (float)Source::Algorithms::Common::convertStringToInt(querry("Enter the Y Position of Level", "", 4));

		// Perform Operation
		lockUnlockLevel(specific_level, lock);

		// Consol Output
		if (lock) { std::cout << "Locked Level At Coords: " << specific_level.x << "," << specific_level.y << "\n"; }
		else { std::cout << "Unlocked Level At Coords: " << specific_level.x << "," << specific_level.y << "\n"; }
	}

	else
	{
		// Load List of Files
		std::string filePath = "../Resources/Data/Levels";

		// Iterate Though Levels Directory
		for (const auto& entry : std::filesystem::directory_iterator(filePath))
		{
			// Save File Path to String
			std::string test = entry.path().string();

			// Remove Path of File Object
			test.replace(0, filePath.length() + 1, "");

			// Transform Entry Into Vec2 of Level Coordinates
			glm::vec2 coords = glm::vec2(0.0f, 0.0f);
			std::string result = "";
			int index = 0;
			for (int i = 0; i < test.size(); i++)
			{
				if (test[i] == ',' || test[i] == '.')
				{
					switch (index)
					{
					case 0: { coords.x = (float)Source::Algorithms::Common::convertStringToInt(result); break; }
					case 1: { coords.y = (float)Source::Algorithms::Common::convertStringToInt(result); break; }
					}
				}

				else
				{
					result += test[i];
				}
			}

			// Perform Opeation
			lockUnlockLevel(coords, lock);
		}
	}

	// Reset Levels
	level->resetLevel();

	// Consol Output
	if (lock) { std::cout << "Locked All Levels\n"; }
	else { std::cout << "Unlocked All Levels\n"; }
}

void Editor::EditorOptions::lockUnlockLevel(glm::vec2 level_coords, bool lock)
{
	// Temporary Values
	std::string object_name;
	short name_size;
	char byte;

	// Get Path of Editor File
	std::string path = "../Resources/Data/EditorLevelData/";
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(level_coords.x)));
	path.append(",");
	path.append(Source::Algorithms::Common::removeTrailingZeros(std::to_string(level_coords.y)));

	// Copy File Editor Content into StringStream
	std::stringstream file_contents;
	std::ifstream in_file(path, std::ios::binary);
	file_contents << in_file.rdbuf();
	in_file.close();

	// Open Same File for Writing
	std::ofstream out_file(path, std::ios::binary);

	// Iterate Through Editor File and Only Modify Lock Value. Copy Data Back Into File
	while (!file_contents.eof())
	{
		// Copy Object Name Size
		file_contents.read((char*)&name_size, sizeof(name_size));
		out_file.write((char*)&name_size, sizeof(name_size));

		// Copy Object Name
		for (int i = 0; i < name_size; i++)
		{
			file_contents.read(&byte, 1);
			out_file.put(byte);
		}

		// Set Lock State
		out_file.put(lock);

		// Copy Clamp Condition
		file_contents.read(&byte, 1);
		out_file.put(byte);
	}

	// Close File
	out_file.close();
}

void Editor::EditorOptions::deleteLevel()
{
	// Ask User for Level Loc
	std::string x = querry("Enter the X Position of Level", "", 4);
	std::string y = querry("Enter the Y Position of Level", "", 4);
	std::string path = "../Resources/Data/Levels/" + x + "," + y;

	// Delete Level
	std::filesystem::remove(path);

	// Consol Output
	std::cout << "Deleted Level At Path: " << path << "\n";
}

void Editor::EditorOptions::cullEmptyLevels()
{
	// Iterate Though Levels Directory
	for (const auto& entry : std::filesystem::directory_iterator("../Resources/Data/Levels"))
	{
		// Save File Path to String
		std::string path = entry.path().string();

		// Open File
		std::ifstream file(path);

		// Test If Every Line is Empty
		std::string line;
		bool isempty_flag = true;
		while (std::getline(file, line))
		{
			if (line != ";")
			{
				isempty_flag = false;
				break;
			}
		}

		// Close File
		file.close();

		// Delete File if Empty
		if (isempty_flag)
		{
			std::filesystem::remove(path);
		}
	}

	// Consol Output
	std::cout << "Culled Empty Levels\n";
}

void Editor::EditorOptions::resetToDefault()
{
	// Common Options
	option_double_click = true;
	option_default_clamp = true;
	option_default_lock = false;
	option_visualize_masks = true;
	option_visualize_lights = true;
	option_clamp_to_clamp = false;
	option_enable_lighting = true;
	option_disable_pass_through = false;
	option_resize = true;
	option_fps = 0;
	option_default_to_window = false;

	// Object Options
	for (int i = 0; i < 13; i++)
	{
		option_interactable[i] = 0;
	}

	// Extra Options
	option_camera_speed = 1.0f;
	option_shift_speed = 1.0f;
	option_scroll_speed = 3.0f;
	option_object_info_max_width_percent = 1.0f;
	option_object_info_text_size_percent = 1.0f;
}
