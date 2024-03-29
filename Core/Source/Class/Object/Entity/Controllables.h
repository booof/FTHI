#pragma once
#ifndef CONTROLLABLES_H
#define CONTROLLABLES_H

#ifndef DLL_HEADER
#include "EntityBase.h"
#endif

// These Entities Are Not Governed by a Script, But Are Instead Governed by External Inputs
// Inputs Can Either be From Global Functions or the Update Function
// Ideal for the Play Object and User Controlled Vehicles

namespace Object::Entity
{
	// Controllables Class
	class Controllables : public EntityBase
	{
	public:

		// Initialize Object
		Controllables(uint32_t& uuid_, EntityData& entity_, ObjectData& data_, glm::vec2& offset);

		// Update Object
		void updateObject();
	};
}

#endif
