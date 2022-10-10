#include "ColorWheel.h"

// Globals
#include "Globals.h"

// Vertices
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Vertices/Circle/CircleVertices.h"
#include "Vertices/Line/LineVertices.h"

ColorWheel::ColorWheel(float xPos, float yPos, float zPos, float diameter, float brightnessOffset, float alphaOffset, float sampleOffset, float Scale, glm::vec4 color)
{
	// Calculate Variables
	Color = color;
	Radius = diameter / 2;
	samplePos = glm::vec2(xPos + sampleOffset, yPos);
	BarScale = Scale;

	brightnessPosX = xPos + brightnessOffset;
	alphaPosX = xPos + alphaOffset;

	position = glm::vec3(xPos, yPos, zPos);
	brightnessSelectorPos.x = brightnessPosX;
	alphaSelectorPos.x = alphaPosX;

	// Create Wheel Object

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
	Vertices::Line::genLineOutlineRect(alphaSelectorPos.x - 1.5f * BarScale, alphaSelectorPos.x + 1.5f * BarScale, yPos - Radius, yPos + Radius, zPos, 0.2f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 168 * sizeof(GLfloat), vertices);

	// Brightness Rectangle Outline
	Vertices::Line::genLineOutlineRect(brightnessSelectorPos.x - 1.5f * BarScale, brightnessSelectorPos.x + 1.5f * BarScale, yPos - Radius, yPos + Radius, zPos, 0.2f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 168 * sizeof(GLfloat), 168 * sizeof(GLfloat), vertices);

	// The Wheel Vertices
	float* CircleVertices = new float[7560];
	Vertices::Circle::genCircleColorWheel(xPos, yPos, zPos, Radius, 360, CircleVertices);
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
	Vertices::Rectangle::genRectColor(0.0f, 0.0f, zPos, 3.2f * BarScale, 1, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), smallVertices);

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
	if (Color.x != 1 || Color.y != 1 || Color.z != 1 || Color.w != 1)
	{
		FindColors(Color);
	}

	// Create Initial Color Vertices
	else
	{
		UpdateBrightnessColors();
	}
}

ColorWheel::ColorWheel()
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

