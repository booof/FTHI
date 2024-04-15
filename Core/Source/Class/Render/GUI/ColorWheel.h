#pragma once
#ifndef COLOR_WHEEL_H
#define COLOR_WHEEL_H

#include "ExternalLibs.h"
#include "Render\GUI\Element.h"

namespace Render::GUI
{
	// Struct for Initial Color Wheel Data
	struct ColorWheelData
	{
		// Z-Pos of Wheel (Might be Removed Later)
		float position_z = -1.2f;

		// The Radius of the Wheel
		float radius = 1.0f;

		// Size of Specific Objects
		float bar_scale = 0.2f;

		// Position of the Brightness Object
		glm::vec2 brightness_pos = glm::vec2(-3.0f, 0.0f);

		// Position of the Alpha Object
		glm::vec2 alpha_pos = glm::vec2(-5.0f, 0.0f);

		// Position of the Sample Object
		glm::vec2 sample_pos = glm::vec2(-8.0f, 0.0f);

		// The Initial Color Selected by Object
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	// A Color Wheel Used to Change Colors
	class ColorWheel : public Element
	{
	public:

		ColorWheel(float xPos, float yPos, float zPos, float diameter, float brightnessOffset, float alphaOffset, float sampleOffset, float Scale, glm::vec4 color);

		ColorWheel(ElementData& data1, ColorWheelData& data2);

		// Default Constructor
		ColorWheel();

		// Get the Selected Color of the Wheel
		glm::vec4& getColor();

		// Takes Color Input and Determines Location on Wheel
		void FindColors(glm::vec4 colors);

		// Update the Element
		bool updateElement(); 

		// Move to Choose Colors of Wheel
		void UpdateWheel(double xPos, double yPos);

		// Use Brightness ScrollBar
		void UpdateBrightness(double yPos);

		// Use Alpha ScrollBar
		void UpdateAlpha(double yPos);

		// Test if Mouse is Indside Objects
		int TestCollisions(float xPos, float yPos, int IndexOffset);

		// Blitz Function
		void blitzElement();

		// Draw Object
		void Blitz(glm::mat4& model);

		// Test if Mouse Intersects Object
		bool testMouseCollisions(float x, float y);

		// Link Value Through a Pointer
		void linkValue(void* value_ptr);

		// Move the GUI Element
		void moveElement(float newX, float newY);

	private:

		// Creates the Color Vertices of Brightness and Alpha Objects
		void UpdateBrightnessColors();

		// Finalizes Color Creation with Alpha
		void UpdateAlphaColors();

		// Updates the Sample Color
		void UpdateSample();

		// Initialize the Wheel
		void initializeWheel();

		// The Color Wheel Data
		ColorWheelData data;

		// Model Matrices
		glm::mat4 wheelSelectorModel = glm::mat4(1.0f);
		glm::mat4 brightnessSelectorModel = glm::mat4(1.0f);
		glm::mat4 alphaSelectorModel = glm::mat4(1.0f);

		// The Selected Color on Wheel
		glm::vec4 WheelColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Local Selector Positions
		glm::vec2 wheelSelectorPos = glm::vec2(0.0f, 0.0f);
		glm::vec2 brightnessSelectorPos = glm::vec2(0.0f, 0.0f);
		glm::vec2 alphaSelectorPos = glm::vec2(0.0f, 0.0f);

		// Intermediate Alpha Value
		float Alpha = 1.0f;

		// Vertex Buffer Objects
		GLuint wheelVAO, wheelVBO;
		GLuint brightnessVAO, brightnessVBO;
		GLuint alphaVAO, alphaVBO;
		GLuint sampleVAO, sampleVBO;

		// Slider Vertex Buffer Objects
		GLuint wheelSelectorVAO, wheelSelectorVBO;
		GLuint brightnessSelectorVAO, brightnessSelectorVBO;
		GLuint alphaSelectorVAO, alphaSelectorVBO;
	};
}

#endif
