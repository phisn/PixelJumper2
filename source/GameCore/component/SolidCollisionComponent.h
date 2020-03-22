#pragma once

#include "Component.h"
#include "ResourceCore/component/SolidCollisionComponent.h"
#include "TileComponent.h"
#include "trait/CollidableTrait.h"

namespace Game
{
	class SolidCollisionComponent
		:
		public Component,
		public CollidableTraitHandler
	{
	public:
		SolidCollisionComponent(const Resource::ComponentResource* const content)
			:
			content(*(Resource::SolidCollisionComponentContent*) content->getInstance<Resource::TileComponent>())
		{
		}

		sf::Vector2f onCollision(
			const CollisionType type,
			const CollidableEvent& collision) override
		{
			collision.player->getProperties().position = collision.info.position;

			sf::Vector2f movement = {};
			sf::Vector2f remainOffset = {};

			if (collision.info.isHorizontal())
			{
				movement.y = collision.player->getProperties().movement->y;
				remainOffset.y = (collision.target.y - collision.info.position.y);

				if (fabsf(movement.y) < content.friction)
				{
					movement.y = 0;
				}
				else
				{
					movement.y += movement.y > 0 ? -content.friction : content.friction;
				}
			}
			else
			{
				movement.x = collision.player->getProperties().movement->x;
				remainOffset.x = (collision.target.x - collision.info.position.x);

				if (fabsf(movement.x) < content.friction)
				{
					movement.x = 0;
				}
				else
				{
					movement.x += movement.x > 0 ? -content.friction : content.friction;
				}
			}

			collision.player->getCollisionContainer().setCollider(
				collision.info.type,
				CollisionContainer::Collider{
					content.inputForceAddition,
					content.density
				});

			collision.player->getProperties().movement = movement;
			return remainOffset;
		}

		bool initialize(EntityView* const entity) override
		{
			tileComponent = entity->findComponent<TileComponent>(Resource::ComponentID::Tile);
			return tileComponent != NULL;
		}

		void registerTraits(EnvironmentView* const environment) override
		{
			Game::CollidableTrait trait;

			trait.handler = this;
			trait.info.position = sf::Vector2f(tileComponent->content->position);
			trait.info.size = sf::Vector2f(tileComponent->content->size);

			environment->pushCollidableTrait(
				CollisionType::NormalCollision,
				trait);
		}

	private:
		TileComponent* tileComponent;
		Resource::SolidCollisionComponentContent content;
	};
}