void ColorWheel::FindColors(glm::vec4 colors)
{
	// Assign New Color Data
	Color = colors;

	// Normalize Colors
	colors.x *= 255;
	colors.y *= 255;
	colors.z *= 255;

	// Calculate Alpha
	Alpha = colors.w;
	alphaSelectorPos.y = (1 - Alpha) * (Radius * 2);

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
		brightnessSelectorPos.y = (float)((Radius * 2) * (1 - ScaleFactor));
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
		double ScaleFactor = 255 / Radius;

		// Distance Between Position and Selector Object
		double Distance = 0.0f;

		// The Angle of Selector Object
		double theta = 0.0f;

		// Red is Lowest 
		if (colors.x < colors.y && colors.x < colors.z)
		{
			// Calculate Distance
			Distance = Radius - (colors.x) / ScaleFactor;

			// Test if Green and Blue are Both Full
			if (colors.y == 255 && colors.z == 255)
			{
				theta = 3.1416;
			}

			// Green is Full (Blue Rising)
			else if (colors.y == 255)
			{
				theta = 3.1416 - (255 - (double)colors.z + (Radius - Distance) * ScaleFactor) / 255;
			}

			// Blue is Full (Green Falling)
			else
			{
				theta = 4.1888 - (colors.y - (Radius - Distance) * ScaleFactor) / 255;
			}
		}

		// Green is Lowest
		else if (colors.y < colors.x && colors.y < colors.z)
		{
			// Calculate Distance
			Distance = Radius - (colors.y) / ScaleFactor;

			// Test if Red and Blue are Both Full
			if (colors.x == 255 && colors.z == 255)
			{
				theta = 5.236;
			}

			// Red is Full (Blue Falling)
			else if (colors.x == 255)
			{
				theta = 6.2832 - (colors.z - (Radius - Distance) * ScaleFactor) / 255;
			}

			// Blue is Full (Red Rising)
			else
			{
				theta = 5.236 - (255 - (double)colors.x + (Radius - Distance) * ScaleFactor) / 255;
			}
		}

		// Blue is Lowest
		else
		{
			// Caclulate Distance
			Distance = Radius - (colors.z) / ScaleFactor;

			// Test if Red and Green are Both Full
			if (colors.x == 255 && colors.y == 255)
			{
				theta = 1.0472;
			}

			// Red is Full (Green Rising)
			else if (colors.x == 255)
			{
				theta = 1.0472 - (255 - (double)colors.y + (Radius - Distance) * ScaleFactor) / 255;
			}

			// Green is Full (Red Falling)
			else
			{
				theta = 2.0944 - (colors.x - (Radius - Distance) * ScaleFactor) / 255;
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

void ColorWheel::UpdateWheel(double xPos, double yPos)
{
	// Set Cursor to Hand
	Global::Selected_Cursor = Global::CURSORS::HAND;

	// Calculate Position on Circle

	// If the Distance from the Position is Greater than the Radius, Make the Selector Position 
	if (pow(pow(xPos - position.x, 2) + pow(yPos - position.y, 2), 0.5) > (double)Radius)
	{
		// Calculate the Angle From the Position to Origin
		double theta = atan((yPos - position.y) / (xPos - position.x));

		// The New X and Y Coordinates
		double newX, newY;

		// Test if xPos is Right of Origin
		if (xPos > position.x)
		{
			// Caclulate the new X and Y
			newY = Radius * sin(theta);
			newX = Radius * cos(theta);
		}

		// Test if xPos is Left of Origin
		else
		{
			// Caclulate the new X and Y
			newY = -Radius * sin(theta);
			newX = -Radius * cos(theta);
		}

		// Assign new Position
		wheelSelectorPos = glm::vec2(newX, newY);
	}

	// The Selector Position is Equal to the Relative Position if Inside Circle
	else
	{
		wheelSelectorPos = glm::vec2(xPos - position.x, yPos - position.y);
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
		double ScaleFactor = 255 / Radius;

		// Calculate Distance From Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Red is Empty
		if (theta >= 2.094 && theta <= 4.189)
		{
			Red = (int)((Radius - Distance) * ScaleFactor);

		}

		// Red is in Decay
		else if (theta > 1.047 && theta < 2.094)
		{
			Red = (int)((Radius - Distance) * ScaleFactor + (255 * (2.094 - theta)));

			// Stop Red From Going Beyond 255
			if (Red > 255)
			{
				Red = 255;
			}
		}

		// Red is Rising
		else
		{
			Red = (int)((Radius - Distance) * ScaleFactor + (255 - (255 * (5.236 - theta))));

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
		double ScaleFactor = 255 / Radius;

		// Calculate Distance from Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Green is Empty
		if (theta >= 4.189)
		{
			Green = (int)((Radius - Distance) * ScaleFactor);
		}

		// Green is in Decay
		else if (theta > 3.142 && theta < 4.189)
		{
			Green = (int)((Radius - Distance) * ScaleFactor + (255 * (4.189 - theta)));

			// Stop Green From Going Beyond 255
			if (Green > 255)
			{
				Green = 255;
			}
		}

		// Green is Rising
		else
		{
			Green = (int)((Radius - Distance) * ScaleFactor + (255 - (255 * (1.047 - theta))));

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
		double ScaleFactor = 255 / Radius;

		// Calculate Distance From Origin
		double Distance = pow(pow(wheelSelectorPos.x, 2) + pow(wheelSelectorPos.y, 2), 0.5);

		// Blue is Empty
		if (theta <= 2.094)
		{
			Blue = (int)((Radius - Distance) * ScaleFactor);
		}

		// Blue is in Decay
		else if (theta > 5.236)
		{
			Blue = (int)((Radius - Distance) * ScaleFactor + (255 * (6.283 - theta)));

			// Stop Blue From Going Beyond 255
			if (Blue > 255)
			{
				Blue = 255;
			}
		}

		// Blue is Rising
		else
		{
			Blue = (int)((Radius - Distance) * ScaleFactor + (255 - (255 * (3.142 - theta))));

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

void ColorWheel::UpdateBrightness(double yPos)
{
	// Calculate Brightness Selector Offset
	brightnessSelectorPos.y = (float)((position.y + Radius) - yPos);

	// Prevent Offset From Going Less Than Minimum Value
	if (brightnessSelectorPos.y < 0)
	{
		brightnessSelectorPos.y = 0;
	}

	// Prevent Offset From Goint Greater Than Minimum Value
	else if (brightnessSelectorPos.y > 2 * Radius)
	{
		brightnessSelectorPos.y = 2 * Radius;
	}

	// Update Colors
	UpdateBrightnessColors();
}

void ColorWheel::UpdateAlpha(double yPos)
{
	// Calculate Alpha Selector Offset
	alphaSelectorPos.y = (float)((position.y + Radius) - yPos);

	// Prevent Offset From Going Less Than Minimum Value
	if (alphaSelectorPos.y < 0)
	{
		alphaSelectorPos.y = 0;
	}

	// Prevent Offset From Goint Greater Than Minimum Value
	else if (alphaSelectorPos.y > 2 * Radius)
	{
		alphaSelectorPos.y = 2 * Radius;
	}

	// Update Colors
	UpdateAlphaColors();
}

int ColorWheel::TestCollisions(float xPos, float yPos, int IndexOffset)
{
	// Test if MouseX is Inside Wheel
	if (xPos >= position.x - Radius && xPos <= position.x + Radius)
	{
		// Calculate the Half Secant at MouseX
		float halfSecant = (float)pow(pow(Radius, 2) - pow((xPos - position.x), 2), 0.5);

		// Test if MouseY is Inside Wheel
		if (yPos >= position.y - halfSecant && yPos <= position.y + halfSecant)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If True, Return Index Offset
			return IndexOffset;
		}
	}

	// Test if MouseY is Inside Brightness and Alpha Bar
	else if (yPos >= position.y - Radius && yPos <= position.y + Radius)
	{
		// Test if MouseX is Inside Brightness Bar
		if (xPos >= brightnessSelectorPos.x - 1.5 * BarScale && xPos <= brightnessSelectorPos.x + 1.5 * BarScale)
		{
			// Set Cursor to Hand
			Global::Selected_Cursor = Global::CURSORS::HAND;

			// If True, Return Index Offset Plus 1
			return IndexOffset + 1;
		}

		// Test if MouseY is Inside Alpha Bar
		else if (xPos >= alphaSelectorPos.x - 1.5 * BarScale && xPos <= alphaSelectorPos.x + 1.5 * BarScale)
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

void ColorWheel::Blitz(glm::mat4& model)
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
	wheelSelectorModel = glm::translate(model, glm::vec3(wheelSelectorPos.x + position.x, wheelSelectorPos.y + position.y, 0.0f));
	brightnessSelectorModel = glm::translate(glm::mat4(1.0f), glm::vec3(brightnessSelectorPos.x, position.y + Radius - brightnessSelectorPos.y, 0.0f)) * model;
	alphaSelectorModel = glm::translate(glm::mat4(1.0f), glm::vec3(alphaSelectorPos.x, position.y + Radius - alphaSelectorPos.y, 0.0f)) * model;

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

void ColorWheel::UpdateBrightnessColors()
{
	// Re-Calculate Vertices

	// Specalized Vertices for Brightness Object
	float vertices[] =
	{
		// Position                                                                 // Colors
		brightnessPosX - 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f,  // Top Left
		brightnessPosX - 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Left
		brightnessPosX + 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Right

		brightnessPosX + 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,         0.0f,         0.0f,         1.0f,  // Bottom Right
		brightnessPosX + 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f,  // Top Right
		brightnessPosX - 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  WheelColor.x, WheelColor.y, WheelColor.z, 1.0f   // Top Left
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
	float BrightnessScale = 1 - (brightnessSelectorPos.y / (2 * Radius));

	// New Color Values
	float newRed = (WheelColor.x * 255) * BrightnessScale;
	float newGreen = (WheelColor.y * 255) * BrightnessScale;
	float newBlue = (WheelColor.z * 255) * BrightnessScale;

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
	Color = glm::vec4(newRed / 255.0f, newGreen / 255.0f, newBlue / 255.0f, Alpha);

	// Update Colors for Alpha and Sample
	UpdateAlphaColors();
}

void ColorWheel::UpdateAlphaColors()
{
	// Recalculate Vertices

	// Specialized Vertices for Alpha Object
	float vertices[] =
	{
		// Position                                                            // Colors
		alphaPosX - 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  Color.x, Color.y, Color.z, 1.0f,  // Top Left
		alphaPosX - 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Left
		alphaPosX + 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Right

		alphaPosX + 1.5f * BarScale, samplePos.y - Radius, position.z - 0.1f,  0.0f,    0.0f,    0.0f,    1.0f,  // Bottom Right
		alphaPosX + 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  Color.x, Color.y, Color.z, 1.0f,  // Top Right
		alphaPosX - 1.5f * BarScale, samplePos.y + Radius, position.z - 0.1f,  Color.x, Color.y, Color.z, 1.0f   // Top Left
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
	Alpha = 1 - (alphaSelectorPos.y / (Radius * 2));

	// Send Alpha Value to Complete Color
	Color.w = Alpha;

	// Update Colors for Sample
	UpdateSample();
}

void ColorWheel::UpdateSample()
{
	// Temporary Vertices Array
	float vertices[42];

	// Bind Vertex Objects
	glBindVertexArray(sampleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sampleVBO);

	//std::cout << Color.x << " " << Color.y << " " << Color.z << " " << Color.w << "\n";

	// Backdrop Vertices
	Vertices::Rectangle::genRectColor(samplePos.x, samplePos.y, position.z, Radius * 2, Radius * 2, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 42 * sizeof(GLfloat), vertices);

	// Sample Vertices
	Vertices::Rectangle::genRectColor(samplePos.x, samplePos.y, position.z + 0.1f, Radius * 2 - 2, Radius * 2 - 1, Color, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 42 * sizeof(GLfloat), 42 * sizeof(GLfloat), vertices);

	// Unbind Vertex Objects
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
