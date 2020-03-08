#include "ComponentFactory.h"
#include "component/SolidCollisionComponent.h"

namespace Game
{
	bool ComponentFactory::CreateComponent(Entity* const entity, 
		Resource::ComponentResource* const componentResource)
	{
		switch (componentResource->getID())
		{
		case Resource::ComponentID::DynamicWorldTransition:

			return true;
		case Resource::ComponentID::CommonExit:

			return true;
		case Resource::ComponentID::SolidCollision:
			entity->registerComponent(new SolidCollisionComponent(
				componentResource->getInstance<Resource::SolidCollisionComponent>())
			);

			return true;
		case Resource::ComponentID::StaticVisible:

			return true;
		case Resource::ComponentID::Tile:

			return true;
		}

		return false;
	}
}
