#pragma once

#include "ResourceCore/component/TileComponent.h"
#include "GameCore/Component.h"

namespace Game
{
	// does currently only provide position for other traits
	class TileComponent
		:
		public Component
	{
	public:
		TileComponent(const Resource::ComponentResource* const resource)
			:
			Component(Resource::ComponentID::Tile),
			content(&resource->getInstance<Resource::TileComponent>()->content)
		{
		}

		bool initialize(EntityView* const entity) override
		{
			return true;
		}

		void registerTraits(EnvironmentView* const environment) override
		{
		}

		Resource::TileComponentContent* const content;
	};
}
