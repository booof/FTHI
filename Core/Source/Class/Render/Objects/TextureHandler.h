#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

#include "ExternalLibs.h"
#include "Render/Struct/Texture.h"

class TextureHandler
{
public:

	// Texture Data
	Struct::Texture texture;

	// The List of Initialized Textures
	std::unique_ptr<int[]> initialized_textures;
	//int* initialized_textures;

	// Initialize TextureHandler
	TextureHandler(int number_of_textures);

	// Test if Texture is Loaded
	// If Not Loaded, Load Texture in Next Open Slot
	// Returns Index of First Image in List
	int loadTexture(int texture_number);

	// Removes a Texture in List and Free Data
	void removeTexture(int texture_number);
};

#endif
