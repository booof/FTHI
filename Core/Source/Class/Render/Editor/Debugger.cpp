#include "Debugger.h"
#include "Globals.h"
#include "Notification.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Events/EventListeners.h"
#include "Render/Shader/Shader.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Rendering/Initialize.h"
#include "Render/Editor/ProjectSelector.h"
#include "Render/GUI/AdvancedString.h"
#include "SceneController.h"
#include "Render\Objects\Level.h"

void Editor::Debugger::readErrorLog()
{
	// The String That Determines if Line is an Error
	const char* error_keyword = "): error ";

	// Debug Error File
	std::ifstream file;

	// Set Looked At Once Flag to True
	look_at_build = false;
	looked_at_least_once = true;

	// Clear Error List
	errors.clear();
	errors_size = 0;

	// Open File
	file.open(Global::script_folder_path + "\\..\\LastBuildOutput.txt");

	// Variables Used to Get Each Line in File
	std::string line;
	std::string current_file_name = "";

	// If First Line Has a CMAKE Error, Rebuild the Solution
	std::getline(file, line);
	if (line.substr(0, 39) == "CMake Error: The current CMakeCache.txt")
	{
		std::cout << Global::project_file_path << "\n";

		std::string cmd_path = "start "" /b CALL \"" + Global::engine_path + "\\Core\\EngineLibs\\ProjectBuilder\\ProjectBuilder.bat\" \"" + Global::project_file_path + "\\..\\Build\"";
		system(cmd_path.c_str());
		return;
	}

	// Read Each Line in File
	while (std::getline(file, line))
	{
		// If First Character is a Space, Save Current_file_name
		if (line[0] == ' ')
		{
			current_file_name = line.substr(2, line.size() - 2);
		}

		else
		{
			// Test if the Error Keyword is in Line
			int error_index = 0;
			for (int i = 0; i < line.size(); i++)
			{
				if (line[i] == error_keyword[error_index])
				{
					error_index++;
					if (error_index == 9)
						break;
				}

				else
					error_index = 0;
			}
			if (error_index != 9)
				continue;

			// Don't Read Empty Lines
			if (line.size() < 2)
				continue;

			// Test if ':' is In Line (This Character Only Appears in Errors After Path)
			int colon_index = 3;
			while (colon_index < line.size() - 2 && line[colon_index] != ':') { colon_index++; }
			if (line[colon_index] == ':')
			{
				// Temp Variable for the Error
				DebugError error;
				error.file_name = current_file_name;

				// Seperate The Coordinates and Path of the Error
				std::string coordinates = "";
				int coordinates_begin = colon_index - 1;
				while (line[coordinates_begin] != '(') { coordinates_begin--; }
				coordinates = line.substr((size_t)coordinates_begin + 1, (size_t)colon_index - coordinates_begin - 2);
				error.file_path = line.substr(0, coordinates_begin);

				// Determine the Coordinates of the Error
				std::string temp_number = "";
				int coordinates_index = 0;
				while (coordinates[coordinates_index] != ',') { coordinates_index++; }
				temp_number = coordinates.substr(0, coordinates_index);
				error.line = Source::Algorithms::Common::convertStringToInt(temp_number);
				temp_number = coordinates.substr((size_t)coordinates_index + 1, coordinates.size() - coordinates_index - 1);
				error.column = Source::Algorithms::Common::convertStringToInt(temp_number);

				// Get the Error Name
				std::string error_type;
				colon_index += 2;
				int secondary_colon_index = colon_index;
				while (line[secondary_colon_index] != ' ') { secondary_colon_index++; }
				error_type = line.substr(colon_index, (size_t)secondary_colon_index - colon_index);
				secondary_colon_index++;
				colon_index = secondary_colon_index;
				while (line[secondary_colon_index] != ':') { secondary_colon_index++; }
				error.error_name = line.substr(colon_index, (size_t)secondary_colon_index - colon_index);
				if (error_type == "error")
					error.error_type = TYPE_ERROR;

				// Get the Type of Error (Error, Linker, etc)
				secondary_colon_index += 2;
				colon_index = secondary_colon_index;
				while (line[secondary_colon_index] != '[') { secondary_colon_index++; }
				error.error_message = line.substr(colon_index, (size_t)secondary_colon_index - colon_index - 1);

				// Store Error in Vector
				errors.push_back(error);

				// Mark That An Error Was Detected
				look_at_build = true;
				errors_size++;
			}
		}
	}

	// Close file
	file.close();

	//std::cout << "begin error\n";
	//for (int i = 0; i < errors.size(); i++)
	//{
	//	std::cout << "Type: " << (int)errors[i].error_type << "\n";
	//	std::cout << "Error: " << errors[i].error_name << "\n";
	//	std::cout << "Message: " << errors[i].error_message << "\n";
	//	std::cout << "File: " << errors[i].file_name << "\n";
	//	std::cout << "Path: " << errors[i].file_path << "\n";
	//	std::cout << "Line: " << errors[i].line << "\n";
	//	std::cout << "Column: " << errors[i].column << "\n";
	//}
	//std::cout << "end error\n\n\n\n\n";

	// Generate ScrollBar
	bar = Render::GUI::VerticalScrollBar(60.0f, 26.0f, 1.0f, 56.0f, 5.0f * errors.size(), 0.0f, -1);
}

