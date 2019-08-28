#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/PlayerProperties.h>
#include <Client/source/game/PlayerRepresentation.h>
#include <Client/source/game/WorldProperties.h>

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

	class PlayerBase
	{
	public:	
		PlayerBase(const Resource::PlayerResource* const resource)
			:
			information(PlayerInformation::Create(resource)),
			representation(PlayerRepresentation::Create(information))
		{
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
		CollisionContainer collisionContainer; // expose

	private:
		bool representationNeedsUpdate = true;

		void updateRepresentation()
		{
			representation->setPosition(properties.position);
		}

		PlayerRepresentation* const representation;
		const PlayerInformation information;

	public:
		void setWorld(WorldProperties* const world)
		{
			currentWorld = world;
		}

		void process()
		{
			processGravity();
			processAirResistance();
			processMovement();
		}

	private:
		void processGravity()
		{
			properties.movement += *currentWorld->gravity * *properties.speed;
		}

		void processAirResistance()
		{

		}

		void processMovement()
		{
		}

		const WorldProperties* currentWorld;
	};
}