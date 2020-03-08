#pragma once

#include "ComponentResource.h"
#include "TrivialResource.h"

namespace Resource
{
	struct SolidCollisionContainerContent
	{
		float density;
		float inputForceAddition;
		float friction;
	};

	typedef TrivialResource<SolidCollisionContainerContent> SolidCollisionComponent;
}
