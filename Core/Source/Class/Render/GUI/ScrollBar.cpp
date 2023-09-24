#include "ScrollBar.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Globals
#include "Globals.h"


bool GUI::ScrollBar::updateElement()
{
	// Determine if Bar Should Try to be Modified
	if (!(modified_by_user ^ is_being_modified))
	{
		// If Left Click is Not Held, Disable Scrolling
		if (!Global::LeftClick)
			is_being_modified = false;

		// If Bar is Currently Being Modified, Scroll the Bar
		else if (is_being_modified)
		{
			was_modified = true;
			Scroll((Global::mouseY / Global::zoom_scale) - scroll_offset);
			return true;
		}

		// If Not Being Modified, Test Collisions
		if (!is_being_modified)
			return TestColloisions();
	}

	return false;
}

void GUI::ScrollBar::linkValue(void* value_ptr)
{
	// Reinterpret Value Pointer as the Percent Pointer
	percent_ptr = static_cast<float*>(value_ptr);
}

void GUI::ScrollBar::blitzElement()
{
	// Send Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model_Background));

	// Send Nullified Brightness Vector to Shader
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	glBindVertexArray(BackgroundVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Send Other Model Matrix to Shader
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model_Scroll));

	glBindVertexArray(ScrollVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
