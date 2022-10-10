#include "EventListeners.h"

// Globals
#include "Globals.h"

//#include "Class/Render/Editor/Selector.h"
//#include "Class/Render/Objects/Level.h"
#include "Class/Render/Editor/EditorOptions.h"
//#include "Class/Render/Camera/Camera.h"

#include "Class/Render/Objects/Level.h"
#include "Class/Render/Editor/Selector.h"
#include "Class/Render/Objects/ChangeController.h"
#include "Class/Render/Editor/ScriptWizard.h"
#include "Class/Render/Editor/ProjectSelector.h"
#include "Class/Render/Editor/Debugger.h"

void Source::Listeners::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Change Viewport
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	// Change Framebuffer Texture Size

	// Core Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Core);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Core);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Core, 0);

	// HDR Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_HDR);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_HDR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_HDR, 0);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Bloom);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Bloom, 0);

	// Multisample Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Multisample);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Global::Frame_Buffer_Texture_Multisample);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA16F, width, height, GL_TRUE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, Global::Frame_Buffer_Texture_Multisample, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, Global::Render_Buffer_Multisample);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Global::Render_Buffer_Multisample);

	// Bloom Framebuffers
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Bloom[i]);
		glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_PingPong[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_PingPong[i], 0);
	}

	// Unbind Framebuffers
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Update ScreenWidth and ScreenHeight
	Global::screenWidth = width;
	Global::screenHeight = height;

	// Calculate ScreenScalar Value
	Global::screenScalar = (float)height / 720.0f;

	// Calculate HalfScalarX Value
	Global::halfScalarX = (width / 14.2f) / Global::screenScalar;

	// Update Projection Matrix
	Global::projection = glm::ortho(-Global::halfScalarX * Global::zoom_scale, Global::halfScalarX * Global::zoom_scale, -50.0f * Global::zoom_scale, 50.0f * Global::zoom_scale, 0.1f, 65.0f);
	Global::projectionStatic = glm::ortho(-Global::halfScalarX, Global::halfScalarX, -50.0f, 50.0f, 0.1f, 100.0f);

	// Update Matrices Uniform Buffer
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(Global::projectionStatic));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Update Static Objects
	Global::framebufferResize = true;

	/*
	// Bind Core Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Core);
	glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_Core);

	// Create Framebuffer Core Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind Core Framebuffer Texture to Core Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame_Buffer_Texture_Core, 0);

	// Check if Core Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Core Framebuffer is not Complete" << std::endl;
	}

	// Bind HDR Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_HDR);

	// Create HDR Framebuffer Texture
	glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_HDR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind HDR Framebuffer Texture to HDR Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame_Buffer_Texture_HDR, 0);

	// Create Bloom Framebuffer Texture
	glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_Bloom);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind Bloom Framebuffer Texture to HDR Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Frame_Buffer_Texture_Bloom, 0);

	// Test if HDR Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Framebuffer is not Complete" << std::endl;
	}

	// Bind Multisample Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Multisample);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Frame_Buffer_Texture_Multisample);
	glBindRenderbuffer(GL_RENDERBUFFER, Render_Buffer_Multisample);

	// Create Multisampled Framebuffer Texture
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA16F, screenWidth, screenHeight, GL_TRUE);

	// Bind Multisampled Framebuffer Texture to Multisampled Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, Frame_Buffer_Texture_Multisample, 0);

	// Create Renderbuffer Object
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);

	// Bind Renderbuffer Object to HDR Framebuffer Object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Render_Buffer_Multisample);

	// Test if Multisampled Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Multisampled Framebuffer is not Complete" << std::endl;
	}

	for (int i = 0; i < 2; i++)
	{
		// Bind Bloom Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Frame_Buffer_Object_Bloom[i]);

		// Create PingPong Texture
		glBindTexture(GL_TEXTURE_2D, Frame_Buffer_Texture_PingPong[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind PingPong Texture to Bloom Framebuffer Object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Frame_Buffer_Texture_PingPong[i], 0);

		// Test if Bloom Framebuffer is Complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER: Bloom Framebuffer is not Complete" << std::endl;
		}
	}


	// Unbind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	*/
}

