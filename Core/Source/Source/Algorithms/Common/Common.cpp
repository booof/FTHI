#include "Common.h"

bool Source::Algorithms::Common::readErrorLog()
{
	bool error_found = false;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		error_found = true;
		std::string error;
		switch (err)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << "OPENGL ERROR: " << err << ": " << error << "\n";
	}
	return error_found;
}

int Source::Algorithms::Common::convertStringToInt(std::string& string)
{
	// Return -1 if string is "NULL"
	if (string == "NULL")
	{
		return -1;
	}

	// Return the Converted Integer
	try
	{
		int i = std::stoi(string);
		return i;
	}

	// Return 0 if the String Could Not be Converted
	catch (std::invalid_argument)
	{
		std::cout << "Bad Input : std::invalid_argument thrown\n";
		return 0;
	}

	// Return 0 if String is Too Large
	catch (std::out_of_range)
	{
		std::cout << "Float Overflow : std::out_of_range_thrown\n";
		return 0;
	}
}

float Source::Algorithms::Common::convertStringToFloat(std::string& string)
{
	// Return -1 if string is "NULL"
	if (string == "NULL")
	{
		return -1;
	}

	// Return the Converted Float
	try
	{
		float f = std::stof(string);
		return f;
	}

	// Return 0 if the String Could Not be Converted
	catch (std::invalid_argument)
	{
		std::cout << "Bad Input : std::invalid_argument thrown\n";
		return 0.0f;
	}

	// Return 0 if String is Too Large
	catch (std::out_of_range)
	{
		std::cout << "Float Overflow : std::out_of_range_thrown\n";
		return 0.0f;
	}
}

bool Source::Algorithms::Common::convertStringToBool(std::string& string)
{
	// Return False Only if Empty or 0. Return True Otherwise
	if (string == "" || string == "0" || string == " ")
		return false;
	return true;
}

std::string Source::Algorithms::Common::removeTrailingZeros(std::string text)
{
	// A List of Character Used for Comparing
	const char* meme = text.c_str();

	// Start From End of String and Delete Zeros
	for (int i = (int)text.length() - 1; i > 0; i--)
	{
		// If the Character is Not Zero, Break the Loop
		if (meme[i] != '0' && meme[i] != ' ')
		{
			// If Character is a Decimal, Remove Before Breaking
			if (meme[i] == '.')
			{
				text.pop_back();
			}

			break;
		}

		// If Character is Zero, Delete it
		else
		{
			text.pop_back();
		}
	}

	return text;
}

template<class Value, class Map>
std::string Source::Algorithms::Common::reverseTextureMap(Value value, Map& map)
{
	// The Resulting Key From Map
	std::string result = "NULL";

	// Iterate Between Each Pair in Textures Map
	for (typename std::map<std::string, Value>::iterator it = map.begin(); it != map.end(); ++it)
	{
		// If Second Value is Value, Resulting Key is First Value
		if (it->second == value)
		{
			result = it->first;
			break;
		}
	}

	return result;
}

int Source::Algorithms::Common::getDirectoryFileNumber(std::string path)
{
	// The Calculated Number of Files
	int fileCount = 0;

	// Iterate Between Each File in Directory and Increase File Count
	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		fileCount++;
	}

	// Return Number of Files in Directory
	return fileCount;
}

bool Source::Algorithms::Common::WGLExtensionSupported(const char* extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

bool Source::Algorithms::Common::testIN(char character, const char* string)
{
	char test = 0;
	while ((test = *string) != '\0')
	{
		string++;
		if (test == character)
			return true;
	}
	return false;
}

std::string Source::Algorithms::Common::getFileName(std::string& file_path, bool include_file_type)
{
	// The File Name
	std::string file_name = "";

	// Index in File Path
	int index2 = file_path.size();
	int index = index2 - 1;
	if (index < 1)
		return file_name;

	// Iterate Until Either '.' , '\', or '/' is found first
	while (index >= 0 && !testIN(file_path[index], ".\\/"))
		index--;

	// If Index is 0, Return ""
	if (!index)
		return file_name;

	// If '.' Was Found, Find Next '/' or '\\'
	if (file_path[index] == '.')
	{
		// If File Path Shouldn't be Included, Set Index2 to Where '.' is
		if (!include_file_type)
			index2 = index;

		// Iterate Until '//' or '/' is Found
		while (index >= 0 && !testIN(file_path[index], "\\/"))
			index--;

		// If Index is 0, Return ""
		if (!index)
			return file_name;
	}

	// Resize The String to Contain the Correct Amount of Data
	file_name.resize(index2 - index - 1);
	
	// Copy File Name From Path Into String
	int i = 0;
	index++;
	while (index < index2)
		file_name[i++] = file_path[index++];

	return file_name;
}

void Source::Algorithms::Common::eraseFileExtension(std::string& file_path)
{
	int start_index = file_path.size() - 1;
	while (start_index != 0 && file_path.at(start_index) != '.')
		start_index--;
	if (start_index == 0)
		return;
	file_path.erase(file_path.begin() + start_index, file_path.end());
}

bool Source::Algorithms::Common::testFileNameInvalidCharacters(std::string& file_name)
{
	// List of Invalid Characters
	std::string invalid_characters = "<>:\"/\\|?*";

	// First, Test if All Characters are Greater Than or Equal to 32
	for (char& c : file_name)
	{
		if (c < 32)
			return false;
	}

	// Next, Test Any Invalid Characters
	for (char& c : invalid_characters)
	{
		if (testIN(c, file_name.c_str()))
			return false;
	}

	// Return True if File Name is Valid
	return true;
}

std::string Source::Algorithms::Common::getFilePath(std::string& file_path)
{
	// The File Name
	std::string result = "";

	// Iterate Through File Path and Find Final "\\" or "/"
	int last_dash = 0;
	for (int i = 0; i < file_path.size(); i++)
	{
		if (testIN(file_path[i], "\\/"))
			last_dash = i;
	}

	// Resize the Result
	result.resize(last_dash);

	// Copy Everything Up Until the Last Dash
	for (int i = 0; i < last_dash; i++)
		result[i] = file_path[i];

	// Return the Result
	return result;
}


