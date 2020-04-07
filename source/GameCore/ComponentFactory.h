#pragma once

#include "Entity.h"
#include "ResourceCore/ComponentResource.h"

namespace Game::ComponentFactory
{
	bool CreateComponent(Entity* const entity,
		const Resource::ComponentResource* const componentResource);
}
