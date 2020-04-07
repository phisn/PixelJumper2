#include "ComponentFactory.h"
#include "component/SolidCollisionComponent.h"
#include "component/StaticVisibleComponent.h"
#include "component/TileComponent.h"

namespace Game
{
	bool ComponentFactory::CreateComponent(Entity* const entity, 
		const Resource::ComponentResource* const componentResource)
	{
		switch (componentResource->getID())
		{
		case Resource::ComponentID::DynamicWorldTransition:

			return true;
		case Resource::ComponentID::CommonExit:

			return true;
		case Resource::ComponentID::SolidCollision:
			entity->registerComponent(
				new SolidCollisionComponent(componentResource)
			);

			return true;
		case Resource::ComponentID::StaticVisible:
			entity->registerComponent(
				new StaticVisibleComponent(componentResource)
			);

			return true;
		case Resource::ComponentID::Tile:
			entity->registerComponent(
				new TileComponent(componentResource)
			);

			return true;
		}

		return false;
	}
}
