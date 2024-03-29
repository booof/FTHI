#pragma once
#ifndef FOREGROUND_H
#define FOREGROUND_H

#ifndef DLL_HEADER
#include "Object/Terrain/TerrainBase.h"
#endif

class Object::Terrain::Foreground : public TerrainBase
{
public:

	int test_var = 1;

	// Initialize Object
	Foreground(Shape::Shape* shape_, ObjectData data_, glm::vec2& offset) : TerrainBase(shape_, data_, offset)
	{
		layer = FOREGROUND;
	}
};

#endif