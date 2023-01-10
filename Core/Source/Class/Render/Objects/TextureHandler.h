#ifndef TEXTURE_HANDLER_H
#define TEXTURE_HANDLER_H

#include "ExternalLibs.h"
#include "Render/Struct/Texture.h"

// Idea for How to Perform Texture Loading/Unloading
// 
// There Should be a Single, Binary Tree that Contains a Texture Object (Node)
// The Array Should be Sorted Based on the Texture Name (Same as in Object Data)
//
// This Texture Object Should Store the Texture Name, Number of Objects Using the
// Texture, and the Pointer to the Texture Buffer
//
// To Test if a Texture Needs to be Loaded, Perform a Binary Search Based on Where the
// Texture is Supposed to be Found. If Found, The Texture Object Should be Stored as a 
// Pointer in the New Object and the Number of Objects Using it Should Increment by 1. Else,
// Insert a new Texture Object in the Tree and Load the Texture
//
// If an Object Using a Texture is Unloaded, Decrement the Number of Objects Using it. If
// the Number of Objects Using it is 0, Add it to a Small Array. Perferably, This Array Will
// be Another Binary Tree. If No New Objects Need the Texture, Unload it and Remove the Texture Object
//
// If Texture Objects are Added or Removed From the Tree, Perform Red/Black Optimization

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
