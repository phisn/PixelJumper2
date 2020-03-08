#include "ComponentResource.h"

#include "component/ScriptComponent.h"
#include "component/DynamicTransitionComponent.h"
#include "component/SolidCollisionComponent.h"
#include "component/StaticVisibleComponent.h"
#include "component/TileComponent.h"

#include <cassert>

namespace Resource
{
	void ComponentResource::initiate(const ComponentID componentID)
	{
		switch (componentID)
		{
		case ComponentID::DynamicWorldTransition:
			initiateFromComponent(new DynamicWorldTransitionComponent);

			break;
		case ComponentID::SolidCollision:
			initiateFromComponent(new SolidCollisionComponent);

			break;
		case ComponentID::StaticVisible:
			initiateFromComponent(new StaticVisibleComponent);

			break;
		case ComponentID::Tile:
			initiateFromComponent(new TileComponent);

			break;
		default:
			assert(true);
		}
	}
}