void Editor::Debugger::initializeDebugger()
{
	// Window Object

	// Generate Window Vertex Object
	glGenVertexArrays(1, &windowVAO);
	glGenBuffers(1, &windowVBO);
	
	// Bind Window Object
	glBindVertexArray(windowVAO);
	glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

	// Allocate Memory
	glBufferData(GL_ARRAY_BUFFER, 1512, NULL, GL_STATIC_DRAW);

	// Generate Outline Vertices
	float vertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 131.0f, 76.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

	// Generate Background Vertices
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 130.0f, 75.0f, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

	// Generate Body Outline Vertices
	Vertices::Rectangle::genRectColor(-1.0f, -2.0f, -1.0f, 121.0f, 56.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 336, 168, vertices);

	// Generate Body Background Vertices
	Vertices::Rectangle::genRectColor(-1.0f, -4.25f, -1.0f, 120.0f, 50.5f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 504, 168, vertices);

	// Generate Label Outline
	Vertices::Rectangle::genRectColor(-1.0f, 23.5f, -1.0f, 120.0f, 4.0f, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 672, 168, vertices);

	// Generate Type/Error Divider
	Vertices::Rectangle::genRectColor(-50.0f, -2.0f, -1.0f, 0.5f, 55.0f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 840, 168, vertices);

	// Generate Error/Message Divider
	Vertices::Rectangle::genRectColor(-35.0f, -2.0f, -1.0f, 0.5f, 55.0f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 1008, 168, vertices);

	// Generate Message/File Divider
	Vertices::Rectangle::genRectColor(25.0f, -2.0f, -1.0f, 0.5f, 55.0f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 1176, 168, vertices);

	// Generate File/Line Divider
	Vertices::Rectangle::genRectColor(50.0f, -2.0f, -1.0f, 0.5f, 55.0f, glm::vec4(0.1f, 0.1f, 0.1f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 1344, 168, vertices);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Position Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Window Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Body Object

	glGenVertexArrays(1, &bodyVAO);
	glGenBuffers(1, &bodyVBO);

	// Bind Window Object
	glBindVertexArray(bodyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);

	// Allocate Memory
	glBufferData(GL_ARRAY_BUFFER, 1848, NULL, GL_STATIC_DRAW);

	// Generate Vertices
	float y_offset = 16.0f;
	for (int i = 0, j = 0; i < 11; i++, j += 168)
	{
		Vertices::Rectangle::genRectColor(-1.0f, y_offset, -1.0f, 120.0f, 0.5f, glm::vec4(0.3f, 0.3f, 0.3f, 0.6f), vertices);
		glBufferSubData(GL_ARRAY_BUFFER, j, 168, vertices);
		y_offset -= 5.0f;
	}

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Position Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Body Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Highlighter Object

	glGenVertexArrays(1, &highlightVAO);
	glGenBuffers(1, &highlightVBO);

	// Bind Window Object
	glBindVertexArray(highlightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, highlightVBO);

	// Generate Vertices
	Vertices::Rectangle::genRectColor(-1.0f, 0.0f, -1.0f, 119.0f, 4.0f, glm::vec4(0.6f, 0.6f, 0.6f, 0.5f), vertices);
	glBufferData(GL_ARRAY_BUFFER, 168, vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Position Vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Highlighter Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Generate Master Element
	master = Render::GUI::MasterElement(glm::vec2(0.0f, 0.0f), 100.0f, 80.0f);

	// Generate Scroll Bar
	bar = Render::GUI::VerticalScrollBar(60.0f, 26.0f, 1.0f, 56.0f, 100.0f, 0.0f, -1);

	// Open File Box
	Render::GUI::BoxDataBundle temp_box_data = Source::Rendering::Initialize::constrtuctBox(Render::GUI::BOX_MODES::NULL_BOX, 18.0f, -33.5f, -1.0f, 20.0f, 5.0f, true, "Open", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 0.7f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
	temp_box_data.data1.is_static = true;
	box_open_file = Render::GUI::Box(temp_box_data.data1, temp_box_data.data2);

	// Compile Box
	temp_box_data.data1.position.x = 48.0f;
	temp_box_data.data2.button_text = Render::GUI::AdvancedString("Compile");
	box_recompile_project = Render:: GUI::Box(temp_box_data.data1, temp_box_data.data2);

	// Exit Box
	temp_box_data = Source::Rendering::Initialize::constrtuctBox(Render::GUI::BOX_MODES::NULL_BOX, 60.0f, 33.0f, -1.0f, 4.0f, 4.0f, false, "", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 0.7f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	box_exit = Render::GUI::Box(temp_box_data.data1, temp_box_data.data2);
}

Editor::Debugger* Editor::Debugger::get()
{
	return &instance;
}

void Editor::Debugger::deleteDebugger()
{
}

void Editor::Debugger::update()
{
	// Test if GUI Needs to Look at Debug Output
	if (!looked_at_least_once && std::filesystem::exists(std::filesystem::path(Global::project_scripts_path + "\\..\\compilation complete")))
	{
		// Read Debug Output
		readErrorLog();

		// If Errors Exist, Prompt User to Debug
		if (look_at_build)
		{
			std::string message = "Build Errors Detected\n\nClick OK to Open Debugger\n\nClick Cancel to Stop Building";
			if (notification_->notificationCancelOption(NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message))
				updateWindow();
		}
	}
}

void Editor::Debugger::enableFlag()
{
	look_at_build = true;
	looked_at_least_once = false;
}

bool Editor::Debugger::testIfCanRun()
{
	// If Project is Not Finished Compiling Return False
	if (!looked_at_least_once)
	{
		std::string message = "Compilation is Not Complete\n\nPlease Wait a Few More Seconds";
		notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message);
		return false;
	}

	// If Project is Finished and Output Needs to be Looked at, Allow User to Debug
	if (look_at_build)
	{
		std::string message = "Build Errors Detected\n\nClick OK to Open Debugger\n\nClick Cancel to Stop Building";
		if (!notification_->notificationCancelOption(NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message))
			return false;

		// Open Debugger Window
		updateWindow();

		// If Build Still Has Errors, Return False
		if (look_at_build)
			return false;
	}

	// Return True Since There is No Other Conditions
	return true;
}

void Editor::Debugger::updateWindow()
{
	// Map for Error Types
	std::string error_map[2] = { "ERROR", "LINK" };

	// The Currently Selected Row
	int selected_row = -1;

	// Disable Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Disable Depth Test
	glDisable(GL_DEPTH_TEST);

	// Bind ScrollBar
	Global::scroll_bar = static_cast<Render::GUI::ScrollBar*>(&bar);
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);

	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, 1);
	glm::mat4 temp = glm::mat4(1.0f);
	glm::mat4 body_offset = glm::mat4(1.0f);
	glm::mat4 highlight_offset = glm::mat4(1.0f);
	bool looping = true;
	while (!glfwWindowShouldClose(Global::window) && looping)
	{
		// Clear Window
		glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Handle Inputs
		glfwPollEvents();
		float modified_mouse_x = (float)Global::mouseX / Global::zoom_scale;
		float modified_mouse_y = (float)Global::mouseY / Global::zoom_scale;
		master.updateElement();

		// If Currently Recompiling, Test if Compiling Finished
		if (!looked_at_least_once && std::filesystem::exists(std::filesystem::path(Global::project_scripts_path + "\\..\\compilation complete")))
		{
			// Read Debug Output
			readErrorLog();

			// If Errors Exist, Prompt User
			if (look_at_build)
			{
				// Display Notification
				std::string message = "Build Errors Detected\n\nClick OK to Continue Debugging";
				notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message);

				// Disable Framebuffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				// Disable Depth Test
				glDisable(GL_DEPTH_TEST);

				// Bind ScrollBar
				Global::scroll_bar = &bar;
				glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);

				// Static Object
				Global::colorShaderStatic.Use();
				glUniform1i(Global::staticLocColor, 1);
			}
		}

		// If Escape is Pressed, Exit Loop
		if (Global::Keys[GLFW_KEY_ESCAPE])
		{
			Global::Keys[GLFW_KEY_ESCAPE] = false;
			looping = false;
		}

		// Draw Window
		Global::colorShaderStatic.Use();
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
		glBindVertexArray(windowVAO);
		glDrawArrays(GL_TRIANGLES, 0, 54);
		glBindVertexArray(0);

		// Draw Boxes
		box_open_file.blitzElement();
		box_recompile_project.blitzElement();
		box_exit.blitzElement();

		// Calculate the Translated Model Matrix for Loop Bodies
		float offset = bar.getOffset();
		while (offset > 10.0f)
			offset -= 10.0f;
		body_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, offset, 0.0f));

		// Set Clipping Area
		glScissor(203, 147, 860, 364);

		// Draw Body
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(body_offset));
		glBindVertexArray(bodyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 66);
		glBindVertexArray(0);

		// Draw Highlighter, If Enabled
		if (selected_row != -1)
		{
			highlight_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 18.5f + bar.getOffset() - selected_row * 5.0f, 0.0f));
			glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highlight_offset));
			glBindVertexArray(highlightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}

		// Reset Clipping Area
		glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

		// Draw ScrollBar
		bar.blitzElement();

		// Test Mouse on ScrollBar
		if (!scrolling && bar.TestColloisions())
		{
			if (Global::LeftClick)
			{
				scrolling = true;
				//scoll_offset = bar.BarPosY - modified_mouse_y;
			}
		}

		// Scroll ScrollBar
		else if (scrolling)
		{
			// Deselect ScrollBar if Left Click is Not Held
			if (!Global::LeftClick)
			{
				scrolling = false;
			}

			// Else, Update ScrollBar Position
			else
			{
				bar.Scroll(modified_mouse_y + scoll_offset);
			}
		}

		// Test Mouse Inside Window Body
		else if (modified_mouse_x > -61.0f && modified_mouse_x < 59.0f && modified_mouse_y > -30.0f && modified_mouse_y < 21.0f)
		{
			if (Global::LeftClick)
			{
				// Get Index of Current Row
				Global::LeftClick = false;
				int temp_selected_row = (int)floor((21.0f - modified_mouse_y + bar.getOffset()) / 5.0f);
				if (temp_selected_row >= errors.size())
					temp_selected_row = -1;

				// If Double Click, Open File
				if (temp_selected_row != -1 && temp_selected_row == selected_row)
				{
					typedef void(__stdcall* openVisualStudioFileAt1)(std::string, std::string, int, int);
					(openVisualStudioFileAt1(GetProcAddress(Global::framework_handle, "openVisualStudioFileAtPoint")))(Global::project_solution_path, errors[selected_row].file_path, errors[selected_row].line, errors[selected_row].column);
				}

				// Store Selected Row
				selected_row = temp_selected_row;
			}
		}

		// Test Mouse on Boxes
		else
		{
			// Test Open File Box
			if (box_open_file.updateElement())
			{
				// Disable Left Click
				Global::LeftClick = false;

				typedef void(__stdcall* openVisualStudioFileAt2)(std::string, std::string, int, int);
				(openVisualStudioFileAt2(GetProcAddress(Global::framework_handle, "openVisualStudioFileAtPoint")))(Global::project_solution_path, errors[selected_row].file_path, errors[selected_row].line, errors[selected_row].column);
			}

			// Test Recompile Box
			else if (looked_at_least_once && box_recompile_project.updateElement())
			{
				// Disable Left Click
				Global::LeftClick = false;

				// Recompile Project
				project_selector->recompileProject();
			}

			// Test Exit Box
			else if (box_exit.updateElement())
			{
				// Disable Left Click
				Global::LeftClick = false;

				// Stop Looping
				looping = false;
			}
		}

		// Draw Global Text
		Global::fontShader.Use();
		Source::Fonts::renderText("Debugger", -62.0f, 30.0f, 5.52f, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), true);
		Source::Fonts::renderText("Type", -59.0f, 22.5f, 1.68f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		Source::Fonts::renderText("Code", -45.5f, 22.5f, 1.68f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		Source::Fonts::renderText("Message", -9.0f, 22.5f, 1.68f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		Source::Fonts::renderText("File", 35.0f, 22.5f, 1.68f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		Source::Fonts::renderText("Line", 52.5f, 22.5f, 1.68f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), true);
		if (!looked_at_least_once)
			Source::Fonts::renderText("Compiling", -60.0f, -35.0f, 2.88f, glm::vec4(0.0f, 0.9f, 0.9f, 1.0f), true);
		else if (look_at_build)
			Source::Fonts::renderText("Errors: " + std::to_string(errors.size()), -60.0f, -35.0f, 2.88f, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), true);
		else
			Source::Fonts::renderText("No Errors :)", -60.0f, -35.0f, 2.88f, glm::vec4(0.0f, 0.9f, 0.0f, 1.0f), true);

		// Set Clipping Area
		glScissor(203, 147, 860, 364);

		// Draw Error Text
		float y_offset = 18.0f + bar.getOffset();
		for (int i = 0; i < errors.size(); i++)
		{
			Source::Fonts::renderText(error_map[errors[i].error_type], -60.0f, y_offset, 1.68f, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), true);
			Source::Fonts::renderText(errors[i].error_name, -47.5f, y_offset, 1.68f, glm::vec4(0.0f, 0.0f, 0.7f, 1.0f), true);
			Source::Fonts::renderText(errors[i].error_message, -33.0f, y_offset, 1.32f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), true);
			Source::Fonts::renderText(errors[i].file_name, 26.0f, y_offset, 1.32f, glm::vec4(0.2f, 0.2f, 0.5f, 1.0f), true);
			Source::Fonts::renderText(std::to_string(errors[i].line), 52.5f, y_offset, 1.68f, glm::vec4(0.7f, 0.7f, 0.0f, 1.0f), true);
			y_offset -= 5.0f;
		}

		// Reset Clipping Area
		glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

		// Draw Boxe Text
		box_open_file.blitzText();
		box_recompile_project.blitzText();

		// Swap Buffer
		glfwSwapBuffers(Global::window);
		glFinish();
	}

	// Unstatic Project
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, 0);

	// Unbind ScrolBar
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);
}

