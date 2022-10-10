#include "QuickMath.h"
#include "Class/Object/Physics/RigidBody/RigidBody.h"
#include "Class/Object/Entity/EntityBase.h"

glm::vec2 Algorithms::Math::parameterizeAngle(float angle)
{
	return glm::vec2(cos(angle), sin(angle));
}

float Algorithms::Math::angle_from_vector(glm::vec2 vector)
{
	return angle_from_vector(vector.x, vector.y);
}

float Algorithms::Math::angle_from_vector(float x, float y)
{
	static float angle;

	// Get initial angle
	angle = atan(y / x);
	if (x < 0)
		angle += 3.14159f;
	if (angle < 0)
		angle += 6.2832f;

	return angle;
}

bool Algorithms::Math::About(float input, float test, float error)
{
	return (input > test - error && input < test + error);
}

bool Algorithms::Math::About(glm::vec2 input, glm::vec2 test, float error)
{
	return (input.x > test.x - error && input.x < test.x + error && input.y > test.y - error && input.y < test.y + error);
}

float Algorithms::Math::fastSquare(float number)
{
	// Typcast to Manipulatable Binary
	static unsigned long int cast_number = *(unsigned long*)&number;

	// Preform Bit Manipulation
	cast_number = (cast_number << 1) - 1064992506;

	// Return Typcasted Float
	return *(float*)&cast_number;
}

float Algorithms::Math::fastRoot(float number)
{
	// Typecast to Manipulatable Binary
	static unsigned long int cast_number = *(unsigned long*)&number;

	// Preform Bit Manipulation
	cast_number = 532496253 + (cast_number >> 1);

	// Return Typcasted Float
	return *(float*)&cast_number;
}

float Algorithms::Math::fastDistance(float num1, float num2)
{
	return fastRoot(fastSquare(num1) + fastSquare(num2));
}

float Algorithms::Math::fastDistance(glm::vec2 point1, glm::vec2 point2)
{
	return fastDistance(point1.x - point2.x, point1.y - point2.y);
}

float Algorithms::Math::fastDistance(glm::vec2 point)
{
	return fastDistance(point.x, point.y);
}

int Algorithms::Math::findClosestWithoutGoingOver(float test_value, const float list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Algorithms::Math::findClosestWithoutGoingOver(double test_value, const double list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Algorithms::Math::findClosestWithoutGoingOver(int test_value, const int list[], int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i] > test_value)
		{
			return i;
		}
	}

	return 0;
}

int Algorithms::Math::findClosestWithoutGoingOver(float test_value, Object::Physics::Rigid::Named_Node* list, int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i].value > test_value)
		{
			return list[i].name;
		}
	}

	return list[0].name;
	//return 0;
}

int Algorithms::Math::findClosestWithoutGoingOver(float test_value, Object::Entity::Named_Node* list, int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i].value > test_value)
		{
			return list[i].name;
		}
	}

	return list[0].name;
	//return 0;
}

int Algorithms::Math::findClosestWithoutGoingOverNamedIndex(float test_value, Object::Physics::Rigid::Named_Node* list, int list_size)
{
	for (int i = 0; i < list_size; i++)
	{
		if (list[i].value > test_value)
		{
			return i;
		}
	}

	return 0;
}
