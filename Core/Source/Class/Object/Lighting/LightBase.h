#ifndef LIGHT_BASE_H
#define LIGHT_BASE_H

#ifndef DLL_HEADER
#include "Object/Object.h"
#include "ExternalLibs.h"
#include "Macros.h"
#include "Class/Render/Struct/Texture.h"
#endif

namespace Object::Light
{
	// Types of Light
	enum LIGHTS : unsigned char
	{
		DIRECTIONAL,
		POINT,
		SPOT,
		BEAM
	};

	// Object for Reading and Writing
	struct LightData
	{
		// Position of Object
		glm::vec2 position;

		// Ambient Lighting
		glm::vec4 ambient;

		// Diffuse Lighting
		glm::vec4 diffuse;

		// Specular Lighting
		glm::vec4 specular;

		// Intensity of Light
		float intensity;

		// Layer of Object
		int layer;

		// Script
		int script;
	};

	// Base Class for Lighting Objects
	class LightBase : public Object
	{
	protected:

		// Data
		LightData data;

		// Shape
		unsigned char shape;

		// If This Flag is Activated, Light Object Will be Resent to Shader
		bool send_light = false;

		// Load Light Into Shaders
		virtual void loadLight() = 0;

#ifdef EDITOR

		// Initialize Visualizer
		virtual void initializeVisualizer() = 0;

	public:

		// Draw Visualizer
		void blitzObject();

		// Vertex Objects
		GLuint VAO, VBO;

		// Model
		glm::mat4 model;

		// Texture
		Struct::SingleTexture texture;

		// Offset in Light Buffer
		uint16_t buffer_offset = 0;

#endif

	public:

		// Layer of Object, Called Z-Pos to Make it Compatible by Sorter
		// Idea: Lights Have Differing Z-Positions To Make Lighting Slightly Different. Z-Position Also Determines Layer Based on Layer Zpos Boundaries
		float zpos;

		// Return Layer
		int returnLayer();

		// Return Pointer to Position
		glm::vec2* pointerToPosition();
	};

	// Different Lighting Objects

	// Directional Object
	namespace Directional {
		class Directional; 
	}

	// Point Object
	namespace Point {
		class Point;
	}

	// Spot Object
	namespace Spot {
		class Spot;
	}

	// Beam Object
	namespace Beam {
		class Spot;
	}
}

#endif