void Editor::Debugger::drawCompilerStatus()
{
	// Four Things Could be Drawn:
	// 1: Currently Compiling (Show Animation for Compiling such as a rotating arrow) (Color Should be Blue)
	// 2: Compilation Complete (--COMPILATION COMPLETE--) (Color Should be Green)
	// 3: Compilation Failed (--COMPILATION FAILED--) (Color Should be Red)
	// 4: Currently Running (--CURRENTLY EXECUTING--) (Color Should be Pink)

	// Bind Font Shader
	Global::fontShader.Use();

	// Currently Running
	if (!Global::editing)
	{
		Source::Fonts::renderText("--CURRENTLY EXECUTING--", -89.0f, 47.0f, 1.8f, glm::vec4(0.9f, 0.0f, 0.9f, 1.0f), true);
	}

	// Currently Compiling
	else if (!looked_at_least_once)
	{
		Source::Fonts::renderText("--CURRENTLY COMPILING--", -89.0f, 47.0f, 1.8f, glm::vec4(0.0f, 0.9f, 0.9f, 1.0f), true);
	}

	// Compiler Error
	else if (look_at_build)
	{
		Source::Fonts::renderText("--COMPILATION FAILED--", -89.0f, 47.0f, 1.8f, glm::vec4(0.8f, 0.0f, 0.0f, 1.0f), true);
	}

	// Compiler Success
	else
	{
		Source::Fonts::renderText("--COMPILATION COMPLETE--", -89.0f, 47.0f, 1.8f, glm::vec4(0.0f, 0.8f, 0.0f, 1.0f), true);
	}
}

