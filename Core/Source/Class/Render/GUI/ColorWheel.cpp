#include "ColorWheel.h"

// Globals
#include "Globals.h"

// Vertices
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Vertices/Circle/CircleVertices.h"
#include "Vertices/Line/LineVertices.h"

Render::GUI::ColorWheel::ColorWheel(float xPos, float yPos, float zPos, float diameter, float brightnessOffset, float alphaOffset, float sampleOffset, float Scale, glm::vec4 color)
{
	// Calculate Variables
	data.color = color;
	data.radius = diameter / 2.0f;
	data.bar_scale = Scale;
	element_data.position = glm::vec2(xPos, yPos);
	data.brightness_pos = glm::vec2(xPos + brightnessOffset, element_data.position.y);
	data.alpha_pos = glm::vec2(xPos + alphaOffset, element_data.position.y);
	data.sample_pos = glm::vec2(xPos + sampleOffset, yPos);
	brightnessSelectorPos.x = data.brightness_pos.x;
	alphaSelectorPos.x = data.alpha_pos.x;

	// Store Storage Type
	{
		using namespace Object;
		storage_type = ELEMENT_COUNT;
	}

	// Create Wheel Object
	initializeWheel();
}

Render::GUI::ColorWheel::ColorWheel(ElementData& data1, ColorWheelData& data2)
{
	// Store Values
	element_data = data1;
	data = data2;

	// Create Wheel Object
	initializeWheel();
}

Render::GUI::ColorWheel::ColorWheel()
{
	WheelColor = glm::vec4(NULL, NULL, NULL, NULL);
	wheelVAO = NULL; wheelVBO = NULL;
	brightnessVAO = NULL; brightnessVBO = NULL;
	alphaVAO = NULL; alphaVBO = NULL;
	sampleVAO = NULL; sampleVBO = NULL;
	wheelSelectorVAO = NULL; wheelSelectorVBO = NULL;
	brightnessSelectorVAO = NULL; brightnessSelectorVBO = NULL;
	alphaSelectorVAO = NULL; alphaSelectorVBO = NULL;
}

glm::vec4& Render::GUI::ColorWheel::getColor()
{
	return data.color;
}

