#include "ComponentResource.h"

#include "component/CommonExitComponent.h"
#include "component/DynamicWorldTransitionComponent.h"
#include "component/SolidCollisionComponent.h"

#include <cassert>

namespace Resource
{
	ComponentResource* ComponentResource::Create(const ComponentID componentID)
	{
		switch (componentID)
		{
		case ComponentID::CommonExit:
		case ComponentID::DynamicWorldTransition:

			return DynamicWorldTransitionComponent();
		case ComponentID::SolidCollision:

		default:
			assert(true);

			return NULL;
		}
	}
}