void Source::Listeners::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Checks Which Keys are Being Pressed
	if (key > 0 && key < 1024)
	{
		// Sets Key in List to Active if it is Currently Being Held Down
		if (action == GLFW_PRESS)
		{
			Global::Keys[key] = true;

#ifdef EDITOR

			// Switch Between Paused and Unpaused
			if (key == GLFW_KEY_P)
			{
				static glm::vec3 old_camera_pos;
				if (Global::paused) {
					Global::paused = false;
					Global::camera_pointer->Position = old_camera_pos;
				}
				else {
					Global::paused = true;
					old_camera_pos = Global::camera_pointer->Position;
				}

			}

			if (!Global::paused && !Global::texting)
			{
				// Switch Between Gameplay and Editor
				if (key == GLFW_KEY_F1)
				{
					project_selector->toggleEngineMode();
				}

				// Editor Functions
				if (Global::editing)
				{
					// Build Current Project
					if (key == GLFW_KEY_F5)
					{
						project_selector->recompileProject();
					}

					// Open Debugger
					if (key == GLFW_KEY_F6)
					{
						debugger->updateWindow();
					}

					// Open Script Wizard
					if (key == GLFW_KEY_F7)
					{
						script_wizard->updateScriptWizard();
					}

					// Open Project Selector
					if (key == GLFW_KEY_F8)
					{
						project_selector->select_project();
					}
				}

				// Enable Frame by Frame
				if (key == GLFW_KEY_UP && !Global::editing)
				{
					Global::Keys[key] = false;
					Global::frame_by_frame = true;
					Global::frame_step = false;
					Global::frame_run = false;
				}
			}

#endif

		}

		// Deactivates Key if no Longer Held
		else if (action == GLFW_RELEASE)
		{
			Global::Keys[key] = false;

#ifdef EDITOR

			if (key == GLFW_KEY_RIGHT && !Global::editing)
			{
				Global::frame_run = false;
			}

#endif

		}
	}
}

void Source::Listeners::TypeCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// If Escape is Pressed, Revert to Original Text and Stop Texting
	if (key == GLFW_KEY_ESCAPE)
	{
		*Global::text = Global::initial_text;
		Global::texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		Global::text_box->setFalse();
		Global::stoped_texting = true;
		return;
	}
	
	// If Enter is Pressed, Stop Texting
	if (key == GLFW_KEY_ENTER)
	{
		Global::texting = false;
		glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
		Global::text_box->setFalse();
		Global::stoped_texting = true;
		return;
	}

	// Type Only if Key is Pressed or Being Held Down
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		// A Placeholder for the Pressed Key
		char character;

		// Test if Index is to be Shifted to the Left
		if (key == GLFW_KEY_LEFT)
		{
			// Prevent Index from Goint Negative
			if (Global::textIndex != 0)
			{
				Global::textIndex--;
			}
		}

		// Test if Index is to be Shited to the Right
		else if (key == GLFW_KEY_RIGHT)
		{
			// Prevent Index from Going Past Length of String
			if (Global::textIndex < Global::text->size())
			{
				Global::textIndex++;
			}
		}

		// Test if Index is Supposed to be First
		else if (key == GLFW_KEY_UP)
		{
			Global::textIndex = 0;
		}

		// Test if Index is Supposed to be Last
		else if (key == GLFW_KEY_DOWN)
		{
			Global::textIndex = (int)Global::text->size();
		}

		// Enter Key is Pressed
		else if (key == GLFW_KEY_ENTER)
		{
			Global::Keys[GLFW_KEY_ENTER] = true;
		}

		// Makes Shure Certain Modifers and / or Keys Dont Affect Text
		else if (key != GLFW_KEY_LEFT_SHIFT)
		{
			// Handle Inputs if Shift Modefier is 
			if (mode == GLFW_MOD_SHIFT)
			{
				// Clears Text if Shift and Backspace are Held
				if (key == GLFW_KEY_BACKSPACE)
				{
					Global::text->clear();
					return;
				}

				// Shift Number Keys
				else if (key > 47 && key < 58)
				{
					character = key - 15;
				}

				// UnderScore
				else if (key == 45)
				{
					character = 95;
				}

				// Character is Capitalized by Default
				else
				{
					character = key;
				}
			}

			// Handle Inputs That Have no Modifier
			else
			{
				// Deletes Previous Character if Backspace is Held
				if (key == GLFW_KEY_BACKSPACE)
				{
					// Delete Character Only if There is a Character to Delete
					if (!Global::text->empty())
					{
						Global::text->pop_back();
					}

					return;
				}

				// Make Letter Characters Lowercase
				if (key > 64 && key < 91)
				{
					character = key + 32;
				}

				// Default Character
				else
				{
					character = key;
				}
			}

			// Append Character Based on Current Mode
			switch (Global::textModifier)
			{

			// All Characters
			case 0:
			{
				*Global::text += character;
				break;
			}

			// Alphabetical
			case 1:
			{
				// Character is a Capitol or Lowercase Letter
				if ((character > 64 && character < 91) || (character > 96 && character < 123))
				{
					*Global::text += character;
				}

				break;
			}

			// Numerical
			case 2:
			{
				// Character is a Number or Decimal
				if ((character > 47 && character < 58) || character == 46)
				{
					*Global::text += character;
				}

				// Character is a Sign
				else if (character == 45)
				{
					// Remove Sign if it Exists
					if (!Global::text->empty() && Global::text->at(0) == '-')
					{
						Global::text->erase(0, 1);
					}

					// Add Sign if it Doesn't Exist
					else
					{
						Global::text->insert(0, "-");
					}
				}

				break;
			}

			// Absolute Numerical
			case 3:
			{
				// Character is Number or Decimal
				if ((character > 47 && character < 58) || character == 46)
				{
					*Global::text += character;
				}

				break;
			}

			// Integer
			case 4:
			{
				// Character is Number
				if (character > 47 && character < 58)
				{
					*Global::text += character;
				}

				// Character is a Sign
				else if (character == 45)
				{
					// Remove Sign if it Exists
					if (!Global::text->empty() && Global::text->at(0) == '-')
					{
						Global::text->erase(0, 1);
					}

					// Add Sign if it Doesn't Exist
					else
					{
						Global::text->insert(0, "-");
					}
				}

				break;
			}

			// Absolute Integer
			case 5:
			{
				// Character is Number
				if (character > 47 && character < 58)
				{
					*Global::text += character;
				}

				break;
			}

			}
		}
	}
}

