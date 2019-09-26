#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/PlayerProperties.h>
#include <Client/source/game/PlayerRepresentation.h>

#include <map>

namespace Game
{
	class CollidableTile;
	class CollisionContainer
	{
	public:
		bool has(
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return !!infos[type];
		}

		CollidableTile* operator[](
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return infos[type];
		}

		void clear()
		{
			infos[0] = { };
			infos[1] = { };
			infos[2] = { };
			infos[3] = { };
		}

		void setCollision(
			const CollisionEngine::CollisionInfo::Type type,
			CollidableTile* const tile)
		{
			infos[type] = tile;
		}

	private:
		CollidableTile* infos[4];
	};

	static float WeightWithFriction(
		const float friction,
		const float weight)
	{
		const float positiveWeight = fabsf(weight);
		return positiveWeight / (positiveWeight + fabsf(friction));
	}

	static sf::Vector2f ApplyFriction(
		const PlayerProperties* const properties,
		const float additionalFriction)
	{
		const float friction = additionalFriction + properties->friction;

		sf::Vector2f movement = properties->movement;

		movement *= WeightWithFriction(
			friction,
			properties->weight);

		if (!properties->inputReducedFriction)
		{
			if (fabsf(movement.x) < friction)
			{
				movement.x = 0;
			}
			else
			{
				movement.x += movement.x < 0
					? friction
					: -friction;
			}

			if (fabsf(movement.y) < friction)
			{
				movement.y = 0;
			}
			else
			{
				movement.y += movement.y < 0
					? friction
					: -friction;
			}
		}

		return movement;
	}

	class PlayerBase
	{
		friend class World;
	public:	
		PlayerBase(const PlayerInformation information)
			:
			information(information),
			representation(PlayerRepresentation::Create(information))
		{
			// properties.loadDefault();

			properties.position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					representationNeedsUpdate = true;
				});

			representation->setSize({ 1.f , 1.f });
		}

		~PlayerBase()
		{
			delete representation;
		}

		virtual void onLogic(const sf::Time time)
		{
			representation->update(time);
		}

		virtual void onDraw(sf::RenderTarget* const target)
		{
			if (representationNeedsUpdate)
			{
				updateRepresentation();
			}

			representation->draw(target);
		}

		const CollisionContainer& getCollisionContainer() const
		{
			return collisionContainer;
		}

		PlayerProperties& getProperties()
		{
			return properties;
		}

		const PlayerInformation& getInformation() const
		{
			return information;
		}

	protected:
		PlayerProperties properties;
		CollisionContainer collisionContainer;

	private:
		bool representationNeedsUpdate = true;

		void updateRepresentation()
		{
			representation->setPosition(properties.position);
		}

		PlayerRepresentation* const representation;
		const PlayerInformation information;
	};
}