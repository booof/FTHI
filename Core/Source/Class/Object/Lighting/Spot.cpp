#include "Spot.h"
#include "Render/Struct/DataClasses.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Globals
#include "Globals.h"

Object::Light::Spot::Spot::Spot(SpotData& spot_, LightData& light_)
{
	// Store Structures
	spot = std::move(spot_);
	data = std::move(light_);
	data.layer = 4;
	spot.direction.w = 1.0f;

	// Store Storage Type
	storage_type = SPOT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer();

	// Store Texture
	texture = Global::Visual_Textures.find("SpotLight.png")->second;

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

void Object::Light::Spot::Spot::loadLight()
{
	// Add Position Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset, 16, glm::value_ptr(glm::vec4(data.position.x, data.position.y, 2.0f, 0.0f)));

	// Add Direction Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 16, 16, glm::value_ptr(spot.direction));

	// Add Ambient, Diffuse, and Specular Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 32, 16, glm::value_ptr(data.ambient * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 48, 16, glm::value_ptr(data.diffuse * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 64, 16, glm::value_ptr(data.specular * data.intensity));

	// Add Attenuation Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 80, 4, &spot.linear);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 84, 4, &spot.quadratic);

	// Add Angle Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 88, 4, &spot.angle1);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 92, 4, &spot.angle2);
}

void Object::Light::Spot::Spot::updateObject()
{
	// If Reload Light Flag is Active, Resend Object to Shader
	if (send_light)
	{
		// Bind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::SpotBuffer);

		// Write Object
		loadLight();

		// Unbind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Reset Flag
		send_light = false;
	}
}

#ifdef EDITOR

void Object::Light::Spot::Spot::initializeVisualizer()
{
	// Save Texture
	//texture = Visual_Textures.find("SpotLight.png")->second;

	// Generate Model Matrix
	glm::vec2 normalized_direction = glm::normalize(glm::vec2(spot.direction.x, spot.direction.y));
	float angle = atan(normalized_direction.y / normalized_direction.x) - 1.571f;
	if (normalized_direction.x < 0) { angle -= 3.142f; }
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));
	model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

	// Generate Vertices
	float vertices[30];
	Vertices::Rectangle::genRectTexture(0.0f, -2.0f, -0.9f, 2.0f, 4.0f, vertices);

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

bool Object::Light::Spot::Spot::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 1.0f && x < data.position.x + 1.0f && y > data.position.y - 2.0f && y < data.position.y + 2.0f)
	{
		return true;
	}

	return false;
}

void Object::Light::Spot::Spot::updateSelectedPosition(float deltaX, float deltaY)
{
	data.position.x += deltaX;
	data.position.y += deltaY;
	initializeVisualizer();
}

glm::vec2 Object::Light::Spot::Spot::returnPosition()
{
	return data.position;
}

#endif

Object::Object* DataClass::Data_Spot::genObject()
{
	return new Object::Light::Spot::Spot(spot, light_data);
}

void DataClass::Data_Spot::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&spot, sizeof(Object::Light::Spot::SpotData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Spot::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&spot, sizeof(Object::Light::Spot::SpotData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Spot::Data_Spot(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::SPOT;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_Spot::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Spot Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &light_data.position.x, &light_data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

DataClass::Data_Object* DataClass::Data_Spot::makeCopy()
{
	return new Data_Spot(*this);
}

void DataClass::Data_Spot::updateSelectedPosition(float deltaX, float deltaY)
{
	light_data.position.x += deltaX;
	light_data.position.y += deltaY;
}

Object::Light::Spot::SpotData& DataClass::Data_Spot::getSpotData()
{
	return spot;
}

void DataClass::Data_Spot::generateInitialValues(glm::vec2& position)
{
	generateInitialLightValues(position);
	spot.direction = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	spot.angle1 = 0.2f;
	spot.angle2 = 0.5f;
	spot.linear = DEFAULT_LINEAR;
	spot.quadratic = DEFAULT_QUADRATIC;
}