void Source::Listeners::CursorCallback(GLFWwindow* window, double xPos, double yPos)
{
	// Show That the Cursor Updated
	Global::cursor_Move = true;

	// Calculate Local Screen Mouse Coordinates
	double temp_mouseX = (double)(xPos - Global::screenWidth * 0.5f) / Global::screenHeight * 101 * Global::zoom_scale;
	double temp_mouseY = (double)(Global::screenHeight * 0.5f - yPos) / Global::screenHeight * 100 * Global::zoom_scale;

	// Calculate Change in Mouse Positions
	Global::deltaMouse = glm::vec2(temp_mouseX - Global::mouseX, temp_mouseY - Global::mouseY);

	// Store New Mouse Position
	Global::mouseX = temp_mouseX;
	Global::mouseY = temp_mouseY;

	//std::cout << Global::mouseX << " " << Global::mouseY << "\n";
}

void Source::Listeners::ScrollCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	// Update Zoom Scale
	Global::zoom_scale -= (float)(offsetY * 0.01);
	Global::zoom = true;

	if (Global::zoom_scale < 0.01f)
	{
		Global::zoom_scale = 0.01f;
	}

	// Update Projection Matrix
	Global::projection = glm::ortho(-Global::halfScalarX * Global::zoom_scale, Global::halfScalarX * Global::zoom_scale, -50.0f * Global::zoom_scale, 50.0f * Global::zoom_scale, 0.1f, 100.0f);

	// Update Matrices Uniform Buffer
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Source::Listeners::ScrollBarCallback(GLFWwindow* window, double offsetX, double offsetY)
{
	// Move ScrollBar
	if (Global::scroll_bar != nullptr)
	{
		Global::scroll_bar->Scroll(Global::scroll_bar->BarPosY + (float)offsetY * Global::editor_options->option_scroll_speed);
	}
}

void Source::Listeners::MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (button == 0)
		{
			Global::LeftClick = true;
		}

		if (button == 1)
		{
			Global::RightClick = true;
		}
	}

	else if (action == GLFW_RELEASE)
	{
		if (button == 0)
		{
			Global::LeftClick = false;
		}

		if (button == 1)
		{
			Global::RightClick = false;
		}
	}
}

