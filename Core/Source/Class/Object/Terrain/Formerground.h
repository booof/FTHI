#pragma once
#ifndef FORMERGROUND_H
#define FORMERGROUND_H

#ifndef DLL_HEADER
#include "Object/Terrain/TerrainBase.h"
#endif

class Object::Terrain::Formerground : public TerrainBase
{
public:

	// Initialize Object
	Formerground(Shape::Shape* shape_, ObjectData data_, glm::vec2& offset) : TerrainBase(shape_, data_, offset)
	{
		layer = FORMERGROUND;
	}
};

#endif