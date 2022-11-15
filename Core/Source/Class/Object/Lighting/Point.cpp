#include "Point.h"

// Selector
#include "Class/Render/Editor/Selector.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Globals
#include "Globals.h"

Object::Light::Point::Point::Point(PointData& point_, LightData& light_)
{
	// Store Structures
	point = point_;
	data = light_;
	data.layer = 4;

	// Store Storage Type
	storage_type = POINT_COUNT;

#ifdef EDITOR

	// Initialize Visualizer
	initializeVisualizer();

	// Store Texture
	texture = Global::Visual_Textures.find("PointLight.png")->second;

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

void Object::Light::Point::Point::loadLight()
{
	// Add Position Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, buffer_offset, 16, glm::value_ptr(glm::vec4(data.position.x, data.position.y, 2.0f, 1.0f)));

	// Add Ambient, Diffuse, and Specular Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 16, 16, glm::value_ptr(data.ambient * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 32, 16, glm::value_ptr(data.diffuse * data.intensity));
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 48, 16, glm::value_ptr(data.specular * data.intensity));

	// Add Attenuation Data
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 64, 4, &point.linear);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, (GLintptr)buffer_offset + 68, 4, &point.quadratic);
}

void Object::Light::Point::Point::updateObject()
{
	// If Reload Light Flag is Active, Resend Object to Shader
	if (send_light)
	{
		// Bind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);

		// Write Object
		loadLight();

		// Unbind Buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		// Reset Flag
		send_light = false;
	}
}

#ifdef EDITOR

void Object::Light::Point::Point::initializeVisualizer()
{
	// Save Texture
	//texture = Visual_Textures.find("PointLight.png")->second;

	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

	// Generate Vertices
	float vertices[30];
	Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -0.9f, 2.0f, 3.0f, vertices);

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

void Object::Light::Point::Point::select(Editor::Selector& selector, Editor::ObjectInfo& object_info)
{
	// Store Object Identifier
	selector.object_identifier[0] = LIGHT;
	selector.object_identifier[1] = POINT;

	// Store Editing Values
	selector.light_data = data;
	selector.point_data = point;
	selector.object_index = object_index;
	selector.editor_data.name_size = (uint8_t)name.size();
	selector.editor_data.clamp = clamp;
	selector.editor_data.lock = lock;
	selector.editor_data.name = name;

	// Store Object Information
	info(object_info, name, data, point);
	
	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

bool Object::Light::Point::Point::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 1.0f && x < data.position.x + 1.0f && y > data.position.y - 1.5f && y < data.position.y + 1.5f)
	{
		return true;
	}

	return false;
}

void Object::Light::Point::Point::write(std::ofstream& object_file, std::ofstream& editor_file)
{
	// Write Object Identifier
	object_file.put(LIGHT);
	object_file.put(POINT);

	// Write Data
	object_file.write((char*)&point, sizeof(point));
	object_file.write((char*)&data, sizeof(data));

	// Write Editor Data
	uint16_t name_size = (uint16_t)name.size();
	editor_file.write((char*)&name_size, sizeof(uint16_t));
	editor_file.write((char*)&clamp, sizeof(bool));
	editor_file.write((char*)&lock, sizeof(bool));
	editor_file.write((char*)&name[0], name_size);
}

glm::vec2 Object::Light::Point::Point::returnPosition()
{
	return data.position;
}

void Object::Light::Point::Point::info(Editor::ObjectInfo& object_info, std::string& name, LightData& data, PointData& point)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Point Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addDoubleValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), "x: ", glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), " y: ", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), &data.position.x, &data.position.y, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), false);
}

#endif