#ifdef EDITOR

void Source::Listeners::FrameByFrameCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Checks Which Keys are Being Pressed
	if (key > 0 && key < 1024)
	{
		// Sets Key in List to Active if it is Currently Being Held Down
		if (action == GLFW_PRESS)
		{
			Global::Keys[key] = true;

			// Disable Frame by Frame
			if (key == GLFW_KEY_UP)
			{
				Global::frame_by_frame = false;
				Global::frame_step = false;
				Global::frame_run = false;
				Global::Keys[key] = false;
			}

			// Step By One Frame
			else if (key == GLFW_KEY_DOWN)
			{
				Global::frame_step = true;
				Global::Keys[key] = false;
			}

			// Enable Frame Run
			else if (key == GLFW_KEY_RIGHT)
			{
				Global::frame_run = true;
				Global::Keys[key] = false;
			}
		}

		else if (action == GLFW_RELEASE)
		{
			Global::Keys[key] = false;
		}
	}
}

void APIENTRY Source::Listeners::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131154) return;

	// Get ID of Error
	std::string string_id;
	switch (id)
	{
	case GL_INVALID_ENUM:                  string_id = "INVALID_ENUM"; break;
	case GL_INVALID_VALUE:                 string_id = "INVALID_VALUE"; break;
	case GL_INVALID_OPERATION:             string_id = "INVALID_OPERATION"; break;
	case GL_STACK_OVERFLOW:                string_id = "STACK_OVERFLOW"; break;
	case GL_STACK_UNDERFLOW:               string_id = "STACK_UNDERFLOW"; break;
	case GL_OUT_OF_MEMORY:                 string_id = "OUT_OF_MEMORY"; break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: string_id = "INVALID_FRAMEBUFFER_OPERATION"; break;
	}

	// Get Source of Error
	std::string string_source;
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:              string_source = "GL_DEBUG_SOURCE_API";              break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:    string_source = "GL_DEBUG_SOURCE_WINDOW_SYSTEM";    break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:  string_source = "GL_DEBUG_SOURCE_SHADER_COMPILER";  break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:      string_source = "GL_DEBUG_SOURCE_THIRD_PARTY";      break;
	case GL_DEBUG_SOURCE_APPLICATION:      string_source = "GL_DEBUG_SOURCE_APPLICATION";      break;
	case GL_DEBUG_SOURCE_OTHER:	           string_source = "GL_DEBUG_SOURCE_OTHER";            break;
	}

	// Get Type of Error
	std::string string_type;
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:                string_type = "GL_DEBUG_TYPE_ERROR";                break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:  string_type = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";  break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:   string_type = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";   break;
	case GL_DEBUG_TYPE_PORTABILITY:          string_type = "GL_DEBUG_TYPE_PORTABILITY";          break;
	case GL_DEBUG_TYPE_PERFORMANCE:          string_type = "GL_DEBUG_TYPE_PERFORMANCE";          break;
	case GL_DEBUG_TYPE_MARKER:               string_type = "GL_DEBUG_TYPE_MARKER";               break;
	case GL_DEBUG_TYPE_PUSH_GROUP:           string_type = "GL_DEBUG_TYPE_PUSH_GROUP";           break;
	case GL_DEBUG_TYPE_POP_GROUP:            string_type = "GL_DEBUG_TYPE_POP_GROUP";            break;
	case GL_DEBUG_TYPE_OTHER:                string_type = "GL_DEBUG_TYPE_OTHER";                break;
	}

	// Get Severity of Error
	std::string string_severity;
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:          string_severity = "GL_DEBUG_SEVERITY_HIGH";         break;
	case GL_DEBUG_SEVERITY_MEDIUM:        string_severity = "GL_DEBUG_SEVERITY_MEDIUM";       break;
	case GL_DEBUG_SEVERITY_LOW:           string_severity = "GL_DEBUG_SEVERITY_LOW";          break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:  string_severity = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;
	}

	// Pring Message
	std::cout << "\n" << "OPENGL ERROR:\n"
		<< "ID:        " << string_id << " " << id << "\n"
		<< "SOURCE:    " << string_source << "\n"
		<< "TYPE:      " << string_type << "\n"
		<< "SEVERITY:  " << string_severity << "\n"
		<< message << "\n \n";
}

