#include "LightBase.h"
#include "Render/Struct/DataClasses.h"
#include "Macros.h"
#include "Globals.h"

#ifdef EDITOR

void Object::Light::LightBase::blitzObject()
{
	// Calculate Matrix
	glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Load Texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture.texture);

	// Draw Object
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

int Object::Light::LightBase::returnLayer()
{
	return data.layer;
}

glm::vec2* Object::Light::LightBase::pointerToPosition()
{
	return &data.position;
}

#endif

Object::Light::LightData& DataClass::Data_Light::getLightData()
{
	return light_data;
}

int& DataClass::Data_Light::getScript()
{
	return light_data.script;
}

glm::vec2& DataClass::Data_Light::getPosition()
{
	return light_data.position;
}

void DataClass::Data_Light::generateInitialLightValues(glm::vec2& new_position)
{
	light_data.position = new_position;
	light_data.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	light_data.intensity = 1.0f;
	light_data.script = 0;
}
