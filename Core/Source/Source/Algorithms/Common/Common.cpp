#include "Common.h"

void Source::Algorithms::Common::readErrorLog()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
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


