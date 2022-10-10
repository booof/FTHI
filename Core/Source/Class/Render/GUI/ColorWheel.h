#ifndef COLOR_WHEEL_H
#define COLOR_WHEEL_H

#include "ExternalLibs.h"

// A Color Wheel Used to Change Colors
class ColorWheel
{
public:

	// The Selected Color
	glm::vec4 Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 WheelColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// The Diameter of the Wheel
	float Radius = 1.0f;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	float brightnessPosX = 0.0f;
	float alphaPosX = 0.0f;

	// Local Selector Positions
	glm::vec2 wheelSelectorPos = glm::vec2(0.0f, 0.0f);
	glm::vec2 brightnessSelectorPos = glm::vec2(0.0f, 0.0f);
	glm::vec2 alphaSelectorPos = glm::vec2(0.0f, 0.0f);
	glm::vec2 samplePos = glm::vec2(0.0f, 0.0f);

	// Scales the Size of Several Objects
	float BarScale = 1.0f;

	ColorWheel(float xPos, float yPos, float zPos, float diameter, float brightnessOffset, float alphaOffset, float sampleOffset, float Scale, glm::vec4 color);

	// Default Constructor
	ColorWheel();

	// Takes Color Input and Determines Location on Wheel
	void FindColors(glm::vec4 colors);

	// Move to Choose Colors of Wheel
	void UpdateWheel(double xPos, double yPos);

	// Use Brightness ScrollBar
	void UpdateBrightness(double yPos);

	// Use Alpha ScrollBar
	void UpdateAlpha(double yPos);

	// Test if Mouse is Indside Objects
	int TestCollisions(float xPos, float yPos, int IndexOffset);

	// Draw Object
	void Blitz(glm::mat4& model);

private:

	// Creates the Color Vertices of Brightness and Alpha Objects
	void UpdateBrightnessColors();

	// Finalizes Color Creation with Alpha
	void UpdateAlphaColors();

	// Updates the Sample Color
	void UpdateSample();

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

	// Model Matrices
	glm::mat4 wheelSelectorModel = glm::mat4(1.0f);
	glm::mat4 brightnessSelectorModel = glm::mat4(1.0f);
	glm::mat4 alphaSelectorModel = glm::mat4(1.0f);
};

#endif
