#include "Beam.h"
#include "Render/Struct/DataClasses.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Line/LineVertices.h"

// Globals
#include "Globals.h"

Object::Light::Beam::Beam::Beam(BeamData& beam_, LightData& light_, glm::vec2& offset)
{
	// Store Data
	beam = std::move(beam_);
	data = std::move(light_);
	data.position += offset;
	beam.position2 += offset;
	data.layer = 4;

	// Store Storage Type
	storage_type = BEAM_COUNT;

	// Calculate Directions

	// Slope of Perpendicular Line
	float slope = (beam.position2.y - data.position.y) / (beam.position2.x - data.position.x);
	float perpendicular_slope = -1.0f / slope;

	// Calculate Normalized Vector of Beam
	direction = glm::vec4(1.0f, slope, 0, 0);
	direction = glm::normalize(direction);

	// Slope Intercept Function for Editing
	Slope = slope;
	Intercept = data.position.y - (slope * data.position.x);

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer();

	// Store Texture
	texture = Global::Visual_Textures.find("BeamLight.png")->second;

	// Only Attach Scripts if in Gameplay Mode
	if (!Global::editing)
	{
		// Initialize Script
		initializeScript(data.script);

		// Run Scripted Initialization
		init(this);
	}

#else

	// Initialize Script
	initializeScript(data.script);

	// Run Scripted Initialization
	init(this);

#endif

}

void Object::Light::Beam::Beam::loadLight()
{
	// Generate Nullified Model
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Add Light and Line Direction Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset, 16, glm::value_ptr(direction));

	// Add Ambient, Diffuse, and Specular Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 16, 16, glm::value_ptr(data.ambient * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 32, 16, glm::value_ptr(data.diffuse * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 48, 16, glm::value_ptr(data.specular * data.intensity));

	// Add Attenuation Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 64, 4, &beam.linear);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 68, 4, &beam.quadratic);

	// Add Endpoint Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 72, 8, glm::value_ptr(data.position));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset + 80, 8, glm::value_ptr(beam.position2));
}

void Object::Light::Beam::Beam::updateObject()
{
	// If Reload Light Flag is Active, Resend Object to Shader
	if (send_light)
	{
		// Bind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);

		// Write Object
		loadLight();

		// Unbind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Reset Flag
		send_light = false;
	}
}

#ifdef EDITOR

void Object::Light::Beam::Beam::initializeVisualizer()
{
	// Save Texture
	//texture = Visual_Textures.find("DirectionalLight.png")->second;

	// Generate Vertices
	float vertices[30];
	Vertices::Line::genLineTexture(0.0f, beam.position2.x - data.position.x, 0.0f, beam.position2.y - data.position.y, -1.5f, 0.4f, vertices);

	// Generate Vertex Objects
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind Vertex Objects
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Bind Data
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Data
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool Object::Light::Beam::Beam::testMouseCollisions(float x, float y)
{
	// Check if Object is Between X-Values
	if ((data.position.x < x && x < beam.position2.x) || (beam.position2.x < x && x < data.position.x))
	{
		// Calculate Localized Y Value of Mask at Object X
		float localY;
		localY = Slope * x + Intercept;

		// Check if Object Y Intercepts Floor Y
		if ((localY - 1.0f) < y && localY + 0.01f > y)
		{
			return true;
		}
	}

	return false;
}

void Object::Light::Beam::Beam::updateSelectedPosition(float deltaX, float deltaY)
{
	// Update Position
	data.position.x += deltaX;
	data.position.y += deltaY;
	beam.position2.x += deltaX;
	beam.position2.y += deltaY;

	// Update the Intercept
	Intercept = data.position.y - (Slope * data.position.x);

	// Update Shader Data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::BeamBuffer);
	loadLight();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

glm::vec2 Object::Light::Beam::Beam::returnPosition()
{
	return data.position;
}

#endif

Object::Object* DataClass::Data_Beam::genObject(glm::vec2& offset)
{
	return new Object::Light::Beam::Beam(beam, light_data, offset);
}

void DataClass::Data_Beam::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&beam, sizeof(Object::Light::Beam::BeamData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Beam::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&beam, sizeof(Object::Light::Beam::BeamData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Beam::updateTraveresPositionHelper(float deltaX, float deltaY)
{
	light_data.position.x += deltaX;
	light_data.position.y += deltaY;
	beam.position2.x += deltaX;
	beam.position2.y += deltaY;
}

DataClass::Data_Beam::Data_Beam(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::BEAM;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_Beam::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Beam Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &light_data.position, false);
	object_info.addPositionValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &beam.position2, false);
	infoColors(object_info);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_Beam::makeCopy()
{
	return new Data_Beam(*this);
}

Object::Light::Beam::BeamData& DataClass::Data_Beam::getBeamData()
{
	return beam;
}

void DataClass::Data_Beam::generateInitialValues(glm::vec2& position, float& size)
{
	generateInitialLightValues(position);
	beam.position2 = position + glm::vec2(size, 0.0f);
	beam.linear = DEFAULT_LINEAR;
	beam.quadratic = DEFAULT_QUADRATIC;
}

void DataClass::Data_Beam::offsetPosition(glm::vec2& offset)
{
	// Update Positions of Both Vertices
	light_data.position += offset;
	beam.position2 += offset;
}

void DataClass::Data_Beam::offsetOppositePosition(glm::vec2& offset)
{
	// Only Update Second Position
	beam.position2 += offset;
}

void DataClass::Data_Beam::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &light_data.position;
	index1 = 2;

	// Position2 is at Index 3
	*position2 = &beam.position2;
	index2 = 3;

	// Position 3 Not Important
	position3Null(index3, position3);
}

