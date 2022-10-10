// Loads Images and Binds them to Textures

// Load Textures into a Texture Map
void Load_Textures(std::string path, std::map<std::string, Texture>& map, GLenum internal_format)
{
	// Iterate Through Each Directory in Path
	for (const auto directory : fs::directory_iterator(path))
	{
		// Get File Type of Object
		std::string new_path = directory.path().string();
		const char* directory_path = new_path.c_str();
		std::string type = new_path;
		type.erase(0, type.length() - 3);

		// Test if Directory is a Texture Folder
		if (type == "tex")
		{
			Iterate_Texture_File(directory_path, map, internal_format);
		}

		// Test if Directory is an Animation Folder
		else if (type == "anm")
		{
			// Generate Name of Animation
			std::string name = directory_path;
			name.erase(0, path.length() + 1);

			Iterate_Animation_File(directory_path, map, name, internal_format);
		}

		// Iterate Through Next Directories
		else
		{
			// Send Path Through Loop
			Load_Textures(new_path, map, internal_format);
		}
	}
}

// Iterate Through a Texture File and Add Objects to Map
void Iterate_Texture_File(std::string path, std::map<std::string, Texture>& map, GLenum internal_format)
{
	// Iterate Through Texture Directory
	for (const auto file : fs::directory_iterator(path))
	{
		// Get Path and Name of Texture File
		std::string temp_path = file.path().string();
		std::string texture_name = temp_path;
		std::string texture_path[1];
		texture_path[0] = temp_path;
		texture_name.erase(0, path.length() + 1);

		// Create Texture
		Texture texture_struct;
		texture_struct.texture = Generate_Textures(texture_path, 1, GL_TEXTURE_2D, internal_format);
		texture_struct.type = GL_TEXTURE_2D;
		texture_struct.size = 0;
		texture_struct.index = GL_TEXTURE0;

		// Add Texture to List
		map.insert({ texture_name, texture_struct });
	}
}

// Iterate Through an Animation File and Add Result to Map
void Iterate_Animation_File(std::string path, std::map<std::string, Texture>& map, std::string name, GLenum internal_format)
{
	// Paths
	std::string* paths = new std::string[20];
	std::string data_path = "";
	unsigned int paths_index = 0;

	// Iterate and Append Paths to Path List
	for (const auto entry : fs::directory_iterator(path))
	{
		// Test if File is a Character File
		if (entry.path().extension().string() == ".dat")
		{
			data_path = entry.path().string();
		}

		// File is an Image File
		else
		{
			paths[paths_index] = entry.path().string();
			paths_index++;
		}
	}

	// Unpack Data File

	// Data and File
	unsigned int line_number = 0;
	std::string line;
	unsigned int index = 0;
	std::string* order = new std::string[20];
	double* data = new double[20];
	std::ifstream file;
	file.open(data_path);

	// Iterate Though Each Line and Add to Data
	while (std::getline(file, line))
	{
		// Insert Line Into Order Array
		if (line_number % 2 == 0)
		{
			order[index] = line;
		}

		// Insert Line Into Delay Array
		else
		{
			data[index] = StringToFloat(line);
			index++;
		}

		// Increase Line Number
		line_number++;
	}

	// Order Image Paths

	// New Path Array and Iterators
	std::string* new_paths = new std::string[20];

	// Iterate Through Number of Paths to Match Paths with Order
	for (int new_paths_index = 0; new_paths_index < paths_index; new_paths_index++)
	{
		// Iterate Through Each Element in Paths then test if Paths Match
		for (int old_paths_index = 0; old_paths_index < paths_index; old_paths_index++)
		{
			// Remove Path and Get File Name
			std::string file_name = paths[old_paths_index];
			file_name.erase(0, std::string(path).length() + 1);

			// If Path is Matched, Add it to New Paths Array and Break Inner Loop
			if (file_name.c_str() == order[new_paths_index])
			{
				new_paths[new_paths_index] = paths[old_paths_index];
				break;
			}
		}
	}

	// Generate Texture
	GLuint texture = Generate_Textures(new_paths, paths_index, GL_TEXTURE_2D_ARRAY, internal_format);

	// Create Texture Structure
	Texture texture_struct;
	texture_struct.texture = texture;
	texture_struct.type = GL_TEXTURE_2D_ARRAY;
	texture_struct.size = paths_index + 1;
	texture_struct.index = GL_TEXTURE1;
	for (int index = 0; index < paths_index; index++)
	{
		texture_struct.data[index] = data[index];
	}

	// Bind Texture to Map
	map.insert({ name, texture_struct });

	// De-Allocate Memory
	delete[] paths;
	delete[] new_paths;
	delete[] order;
	delete[] data;
}

