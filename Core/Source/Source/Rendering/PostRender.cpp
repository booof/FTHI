#include "PostRender.h"
#include "Globals.h"
#include "ExternalLibs.h"
#include "Class/Render/Shader/Shader.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Loaders/Textures.h"
#include "Source/Events/EventListeners.h"
#include "Render/GUI/SelectedText.h"

void Source::Rendering::Post::postRender()
{
	// If Texting and Left Click, Disable Texting
	if (selected_text->isActive() && Global::LeftClick)
		selected_text->stopSelecting();

	// Disable Loop Flags
	Global::framebufferResize = false;
	Global::zoom = false;
	Global::cursor_Move = false;
	Global::reload_all = false;
	Global::DoubleClick = false;
	Global::reload_lights = false;

	glFlush();

	// Blitz Multisampled Framebuffer to Framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, Global::Frame_Buffer_Object_Multisample);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Global::Frame_Buffer_Object_HDR);
	glBlitFramebuffer(0, 0, Global::screenWidth, Global::screenHeight, 0, 0, Global::screenWidth, Global::screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	// Bind Core Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Core);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	// Draw Screen Texture
	Global::FramebufferShader_HDR.Use();
	glBindVertexArray(Global::screenVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_HDR);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Draw the Heads Up Display
	Global::texShaderStatic.Use();
	glUniform1i(Global::staticLocTexture, 1);

	// Draw FPS
	display_FPS();

	// Apply Bloom
	//Bloom(Global::Frame_Buffer_Texture_Bloom, 6);

	// Draw Core Texture
	Global::FramebufferShader_Core.Use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(Global::screenVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Core);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unbind Objects
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	// Update Window
	glfwSwapBuffers(Global::window);
	glFinish();

	// Update Selected Cursor
	Source::Textures::ChangeCursor(Global::window);
}

void Source::Rendering::Post::display_FPS()
{
	// Draw FPS if Enabled
	if (Global::editor_options->option_fps == 2)
	{
		Global::fontShader.Use();
		Source::Fonts::renderTextAdvanced(std::to_string(Global::last_frameCount), 80.0f, 94.0f, 100.0f, 0.2f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), false);
	}
}