void Render::GUI::ColorWheel::FindColors(glm::vec4 colors)
{
	// Assign New Color Data
	data.color = colors;

	// Normalize Colors
	colors.x *= 255;
	colors.y *= 255;
	colors.z *= 255;

	// Calculate Alpha
	Alpha = colors.w;
	alphaSelectorPos.y = (1 - Alpha) * (data.radius * 2);

	// Remove Brightness Effect if Neccessary
	if (colors.x < 255 && colors.y < 255 && colors.z < 255)
	{
		// Scale Factor to Bring Color to Full Brightness
		double ScaleFactor = 1.0f;

		// Red is Highest
		if (colors.x > colors.y && colors.x > colors.z)
		{
			ScaleFactor = colors.x / 255;
		}

		// Green is Highest
		else if (colors.y > colors.x && colors.y > colors.z)
		{
			ScaleFactor = colors.y / 255;
		}

		// Blue is Highest
		else
		{
			ScaleFactor = colors.z / 255;
		}

		// Calculate New Colors
		if (ScaleFactor != 0)
		{
			colors.x = (float)(colors.x / ScaleFactor);
			colors.y = (float)(colors.y / ScaleFactor);
			colors.z = (float)(colors.z / ScaleFactor);
		}

		// Calculate Brightness Selector Position
		brightnessSelectorPos.y = (float)((data.radius * 2) * (1 - ScaleFactor));
	}

	// Assign Base Wheel Color
	WheelColor = glm::vec4(colors.x / 255, colors.y / 255, colors.z / 255, 1.0f);

	// Determine if Input is Not Centered
	if (colors.x == colors.y && colors.y == colors.z)
	{
		wheelSelectorPos = glm::vec2(0.0f, 0.0f);
		WheelColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// Selector is Elsewhere on Wheel
	else
	{
		// Calculate Scale Factor
		double ScaleFactor = 255 / data.radius;

		// Distance Between Position and Selector Object
		double Distance = 0.0f;

		// The Angle of Selector Object
		double theta = 0.0f;

		// Red is Lowest 
		if (colors.x < colors.y && colors.x < colors.z)
		{
			// Calculate Distance
			Distance = data.radius - (colors.x) / ScaleFactor;

			// Test if Green and Blue are Both Full
			if (colors.y == 255 && colors.z == 255)
			{
				theta = 3.1416;
			}

			// Green is Full (Blue Rising)
			else if (colors.y == 255)
			{
				theta = 3.1416 - (255 - (double)colors.z + (data.radius - Distance) * ScaleFactor) / 255;
			}

			// Blue is Full (Green Falling)
			else
			{
				theta = 4.1888 - (colors.y - (data.radius - Distance) * ScaleFactor) / 255;
			}
		}

		// Green is Lowest
		else if (colors.y < colors.x && colors.y < colors.z)
		{
			// Calculate Distance
			Distance = data.radius - (colors.y) / ScaleFactor;

			// Test if Red and Blue are Both Full
			if (colors.x == 255 && colors.z == 255)
			{
				theta = 5.236;
			}

			// Red is Full (Blue Falling)
			else if (colors.x == 255)
			{
				theta = 6.2832 - (colors.z - (data.radius - Distance) * ScaleFactor) / 255;
			}

			// Blue is Full (Red Rising)
			else
			{
				theta = 5.236 - (255 - (double)colors.x + (data.radius - Distance) * ScaleFactor) / 255;
			}
		}

		// Blue is Lowest
		else
		{
			// Caclulate Distance
			Distance = data.radius - (colors.z) / ScaleFactor;

			// Test if Red and Green are Both Full
			if (colors.x == 255 && colors.y == 255)
			{
				theta = 1.0472;
			}

			// Red is Full (Green Rising)
			else if (colors.x == 255)
			{
				theta = 1.0472 - (255 - (double)colors.y + (data.radius - Distance) * ScaleFactor) / 255;
			}

			// Green is Full (Red Falling)
			else
			{
				theta = 2.0944 - (colors.x - (data.radius - Distance) * ScaleFactor) / 255;
			}
		}

		// Calculate Positions
		float newX = (float)(Distance * cos(theta));
		float newY = (float)(Distance * sin(theta));

		// Send Positions to Selector Position
		wheelSelectorPos = glm::vec2(newX, newY);
	}

	// Update Colors
	UpdateBrightnessColors();
}

bool Render::GUI::ColorWheel::updateElement()
{
	// Get the Mouse Positions
	double xPos = gui_mouse_position.x;
	double yPos = gui_mouse_position.y;

	// Determine Index of Collision
	int index = TestCollisions(xPos, yPos, 1);

	// Update Specified Part Based on Index
	switch (index)
	{
	case 1: UpdateWheel(xPos, yPos); break;
	case 2: UpdateBrightness(yPos); break;
	case 3: UpdateAlpha(yPos);
	}

	// Return Index is Not 0
	return index != 0;
}

void Render::GUI::ColorWheel::UpdateWheel(double xPos, double yPos)
{
	// Set Cursor to Hand
	Global::Selected_Cursor = Global::CURSORS::HAND;

	// Calculate Position on Circle

	// If the Distance from the Position is Greater than the Radius, Make the Selector Position 
	if (pow(pow(xPos - element_data.position.x, 2) + pow(yPos - element_data.position.y, 2), 0.5) > (double)data.radius)
	{
		// Calculate the Angle From the Position to Origin
		double theta = atan((yPos - element_data.position.y) / (xPos - element_data.position.x));

		// The New X and Y Coordinates
		double newX, newY;

		// Test if xPos is Right of Origin
		if (xPos > element_data.position.x)
		{
			// Caclulate the new X and Y
			newY = data.radius * sin(theta);
			newX = data.radius * cos(theta);
		}

		// Test if xPos is Left of Origin
		else
		{
			// Caclulate the new X and Y
			newY = -data.radius * sin(theta);
			newX = -data.radius * cos(theta);
		}

		// Assign new Position
		wheelSelectorPos = glm::vec2(newX, newY);
	}

	// The Selector Position is Equal to the Relative Position if Inside Circle
	else
	{
		wheelSelectorPos = glm::vec2(xPos - element_data.position.x, yPos - element_data.position.y);
	}

	// The Angle on the Circle
	double theta = atan((-wheelSelectorPos.y) / (-wheelSelectorPos.x));

	// If Selector X is Left of Origin, add PI to Theat
	if (wheelSelectorPos.x <= 0)
	{
		theta += 3.1416;

		// Selector X is Origin
		if (wheelSelectorPos.x == 0)
		{
			// Selector Y is Above Origin
			if (wheelSelectorPos.y > 0)
			{
				theta = 1.5708;
			}

			// Selector Y is Below Origin
			else
			{
				theta = 4.7124;
			}
		}
	}

	// If Selector is in Quadrent 4, add 2 PI
	else if (wheelSelectorPos.y < 0)
	{
		theta += 6.2832;
	}

	// Calculate Color Red
	int Red = 0;

	// Red is Full
	if (theta <= 1.047 || theta >= 5.236)
	{
		Red = 255;
	}

	// Red is Dynamic
	else
	{
		// Calculate Scale Factor
		double ScaleFactor = 255 / data.radius;

		// Calculate Distance From Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Red is Empty
		if (theta >= 2.094 && theta <= 4.189)
		{
			Red = (int)((data.radius - Distance) * ScaleFactor);

		}

		// Red is in Decay
		else if (theta > 1.047 && theta < 2.094)
		{
			Red = (int)((data.radius - Distance) * ScaleFactor + (255 * (2.094 - theta)));

			// Stop Red From Going Beyond 255
			if (Red > 255)
			{
				Red = 255;
			}
		}

		// Red is Rising
		else
		{
			Red = (int)((data.radius - Distance) * ScaleFactor + (255 - (255 * (5.236 - theta))));

			// Stop Red From Going Below 0
			if (Red < 0)
			{
				Red = 0;
			}

			// Stop Red From Going Beyond 255
			else if (Red > 255)
			{
				Red = 255;
			}
		}
	}


	// Calculate Color Green
	int Green = 0;

	// Green is Full
	if (theta >= 1.047 && theta <= 3.142)
	{
		Green = 255;
	}

	// Green is Dynamic
	else
	{
		// Calculate Scale Factor
		double ScaleFactor = 255 / data.radius;

		// Calculate Distance from Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Green is Empty
		if (theta >= 4.189)
		{
			Green = (int)((data.radius - Distance) * ScaleFactor);
		}

		// Green is in Decay
		else if (theta > 3.142 && theta < 4.189)
		{
			Green = (int)((data.radius - Distance) * ScaleFactor + (255 * (4.189 - theta)));

			// Stop Green From Going Beyond 255
			if (Green > 255)
			{
				Green = 255;
			}
		}

		// Green is Rising
		else
		{
			Green = (int)((data.radius - Distance) * ScaleFactor + (255 - (255 * (1.047 - theta))));

			// Stop Green From Going Below 0
			if (Green < 0)
			{
				Green = 0;
			}

			// Stop Green From Going Beyond 255
			else if (Green > 255)
			{
				Green = 255;
			}
		}
	}

	// Calculate Color Blue
	int Blue = 0;

	// Blue is Full
	if (theta >= 3.142 && theta <= 5.236)
	{
		Blue = 255;
	}

	// Blue is Dynamic
	else
	{
		// Calculate Scale Factor
		double ScaleFactor = 255 / data.radius;

		// Calculate Distance From Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Blue is Empty
		if (theta <= 2.094)
		{
			Blue = (int)((data.radius - Distance) * ScaleFactor);
		}

		// Blue is in Decay
		else if (theta > 5.236)
		{
			Blue = (int)((data.radius - Distance) * ScaleFactor + (255 * (6.283 - theta)));

			// Stop Blue From Going Beyond 255
			if (Blue > 255)
			{
				Blue = 255;
			}
		}

		// Blue is Rising
		else
		{
			Blue = (int)((data.radius - Distance) * ScaleFactor + (255 - (255 * (3.142 - theta))));

			// Stop Blue From Going Below 0
			if (Blue < 0)
			{
				Blue = 0;
			}

			// Stop Blue From Going Beyond 255
			else if (Blue > 255)
			{
				Blue = 255;
			}
		}
	}

	// Calculate Color Scale Factor
	float newRed = (float)Red / 255.0f;
	float newGreen = (float)Green / 255.0f;
	float newBlue = (float)Blue / 255.0f;

	// Assign Colors to Color Variable
	WheelColor = glm::vec4(newRed, newGreen, newBlue, 1.0f);

	// Update Colors
	UpdateBrightnessColors();
}

void Render::GUI::ColorWheel::UpdateBrightness(double yPos)
{
	// Calculate Brightness Selector Offset
	brightnessSelectorPos.y = (float)((element_data.position.y + data.radius) - yPos);

	// Prevent Offset From Going Less Than Minimum Value
	if (brightnessSelectorPos.y < 0)
	{
		brightnessSelectorPos.y = 0;
	}

	// Prevent Offset From Goint Greater Than Minimum Value
	else if (brightnessSelectorPos.y > 2 * data.radius)
	{
		brightnessSelectorPos.y = 2 * data.radius;
	}

	// Update Colors
	UpdateBrightnessColors();
}

void Render::GUI::ColorWheel::UpdateAlpha(double yPos)
{
	// Calculate Alpha Selector Offset
	alphaSelectorPos.y = (float)((element_data.position.y + data.radius) - yPos);

	// Prevent Offset From Going Less Than Minimum Value
	if (alphaSelectorPos.y < 0)
	{
		alphaSelectorPos.y = 0;
	}

	// Prevent Offset From Goint Greater Than Minimum Value
	else if (alphaSelectorPos.y > 2 * data.radius)
	{
		alphaSelectorPos.y = 2 * data.radius;
	}

	// Update Colors
	UpdateAlphaColors();
}

int Render::GUI::ColorWheel::TestCollisions(float xPos, float yPos, int IndexOffset)
{
	// Test if MouseX is Inside Wheel
	if (xPos >= element_data.position.x - data.radius && xPos <= element_data.position.x + data.radius)
	{
		// Calculate the Half Secant at MouseX
		float halfSecant = (float)pow(pow(data.radius, 2) - pow((xPos - element_data.position.x), 2), 0.5);

		// Test if MouseY is Inside Wheel
		if (yPos >= element_data.position.y - halfSecant && yPos <= element_data.position.y + halfSecant)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If True, Return Index Offset
			return IndexOffset;
		}
	}

	// Test if MouseY is Inside Brightness and Alpha Bar
	else if (yPos >= element_data.position.y - data.radius && yPos <= element_data.position.y + data.radius)
	{
		// Test if MouseX is Inside Brightness Bar
		if (xPos >= brightnessSelectorPos.x - 1.5 * data.bar_scale && xPos <= brightnessSelectorPos.x + 1.5 * data.bar_scale)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If True, Return Index Offset Plus 1
			return IndexOffset + 1;
		}

		// Test if MouseY is Inside Alpha Bar
		else if (xPos >= alphaSelectorPos.x - 1.5 * data.bar_scale && xPos <= alphaSelectorPos.x + 1.5 * data.bar_scale)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If True, Return Index Offset Plus 2
			return IndexOffset + 2;
		}
	}

	// Return 0 if no Collisions Occoured
	return 0;
}

