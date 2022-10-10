#ifndef QUICK_MATH_H
#define QUICK_MATH_H

#include "ExternalLibs.h"

// Named Node Declaration
namespace Object::Physics::Rigid { struct Named_Node; }
namespace Object::Entity { struct Named_Node; }

namespace Algorithms::Math
{
    // The Only Reason Why This Function Definition is In This Header File is Because Visual Studio Decided to be a Fucking Faggot

    template <class Type> int8_t getSign(Type var);

    template<class Type>
    int8_t getSign(Type var)
    {
        //return ((Type(0) < var) - (var < Type(0)));
		if (var >= 0) { return 1; }
		return -1;
    }

	template<class Type>
	int8_t getSignWithZero(Type var)
	{
		return ((Type(0) < var) - (var < Type(0)));
	}

    // Parameterize Angle
    glm::vec2 parameterizeAngle(float angle);

	// Calculates the Angle of a Vector
	float angle_from_vector(glm::vec2 vector);

	// Calculates the Angle of a Vector
    float angle_from_vector(float x, float y);

	// Determines if a Value is Inside a Number With a Margin of Error
	bool About(float input, float test, float error);

	// Determines if a Value is Inside a Number With a Margin of Error
	bool About(glm::vec2 input, glm::vec2 test, float error);

	// Computes the Square of a Number Quickly
	float fastSquare(float number);

	// Computes the Square Root of a Number Quickly
	float fastRoot(float number);

	// Computes the Distance of Two Numbers Quickly
	float fastDistance(float num1, float num2);

	// Computes the Distance Between Two Points Quickly
	float fastDistance(glm::vec2 point1, glm::vec2 point2);

	// Computes the Distance Between Two Points Quickly
	float fastDistance(glm::vec2 point);

	// Find Index of Closest Value Without Going Over
	int findClosestWithoutGoingOver(float test_value, const float list[], int list_size);
	int findClosestWithoutGoingOver(double test_value, const double list[], int list_size);
	int findClosestWithoutGoingOver(int test_value, const int list[], int list_size);
	int findClosestWithoutGoingOver(float test_value, Object::Physics::Rigid::Named_Node* list, int list_size);
	int findClosestWithoutGoingOver(float test_value, Object::Entity::Named_Node* list, int list_size);
	int findClosestWithoutGoingOverNamedIndex(float test_value, Object::Physics::Rigid::Named_Node* list, int list_size);
}

#endif:
