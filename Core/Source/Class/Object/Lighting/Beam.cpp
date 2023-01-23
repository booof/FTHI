#include "Beam.h"
#include "Render/Struct/DataClasses.h"

// Selector
#include "Class/Render/Editor/Selector.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Line/LineVertices.h"

// Globals
#include "Globals.h"

Object::Light::Beam::Beam::Beam(BeamData& beam_, LightData& light_)
{
	// Store Data
	beam = std::move(beam_);
	data = std::move(light_);
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

	// Generate Nullified Model
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Generate Vertices
	float vertices[30];
	Vertices::Line::genLineTexture(0.0f, beam.position2.x - data.position.x, 0.0f, beam.position2.y - data.position.y, -0.9f, 0.4f, vertices);

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

glm::vec2 Object::Light::Beam::Beam::returnPosition()
{
	return data.position;
}

#endif

Object::Object* DataClass::Data_Beam::genObject()
{
	return new Object::Light::Beam::Beam(beam, light_data);
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

DataClass::Data_Beam::Data_Beam()
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::BEAM;
	object_identifier[2] = 0;
}

void DataClass::Data_Beam::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Beam Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &light_data.position.x, &light_data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
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

