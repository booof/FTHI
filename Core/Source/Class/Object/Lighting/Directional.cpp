#include "Directional.h"

// Selector
#include "Class/Render/Editor/Selector.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Line/LineVertices.h"

// Globals
#include "Globals.h"

Object::Light::Directional::Directional::Directional(DirectionalData& directional_, LightData light_)
{
	// Store Data
	directional = std::move(directional_);
	data = std::move(light_);
	data.layer = 4;

	// Store Storage Type
	storage_type = DIRECTIONAL_COUNT;

	// Calculate Directions

	// Slope of Perpendicular Line
	float slope = (directional.position2.y - data.position.y) / (directional.position2.x - data.position.x);
	float perpendicular_slope = -1.0f / slope;

	// Determine Which Direction the Light Should Go From Line
	direction = 1.0f;
	if (directional.position2.x < data.position.x)
	{
		direction = -1.0f;
	}

	// Calculate Vectors
	light_direction = glm::vec4(direction, perpendicular_slope * direction, 0, 0);
	line_direction = glm::vec4(1.0f, slope, 0, 0);

	// Normalize Vector
	light_direction = glm::normalize(-light_direction);
	light_direction.z = 1.0f;
	line_direction = glm::normalize(line_direction);

	// Slope Intercept Function for Editing
	Slope = slope;
	Intercept = data.position.y - (slope * data.position.x);

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer();

	// Store Texture
	texture = Global::Visual_Textures.find("DirectionalLight.png")->second;

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

void Object::Light::Directional::Directional::loadLight()
{
	// Add Light and Line Direction Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset, 16, glm::value_ptr(light_direction));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 16, 16, glm::value_ptr(line_direction));

	// Add Ambient, Diffuse, and Specular Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 32, 16, glm::value_ptr(data.ambient * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 48, 16, glm::value_ptr(data.diffuse * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 64, 16, glm::value_ptr(data.specular * data.intensity));

	// Add Endpoint Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 80, 8, glm::value_ptr(data.position));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 88, 8, glm::value_ptr(directional.position2));
}

void Object::Light::Directional::Directional::updateObject()
{
	// If Reload Light Flag is Active, Resend Object to Shader
	if (send_light)
	{
		// Bind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::DirectionalBuffer);

		// Write Object
		loadLight();

		// Unbind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Reset Flag
		send_light = false;
	}
}

#ifdef EDITOR

void Object::Light::Directional::Directional::initializeVisualizer()
{
	// Save Texture
	//texture = Visual_Textures.find("DirectionalLight.png")->second;

	// Generate Nullified Model
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Generate Vertices
	float vertices[30];
	Vertices::Line::genLineTexture(0.0f, directional.position2.x - data.position.x, 0.0f, directional.position2.y - data.position.y, -0.9f, 0.4f, vertices);

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

void Object::Light::Directional::Directional::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = LIGHT;
	selector.object_identifier[1] = DIRECTIONAL;

	// Store Editing Values
	selector.light_data = data;
	selector.directional_data = directional;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Directional Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos1: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
	object_info.addDoubleValue("Pos2: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &directional.position2.x, &directional.position2.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

bool Object::Light::Directional::Directional::testMouseCollisions(float x, float y)
{
	// Check if Object is Between X-Values
	if ((data.position.x < x && x < directional.position2.x) || (directional.position2.x < x && x < data.position.x))
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

void Object::Light::Directional::Directional::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(LIGHT);
	object_file.put(DIRECTIONAL);

	// Write Data
	object_file.write((char*)&directional, sizeof(directional));
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name.size());
}

glm::vec2 Object::Light::Directional::Directional::returnPosition()
{
	return data.position;
}

#endif

