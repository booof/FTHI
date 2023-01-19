#pragma once
#ifndef BACKGROUND_H
#define BACKGROUND_H

#ifndef DLL_HEADER
#include "Object/Terrain/TerrainBase.h"
#endif

class Object::Terrain::Background : public TerrainBase
{
public:

	// Initialize Object
	Background(Shape::Shape* shape_, ObjectData data_, uint8_t layer) : TerrainBase(shape_, data_)
	{
		layer = BACKGROUND_3 + layer;
	}
};

#endif

