#pragma once
#ifndef TEXTURES_H
#define TEXTURES_H

#include "ExternalLibs.h"
#include "Class/Render/Struct/Texture.h"

namespace Source::Textures
{
	// Load Textures into a Texture Map
	void Load_Textures(std::string path, std::map<std::string, Struct::SingleTexture>& map, GLenum internal_format);

	// Load a Single Texture
	void loadSingleTexture(std::string path, Struct::SingleTexture& texture);

	// Iterate Through a Texture File and Add Objects to Map
	void Iterate_Texture_File(std::string path, std::map<std::string, Struct::SingleTexture>& map, GLenum internal_format);

	// Iterate Through an Animation File and Add Result to Map
	void Iterate_Animation_File(std::string path, std::map<std::string, Struct::SingleTexture>& map, std::string name, GLenum internal_format);

	// Loads Materials into Material Map
	void Load_Materials_Lighting(std::string path, std::map<std::string, Struct::Material>& map);

	// Loads Materials into Materials Map
	void Load_Materials_Friction(std::string path);

	// Create A Single Texture Image
	GLuint Generate_Textures(std::string path[], int image_count, GLenum type, GLenum internal_format);

	// Changes Cursor Depending on Current Action
	void ChangeCursor(GLFWwindow* window);
}

#endif