void Editor::Debugger::getTextPosition(std::string& header_string, glm::vec2& position)
{
	// Get the Level Coordinates of the Position
	glm::i16vec2 level_pos = glm::vec2(0.0f);
	level_pos.x = floor(position.x / level_width);
	level_pos.y = floor(position.y / level_height);

	// Clear the String
	header_string.clear();

	// Generate the New String
	header_string += std::to_string(position.x) + ", " + std::to_string(position.y) + " [" + std::to_string(level_pos.x) + ", " + std::to_string(level_pos.y) + "]";
}

void Editor::Debugger::drawHeader()
{
	// Things That Will be Drawn:
	// Compiler Status
	// Camera Position
	// Mouse Position
	// Level / Scene Name
	// Object Count

	// Possible Future Idea:
	// Multiple States That Determine What Will be Shown
	// States are Cycled Through a Keybind

	// Draw the Compiler Status
	drawCompilerStatus();

	// Draw Camera Position
	Source::Fonts::renderText("Camera:", -89.0f, 44.0f, 1.8f, glm::vec4(0.0f, 0.0f, 0.95f, 1.0f), true);
	Source::Fonts::renderText(header_cam_pos, -79.0f, 44.0f, 1.8f, glm::vec4(0.0f, 0.0f, 0.85f, 1.0f), true);

	// Draw Mouse Position
	Source::Fonts::renderText("Mouse:", -89.0f, 41.0f, 1.8f, glm::vec4(0.0f, 0.85f, 0.85f, 1.0f), true);
	Source::Fonts::renderText(header_mouse_pos, -80.5f, 41.0f, 1.8f, glm::vec4(0.0f, 0.75f, 0.75f, 1.0f), true);

	// Draw Object Count
	Source::Fonts::renderText("Object Count:", -89.0f, 38.0f, 1.8f, glm::vec4(0.85f, 0.0f, 0.85f, 1.0f), true);
	Source::Fonts::renderText(header_object_count, -74.0f, 38.0f, 1.8f, glm::vec4(0.75f, 0.0f, 0.75f, 1.0f), true);

	// Draw Project Path
	Source::Fonts::renderText("Project Path:", -89.0f, -49.0f, 1.8f, glm::vec4(0.0f, 0.35f, 0.35f, 1.0f), true);
	Source::Fonts::renderText(Global::project_file_path, -75.0f, -49.0f, 1.8f, glm::vec4(0.0f, 0.25f, 0.25f, 1.0f), true);

	// Draw Project Name
	Source::Fonts::renderText("Project Name:", -89.0f, -46.0f, 1.8f, glm::vec4(0.0f, 0.35f, 0.0f, 1.0f), true);
	Source::Fonts::renderText(Global::project_name , -73.5f, -46.0f, 1.8f, glm::vec4(0.0f, 0.25f, 0.0f, 1.0f), true);

	// Draw Scene Name
	Source::Fonts::renderText("Scene Name:", -89.0f, -43.0f, 1.8f, glm::vec4(0.35f, 0.0f, 0.0f, 1.0f), true);
	Source::Fonts::renderText(scene_controller->getCurrentInstanceName(), -75.0f, -43.0f, 1.8f, glm::vec4(0.25f, 0.0f, 0.0f, 1.0f), true);
}

void Editor::Debugger::setCameraPosition(glm::vec2 position)
{
	getTextPosition(header_cam_pos, position);
}

void Editor::Debugger::setMousePosition(glm::vec2 position)
{
	// Wrap Mouse Coordinates First
	if (main_level->getContainerType() == Render::CONTAINER_TYPES::LEVEL)
		static_cast<Render::Objects::Level*>(main_level)->wrapObjectPos(position);
	getTextPosition(header_mouse_pos, position);
}

void Editor::Debugger::cycleHeaderState()
{
	header_state++;
	if (header_state == 2)
		header_state = 0;
}

void Editor::Debugger::storeLevelPositions(float width, float height)
{
	level_width = width;
	level_height = height;
}

void Editor::Debugger::storeObjectCount(int count)
{
	header_object_count = std::to_string(count);
}

bool Editor::Debugger::returnIfLookedOver()
{
	return looked_at_least_once;
}

bool Editor::Debugger::returnNoErrors()
{
	return !look_at_build;
}

void Editor::Debugger::storeCurrentLevel(Render::Container* level)
{
	main_level = level;
}

Editor::Debugger Editor::Debugger::instance;
