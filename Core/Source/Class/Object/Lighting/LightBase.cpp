#include "LightBase.h"
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
