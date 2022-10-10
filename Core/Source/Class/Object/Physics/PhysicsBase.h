#ifndef PHYSICS_BASE_H
#define PHYSICS_BASE_H

#ifndef DLL_HEADER
#include "Class/Object/Object.h"
#include "Class/Render/Struct/Struct.h"
#endif

namespace Object::Physics
{
	// Physics Bases
	enum class PHYSICS_BASES : uint8_t
	{
		RIGID_BODY,
		SOFT_BODY,
		HINGE_BASE
	};

	// Rigid Body Types
	enum class RIGID_BODY_TYPES : uint8_t
	{
		RECTANGLE,
		TRAPEZOID,
		TRIANGLE,
		CIRCLE,
		POLYGON
	};

	// Soft Body Types
	enum class SOFT_BODY_TYPES : uint8_t
	{
		SPRING_MASS,
		WIRE
	};

	// Hinge Types
	enum class HINGES : uint8_t
	{
		ANCHOR,
		HINGE
	};

	// Compined Physics Types
	enum class PHYSICS_TYPES : uint8_t
	{
		TYPE_RECTANGLE,
		TYPE_TRAPEZOID,
		TYPE_TRIANGLE,
		TYPE_CIRCLE,
		TYPE_POLYGON,
		TYPE_SPRING_MASS,
		TYPE_WIRE,
		TYPE_ANCHOR,
		TYPE_HINGE
	};

	// Definition for Base Physics Object Class
	class PhysicsBase : public SubObject
	{

	public:

		// Type of Physics Object
		PHYSICS_BASES base;

		// Individual Object Type
		PHYSICS_TYPES type;

		// UUID of Object
		uint32_t uuid = 0;

		// Draw Object
		virtual void drawObject() = 0;
	};
}

#endif