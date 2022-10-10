#ifndef FONTS_H
#define FONTS_H

#include "ExternalLibs.h"
#include "Class/Render/Struct/Texture.h"

namespace Source::Fonts
{
	// Loads Fonts
	void loadFont(FT_Face face, int length);

	// Changes Between Different Fonts
	void changeFont(std::string font, int width, int height);

	// Draws Text to the Screen
	float renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color, bool Static);

	// Renders Text That Allows for Centering and Text Length
	float renderTextAdvanced(std::string text, GLfloat x, GLfloat y, GLfloat maxLength, GLfloat scale, glm::vec4 color, bool centered);

	// Renders Text Offset by a Model Matrix
	float renderTextOffset(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color);

	// Draws Text in World
	float renderTextGlobal(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color);

	// Draw Text Helper Function
	float renderTextHelper(std::string text, GLfloat x, GLfloat y, GLfloat scale);

	// Draws an Ind ividual Character
	GLfloat renderChar(Struct::Character chr, GLfloat x, GLfloat y, GLfloat scale);

	// Determine the Size of Text
	float getTextSize(std::string text, GLfloat scale);
}

#endif