void Source::Listeners::SmoothKeyCallback_Editor(Render::Camera::Camera& camera, Editor::Selector& selector, Render::Objects::Level& level, glm::vec4& acceleration_timer)
{
	// ALT Modifiers
	if (Global::Keys[GLFW_KEY_LEFT_ALT] || Global::Keys[GLFW_KEY_RIGHT_ALT])
	{
		// WASD Moves by Very Small Amount Determined in Editor Window
		// No Acceleration Timer

		return;
	}

	// CTRL Modifiers
	if (Global::Keys[GLFW_KEY_LEFT_CONTROL] || Global::Keys[GLFW_KEY_RIGHT_CONTROL])
	{
		// WASD Moves By X Levels Where X is Defined as Level Stride in Editor Options
		// No Acceleration Timer

		// Undo Change
		if (Global::Keys[GLFW_KEY_Z])
		{
			change_controller->undo();
			Global::Keys[GLFW_KEY_Z] = false;
		}

		// Redo Change
		if (Global::Keys[GLFW_KEY_Y])
		{
			change_controller->redo();
			Global::Keys[GLFW_KEY_Y] = false;
		}

		// Reset Level to Default State
		if (Global::Keys[GLFW_KEY_R])
		{

		}

		return;
	}

	// SHIFT Modifiers
	if (Global::Keys[GLFW_KEY_LEFT_SHIFT] || Global::Keys[GLFW_KEY_RIGHT_SHIFT])
	{
		// WASD Moves By a Single Level
		// No Acceleration Timer

		// Open ScriptWizard
		if (Global::Keys[GLFW_KEY_Y])
		{
			Global::Keys[GLFW_KEY_Y] = false;
			script_wizard->updateScriptWizard();
		}

		// Open Debugger
		if (Global::Keys[GLFW_KEY_U])
		{
			Global::Keys[GLFW_KEY_U] = false;
			debugger->updateWindow();
		}

		return;
	}

	// No Modifiers

	// Disable Editing Window && / || Unselect an Object if Escape is Held
	if (Global::Keys[GLFW_KEY_ESCAPE])
	{
		// Quit Window if Active
		if (selector.active_window)
		{
			// Window Mode is New Object
			if (selector.editing_mode == EDITING_MODES::NEW_OBJECT)
			{
				if (!selector.traverseBackNewObject())
				{
					selector.active = false;
					selector.active_window = false;
					glfwSetScrollCallback(Global::window, ScrollCallback);
				}
			}

			// Window Mode is Edit Object
			else if (selector.editing_mode == EDITING_MODES::EDIT_OBJECT)
			{
				selector.active_window = false;
				glfwSetScrollCallback(Global::window, ScrollCallback);
			}
		}

		// Stop Editing Object
		else if (selector.active)
		{
			selector.deselectObject();
		}

		// Otherwise, Toggle Editor Options GUI
		else
		{
			Global::editor_options->Active = true;
			glfwSetScrollCallback(Global::window, ScrollBarCallback);
		}

		// Disable Escape Key from Interfering with Other Objects
		Global::Keys[GLFW_KEY_ESCAPE] = false;
	}

	// Drop Selected Object and Create New Object
	if (Global::Keys[GLFW_KEY_SPACE] && !selector.active_window)
	{
		// Stop Editing Object
		if (selector.editing)
		{
			selector.deselectObject();
		}

		// Initialize Selector
		selector.active = true;

		// Disable Space Key
		Global::Keys[GLFW_KEY_SPACE] = false;

		// Initialize New Object Window
		selector.genNewObjectWindow();
		selector.active_window = true;

		// Set Scroll Callback
		glfwSetScrollCallback(Global::window, ScrollBarCallback);
	}

	// Delete Object
	if (Global::Keys[GLFW_KEY_BACKSPACE] && selector.editing)
	{
		selector.clear();
	}

	// Move Camera North if W is Held
	if (Global::Keys[GLFW_KEY_W])
	{
		// Normal Speed
		if (acceleration_timer.x < 150)
		{
			camera.accelerationY = 1.0f;
			acceleration_timer.x++;

			// Reset Southern Timer
			acceleration_timer.y = 0;
		}

		// Double Speed
		else if (acceleration_timer.x < 300)
		{
			camera.accelerationY = 2.0f;
			acceleration_timer.x++;
		}

		// 5x Speed
		else
		{
			camera.accelerationY = 5.0f;
		}

		camera.moveCamera(Render::Camera::NORTH);

		if (selector.editing)
		{
			selector.moveWithCamera(camera, Editor::NORTH);
		}
	}

	// Move Camera South if S is Held
	if (Global::Keys[GLFW_KEY_S])
	{
		// Normal Speed
		if (acceleration_timer.y < 150)
		{
			camera.accelerationY = 1.0f;
			acceleration_timer.y++;

			// Reset Southern Timer
			acceleration_timer.x = 0;
		}

		// Double Speed
		else if (acceleration_timer.y < 300)
		{
			camera.accelerationY = 2.0f;
			acceleration_timer.y++;
		}

		// 5x Speed
		else
		{
			camera.accelerationY = 5.0f;
		}

		camera.moveCamera(Render::Camera::SOUTH);

		if (selector.editing)
		{
			selector.moveWithCamera(camera, Editor::SOUTH);
		}
	}

	// Move Camera East if D is Held
	if (Global::Keys[GLFW_KEY_D])
	{
		// Normal Speed
		if (acceleration_timer.z < 150)
		{
			camera.accelerationR = 1.0f;
			acceleration_timer.z++;

			// Reset Southern Timer
			acceleration_timer.w = 0;
		}

		// Double Speed
		else if (acceleration_timer.z < 300)
		{
			camera.accelerationR = 2.0f;
			acceleration_timer.z++;
		}

		// 5x Speed
		else
		{
			camera.accelerationR = 5.0f;
		}

		camera.moveCamera(Render::Camera::EAST);

		if (selector.editing)
		{
			selector.moveWithCamera(camera, Editor::EAST);
		}
	}

	// Move Camera West if A is Held
	if (Global::Keys[GLFW_KEY_A])
	{
		// Normal Speed
		if (acceleration_timer.w < 150)
		{
			camera.accelerationL = 1.0f;
			acceleration_timer.w++;

			// Reset Southern Timer
			acceleration_timer.z = 0;
		}

		// Double Speed
		else if (acceleration_timer.w < 300)
		{
			camera.accelerationL = 2.0f;
			acceleration_timer.w++;
		}

		// 5x Speed
		else
		{
			camera.accelerationL = 5.0f;
		}

		camera.moveCamera(Render::Camera::WEST);

		if (selector.editing)
		{
			selector.moveWithCamera(camera, Editor::WEST);
		}
	}

	// Shift Up
	if (Global::Keys[GLFW_KEY_UP])
	{
		selector.moveWithArrowKeys(Editor::NORTH);
	}

	// Shift Down
	if (Global::Keys[GLFW_KEY_DOWN])
	{
		selector.moveWithArrowKeys(Editor::SOUTH);
	}

	// Shift Left
	if (Global::Keys[GLFW_KEY_LEFT])
	{
		selector.moveWithArrowKeys(Editor::WEST);
	}

	// Shift Right
	if (Global::Keys[GLFW_KEY_RIGHT])
	{
		selector.moveWithArrowKeys(Editor::EAST);
	}

	// Print Location at Mouse Position
	if (Global::Keys[GLFW_KEY_Q])
	{
		// Disable Key
		Global::Keys[GLFW_KEY_Q] = false;

		// Get Level Location
		glm::vec2 coords;
		level.updateLevelPos(glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY), coords);

		// Print Location
		std::cout << "Location X: " << Global::mouseRelativeX << "  Location Y: " << Global::mouseRelativeY << "  Level Coords: " << coords.x << "," << coords.y << "\n";
	}

	// Toggle Level Border Visualizer
	if (Global::Keys[GLFW_KEY_Z])
	{
		// Disable Key
		Global::Keys[GLFW_KEY_Z] = false;

		// Invert Level Borders
		Global::level_border = !Global::level_border;
	}

	// Toggles Normal Visualizer
	if (Global::Keys[GLFW_KEY_X])
	{
		// Disable Key
		Global::Keys[GLFW_KEY_X] = false;

		// Invert Normals
		Global::normals = !Global::normals;
	}

	// Toggle Object Lock
	if (Global::Keys[GLFW_KEY_E])
	{
		// Disable Key
		Global::Keys[GLFW_KEY_E] = false;

		// Enable Lock Flag
		Global::lock_object = true;
	}

	// Clone Object
	if (Global::Keys[GLFW_KEY_C])
	{
		// Disable Key
		Global::Keys[GLFW_KEY_C] = false;

		// Clone Object if One is Being Edited
		if (selector.editing)
		{
			selector.deselectObject();
			selector.editing = true;
			selector.active = true;
		}
	}

	// Rotate Object
	if (selector.editing && Global::Keys[GLFW_KEY_R])
	{
		if (!selector.rotating)
		{
			selector.pivot = glm::vec2(Global::mouseRelativeX, Global::mouseRelativeY);
		}
		selector.rotating = true;
	}

	else { selector.rotating = false; }

	// Temp Disable Resizing
	if (Global::Keys[GLFW_KEY_F])
	{
		selector.stopResizing();
	}

	// Save Level
	if (Global::Keys[GLFW_KEY_ENTER])
	{
		Global::Keys[GLFW_KEY_ENTER] = false;

		// Save Change Controller
		if (change_controller->returnIfUnsaved())
			change_controller->save();

		// Save Scripts
		if (script_wizard->modified)
			script_wizard->writeScriptData();

		// Compile the Project
		project_selector->recompileProject();
	}
}

