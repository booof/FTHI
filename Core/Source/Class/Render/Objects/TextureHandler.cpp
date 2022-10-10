#include "TextureHandler.h"

TextureHandler::TextureHandler(int number_of_textures)
{
	// Allocate Memory for Array of Textures
	initialized_textures = std::make_unique<int[]>(number_of_textures);
}

int TextureHandler::loadTexture(int texture_number)
{
	// Test if Texture is Already Loaded
	if (initialized_textures[texture_number])
		return initialized_textures[texture_number];

	// Else, Load Texture

	// Find Location to Insert Texture

	// Return Index of First Texture

	return 0;
}

void TextureHandler::removeTexture(int texture_number)
{
}
