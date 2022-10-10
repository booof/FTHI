#include "PreRender.h"

#include "ExternalLibs.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Globals.h"
#include "Source/Events/EventListeners.h"
#include "Class/Render/Shader/Shader.h"
#include "Source/Loaders/Fonts.h"

void Source::Rendering::Pre::preRender()
{
	// Process Error Log
	//Error_Log();

	// Process Time
	getDeltaTime();

	// Handle Inputs
	glfwPollEvents();

#ifdef EDITOR

	// If Zoom Scale Changed, Change Border Values
	if (Global::framebufferResize || Global::zoom)
	{
		Global::borderShader.Use();
		glUniform1f(Global::level_border_screen_width_loc, -Global::halfScalarX * Global::zoom_scale);
		glUniform1f(Global::level_border_screen_height_loc, -50.0f * Global::zoom_scale);
	}

#endif

	// Bind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Multisample);
	glEnable(GL_DEPTH_TEST);

	// Clear Window
	glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);
}

void Source::Rendering::Pre::Error_Log()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) 
	{
		std::string error;
		switch (err)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << "OPENGL ERROR: " << err << ": " << error << "\n";
	}
}

void Source::Rendering::Pre::getDeltaTime()
{
	static double last_frame = 0;
	static double current_frame = 0;

	// Get Age of Program
	current_frame = glfwGetTime() - Global::framebyframeOffset;

	// Calculate Difference Between Frames
	Global::deltaTime = (float)(current_frame - last_frame);
	last_frame = current_frame;

	// Calculate FPS if Neccessary
	if (Global::editor_options->option_fps)
		getFPS(current_frame);

	// Update RNG
	xorshift64();
}

void Source::Rendering::Pre::getFPS(double current_frame)
{
	// Memory for Last Second
	static double last_second = 0;

	// Memory for Frame Count
	static short frame_count = 0;

	// Increment Frame Count
	frame_count++;

	// For Every Second That Passes Update FPS
	if (current_frame > last_second + 1.0)
	{
		std::cout << frame_count << " FPS\n" << " " << Global::frame_by_frame << " " << Global::frame_step << " " << Global::frame_run << " \n";

		// Store Frame Count
		Global::last_frameCount = frame_count;

		// Reset FPS Values
		frame_count = 0;
		last_second = current_frame;
	}
}

void Source::Rendering::Pre::xorshift64()
{
	uint64_t x = Global::rng;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	Global::rng = x;
	Global::rng *= 0x2545F4914F6CDD1DULL;
}

long double Source::Rendering::Pre::randomFloat()
{
	long double decimal_rng = Global::rng * 0.0000000000000000001;
	decimal_rng -= (floor(decimal_rng) == 1);
	xorshift64();

	return decimal_rng;
}