void Render::GUI::ColorWheel::blitzElement()
{
	// Generate the Model (Should be the Identity Since Model is Activated by Master)
	glm::mat4 model = glm::mat4(1.0f);

	// Blitz the Wheel
	Blitz(model);
}

void Render::GUI::ColorWheel::Blitz(glm::mat4& model)
{
	// Draw Wheel and Bar Outlines
	//glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 0.0f);
	glBindVertexArray(wheelVAO);
	glDrawArrays(GL_TRIANGLES, 0, 1128);
	glBindVertexArray(0);

	// Draw Brightness Bar
	glBindVertexArray(brightnessVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw Alpha Bar
	glBindVertexArray(alphaVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw Sample Bar
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, Alpha);
	glBindVertexArray(sampleVAO);
	glDrawArrays(GL_TRIANGLES, 0, 12);
	glBindVertexArray(0);
	glUniform4f(Global::brightnessLoc, 0.0f, 0.0f, 0.0f, 1.0f);

	// Calculate Selector Model
	wheelSelectorModel = glm::translate(model, glm::vec3(wheelSelectorPos.x + element_data.position.x, wheelSelectorPos.y + element_data.position.y, 0.0f));
	brightnessSelectorModel = glm::translate(glm::mat4(1.0f), glm::vec3(brightnessSelectorPos.x, element_data.position.y + data.radius - brightnessSelectorPos.y, 0.0f)) * model;
	alphaSelectorModel = glm::translate(glm::mat4(1.0f), glm::vec3(alphaSelectorPos.x, element_data.position.y + data.radius - alphaSelectorPos.y, 0.0f)) * model;

	// Draw Wheel Selector Object
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(wheelSelectorModel));
	glBindVertexArray(wheelSelectorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 30);
	glBindVertexArray(0);

	// Draw Brightness Selector Object
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(brightnessSelectorModel));
	glBindVertexArray(brightnessSelectorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// Draw Alpha Selector Object
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(alphaSelectorModel));
	glBindVertexArray(alphaSelectorVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

bool Render::GUI::ColorWheel::testMouseCollisions(float x, float y)
{
	// Determine Index of Collision
	int index = TestCollisions(x, y, 1);

	// Return If Index is Not Zero (Means Collision Occoured)
	return index != 0;
}

void Render::GUI::ColorWheel::linkValue(void* value_ptr)
{
	// TODO: Link Color Output to Pointer
}

void Render::GUI::ColorWheel::moveElement(float newX, float newY)
{
	// Calculate the Change in Position
	float deltaX = newX - element_data.position.x;
	float deltaY = newY - element_data.position.y;

	// Update Coordinates of Object
	element_data.position.x = newX;
	element_data.position.y = newY;

	// Update the Selectors for All Parts
	wheelSelectorPos.x += deltaX;
	wheelSelectorPos.y += deltaY;
	brightnessSelectorPos.x += deltaX;
	brightnessSelectorPos.y += deltaY;
	alphaSelectorPos.x += deltaX;
	alphaSelectorPos.y += deltaY;
}

void Render::GUI::ColorWheel::UpdateBrightnessColors()
{
	// Re-Calculate Vertices

	// Specalized Vertices for Brightness Object
	float vertices[] =
	{
		// Position                                                                                                  // Colors
		data.brightness_pos.x - 1.5f * data.bar_scale, data.brightness_pos.y + data.radius, data.position_z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f,  // Top Left
		data.brightness_pos.x - 1.5f * data.bar_scale, data.brightness_pos.y - data.radius, data.position_z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Left
		data.brightness_pos.x + 1.5f * data.bar_scale, data.brightness_pos.y - data.radius, data.position_z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Right

		data.brightness_pos.x + 1.5f * data.bar_scale, data.brightness_pos.y - data.radius, data.position_z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Right
		data.brightness_pos.x + 1.5f * data.bar_scale, data.brightness_pos.y + data.radius, data.position_z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f,  // Top Right
		data.brightness_pos.x - 1.5f * data.bar_scale, data.brightness_pos.y + data.radius, data.position_z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f   // Top Left
	};

	// Bind Brightness Vertex Objects
	glBindVertexArray(brightnessVAO);
	glBindBuffer(GL_ARRAY_BUFFER, brightnessVBO);

	// Assign Vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Brightness Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Calculate New Color

	// The Scaled Brightness Value to Calculate Real Color
	float BrightnessScale = 1 - (brightnessSelectorPos.y / (2 * data.radius));

	// New Color Values
	float newRed = (WheelColor.x * 255.0f) * BrightnessScale;
	float newGreen = (WheelColor.y * 255.0f) * BrightnessScale;
	float newBlue = (WheelColor.z * 255.0f) * BrightnessScale;

	// Prevent Red From Going Negative
	if (newRed < 0)
	{
		newRed = 0;
	}

	// Prevent Green From Going Negative
	if (newGreen < 0)
	{
		newGreen = 0;
	}

	// Prevent Blue From Going Negative
	if (newBlue < 0)
	{
		newBlue = 0;
	}

	// Assign Colors to Main Color Variable
	data.color = glm::vec4(newRed / 255.0f, newGreen / 255.0f, newBlue / 255.0f, Alpha);

	// Update Colors for Alpha and Sample
	UpdateAlphaColors();
}

void Render::GUI::ColorWheel::UpdateAlphaColors()
{
	// Recalculate Vertices

	// Specialized Vertices for Alpha Object
	float vertices[] =
	{
		// Position                                                            // Colors
		data.alpha_pos.x - 1.5f * data.bar_scale, data.alpha_pos.y + data.radius, data.position_z - 0.1f,  data.color.x, data.color.y, data.color.z, 1.0f,  // Top Left
		data.alpha_pos.x - 1.5f * data.bar_scale, data.alpha_pos.y - data.radius, data.position_z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Left
		data.alpha_pos.x + 1.5f * data.bar_scale, data.alpha_pos.y - data.radius, data.position_z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Right

		data.alpha_pos.x + 1.5f * data.bar_scale, data.alpha_pos.y - data.radius, data.position_z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Right
		data.alpha_pos.x + 1.5f * data.bar_scale, data.alpha_pos.y + data.radius, data.position_z - 0.1f,  data.color.x, data.color.y, data.color.z, 1.0f,  // Top Right
		data.alpha_pos.x - 1.5f * data.bar_scale, data.alpha_pos.y + data.radius, data.position_z - 0.1f,  data.color.x, data.color.y, data.color.z, 1.0f   // Top Left
	};

	// Bind Object
	glBindVertexArray(alphaVAO);
	glBindBuffer(GL_ARRAY_BUFFER, alphaVBO);

	// Assign Vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Calculate Alpha

	// The Scaled Alpha Value to Calculate Real Color
	Alpha = 1 - (alphaSelectorPos.y / (data.radius * 2));

	// Send Alpha Value to Complete Color
	data.color.w = Alpha;

	// Update Colors for Sample
	UpdateSample();
}

void Render::GUI::ColorWheel::UpdateSample()
{
	// Temporary Vertices Array
	float vertices[42];

	// Bind Vertex Objects
	glBindVertexArray(sampleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sampleVBO);

	// Backdrop Vertices
	Vertices::Rectangle::genRectColor(data.sample_pos.x, data.sample_pos.y, data.position_z, data.radius * 2, data.radius * 2, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 42 * sizeof(GLfloat), vertices);

	// Sample Vertices
	Vertices::Rectangle::genRectColor(data.sample_pos.x, data.sample_pos.y, data.position_z + 0.1f, data.radius * 2 - 2, data.radius * 2 - 1, data.color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 42 * sizeof(GLfloat), 42 * sizeof(GLfloat), vertices);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Render::GUI::ColorWheel::initializeWheel()
{
	// Generate Wheel Vertex Objects
	glGenVertexArrays(1, &wheelVAO);
	glGenBuffers(1, &wheelVBO);

	// Bind Wheel Vertex Array Object
	glBindVertexArray(wheelVAO);

	// Bind Wheel Vertex Buffer Object and Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, wheelVBO);
	glBufferData(GL_ARRAY_BUFFER, 7896 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);

	// Temporary Vertices for Outline Objects
	float vertices[168];

	// Alpha Rectangle Outline
	Vertices::Line::genLineOutlineRect(data.alpha_pos.x - 1.5f * data.bar_scale, data.alpha_pos.x + 1.5f * data.bar_scale, data.alpha_pos.y - data.radius, data.alpha_pos.y + data.radius, data.position_z, 0.2f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168 * sizeof(GLfloat), vertices);

	// Brightness Rectangle Outline
	Vertices::Line::genLineOutlineRect(data.brightness_pos.x - 1.5f * data.bar_scale, data.brightness_pos.x + 1.5f * data.bar_scale, data.brightness_pos.y - data.radius, data.brightness_pos.y + data.radius, data.position_z, 0.2f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 168 * sizeof(GLfloat), 168 * sizeof(GLfloat), vertices);

	// The Wheel Vertices
	float* CircleVertices = new float[7560];
	Vertices::Circle::genCircleColorWheel(element_data.position.x, element_data.position.y, data.position_z, data.radius, 360, CircleVertices);
	glBufferSubData(GL_ARRAY_BUFFER, 336 * sizeof(GLfloat), 7560 * sizeof(GLfloat), CircleVertices);
	delete[] CircleVertices;

	// Assign Position Vertices to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Wheel Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Wheel Array Object
	glBindVertexArray(0);


	// Create Brightness Object

	// Generate Brightness Vertex Objects
	glGenVertexArrays(1, &brightnessVAO);
	glGenBuffers(1, &brightnessVBO);

	// Bind Brightness VAO
	glBindVertexArray(brightnessVAO);

	// Bind Brightness VBO and Assign Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, brightnessVBO);
	glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	// Assign Position Vertices to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Brightness VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Brightness VAO
	glBindVertexArray(0);


	// Create Alpha Object

	// Generate Alpha Vertex Objects
	glGenVertexArrays(1, &alphaVAO);
	glGenBuffers(1, &alphaVBO);

	// Bind Alpha VAO
	glBindVertexArray(alphaVAO);

	// Bind Alpha VBO and Assign Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, alphaVBO);
	glBufferData(GL_ARRAY_BUFFER, 42 * sizeof(vertices), NULL, GL_DYNAMIC_DRAW);

	// Assign Position Vertices to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Alpha VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Alpha VAO
	glBindVertexArray(0);


	// Create Sample Object

	// Generate Sample Vertex Objects
	glGenVertexArrays(1, &sampleVAO);
	glGenBuffers(1, &sampleVBO);

	// Bind Sample VAO
	glBindVertexArray(sampleVAO);

	// Bind Sample VBO and Assign Nullified Vertices
	glBindBuffer(GL_ARRAY_BUFFER, sampleVBO);
	glBufferData(GL_ARRAY_BUFFER, 84 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

	// Assign Position Data to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Data to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Sample Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Sample Array Object
	glBindVertexArray(0);


	// Create Wheel Selector Object

	// Generate Wheel Selector Vertices
	float wheelSelectorVertices[210];
	Vertices::Circle::genCircleColorFull(0.0f, 0.0f, -1.1f, 1.0f, 10, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), wheelSelectorVertices);

	// Generate Wheel Selector Vertex Objects
	glGenVertexArrays(1, &wheelSelectorVAO);
	glGenBuffers(1, &wheelSelectorVBO);

	// Bind Wheel Selector VAO
	glBindVertexArray(wheelSelectorVAO);

	// Bind Wheel Selector VBO and Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, wheelSelectorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wheelSelectorVertices), wheelSelectorVertices, GL_STATIC_DRAW);

	// Send Position Data to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Send Color Data to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Wheel Selector VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Wheel Selector VAO
	glBindVertexArray(0);

	// Selector Vertices
	float smallVertices[42];
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, data.position_z, 3.2f * data.bar_scale, 1, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), smallVertices);

	// Create Brightness Selector Object

	// Generate Brightness Selector Vertex Objects
	glGenVertexArrays(1, &brightnessSelectorVAO);
	glGenBuffers(1, &brightnessSelectorVBO);

	// Bind Brightness Selector VAO
	glBindVertexArray(brightnessSelectorVAO);

	// Bind Brightness Selector VBO and Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, brightnessSelectorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallVertices), smallVertices, GL_STATIC_DRAW);

	// Assign Position Vertices to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Brightness Selector VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Brightness Selector VAO
	glBindVertexArray(0);


	// Create Alpha Selector Object

	// Generate Alpha Selector Vertex Objects
	glGenVertexArrays(1, &alphaSelectorVAO);
	glGenBuffers(1, &alphaSelectorVBO);

	// Bind Alpha Selector VAO
	glBindVertexArray(alphaSelectorVAO);

	// Bind Alpha Selector VBO and Bind Vertices
	glBindBuffer(GL_ARRAY_BUFFER, alphaSelectorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallVertices), smallVertices, GL_STATIC_DRAW);

	// Assign Position Verices to Shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(0));
	glEnableVertexAttribArray(0);

	// Assign Color Vertices to Shader
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind Alpha Selector VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind Alpha Selector VAO
	glBindVertexArray(0);

	// If Color is Not Default, Find it
	if (data.color.x != 1 || data.color.y != 1 || data.color.z != 1 || data.color.w != 1)
	{
		FindColors(data.color);
	}

	// Create Initial Color Vertices
	else
	{
		UpdateBrightnessColors();
	}
}
