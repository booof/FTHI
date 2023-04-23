#include "ScriptWizard.h"
#include "Globals.h"
#include "Render/Shader/Shader.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Loaders/Textures.h"
#include "Source/Events/EventListeners.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Rendering/Initialize.h"
#include "Render/Editor/Notification.h"
#include "Source/Events/EventListeners.h"
#include "Render/GUI/AdvancedString.h"
#include "Render/GUI/SelectedText.h"

// Replaces Backslashes With Forwardslashes
std::string backToForwardShash(std::string& input)
{
    // Copy Input Into Result
    std::string result = input;

    // Iterate Through Result And Swap Slashes
    for (int i = 0; i < result.size(); i++)
    {
        if (result[i] == '\\')
            result[i] = '/';
    }

    // Return Result
    return result;
}

// Function to Compare Strings
// 0 = Less, 1 = Equal, 2 = Greater
uint8_t compareString(std::string& string1, std::string& string2)
{
    // Retrun Less Than if Size of String1 is Less Than Size of String2
    if (string1.size() < string2.size())
        return 0;

    // Return Greater Than if Size of String1 is Greater Than Size of String2
    if (string1.size() > string2.size())
        return 2;

    // Compare the Letters in the Strings
    uint8_t temp = 0;
    for (int i = 0; i < string1.size(); i++)
    {
        temp = 1 - (string1[i] < string2[i]) + (string1[i] > string2[i]);
        if (temp)
            return temp;
    }

    // Return Equal Since The Strings Match
    return 0;
}

