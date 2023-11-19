#include "Point.h"
#include "Render/Struct/DataClasses.h"

// Object Info
#include "Class/Render/Editor/ObjectInfo.h"

// Vertices
#include "Source/Vertices/Rectangle/RectangleVertices.h"

// Globals
#include "Globals.h"

Object::Light::Point::Point::Point(PointData& point_, LightData& light_, glm::vec2& offset)
{
	// Store Structures
	point = point_;
	data = light_;
	data.position += offset;
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
	// Generate Model Matrix
	model = glm::translate(glm::mat4(1.0f), glm::vec3(data.position.x, data.position.y, 0.0f));

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

	// Generate Vertices
	float vertices[30];
	Vertices::Rectangle::genRectTexture(0.0f, 0.0f, -1.5f, 2.0f, 3.0f, vertices);

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

bool Object::Light::Point::Point::testMouseCollisions(float x, float y)
{
	if (x > data.position.x - 1.0f && x < data.position.x + 1.0f && y > data.position.y - 1.5f && y < data.position.y + 1.5f)
	{
		return true;
	}

	return false;
}

void Object::Light::Point::Point::updateSelectedPosition(float deltaX, float deltaY)
{
	// Update Position
	data.position.x += deltaX;
	data.position.y += deltaY;

	// Update Shader Data
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, Global::PointBuffer);
	loadLight();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

glm::vec2 Object::Light::Point::Point::returnPosition()
{
	return data.position;
}

#endif

Object::Object* DataClass::Data_Point::genObject(glm::vec2& offset)
{
	return new Object::Light::Point::Point(point, light_data, offset);
}

void DataClass::Data_Point::writeObjectData(std::ofstream& object_file)
{
	object_file.write((char*)&point, sizeof(Object::Light::Point::PointData));
	object_file.write((char*)&light_data, sizeof(Object::Light::LightData));
}

void DataClass::Data_Point::readObjectData(std::ifstream& object_file)
{
	object_file.read((char*)&point, sizeof(Object::Light::Point::PointData));
	object_file.read((char*)&light_data, sizeof(Object::Light::LightData));
}

DataClass::Data_Point::Data_Point(uint8_t children_size)
{
	// Set Object Identifier
	object_identifier[0] = Object::LIGHT;
	object_identifier[1] = Object::Light::POINT;
	object_identifier[2] = 0;
	object_identifier[3] = children_size;
}

void DataClass::Data_Point::info(Editor::ObjectInfo& object_info)
{
	// Store Object Information
	object_info.clearAll();
	object_info.setObjectType("Point Light", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	object_info.addTextValue("Name: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &name, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	object_info.addPositionValue("Pos: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.9f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), &light_data.position, false);
	infoColors(object_info);
	object_info.addSingleValue("Index: ", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), &object_index, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
}

DataClass::Data_Object* DataClass::Data_Point::makeCopy()
{
	return new Data_Point(*this);
}

void DataClass::Data_Point::updateSelectedPosition(float deltaX, float deltaY, bool update_real)
{
	light_data.position.x += deltaX;
	light_data.position.y += deltaY;
	updateSelectedPositionsHelper(deltaX, deltaY, update_real);
}

Object::Light::Point::PointData& DataClass::Data_Point::getPointData()
{
	return point;
}

void DataClass::Data_Point::generateInitialValues(glm::vec2& position)
{
	generateInitialLightValues(position);
	point.linear = DEFAULT_LINEAR;
	point.quadratic = DEFAULT_QUADRATIC;
}

void DataClass::Data_Point::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
	// Position 1 is at Index 2
	*position1 = &light_data.position;
	index1 = 2;

	// Others are Not Important
	position23Null(index2, index3, position2, position3);
}
