#pragma once

#include "Resource/ComponentResource.h"
#include "Resource/TrivialResource.h"

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
