#pragma once

#include "ResourceCore/ComponentResource.h"
#include "ResourceCore/TrivialResource.h"

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
