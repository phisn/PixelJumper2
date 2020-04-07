#pragma once

#include "Environment.h"
#include "TileComponent.h"

#include "trait/DynamicWorldEntry.h"
#include "trait/DynamicWorldExit.h"

#include "ResourceCore/component/DynamicTransitionComponent.h"

namespace Game
{
	// duplex
	class DynamicWorldTransitionComponent
		:
		public Component,
		public CollidableTraitHandler,
		public DynamicWorldEntryHandler,
		public DynamicWorldExitHandler
	{
	public:
		DynamicWorldTransitionComponent(const Resource::ComponentResource* const resource)
			:
			Component(Resource::ComponentID::DynamicWorldTransition),
			content(((Resource::DynamicWorldTransitionComponent*)
				resource->getInstance<Resource::DynamicWorldTransitionComponent>()
			)->content)
		{
		}

		bool initialize(EntityView* const entity) override
		{
			tileComponent = entity->findComponent<TileComponent>(Resource::ComponentID::Tile);
			return tileComponent != NULL;
		}
		
		void registerTraits(EnvironmentView* const environment) override
		{
			CollidableTrait collidableTrait;
			collidableTrait.handler = this;
			collidableTrait.info.position = sf::Vector2f(tileComponent->content->position);
			collidableTrait.info.size = sf::Vector2f(tileComponent->content->size);

			environment->pushCollidableTrait(
				CollisionType{ true, false, true },
				collidableTrait);

			environment->pushDynamicWorldEntry(
				this,
				content.worldEntryID);
			environment->pushDynamicWorldExit(this);
		}

		void handleWorldEntry(
			PlayerBase* const player,
			const DynamicWorldEntryEvent& dwee) override
		{
			player->getProperties().position += sf::Vector2f(tileComponent->content->position) - dwee.offsetSource;
		}

		void notifyOnExit()
		{
			DynamicWorldExitEvent event;

			event.offset = sf::Vector2f(tileComponent->content->position);
			event.targetWorld = content.targetWorld;
			event.targetEntry = content.targetEntry;
			
			DynamicWorldExitHandler::notifyOnExit(event);
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const CollidableEvent& collision) override
		{
			sf::Vector2f result;

			if (collision.info.isHorizontal())
			{
				collision.player->getProperties().position =
				{
					collision.target.x,
					collision.info.position.y
				};

				result = { 0.f, collision.target.y - collision.info.position.y };
			}
			else
			{
				collision.player->getProperties().position =
				{
					collision.info.position.x,
					collision.target.y
				};

				result = { collision.target.x - collision.info.position.x, 0.f };
			}

			notifyOnExit();
			return result;
		}

	private:
		TileComponent* tileComponent;
		const Resource::DynamicWorldTransitionComponentContent content;
	};
}
