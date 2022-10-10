#ifndef BACKDROP_H
#define BACKDROP_H

#ifndef DLL_HEADER
#include "Object/Terrain/TerrainBase.h"
#endif

class Object::Terrain::Backdrop : public TerrainBase
{
public:

	// Initialize Object
	Backdrop(Shape::Shape* shape_, ObjectData data_) : TerrainBase(shape_, data_)
	{
		layer = BACKDROP;
	}
};

#endif