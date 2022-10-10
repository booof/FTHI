#include "Shader.h"

std::string Shader::Shader::readFile(const GLchar* path)
{
	std::cout << "Compiling Shader: " << path << "\n";

	// File Object
	std::ifstream file;

	// Temporary Holder for Raw Code
	std::string code;

	// File Stream
	std::stringstream file_stream;
	
	// File Exception Handler
	file.exceptions(std::ifstream::badbit);

	// Read Shader File, Throw Error if File Does Not Exist
	try
	{
		// Open File
		file.open(path);

		// Read File
		file_stream << file.rdbuf();

		// Close File
		file.close();

		// Turn Data Into a String
		code = file_stream.str();
	}
	
	// Display Error if Files are Unreadable
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n";
	}

	// Get Const Char of Code
	return code;
}

void Shader::Shader::compileVertex(std::string string_code)
{
	// Conver Code to Const Char*
	const char* code = string_code.c_str();

	GLint success;
	GLchar infoLog[512];

	// Compile Vertex Shader
	Vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Vertex, 1, &code, NULL);
	glCompileShader(Vertex);

	// Vertex Shader Error
	glGetShaderiv(Vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	// Link Vertex Shader
	glAttachShader(Program, Vertex);
}

void Shader::Shader::compileGeometry(std::string string_code)
{
	// Conver Code to Const Char*
	const char* code = string_code.c_str();

	GLint success;
	GLchar infoLog[512];

	// Compile Geometry Shader
	Geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(Geometry, 1, &code, NULL);
	glCompileShader(Geometry);

	// Geometry Shader Error
	glGetShaderiv(Geometry, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Geometry, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	// Link Geometry Shader
	glAttachShader(Program, Geometry);
}

void Shader::Shader::compileFragment(std::string string_code)
{
	// Conver Code to Const Char*
	const char* code = string_code.c_str();

	GLint success;
	GLchar infoLog[512];

	// Compile Fragment Shader
	Fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fragment, 1, &code, NULL);
	glCompileShader(Fragment);

	// Fragment Shader Error
	glGetShaderiv(Fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(Fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
	}

	// Link Fragment Shader
	glAttachShader(Program, Fragment);
}

void Shader::Shader::linkShader()
{
	GLint success;
	GLchar infoLog[512];

	// Link Shader
	glLinkProgram(Program);

	// Link Error
	glGetProgramiv(Program, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(Program, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
	}
}

Shader::Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
{
	// Create Shader Program
	Program = glCreateProgram();

	// Compile Vertex Shader
	compileVertex(readFile(vertexPath));

	// Compile Geometry Shader
	compileGeometry(readFile(geometryPath));

	// Compile Fragment Shader
	compileFragment(readFile(fragmentPath));

	// Link Shader
	linkShader();
}

Shader::Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	// Create Shader Program
	Program = glCreateProgram();

	// Compile Vertex Shader
	compileVertex(readFile(vertexPath));

	// Compile Fragment Shader
	compileFragment(readFile(fragmentPath));

	// Link Shader
	linkShader();
}

Shader::Shader::Shader()
{
}

void Shader::Shader::swapVertexShader(const GLchar* vertexPath)
{
	// Remove Vertex Shader
	glDetachShader(Program, Vertex);
	glDeleteShader(Vertex);

	// Compile Vertex Shader
	compileVertex(readFile(vertexPath));

	// Link Shader
	linkShader();
}

void Shader::Shader::swapGeometryShader(const GLchar* geometryPath)
{
	// Remove Geometry Shader
	glDetachShader(Program, Geometry);
	glDeleteShader(Geometry);

	// Compile Geometry Shader
	compileGeometry(readFile(geometryPath));

	// Link Shader
	linkShader();
}

void Shader::Shader::swapFragmentShader(const GLchar* fragmentPath)
{
	// Remove Fragment Shader
	glDetachShader(Program, Fragment);
	glDeleteShader(Fragment);

	// Compile Fragment Shader
	compileFragment(readFile(fragmentPath));

	// Link Shader
	linkShader();
}

void Shader::Shader::Use()
{
	glUseProgram(Program);
}