void Editor::ScriptWizard::initializeScriptWizard()
{
    // Window Object

    // Generate the Window Vertex Object
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);

    // Bind Window Vertex Object
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    // Allocate Memory
    glBufferData(GL_ARRAY_BUFFER, 1848, NULL, GL_STATIC_DRAW);
    
    // Generate Outline Vertices
    float vertices[42];
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 121.0f, 78.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

    // Generate Background Vertices
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 120.0f, 77.0f, glm::vec4(0.72f, 0.72f, 0.72f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

    // Generate Secondary Outline Vertices
    Vertices::Rectangle::genRectColor(-1.0f, -4.0f, -1.0f, 111.0f, 56.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 336, 168, vertices);
    
    // Generate Tab Outline Vertices
    Vertices::Rectangle::genRectColor(-32.0f, 26.0f, -1.0f, 38.0f, 4.5f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 504, 168, vertices);

    // Generate Secondary Background Vertices
    Vertices::Rectangle::genRectColor(-1.0f, -4.0f, -1.0f, 110.0f, 55.0f, glm::vec4(0.87f, 0.87f, 0.87f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 672, 168, vertices);

    // Generate Files Tab Vertices
    Vertices::Rectangle::genRectColor(-44.5f, 25.75f, -1.0f, 12.0f, 4.0f, glm::vec4(0.87f, 0.87f, 0.87f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 840, 168, vertices);

    // Generate Global Tab Vertices
    Vertices::Rectangle::genRectColor(-32.0f, 25.75f, -1.0f, 12.0f, 4.0f, glm::vec4(0.87f, 0.87f, 0.87f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 1008, 168, vertices);

    // Generate Object Tab Vertices
    Vertices::Rectangle::genRectColor(-19.5f, 25.75f, -1.0f, 12.0f, 4.0f, glm::vec4(0.87f, 0.87f, 0.87f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 1176, 168, vertices);

    // Generate Files Tab Highlight Vertices
    Vertices::Rectangle::genRectColor(-44.5f, 25.75f, -1.0f, 11.0f, 3.0f, glm::vec4(0.95f, 0.95f, 0.95f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 1344, 168, vertices);

    // Generate Global Tab Highlight Vertices
    Vertices::Rectangle::genRectColor(-32.0f, 25.75f, -1.0f, 11.0f, 3.0f, glm::vec4(0.95f, 0.95f, 0.95f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 1512, 168, vertices);

    // Generate Object Tab Highlight Vertices
    Vertices::Rectangle::genRectColor(-19.5f, 25.75f, -1.0f, 11.0f, 3.0f, glm::vec4(0.95f, 0.95f, 0.95f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 1680, 168, vertices);

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Vertices
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    
    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Body Object

    // Generate the Single String Vertex Object
    glGenVertexArrays(1, &bodyVAO);
    glGenBuffers(1, &bodyVBO);

    // Bind the Single String Vertex Object
    glBindVertexArray(bodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);

    // Allocate Memory
    glBufferData(GL_ARRAY_BUFFER, 4872, NULL, GL_STATIC_DRAW);

    // Generate Horizontal Lines
    for (float i = 0, j = 18.0f; i < 3360; i += 336, j -= 6.0f)
    {
        Vertices::Rectangle::genRectColor(-1.0f, j, -1.0f, 110.0f, 1.3f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, (GLsizeiptr)i, 168, vertices);
        Vertices::Rectangle::genRectColor(-1.0f, j, -1.0f, 110.0f, 1.0f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, (GLsizeiptr)i + 168, 168, vertices);
    }

    // Generate the Number Background
    Vertices::Rectangle::genRectColor(-54.0f, -6.0f, -1.0f, 4.0f, 65.0f, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3360, 168, vertices);
     
    // Generate Single Vertical Line
    Vertices::Rectangle::genRectColor(-27.0f, -6.0f, -1.0f, 1.3f, 65.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3528, 168, vertices);
    Vertices::Rectangle::genRectColor(-27.0f, -6.0f, -1.0f, 1.0f, 65.0f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 3696, 168, vertices);

    // Generate Multiple Vertical Lines
    for (float i = 3864, j = -26.0f; i < 4872; i += 336, j += 27.5f)
    {
        Vertices::Rectangle::genRectColor(j, -6.0f, -1.0f, 1.3f, 65.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)i, 168, vertices);
        Vertices::Rectangle::genRectColor(j, -6.0f, -1.0f, 1.0f, 65.0f, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)i + 168, 168, vertices);
    }

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Vertices
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Highlight Object

    // Generate the Single String Vertex Object
    glGenVertexArrays(1, &highlightVAO);
    glGenBuffers(1, &highlightVBO);

    // Bind the Single String Vertex Object
    glBindVertexArray(highlightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, highlightVBO);

    // Allocate Memory
    glBufferData(GL_ARRAY_BUFFER, 672, NULL, GL_STATIC_DRAW);

    // Generate Global Highlighter
    Vertices::Rectangle::genRectColor(1.0f, 0.0f, -1.0f, 105.0f, 4.0f, glm::vec4(0.7f, 0.7f, 0.7f, 0.7f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

    // Generate Object Highlighter
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 25.0f, 4.0f, glm::vec4(0.7f, 0.7f, 0.7f, 0.7f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

    // Generate File Name Highlighter
    Vertices::Rectangle::genRectColor(-39.75f, 0.0f, -1.0f, 23.0f, 4.0f, glm::vec4(0.7f, 0.7f, 0.7f, 0.7f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 336, 168, vertices);

    // Generate File Path Highlighter
    Vertices::Rectangle::genRectColor(13.75f, 0.0f, -1.0f, 79.0f, 4.0f, glm::vec4(0.7f, 0.7f, 0.7f, 0.7f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 504, 168, vertices);

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Vertices
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generate Master Element
    master = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 100.0f, 80.0f);

    // Generate ScrollBar
    bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 100.0f, 0.0f);

    // Generate Add Instance Box
    GUI::BoxData temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::NULL_BOX, -46.5f, -35.5f, -1.0f, 20.0f, 5.0, true, "Add", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 0.7f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    box_add_instance = GUI::Box(temp_box_data);

    // Generate Remove Instance Box
    temp_box_data.position.x += 23.0f;
    temp_box_data.button_text = GUI::AdvancedString("Remove");
    box_remove_instance = GUI::Box(temp_box_data);

    // Generate Load Instance Box
    temp_box_data.position.x += 23.0f;
    temp_box_data.button_text = GUI::AdvancedString("Load");
    box_load_instance = GUI::Box(temp_box_data);

    // Generate Modify Instance Box
    temp_box_data.button_text = GUI::AdvancedString("Modify");
    box_modify_instance = GUI::Box(temp_box_data);

    // Generate Move Instance Box
    temp_box_data.position.x += 23.0f;
    temp_box_data.button_text = GUI::AdvancedString("Move");
    box_move_instance = GUI::Box(temp_box_data);

    // Generate Open Instance Box
    temp_box_data.position.x += 23.0f;
    temp_box_data.button_text = GUI::AdvancedString("Open");
    box_open_instance = GUI::Box(temp_box_data);

    // Generate Exit Box
    temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::NULL_BOX, 56.0f, 35.5f, -1.0f, 4.0f, 4.0, true, "", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 0.7f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    box_exit = GUI::Box(temp_box_data);

    // Generate Edit Box
    temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::GENERAL_TEXT_BOX, 0.0f, 0.f, -1.0f, 75.0f, 5.0, true, "", glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 0.7f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    box_edit_value = GUI::Box(temp_box_data);
}

void Editor::ScriptWizard::updateScriptWizard()
{
	// Loop Vars
	int numbers_size = 1;
    selected_row = -1;
	selected_column = -1;
    int highlighter_index = -1;
    glm::mat4 highlighter_offset = glm::mat4(1.0f);

	// Set Loop Settings

	// File Loop
	if (current_loop == 0)
	{
		bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * files_size, 0.0f);
		numbers_size = files_size;

	}

	// Global Loop
	else if (current_loop == 1)
	{
		bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * globals_size, 0.0f);
		numbers_size = globals_size;
	}

	// Object Loop
	else
	{
		bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * objects_size, 0.0f);
		numbers_size = objects_size;
	}

    // Store ScrollBar
    Global::scroll_bar = &bar;
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);

	// Disable Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Disable Depth Test
	glDisable(GL_DEPTH_TEST);

	// Main Loop for Interacting With Script Wizard
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, 1);
	glm::mat4 temp = glm::mat4(1.0f);
	bool looping = true;
    selected_text->assignCloser([this]()->void { this->textCloser(); });
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

        // If Escape is Pressed, Exit Loop
        if (Global::Keys[GLFW_KEY_ESCAPE])
        {
            Global::Keys[GLFW_KEY_ESCAPE] = false;
            looping = false;
        }

		// Reset Mouse Icon
		Global::Selected_Cursor = Global::CURSORS::ARROW;

		// Draw Window
		Global::colorShaderStatic.Use();
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
		glBindVertexArray(windowVAO);
		glDrawArrays(GL_TRIANGLES, 0, 48);

		// Draw Tab Highlighter
		glDrawArrays(GL_TRIANGLES, 48 + 6 * current_loop, 6);
		glBindVertexArray(0);

		// Draw Loop Specific Objects

		// Calculate the Translated Model Matrix for Loop Bodies
		float offset = bar.BarOffset;
		while (offset > 5.5f)
			offset -= 6.0f;
		glm::mat4 body_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, offset, 0.0f));
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(body_offset));

		// Set Clipping Area
		glScissor((GLint)242, (GLint)133, (GLsizei)782, (GLsizei)396);

		// Draw Horizontal Section of Body
		glBindVertexArray(bodyVAO);
		glDrawArrays(GL_TRIANGLES, 0, 126);

        // Draw Vertical Line for File Loop
        if (current_loop == 0)
        {
            glDrawArrays(GL_TRIANGLES, 126, 12);
        }

        // Draw Vertcial Lines for Object Loop
        if (current_loop == 2)
        {
            glDrawArrays(GL_TRIANGLES, 138, 36);
        }

		// Unbind Body of Window
		glBindVertexArray(0);

        // Draw Highlighter, If Highlighting
        if (highlighter_index != -1)
        {
            glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highlighter_offset));
            glBindVertexArray(highlightVAO);
            glDrawArrays(GL_TRIANGLES, 6 * highlighter_index, 6);
            glBindVertexArray(0);
        }

		// Reset Clipping Area
		glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

		// Draw ScrollBar
		bar.blitzElement();

		// Draw Boxes
		glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
		box_add_instance.blitzElement();
        box_remove_instance.blitzElement();
        if (current_loop == 0)
            box_load_instance.blitzElement();
        else
            box_modify_instance.blitzElement();
        box_move_instance.blitzElement();
        box_open_instance.blitzElement();
        box_exit.blitzElement();

		// Edit Text of Object if a TextBox is Selected
		if (selected_text->isActive())
		{
            // Set Modified Flag
            modified = true;
		}

		// Test Mouse on ScrollBar
		else if (!scrolling && bar.TestColloisions())
		{
			if (Global::LeftClick)
			{
				scrolling = true;
				scoll_offset = bar.BarPosY - modified_mouse_y;
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

		// Test Mouse Collisions on Tabs
		else if (modified_mouse_x > -51.0f && modified_mouse_x < -13.0f && modified_mouse_y > 23.75f && modified_mouse_y < 28.25f)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If Left Click is Pressed, Change Current Loop
			if (Global::LeftClick)
			{
				Global::LeftClick = false;
				current_loop = 4 - (uint8_t)floor((13.0f - modified_mouse_x) / 13);
                selected_row = 0;
                selected_column = 0;
                highlighter_index = -1;

				// Set Loop Settings

				// File Loop
				if (current_loop == 0)
				{
					bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * files_size, 0.0f);
					numbers_size = files_size;

				}

				// Global Loop
				else if (current_loop == 1)
				{
					bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * globals_size, 0.0f);
					numbers_size = globals_size;
				}

				// Object Loop
				else
				{
					bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * objects_size, 0.0f);
					numbers_size = objects_size;
				}

                // Store ScrollBar
                Global::scroll_bar = &bar;
			}
		}

		// Test Mouse Collisions on Body of Window
        else if (modified_mouse_x > -52.0f && modified_mouse_x < 54.5f && modified_mouse_y > -31.5f && modified_mouse_y < 23.5f)
        {
            // Set Cursor to Hand
            Global::Selected_Cursor = Global::CURSORS::HAND;

            // If Left Click, Select Box
            if (Global::LeftClick)
            {
                Global::LeftClick = false;

                // Determine the Row Index
                int temp_new_row = (int)floor((23.5f - modified_mouse_y + bar.BarOffset) / 6.0f);
                if (temp_new_row >= numbers_size)
                    temp_new_row = -1;
                if (current_loop && !temp_new_row)
                    temp_new_row = -1;

                // If Selected Row is Valid, Enable Highlighter
                if (temp_new_row != -1)
                {
                    // File Loop
                    if (current_loop == 0)
                    {
                        highlighter_index = (modified_mouse_x < -27.0f) ? 2 : 3;
                        highlighter_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f - 6.0f * temp_new_row - bar.BarOffset, 0.0f));
                    }

                    // Global Loop
					else if (current_loop == 1)
					{
                        highlighter_index = 0;
						highlighter_offset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 21.0f - 6.0f * temp_new_row - bar.BarOffset, 0.0f));

                        // If Value Was Double-Clicked, Allow Editing of Object
                        if (selected_row == temp_new_row)
                        {
                            box_edit_value.setDataPointer(&globals[selected_row]);
                            box_edit_value.setTrue();
                        }
                    }

                    // Object Loop
                    else
                    {
                        // Enable Highlighter
                        highlighter_index = 1;
                        int temp_new_column = (int)floor((modified_mouse_x + 53.5f + bar.BarOffset) / 27.0f);
                        highlighter_offset = glm::translate(glm::mat4(1.0f), glm::vec3(-39.5f + 27.0f * temp_new_column, 21.0f - 6.0f * temp_new_row - bar.BarOffset, 0.0f));

                        // If Value Was Double-Clicked, Allow Editing of Object
                        if (selected_column == temp_new_column && selected_row == temp_new_row)
                        {
                            // Name
                            if (selected_column == 0)
                            {
                                box_edit_value.setDataPointer(&objects[selected_row].name);
                                box_edit_value.setTrue();
                            }

                            // Initializer
                            else if (selected_column == 1)
                            {
                                box_edit_value.setDataPointer(&objects[selected_row].initializer);
                                box_edit_value.setTrue();
                            }

                            // Update
                            else if (selected_column == 2)
                            {
                                box_edit_value.setDataPointer(&objects[selected_row].update);
                                box_edit_value.setTrue();
                            }

                            // Uninitializer
                            else
                            {
                                box_edit_value.setDataPointer(&objects[selected_row].uninitializer);
                                box_edit_value.setTrue();
                            }
                        }
                        selected_column = temp_new_column;
                    }
                }

                // Else, Turn Off Highlighter
                else
                {
                    highlighter_index = -1;
                }

                // Store Currently Selected Row
                selected_row = temp_new_row;
            }
        }

		// Test Mouse Collisions on Buttons
		else
		{
			// Test Mouse on Add Instance
			if (box_add_instance.updateElement())
			{
				Global::LeftClick = false;

                // File Loop
                if (current_loop == 0)
                {
                    if (addFile())
                    {
                        bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * files_size, 0.0f);
                        numbers_size = files_size;
                    }
                }

                // Global Loop
                else if (current_loop == 1)
                {
                    addGlobal();
                    bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * globals_size, 0.0f);
                    numbers_size = globals_size;
                }

                // Object Loop
                else
                {
                    addObject();
                    bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * objects_size, 0.0f);
                    numbers_size = objects_size;
                }

                // Store ScrollBar
                Global::scroll_bar = &bar;
			}

			// Test Mouse on Remove Instance Box
            if (selected_row != -1 && box_remove_instance.updateElement())
            {

            }

            // If Loop is File Loop, Test Mouse on Load Instance Box
            else if (current_loop == 0 && box_load_instance.updateElement())
            {
                Global::LeftClick = false;
                if (loadFile())
                {
                    bar = GUI::VerticalScrollBar(55.0f, 24.0f, 1.0f, 56.0f, 6.0f * files_size, 0.0f);
                    numbers_size = files_size;
                    Global::scroll_bar = &bar;
                }
            }

            // If Loop is Not File Loop, Test Mouse on Modify Instance Box
            else if (current_loop && selected_row != -1 && box_modify_instance.updateElement())
            {

            }

            // Test Mouse on Move Instance Box
            else if (selected_row != -1 && box_move_instance.updateElement())
            {

            }

            // Test Mouse on Open Instance Box
            else if (selected_row != -1 && box_open_instance.updateElement())
            {
                Global::LeftClick = false;
                openFile(selected_row);
            }

            // Test Mouse on Exit Box
            else if (box_exit.updateElement())
            {
                Global::LeftClick = false;
                looping = false;
            }
		}

        // Draw Global Text
        Global::fontShader.Use();
        Source::Fonts::renderText("Project Wizard", -58.5f, 31.0f, 0.23f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);

		// Set Clipping Area
		glScissor((GLint)242, (GLint)133, (GLsizei)782, (GLsizei)396);

		// Draw Number Labels
        GLfloat j = 20.0f + bar.BarOffset;
		for (int i = 0; i < numbers_size; i++, j -= 6.0f)
		{
			Source::Fonts::renderText(std::to_string(i), -55.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
		}

		// Draw Loop Specific Text

		// File Loop
		if (current_loop == 0)
		{
            j = 20.0f + bar.BarOffset;
			for (int i = 0; i < files_size; i++, j -= 6.0f)
			{
				Source::Fonts::renderText(files[i].name, -50.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
                Source::Fonts::renderText(files[i].path, -25.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
			}
		}

		// Global Loop
		else if (current_loop == 1)
		{
            j = 20.0f + bar.BarOffset;
            for (int i = 0; i < globals_size; i++, j -= 6.0f)
            {
                Source::Fonts::renderText(globals[i], -50.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
            }
		}

		// Object Loop
		else
		{
            j = 20.0f + bar.BarOffset;
            for (int i = 0; i < objects_size; i++, j -= 6.0f)
            {
                Source::Fonts::renderText(objects[i].name, -50.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
                Source::Fonts::renderText(objects[i].initializer, -23.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
                Source::Fonts::renderText(objects[i].update, 3.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
                Source::Fonts::renderText(objects[i].uninitializer, 30.5f, j, 0.05f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true);
            }
		}

		// Reset Clipping Area
		glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

		// Draw Box Text
		box_add_instance.blitzText();
        box_remove_instance.blitzText();
        if (current_loop == 0)
            box_load_instance.blitzText();
        else
            box_modify_instance.blitzText();
        box_move_instance.blitzText();
        box_open_instance.blitzText();

        // If Texting, Draw Text Box
        if (selected_text->isActive())
        {
            Global::colorShaderStatic.Use();
            box_edit_value.blitzElement();
            Global::fontShader.Use();
            box_edit_value.blitzText();
        }

		// Set Mouse Cursor
		Source::Textures::ChangeCursor(Global::window);

		// Swap Buffer
		glfwSwapBuffers(Global::window);
		glFinish();
	}

	// Unstatic Project
	Global::colorShaderStatic.Use();
	glUniform1i(Global::staticLocColor, 0);

    // Reset Scroll Callback
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);

    // Clear Closer
    selected_text->assignCloser(nullptr);
}

void Editor::ScriptWizard::loadScriptData()
{
    // String to Get Lines
    std::string line;

    // The Files to Read
    std::ifstream script_size_file;
    std::ifstream file_list_file;
    std::ifstream global_scripts_file;
    std::ifstream object_scripts_file;
    std::ifstream object_scripts_map_file;

    // The Counter for File Lines
    int line_counter = 0;
    int line_iterator = 0;
    int line_iterator2 = 0;

    // If Previously Initialized, Delete Arrays
    if (initialized)
    {
        delete[] files;
        delete[] globals;
        delete[] objects;
        delete[] object_script_map;
    }

    // Read Script Size File and Allocate Memory
    script_size_file.open(Global::project_scripts_path + "ScriptCounts.lst");
    std::getline(script_size_file, line);
    files_size = Source::Algorithms::Common::convertStringToInt(line);
    files = new FileInstances[files_size];
    std::getline(script_size_file, line);
    globals_size = Source::Algorithms::Common::convertStringToInt(line);
    globals = new std::string[globals_size];
    std::getline(script_size_file, line);
    objects_size = Source::Algorithms::Common::convertStringToInt(line);
    objects = new ObjectScript[objects_size];
    object_script_map = new int[objects_size];
    script_size_file.close();

    // Load Array of Files
    if (files_size)
    {
        file_list_file.open(Global::project_map_path + "FileList.lst");
        line_counter = 0;
        line_iterator2 = 0;
        while (std::getline(file_list_file, line))
        {
            if (line_counter % 2)
            {
                files[line_iterator2].path = line;
                line_iterator2++;
            }

            else
            {
                files[line_iterator2].name = line;
            }
            line_counter++;
        }
        file_list_file.close();
    }

    // Load Global Files
    globals[0] = "null_function";
    if (globals_size > 1)
    {
        global_scripts_file.open(Global::project_scripts_path + "GlobalScripts.h");
        line_counter = 1;
        while (std::getline(global_scripts_file, line))
        {
            line_iterator = 18;
            while (line[line_iterator] != ' ')
                line_iterator++;
            globals[line_counter] = line.substr(18, line_iterator - 18);
            line_counter++;
        }
        global_scripts_file.close();
    }

    // Load Object Scripts
    objects[0].name = "NULL";
    objects[0].initializer = "null_function_object";
    objects[0].update = "null_function_object";
    objects[0].uninitializer = "null_function_object";
    if (objects_size > 1)
    {
        // Read Functions
        object_scripts_file.open(Global::project_scripts_path + "ObjectScripts.h");
        line_counter = 1;
        while (std::getline(object_scripts_file, line))
        {
            // Read Initializer Function
            line_iterator = 23;
            line_iterator2 = 0;
            while (line[line_iterator] != ' ')
                line_iterator++;
            objects[line_counter].initializer = line.substr(23, line_iterator - 23);

            // Read Update Function
            line_iterator += 7;
            line_iterator2 = line_iterator;
            while (line[line_iterator] != ' ')
                line_iterator++;
            objects[line_counter].update = line.substr(line_iterator2, (size_t)line_iterator - line_iterator2);

            // Read Uninitializer Function
            line_iterator += 7;
            line_iterator2 = line_iterator;
            while (line[line_iterator] != ' ')
                line_iterator++;
            objects[line_counter].uninitializer = line.substr(line_iterator2, (size_t)line_iterator - line_iterator2);

            // Increment Line Counter
            line_counter++;
        }
        object_scripts_file.close();

        // Read Name
        object_scripts_map_file.open(Global::project_map_path + "ScriptMap.lst");
        line_counter = 0;
        while (std::getline(object_scripts_map_file, line))
        {
            objects[line_counter].name = line;
            line_counter++;
        }
        object_scripts_map_file.close();
    }

    // Reset Modified Flag
    modified = false;
}

void Editor::ScriptWizard::writeScriptData()
{
    // Files
    std::ofstream script_size_file;
    std::ofstream array_size_file;
    std::ofstream file_list_file;
    std::ofstream global_scripts_header_file;
    std::ofstream global_scripts_file;
    std::ofstream object_scripts_header_file;
    std::ofstream object_scripts_file;
    std::ofstream object_scripts_map_file;

    // Perform Object Script Mapping to All Files, if Object Scripts Were Moved
    if (object_scripts_moved)
    {
        
    }

    // Write Script Sizes
    script_size_file.open(Global::project_scripts_path + "ScriptCounts.lst");
    script_size_file << std::to_string(files_size) << "\n";
    script_size_file << std::to_string(globals_size) << "\n";
    script_size_file << std::to_string(objects_size) << "\n";
    script_size_file.close();

    // Write Array Sizes
    array_size_file.open(Global::project_scripts_path + "ArraySizes.h");
    array_size_file << "const unsigned int GLOBAL_SCRIPT_SIZE = " << std::to_string(globals_size) << ";\n";
    array_size_file << "const unsigned int OBJECT_SCRIPT_SIZE = " << std::to_string(objects_size) << ";\n";
    array_size_file.close();

    // Write Files to File Lists
    file_list_file.open(Global::project_map_path + "FileList.lst");
    for (int i = 0; i < files_size; i++)
    {
        file_list_file << files[i].name << "\n";
        file_list_file << files[i].path << "\n";
    }
    file_list_file.close();

    // Write Files to CMakeLists
    genCMakeList();

    // Write Global Scripts Header
    int temp_global_array_size = globals_size;
    std::string* temp_global_array = new std::string[globals_size];
    for (int i = 0; i < globals_size; i++)
        temp_global_array[i] = globals[i];
    sortScriptHeader(temp_global_array, temp_global_array_size);
    global_scripts_header_file.open(Global::project_scripts_path + "\\GlobalScriptHeader.h");
    global_scripts_header_file << "namespace Object { class Object; }\n";
    for (int i = 0; i < temp_global_array_size; i++)
        global_scripts_header_file << "void " << temp_global_array[i] << "();\n";
    delete[] temp_global_array;

    // Write Global Scripts
    global_scripts_file.open(Global::project_scripts_path + "\\GlobalScripts.h");
    for (int i = 1; i < globals_size; i++)
        global_scripts_file << ", GlobalScript({ &" << globals[i] << " })\n";
        global_scripts_file.close();

    // Write Object Scripts Header
    int temp_object_array_size = objects_size * 3;
    std::string* temp_object_array = new std::string[temp_object_array_size];
    for (int i = 0, j = 0; i < temp_object_array_size; i += 3, j++) {
        temp_object_array[i] = objects[j].initializer;
        temp_object_array[i + 1] = objects[j].update;
        temp_object_array[i + 2] = objects[j].uninitializer;
    }
    sortScriptHeader(temp_object_array, temp_object_array_size);
    object_scripts_header_file.open(Global::project_scripts_path + "\\ObjectScriptHeader.h");
    object_scripts_header_file << "namespace Object { class Object; }\n";
    for (int i = 0; i < temp_object_array_size; i++)
        object_scripts_header_file << "void " << temp_object_array[i] << "(Object::Object* object_pointer);\n";
    delete[] temp_object_array;
    
    // Write Object Scripts
    object_scripts_file.open(Global::project_scripts_path + "\\ObjectScripts.h");
    for (int i = 1; i < objects_size; i++)
        object_scripts_file << ", ObjectScriptGroup({ &" << objects[i].initializer << " }, { &" << objects[i].update << " }, { &" << objects[i].uninitializer << " })\n";
    object_scripts_file.close();

    // Write Object Scripts Map
    object_scripts_map_file.open(Global::project_map_path + "ScriptMap.lst");
    for (int i = 0; i < objects_size; i++)
        object_scripts_map_file << objects[i].name << "\n";
    object_scripts_map_file.close();

    // Reset Modified Flag
    modified = false;
}

void Editor::ScriptWizard::genCMakeList()
{
    // CMake Files
    std::ifstream cmake_read;
    std::ofstream cmake_write;

    // Open Files
    cmake_read.open("../Resources/ProjectCodeTemplates/CMakeLists.txt");
    cmake_write.open(Global::project_scripts_path + "\\..\\CMakeLists.txt");
    std::string line = "";

    // Lambda for Direct, Unmodified Copying of Data Between Files
    auto copy = [&](int n)->void {
        for (; n > 0; n--) {
            std::getline(cmake_read, line);
            cmake_write << line << "\n";
        }
    };

    // Write CMake Version and Declare Project
    copy(2);

    // Store Project Name Determined from Project Selector
    cmake_write << Global::project_name << "\n";
    
    // Close Project, Write C++ Specifications, DLL Specifications, and Declare Project DLL
    copy(5);

    // Copy Source Files
    for (int i = 0; i < files_size; i++)
        cmake_write << "\"" << backToForwardShash(files[i].path) << files[i].name << "\"\n";

    // Close Project DLL, Declare Precompiled Headers
    copy(2);

    // Write Absolute Path to the DLLHeader File
    cmake_write << "\"" << backToForwardShash(Global::engine_path) << "/Core/EngineLibs/Header/DLLHeader.h\"\n";

    // Copy Rest of the Precompiled Headers, Declare the Engine Library
    copy(4);

    // Write the Generators for the Debug/Release Build of the Library
    cmake_write << "\"$<$<CONFIG:Release>:" << backToForwardShash(Global::engine_path) << "/Core/EngineLibs/Code/Build/Release/enginecode.lib>\"\n";
    cmake_write << "\"$<$<CONFIG:Debug>:" << backToForwardShash(Global::engine_path) << "/Core/EngineLibs/Code/Build/Debug/enginecode.lib>\"\n";

    // Copy the Rest of the CMake File
    copy(6);

    // Close the Files
    cmake_write.close();
    cmake_read.close();
}

Editor::ScriptWizard* Editor::ScriptWizard::get()
{
    return &instance;
}

void Editor::ScriptWizard::textCloser()
{
    // If Value is a File Path, Test if Path is Unique
    if (current_loop == 0)
    {
        //highlighter_index == 3;
    }

    // If Value is a Object Script Name, Test if Name is Unique
    if (current_loop == 2 && selected_column == 0)
    {
        std::string& test_text = objects[selected_row].name;
        for (int i = 0; i < objects_size; i++)
        {
            // If Rows Match, Continue
            if (selected_row == i)
                continue;

            // If Values Math, Throw Error
            if (test_text == objects[i].name)
            {
                test_text = selected_text->getBackup();
                std::string message = "REDEFINITION ERROR DETECTED\nValue Is Already Defined\n\nPlease Choose A Different Value\n\nThe Variable Has Been Reverted To Its\nPrevious Value";
                notification_->notificationMessage(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
                Global::colorShaderStatic.Use();
                glUniform1i(Global::staticLocColor, 1);
                break;
            }
        }
    }
}

void Editor::ScriptWizard::sortScriptHeader(std::string* raw_array, int& raw_array_size)
{
    // Sort Array
    uint8_t compare_value = 0;
    for (int i = 0; i < raw_array_size; i++)
    {
        for (int j = 0; j < raw_array_size - 1; j++)
        {
            // Get Comparison Value of Strings
            compare_value = compareString(raw_array[j], raw_array[j + 1]);

            // If Equal, Shift All Proceding Instances Down One Index
            if (compare_value == 1)
            {
                for (int k = j; k < raw_array_size - 1; k++)
                {
                    raw_array[k] = raw_array[k + 1];
                }
                raw_array_size--;
            }

            // If Greater Than, Swap Values
            else if (compare_value == 2)
            {
                std::string temp = raw_array[j + 1];
                raw_array[j + 1] = raw_array[j];
                raw_array[j] = temp;
            }

            // If Less Than, Do Nothing
        }
    }
}

void Editor::ScriptWizard::resizeFile(int new_size)
{
    // Only Resize if New Size is Greater Than Current Size
    if (new_size <= files_size)
        return;

    // Generate New Array
    FileInstances* new_array = new FileInstances[new_size];
    
    // Copy Contents From Old Array Into New Array
    for (int i = 0; i < files_size; i++)
        new_array[i] = files[i];

    // Delete Old Array
    delete[] files;

    // Store New Array and Size
    files = new_array;
    files_size = new_size;
}

bool Editor::ScriptWizard::addFile()
{
    // Generate Explorer Context
    OPENFILENAME file_dialogue = { 0 };
    TCHAR szFile[260] = { 0 };
    TCHAR szTitle[260] = { 0 };
    file_dialogue.lStructSize = sizeof(OPENFILENAME);
    file_dialogue.lpstrFile = szFile;
    file_dialogue.nMaxFile = sizeof(szFile);
    file_dialogue.lpstrFilter = ("C++ File\0*.CPP");
    file_dialogue.nFilterIndex = 1;
    file_dialogue.lpstrFileTitle = szTitle;
    file_dialogue.lpstrTitle = "Select A Valid Cpp File (.cpp)";
    file_dialogue.nMaxFileTitle = sizeof(szTitle);
    file_dialogue.lpstrInitialDir = Global::script_folder_path.c_str();
    file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // If Successful, Add File
    if (GetSaveFileName(&file_dialogue) == TRUE)
    {
        // Reallocate File to Fit New Size
        resizeFile(files_size + 1);

        // Index of File Instance
        int index = files_size - 1;

        // Insert File Into Array
        files[index].name = file_dialogue.lpstrFileTitle;
        files[index].path = file_dialogue.lpstrFile;

        // Generate File
        std::ofstream file;
        file.open(files[index].path + ".cpp");
        file.close();

        // Remove File Name From Path
        files[index].path = files[index].path.substr(0, files[index].path.size() - files[index].name.size());

        // Add .cpp Identifier to File Name
        files[index].name += ".cpp";

        // Set Modified Flag
        modified = true;

        return true;
    }

    return false;
}

void Editor::ScriptWizard::removeFile(int index)
{
    
}

bool Editor::ScriptWizard::loadFile()
{
    // Generate Open File Dialogue
    OPENFILENAME file_dialogue = { 0 };
    TCHAR szFile[260] = { 0 };
    TCHAR szTitle[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    file_dialogue.lStructSize = sizeof(OPENFILENAME);
    //file_dialogue.hwndOwner = hWnd;
    file_dialogue.lpstrFile = szFile;
    file_dialogue.nMaxFile = sizeof(szFile);
    file_dialogue.lpstrFilter = ("C++ File\0*.CPP");
    file_dialogue.nFilterIndex = 1;
    file_dialogue.lpstrFileTitle = szTitle;
    file_dialogue.lpstrTitle = "Select A Valid Source File (.cpp)";
    file_dialogue.nMaxFileTitle = sizeof(szTitle);
    file_dialogue.lpstrInitialDir = Global::script_folder_path.c_str();
    file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // If Successful, Add File
    if (GetOpenFileName(&file_dialogue) == TRUE)
    {
        // Reallocate File to Fit New Size
        resizeFile(files_size + 1);

        // Index of File Instance
        int index = files_size - 1;

        // Insert File Into Array
        files[index].name = file_dialogue.lpstrFileTitle;
        files[index].path = file_dialogue.lpstrFile;

        // Remove File Name From Path
        files[index].path = files[index].path.substr(0, files[index].path.size() - files[index].name.size());

        // Set Modified Flag
        modified = true;

        return true;
    }

    return false;
}

void Editor::ScriptWizard::openFile(int index)
{
    typedef void(__stdcall* openVisualStudioFile)(std::string, std::string);
    (openVisualStudioFile(GetProcAddress(Global::framework_handle, "openVisualStudioFile")))(Global::project_solution_path, files[index].path + files[index].name);
}

void Editor::ScriptWizard::resizeGlobal(int new_size)
{
    // Only Resize if New Size is Greater Than Current Size
    if (new_size <= globals_size)
        return;

    // Generate New Array
    std::string* new_array = new std::string[new_size];

    // Copy Contents From Old Array Into New Array
    for (int i = 0; i < globals_size; i++)
        new_array[i] = globals[i];

    // Delete Old Array
    delete[] globals;

    // Store New Array and Size
    globals = new_array;
    globals_size = new_size;
}

void Editor::ScriptWizard::addGlobal()
{
    // Reallocate File to Fit New Size
    resizeGlobal(globals_size + 1);

    // Store Nullified Data in New Slot
    globals[globals_size - 1] = "null_function";

    // Set Modified Flag
    modified = true;
}

void Editor::ScriptWizard::removeGlobal(int index)
{
}

void Editor::ScriptWizard::openGlobal(int index)
{
}

void Editor::ScriptWizard::resizeObject(int new_size)
{
    // Only Resize if New Size is Greater Than Current Size
    if (new_size <= objects_size)
        return;

    // Generate New Array
    ObjectScript* new_array = new ObjectScript[new_size];

    // Copy Contents From Old Array Into New Array
    for (int i = 0; i < objects_size; i++)
        new_array[i] = objects[i];

    // Delete Old Array
    delete[] objects;

    // Store New Array and Size
    objects = new_array;
    objects_size = new_size;
}

void Editor::ScriptWizard::addObject()
{
    // Reallocate File to Fit New Size
    resizeObject(objects_size + 1);

    // Store Nullified Data in New Slot
    objects[objects_size - 1].name = "Script" + std::to_string(objects_size - 1);
    objects[objects_size - 1].initializer = "null_function_object";
    objects[objects_size - 1].update = "null_function_object";
    objects[objects_size - 1].uninitializer = "null_function_object";

    // Set Modified Flag
    modified = true;
}

void Editor::ScriptWizard::removeObject(int index)
{
}

void Editor::ScriptWizard::openObject(int index, int index2)
{
}

Editor::ScriptWizard Editor::ScriptWizard::instance;
