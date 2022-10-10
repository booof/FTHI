#include "Fonts.h"
#include "Class/Render/Shader/Shader.h"
#include "Globals.h"

void Source::Fonts::loadFont(FT_Face face, int length)
{
	Global::Current_Font.clear();

	for (GLubyte chr = 0; chr < length; chr++)
	{
		// Load Character Glyph
		if (FT_Load_Char(face, chr, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYPE::FAILED_TO_LOAD_GLYPH: " << chr << std::endl;
		}

		// Generate a Texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		// Assign Glyph to Texture
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// Set Texture Options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Generate Character in Structure
		Struct::Character character =
		{
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			(GLuint)face->glyph->advance.x
		};

		// Store Character in Map
		Global::Current_Font.insert(std::pair<GLchar, Struct::Character>(chr, character));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Source::Fonts::changeFont(std::string font, int width, int height)
{
	// Get Font Number from Font List to Test for Which Font to Initialize
	switch (Global::FontList.find(font)->second)
	{

		// Change to Arial Font
	case 0:
	{
		// Set Size of Font
		FT_Set_Pixel_Sizes(Global::Arial, width, height);

		// Load as Usable Font
		loadFont(Global::Arial, 128);

		break;
	}

	// Change to Cambria
	case 1:
	{
		FT_Set_Pixel_Sizes(Global::CambriaMath, width, height);
		loadFont(Global::CambriaMath, 128);
		break;
	}

	// Change to SEGOEUI
	case 2:
	{
		FT_Set_Pixel_Sizes(Global::SEGOEUI, width, height);
		loadFont(Global::SEGOEUI, 128);
		break;
	}

	// Change to test
	case 3:
	{
		FT_Set_Pixel_Sizes(Global::TEST, width, height);
		loadFont(Global::TEST, 185);
		break;
	}

	}
}

float Source::Fonts::renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color, bool Static)
{
	// Send Projection Matrix to Shader
	if (Static) { glUniformMatrix4fv(Global::projectionLocFont, 1, GL_FALSE, glm::value_ptr(Global::projectionStatic)); } // Use the Static Projection Matrix
	else { glUniformMatrix4fv(Global::projectionLocFont, 1, GL_FALSE, glm::value_ptr(Global::projection)); } // Use the Standard Dynamic Projection Matrix

	// Send Colors to Shader
	glUniform3f(Global::texcolorLocFont, color.x, color.y, color.z);

	// Draw Text
	return renderTextHelper(text, x, y, scale);
}

float Source::Fonts::renderTextAdvanced(std::string text, GLfloat x, GLfloat y, GLfloat maxLength, GLfloat scale, glm::vec4 color, bool centered)
{
	// The Character in Text
	std::string::const_iterator c;

	// Send Static Projection Matrix to Shader
	glUniformMatrix4fv(Global::projectionLocFont, 1, GL_FALSE, glm::value_ptr(Global::projectionStatic));

	// Send Colors to Shader
	glUniform3f(Global::texcolorLocFont, color.x, color.y, color.z);

	// Temporary Width of Decimal Points at End of Text
	float DecimalPointWidth = (Global::Current_Font[46].Advance >> 6) * scale * 2;

	// The Current Length of the Iterated Text
	float TextLength = 0;

	// A Modified Text Variable Fit for Max Length
	std::string newText = "";

	// Iterate Between Each Character in Text to Test if it Passes Max Length
	for (c = text.begin(); c != text.end(); c++)
	{
		// Increase Length of Text
		TextLength += (Global::Current_Font[*c].Advance >> 6) * scale;

		// Append Character to newText
		newText += *c;

		// Prevent Text From Going Past Max Length
		if (TextLength + DecimalPointWidth >= maxLength)
		{
			TextLength += DecimalPointWidth;
			newText += "..";
			break;
		}
	}

	// Shift x Coordinate if Centered
	if (centered)
	{
		x -= TextLength / 2;
	}

	// Draw Text
	return renderTextHelper(text, x, y, scale);
}

float Source::Fonts::renderTextOffset(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
	// Send Projection Matrix to Shader
	glUniformMatrix4fv(Global::projectionLocRelativeFont, 1, GL_FALSE, glm::value_ptr(Global::projectionStatic));

	// Send Colors to Shader
	glUniform3f(Global::texcolorLocRelativeFont, color.x, color.y, color.z);

	// Draw Text
	return renderTextHelper(text, x, y, scale);
}

float Source::Fonts::renderTextGlobal(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 color)
{
	// Enable Shader
	//Global::fontGlobalShader.Use();

	// Send Matricies to Shader
	glUniformMatrix4fv(Global::projectionLocGlobalFont, 1, GL_FALSE, glm::value_ptr(Global::projection));
	glUniformMatrix4fv(Global::viewLocGlobalFont, 1, GL_FALSE, glm::value_ptr(Global::view));

	// Send Colors to Shader
	glUniform3f(Global::textcolorLocGlobalFont, color.x, color.y, color.z);

	// Draw Text
	return renderTextHelper(text, x, y, scale);
}

float Source::Fonts::renderTextHelper(std::string text, GLfloat x, GLfloat y, GLfloat scale)
{
	// Bind Texture and Buffer Object
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(Global::fontVAO);

	// Iterate Through all Characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		// An Individual Character in the String
		Struct::Character chr = Global::Current_Font[*c];

		// Draw Current Character
		x += renderChar(chr, x, y, scale);
	}

	// Unbind Buffer and Texture Object
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	// Return X Position
	return x;
}

GLfloat Source::Fonts::renderChar(Struct::Character chr, GLfloat x, GLfloat y, GLfloat scale)
{
	// Calculate X and Y Positions of Glyph
	GLfloat xPos = x + chr.Bearing.x * scale;
	GLfloat yPos = y - (chr.Size.y - chr.Bearing.y) * scale;

	// Calculate Width and Height of Glyph
	GLfloat width = chr.Size.x * scale;
	GLfloat height = chr.Size.y * scale;

	// Update VBO of Each Character
	GLfloat vertices[6][4] =
	{
		{ xPos,         yPos + height,  0.0f, 0.0f },
		{ xPos,         yPos,           0.0f, 1.0f},
		{ xPos + width, yPos,           1.0f, 1.0f},

		{ xPos,         yPos + height,  0.0f, 0.0f },
		{ xPos + width, yPos,           1.0f, 1.0f },
		{ xPos + width, yPos + height,  1.0f, 0.0f }
	};

	// Bind Glyph to Texture
	glBindTexture(GL_TEXTURE_2D, chr.TextureID);

	// Update Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, Global::fontVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Draw Glyph
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Advance Positions for Next Character
	return (chr.Advance >> 6) * scale;
}

float Source::Fonts::getTextSize(std::string text, GLfloat scale)
{
	// Calculate the Size of the Text
	float size = 0;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		// An Individual Character in the String
		Struct::Character chr = Global::Current_Font[*c];

		// Increment the Size by the Character Advance
		size += (chr.Advance >> 6);
	}

	// Scale the Text Size
	size *= scale;

	// Return the Size
	return size;
}
