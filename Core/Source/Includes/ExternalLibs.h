#pragma once
#ifndef EXTERNAL_LIBS_H
#define EXTERNAL_LIBS_H

// Standard Library
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <map>
#include <filesystem>
#include <Windows.h>

// OpenGL Libraries
#include <GLAD/glad.h>
#include <GLFW/GLFW/include/GLFW/glfw3.h>

#include <GLM/GLM/glm/glm.hpp>
#include <GLM/GLM/glm/gtc/matrix_transform.hpp>
#include <GLM/GLM/glm/gtc/type_ptr.hpp>

// OpenAL Libraries
#include <OPENAL/OPENAL/include/AL/al.h>
#include <OPENAL/OPENAL/include/AL/alc.h>

// Loaders 
#include "SOIL2/SOIL2/src/SOIL2/SOIL2.h"
#include <FREETYPE/FREETYPE/include/ft2build.h>
#include FT_FREETYPE_H

//#define GL_VERSION_4_5
#include "Includes/Khronos/GLEXT/glext.h"
#include "Includes/Khronos/GLEXT/wglext.h"

#endif
