#pragma once
#ifndef COLLISION_MASK_H
#define COLLISION_MASK_H

#ifndef DLL_HEADER
#include "Object/Object.h"
#endif

namespace Object::Mask
{
	// List of Mask Types
	enum MASKS : unsigned char
	{
		FLOOR,
		LEFT_WALL,
		RIGHT_WALL,
		CEILING,
		TRIGGER
	};

	// Lift of Horizontal Mask Shapes
	enum HORIZONTAL_SHAPES : unsigned char
	{
		HORIZONTAL_LINE,
		HORIZONTAL_SLANT,
		HORIZONTAL_SLOPE
	};

	// List of Vertical Mask Shapes
	enum VERTICAL_SHAPES : unsigned char
	{
		VERTICAL_LINE,
		VERTICAL_CURVE
	};

	// Base Class for Collision Masks
	class CollisionMask: public Object
	{
	protected:

#ifdef EDITOR

		// Editor Data
		bool Clamp;
		bool Lock;

		// Vertex Objects
		GLuint VAO, VBO;

		// Number of Vertices to Render
		int number_of_vertices;

		// Model Matrix
		glm::mat4 model;

	public:

		// Test Mouse Collisions of Object
		bool testMouseCollisions(float x, float y);

		// Function to Draw Line of Mask
		void blitzLine();

#endif

	public:

		// Test Collisions of Object
		virtual bool testCollisions(glm::vec2 test_position, float error) = 0;

		// Function to Return Collision Values
		void returnCollisionValues(float& value, float& angle);

		// Function to Return Material
		virtual void returnMaterial(int& material_) = 0;

		// Update Object
		void updateObject();

		// Returned Value
		float returned_value;

		// Angle of Returned Value
		float returned_angle;

		int material;
	};

	// Collision Mask Types

	// Floor Collisions
	namespace Floor {
		class FloorMask; 
	}

	// Left Wall Collisions
	namespace Left {
		class LeftMask;
	}

	// Right Wall Collisions
	namespace Right {
		class RightMask;
	}

	// Ceiling Collisions
	namespace Ceiling {
		class CeilingMask;
	}

	// Trigger Collisions
	namespace Trigger {
		class TriggerMask;
		struct TriggerData;
	}

	// Collision Mask Shapes
	class HorizontalLine;
	class Slant;
	class Slope;
	class VerticalLine;
	class Curve;

	// Collision Mask Shape Data
	struct HorizontalLineData;
	struct SlantData;
	struct SlopeData;
	struct VerticalLineData;
	struct CurveData;
}

#endif
