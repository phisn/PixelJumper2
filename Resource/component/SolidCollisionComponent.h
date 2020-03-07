#pragma once

#include "ComponentResource.h"
#include "TrivialResource.h"

namespace Resource
{
	struct SolidCollisionComponentContent
	{
		float density;
		float inputForceAddition;
		float friction;
	};

	typedef TrivialResource<SolidCollisionComponentContent> SolidCollisionComponent;
}
