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
#include <GLFW/include/GLFW/glfw3.h>

#include <../Com/GLM/glm.hpp>
#include <../Com/GLM/gtc/matrix_transform.hpp>
#include <../Com/GLM/gtc/type_ptr.hpp>

// OpenAL Libraries
#include <OPENAL/include/AL/al.h>
#include <OPENAL/include/AL/alc.h>

// Loaders 
#include "SOIL2.h"
#include <ft2build.h>
#include FT_FREETYPE_H

//#define GL_VERSION_4_5
#include "Includes/Khronos/GLEXT/glext.h"
#include "Includes/Khronos/GLEXT/wglext.h"

#endif
