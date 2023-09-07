#pragma once
#ifndef COMMON_H
#define COMMON_H

#include "ExternalLibs.h"

namespace Source::Algorithms::Common
{
	// OpenGL Error Log
	bool readErrorLog();

	// Converts Strings to Integers
	int convertStringToInt(std::string& string);

	// Converts Strings to Floats
	float convertStringToFloat(std::string& string);

	// Converts Strings to Boolean
	bool convertStringToBool(std::string& string);

	// Removes Zeros On End of String
	std::string removeTrailingZeros(std::string text);

	// Returns the Opposite Value of Texture Map
	template <class Value, class Map> std::string reverseTextureMap(Value value, Map& map);

	// Returns The Number of Objects in a Directory
	int getDirectoryFileNumber(std::string path);

	// Enable OpenGL Extensions
	bool WGLExtensionSupported(const char* extension_name);

	// Tests if a Character is In a String
	bool testIN(char character, const char* string);

	// Isolates the Name of a File from File Path
	std::string getFileName(std::string& file_path, bool include_file_type);

	// Erases the File Extension of a File Path
	void eraseFileExtension(std::string& file_path);
}

#endif
