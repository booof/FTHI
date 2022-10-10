#ifndef RIGHT_MASK_H
#define RIGHT_MASK_H

#ifndef DLL_HEADER
#include "Object/Collision/CollisionMask.h"
#endif

namespace Object::Mask::Right
{
	// Class for Right Wall Collision Masks
	class RightMask : public CollisionMask
	{
	public:

		// Type of Object
		unsigned char type;

		// Get the Top and Bottom Edge Vertices
		virtual void getTopAndBottomEdgeVertices(glm::vec2& top, glm::vec2& bottom) = 0;
	};

	// Types of Right Walls
	class RightMaskLine;
	class RightMaskCurve;
}

#endif
