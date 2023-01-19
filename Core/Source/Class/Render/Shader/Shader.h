#pragma once
#ifndef SHADER_H
#define SHADER_H

#include "ExternalLibs.h"

namespace Shader
{
	// Shader Class
	class Shader
	{		
		// Shader Files
		GLuint Vertex, Geometry, Fragment;

		// Read Shader File
		std::string readFile(const GLchar* path);

		// Compile Vertex Shader
		void compileVertex(std::string string_code);

		// Compile Geometry Shader
		void compileGeometry(std::string string_code);

		// Compile Framgent Shader
		void compileFragment(std::string string_code);

		// Link Shader
		void linkShader();

	public:

		// Initialize Shader With Geometry Shader
		Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);

		// Initialize Shader Without Geometry Shader
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath);

		// Nullified Shader Class
		Shader();

		// Swap Vertex Shader
		void swapVertexShader(const GLchar* vertexPath);

		// Swap Geometry Shader
		void swapGeometryShader(const GLchar* geometryPath);

		// Swap Fragment Shader
		void swapFragmentShader(const GLchar* fragmentPath);

		// Switch to This Shader Program
		void Use();

		// Shader Program 
		GLuint Program;
	};
}

#endif

