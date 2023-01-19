#pragma once
#ifndef LEFT_MASK_H
#define LEFT_MASK_H

#ifndef DLL_HEADER
#include "Object/Collision/CollisionMask.h"
#endif

namespace Object::Mask::Left
{
	// Clas for Left Wall Collision Masks
	class LeftMask : public CollisionMask
	{
	public:

		// Type of Object
		unsigned char type;

		// Get the Top and Bottom Edge Vertices
		virtual void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom) = 0;
	};

	// Types of Left Walls
	class LeftMaskLine;
	class LeftMaskCurve;
}

#endif