void Source::Listeners::smoothKeyCallbackEditorSimplified(Camera& camera, glm::vec4& acceleration_timer)
{
	// Move Camera North if W is Held
	if (Global::Keys[GLFW_KEY_W])
	{
		// Normal Speed
		if (acceleration_timer.x < 150)
		{
			camera.accelerationY = 1.0f;
			acceleration_timer.x++;

			// Reset Southern Timer
			acceleration_timer.y = 0;
		}

		// Double Speed
		else if (acceleration_timer.x < 300)
		{
			camera.accelerationY = 2.0f;
			acceleration_timer.x++;
		}

		// 5x Speed
		else
		{
			camera.accelerationY = 5.0f;
		}

		camera.moveCamera(Render::Camera::NORTH);
	}

	// Move Camera South if S is Held
	if (Global::Keys[GLFW_KEY_S])
	{
		// Normal Speed
		if (acceleration_timer.y < 150)
		{
			camera.accelerationY = 1.0f;
			acceleration_timer.y++;

			// Reset Southern Timer
			acceleration_timer.x = 0;
		}

		// Double Speed
		else if (acceleration_timer.y < 300)
		{
			camera.accelerationY = 2.0f;
			acceleration_timer.y++;
		}

		// 5x Speed
		else
		{
			camera.accelerationY = 5.0f;
		}

		camera.moveCamera(Render::Camera::SOUTH);
	}

	// Move Camera East if D is Held
	if (Global::Keys[GLFW_KEY_D])
	{
		// Normal Speed
		if (acceleration_timer.z < 150)
		{
			camera.accelerationR = 1.0f;
			acceleration_timer.z++;

			// Reset Southern Timer
			acceleration_timer.w = 0;
		}

		// Double Speed
		else if (acceleration_timer.z < 300)
		{
			camera.accelerationR = 2.0f;
			acceleration_timer.z++;
		}

		// 5x Speed
		else
		{
			camera.accelerationR = 5.0f;
		}

		camera.moveCamera(Render::Camera::EAST);
	}

	// Move Camera West if A is Held
	if (Global::Keys[GLFW_KEY_A])
	{
		// Normal Speed
		if (acceleration_timer.w < 150)
		{
			camera.accelerationL = 1.0f;
			acceleration_timer.w++;

			// Reset Southern Timer
			acceleration_timer.z = 0;
		}

		// Double Speed
		else if (acceleration_timer.w < 300)
		{
			camera.accelerationL = 2.0f;
			acceleration_timer.w++;
		}

		// 5x Speed
		else
		{
			camera.accelerationL = 5.0f;
		}

		camera.moveCamera(Render::Camera::WEST);
	}
}

#endif

