#pragma once

#include "GameCore/Component.h"
#include "ResourceCore/component/StaticVisibleComponent.h"
#include "TileComponent.h"
#include "GameCore/trait/StaticVisibleTrait.h"

namespace Game
{
	class StaticVisibleComponent
		:
		public Component
	{
	public:
		StaticVisibleComponent(const Resource::ComponentResource* const content)
			:
			Component(Resource::ComponentID::StaticVisible),
			content(((Resource::StaticVisibleComponent*) content->getInstance<Resource::TileComponent>())->content)
		{
		}

		bool initialize(EntityView* const entity) override
		{
			tileComponent = entity->findComponent<TileComponent>(Resource::ComponentID::Tile);
			return tileComponent != NULL;
		}

		void registerTraits(EnvironmentView* const environment) override
		{
			Game::StaticVisibleTrait trait;
			trait.position = sf::Vector2f(tileComponent->content->position);
			trait.size = sf::Vector2f(tileComponent->content->size);
			trait.color = content.color;

			environment->pushStaticVisibleTrait(trait);
		}

	private:
		TileComponent* tileComponent;
		Resource::StaticVisibleComponentContent content;
	};
}