// Loads Materials into Material Map
void Load_Materials_Lighting(std::string path, std::map<std::string, Material>& map)
{
	// The File
	std::ifstream file(path);

	// Current Line in File
	std::string line;

	// Current Element in Line
	std::string element;

	// Current Index in Line
	int index;

	// Current material count
	int material_count = 0;

	// Name of the Line
	std::string name;

	// Iterate Through Each Line in Materials File
	while (std::getline(file, line))
	{
		// Initialize New Material
		Material new_material;

		// Reset Some Values
		element = "";
		index = 0;
		name = "";

		// Iterate Through Every Character in Line
		for (int i = 0; i <= line.size(); i++)
		{
			// If Whitespace is Found, Continue to Next Character
			if (line[index] == ' ')
			{
				continue;
			}

			// Test if There is a Semicolor to End Line
			else if (line[i] == ';')
			{
				new_material.Shininess = StringToFloat(element);
			}

			// Test if There is a Coma to End Element
			else if (line[i] == ',')
			{
				// Determine Where to Save the Element
				switch (index)
				{

				// Name
				case 0:
				{
					name = element;
					break;
				}

				// Ambient
				case 1:
				{
					new_material.Ambient = StringToFloat(element);
					break;
				}

				// Specular
				case 2:
				{
					new_material.Specular = StringToFloat(element);
				}

				}

				// Reset Element and Increase Index
				element = "";
				index++;
			}

			// Add Character to Element
			else
			{
				element += line[i];
			}
		}

		// Save Material to Map
		new_material.Index = material_count;
		material_count++;
		map.insert({ name, new_material });
	}

	// Close File
	file.close();

	// Create Temp Friction File
	std::ofstream file_2("../Resources/Materials/Friction_Temp.txt");
	int i = 0;
	for (auto it = map.begin(); it != map.end(); it++, i++)
	{
		for (auto it2 = it; it2 != map.end(); it2++)
		{
			std::string temp = "";
			temp += it->first;
			temp += " ";
			temp += it2->first;
			temp += " 1.0 0.5 \n";
			file_2 << temp;
		}
	}
	file_2.close();

	// Created Temp Lighting File
	std::ofstream file_3("../Resources/Materials/Lighting_Temp.txt");
	i = 0;
	for (auto it = map.begin(); it != map.end(); it++, i++)
	{
		std::string temp = "";
		temp += it->first;
		temp += ", ";
		temp += RemoveTrailingZeros(std::to_string(it->second.Ambient));
		temp += ", ";
		temp += RemoveTrailingZeros(std::to_string(it->second.Specular));
		temp += ", ";
		temp += RemoveTrailingZeros(std::to_string(it->second.Shininess));
		temp += ";\n";
		file_3 << temp;
	}
	file_3.close();
}

// Loads Materials into Materials Map
void Load_Materials_Friction(std::string path)
{
	// Open File
	std::ifstream file(path);

	// Current Line in file
	std::string line;

	// Current token in file
	std::string token;
	int current_token;

	// Current index in table
	int table_index = 0;

	// Iterate Through All lines in file
	while (std::getline(file, line))
	{
		current_token = 0;
		token = "";

		// Iterate through each line and retrieve coefficients of friction
		for (int i = 0; i < line.size(); i++)
		{
			// If space, skip and append token to table
			if (line[i] == ' ')
			{
				switch (current_token)
				{
				// Static Friction
				case 2:
					static_friction[table_index] = StringToFloat(token);
					break;
				// Kinetic Friction
				case 3:
					kinetic_friction[table_index] = StringToFloat(token);
				}
				token = "";
				current_token++;
			}

			// Else, append character to token
			else
			{
				token += line[i];
			}
		}

		// Increment Table Index
		table_index++;
	}

	// Close File
	file.close();
}

// Create A Single Texture Image
GLuint Generate_Textures(std::string path[], int image_count, GLenum type, GLenum internal_format)
{
	GLuint texture;

	// Create and Bind Texture
	glGenTextures(1, &texture);
	glBindTexture(type, texture);

	// Variables Used for Image Creation
	int imageWidth, imageHeight;
	unsigned char* image;

	// 2D Texture
	if (type == GL_TEXTURE_2D)
	{
		// Create Image
		image = SOIL_load_image(path[0].c_str(), &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);

		// Bind Image to Texture
		glTexImage2D(type, 0, internal_format, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		// Clear Image
		SOIL_free_image_data(image);
	}

	// 2D Texture Array
	else
	{
		// Create Base for Texture
		image = SOIL_load_image(path[0].c_str(), &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
		glTexStorage3D(type, image_count, GL_RGBA, imageWidth, imageHeight, image_count);
		SOIL_free_image_data(0);

		// Assign Image to Texture
		for (int i = 0; i < image_count; i++)
		{
			// Load Image
			image = SOIL_load_image(path[i].c_str(), &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);

			// Store Image Data
			glTexImage3D(type, i, internal_format, imageWidth, imageHeight, image_count, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexSubImage3D(type, 0, 0, 0, i, imageWidth, imageHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, image);

			// Clear Image
			SOIL_free_image_data(image);
		}
	}

	// Generate Mipmaps
	glGenerateMipmap(type);

	// Set Texture Options
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Unbind Texture
	glBindTexture(type, 0);

	// Return Texture Index
	return texture;
}

// Changes Cursor Depending on Current Action
void ChangeCursor(GLFWwindow* window)
{
	// Switch Between Cursor Types
	switch (Selected_Cursor)
	{

		// Default Arrow Cursor
	case 0:
	{
		glfwSetCursor(window, Mouse_Textures.find("Cursor")->second);
		break;
	}

	// Horizontal Resize Cursor
	case 1:
	{
		glfwSetCursor(window, Mouse_Textures.find("ReSizeHorizontal")->second);
		break;
	}

	// Verticle Resize Cursor
	case 2:
	{
		glfwSetCursor(window, Mouse_Textures.find("ReSizeVertical")->second);
		break;
	}

	// The Cursor That Looks Like a Hand
	case 3:
	{
		glfwSetCursor(window, Mouse_Textures.find("Hand")->second);
		break;
	}

	// The Cursor That Types
	case 4:
	{
		glfwSetCursor(window, Mouse_Textures.find("Type")->second);
		break;
	}

	}

	Current_Cursor = Selected_Cursor;
}