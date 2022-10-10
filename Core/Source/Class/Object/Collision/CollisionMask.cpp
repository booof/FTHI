#include "CollisionMask.h"
#include "Globals.h"

#ifdef EDITOR

bool Object::Mask::CollisionMask::testMouseCollisions(float x, float y)
{
	return testCollisions(glm::vec2(x, y), 0.2f);
}

void Object::Mask::CollisionMask::blitzLine()
{

	// Bind Model Matrix
	glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));

	// Bind Vertex Array Object
	glBindVertexArray(VAO);

	// Draw Object
	glDrawArrays(GL_LINES, 0, number_of_vertices);

	// Unbind Vertex Array
	glBindVertexArray(0);
}

#endif

void Object::Mask::CollisionMask::returnCollisionValues(float& value, float& angle)
{
	value = returned_value;
	angle = returned_angle;
}

void Object::Mask::CollisionMask::updateObject()
{
	// Include Something Here for Functure Functionality
}
