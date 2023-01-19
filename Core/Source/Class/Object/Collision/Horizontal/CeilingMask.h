#pragma once
#ifndef CEILING_MASK_H
#define CEILING_MASK_H

#ifndef DLL_HEADER
#include "Object/Collision/CollisionMask.h"
#endif

namespace Object::Mask::Ceiling
{
	// Class for Ceiling Collision Masks
	class CeilingMask : public CollisionMask
	{
	protected:

		// Type of Object
		unsigned char type;

	public:

		// Get the Left and Right Edge Vertices
		virtual void getLeftRightEdgeVertices(glm::vec2& left, glm::vec2& right) = 0;
	};

	// Ceiling Mask Shapes
	class CeilingMaskLine;
	class CeilingMaskSlant;
	class CeilingMaskSlope;
}

#endif
