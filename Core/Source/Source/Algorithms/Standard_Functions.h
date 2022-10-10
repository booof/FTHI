// More Functions For Standard Library

// OpenGL Error Log
void Error_Log()
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

// Converts Strings to Integers
int StringToInt(std::string string)
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

// Converts Strings to Floats
float StringToFloat(std::string string)
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

// Converts Strings to Boolean
bool StringToBool(std::string string)
{
	// Return False Only if Empty or 0. Return True Otherwise
	if (string == "" || string == "0" || string == " ")
		return false;
	return true;
}

// Removes Zeros On End of String
std::string RemoveTrailingZeros(std::string text)
{
	// A List of Character Used for Comparing
	const char* meme = text.c_str();

	// Start From End of String and Delete Zeros
	for (int i = text.length() - 1; i > 0; i--)
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

// Determines if a Value is Inside a Number With a Margin of Error
bool About(float input, float test, float error)
{
	return (input > test - error && input < test + error);
}

// Determines if a Value is Inside a Number With a Margin of Error
bool About(glm::vec2 input, glm::vec2 test, float error)
{
	return (input.x > test.x - error && input.x < test.x + error && input.y > test.y - error && input.y < test.y + error);
}

// Returns the Opposite Value of Texture Map
template <class Value, class Map> std::string ReverseTextureMap(Value value, Map& map)
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

// Returns The Number of Objects in a Directory
int getDirectoryFileNumber(std::string path)
{
	// The Calculated Number of Files
	int fileCount = 0;

	// Iterate Between Each File in Directory and Increase File Count
	for (const auto& entry : fs::directory_iterator(path))
	{
		fileCount++;
	}

	// Return Number of Files in Directory
	return fileCount;
}

// Bubble Sorting Algorithm With Offsets
template <class Type> Type* BubbleSort(Type object[], int size, int offset)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = offset; i < size + offset; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = offset; j < size - i + offset - 1; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j] > object[j + 1])
			{
				Type intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}

	// Return Sorted Object
	return object;
}

// Bubble Sorting Algorithm Without Offsets
template <class Type> void BubbleSort(Type object[], int size)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = 0; i < size; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = 0; j < size - i - 1; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j] > object[j + 1])
			{
				Type intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}
}


// Returns Sign of Number
template <class Type> int Sign(Type number)
{
	if (number >= 0) { return 1; }
	return -1;
}

// Returns Sign of Number with 0
template <class Type> int Sign_Zero(Type number)
{
	if (!number) { return 0; }
	if (number > 0) { return 1; }
	return -1;
}

// Returns the Cardinal Direction Closest to Angle
float cardinalDirection(float angle)
{
	int iterations = -1;
	while (angle > -0.7854f)
	{
		iterations++;
		angle -= 1.5708f;
	}

	//std::cout << iterations << " r\n";
	return iterations * 1.5708f;
}

// Creates a Box Object Similar to Draw_Rect
box Create_Box(int name, float xPos, float yPos, float width, float height)
{
	box Box;
	Box.name = name;
	Box.center_x = xPos;
	Box.center_y = yPos;
	Box.width = width;
	Box.height = height;
	Box.left_x = xPos - width / 2;
	Box.right_x = xPos + width / 2;
	Box.bottom_y = yPos - height / 2;
	Box.top_y = yPos + height / 2;
	return Box;
}

bool WGLExtensionSupported(const char* extension_name)
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

// Computes the Square of a Number Quickly
float fast_square(float number)
{
	// Typcast to Manipulatable Binary
	static unsigned long int cast_number = *(unsigned long*)&number;

	// Preform Bit Manipulation
	cast_number = (cast_number << 1) - 1064992506;

	// Return Typcasted Float
	return *(float*)&cast_number;
}

// Computes the Square of a Number Quickly
float fast_root(float number)
{
	// Typecast to Manipulatable Binary
	static unsigned long int cast_number = *(unsigned long*)&number;

	// Preform Bit Manipulation
	cast_number = 532496253 + (cast_number >> 1);

	// Return Typcasted Float
	return *(float*)&cast_number;
}

// Computes the Distance of Two Numbers Quickly
float fast_distance(float num1, float num2)
{
	return fast_root(fast_square(num1) + fast_square(num2));
}

// Computes the Distance Between Two Points Quickly
float fast_distance(glm::vec2 point1, glm::vec2 point2)
{
	return fast_distance(point1.x - point2.x, point1.y - point2.y);
}

// Computes the Distance Between Two Points Quickly
float fast_distance(glm::vec2 point)
{
	return fast_distance(point.x, point.y);
}

// Sort an Array of Named Nodes
void sort_Named_Nodes(Named_Node (&object)[4], int size)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = 0; i < 4; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = 0; j < 3 - i; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j].value > object[j + 1].value)
			{
				Named_Node intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}
}

// Sort an Array of Named Nodes
void sort_Named_Nodes(Named_Node (&object)[3], int size)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = 0; i < 3; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = 0; j < 2 - i; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j].value > object[j + 1].value)
			{
				Named_Node intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}
}

// Find Index of Closest Value Without Going Over
int Closest_Without_Going_Over(float test_value, const float list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Closest_Without_Going_Over(double test_value, const double list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Closest_Without_Going_Over(int test_value, const int list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Closest_Without_Going_Over(float test_value, const Named_Node list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i].value > test_value)
		{
			return list[i].name;
		}
	}

	return list[0].name;
	//return 0;
}

int Closest_Without_Going_Over_Named_Index(float test_value, const Named_Node list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i].value > test_value)
		{
			return i;
		}
	}

	return 0;
}

// Parameterize Angle
glm::vec2 Parameterize_Angle(float angle)
{
	return glm::vec2(cos(angle), sin(angle));
}

// Slurp
std::string slurp(std::ifstream& in) 
{
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

// Slurp
ustring slurp(ufile& in)
{
	ustringstream stream;
	stream << in.rdbuf();
	return stream.str();
}

