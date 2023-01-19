#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#ifndef DLL_HEADER
#include "ExternalLibs.h"
#include "Struct.h"
#endif

// Textures
struct Struct::Texture
{
	GLuint* texture = 0;                // Data For Texture
	GLuint* material = 0;               // Data For Material
	GLuint* mapping = 0;                // Data For Normal Map
	short layers[1024];                 // Pointer to Each Texture in Layer
	short location_of_next_image[1024]; // Location of Next Image To Switch To in Animation
	float animation_timer[1024];        // How Much Time To Stay on Image
};

// Static Texture
struct Struct::StaticTexture
{
	GLuint* texture = 0;
	std::unique_ptr<short> layers;
};

// Single Texture
struct Struct::SingleTexture
{
	GLuint texture;  // The Image Data of Texture
	GLenum type;     // The Type of Texture (2D or 2D Array)
	GLint size;      // The Amount of Layers in Texture
	GLenum index;    // Index of Texture in Shaders
	float data[20];  // The Delay Between Each Image
};

// Character Textures
struct Struct::Character
{
	GLuint TextureID;   // ID of Glyph Texture
	glm::ivec2 Size;    // Height and Width of the Glyph
	glm::ivec2 Bearing; // Offset from Baseline to Top/Left of the Glyph
	GLuint Advance;     // Offset to Next Glyph
};

// Material Structure
struct Struct::Material
{
	GLfloat Ambient;    // Ambinet Multiplier
	GLfloat Specular;   // Specular Multiplier
	GLfloat Shininess;  // Shininess Multiplier
	GLint Index;        // Index for Friction
};

#endif
