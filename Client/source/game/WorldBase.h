#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/WorldState.h>

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/InitializableTile.h>

#include <Client/source/resource/WorldResource.h>

#include <vector>

namespace Game
{
	class WorldBase
	{
	public:
		WorldState state;

		// return in percent lose of movement
		static float CalculateFrictionLose(const float friction, const float weight)
		{
			const float positiveTileDragValue = fabs(friction);
			const float positivePlayerWeight = fabs(weight);

			return positivePlayerWeight / (positiveTileDragValue + positivePlayerWeight);
		}
		
		static float FrictionLoseOverTime(const float lose, const float timeValue)
		{
			return std::powf(lose, timeValue);
		}

		bool initialize(
			Resource::World* const resource)
		{
			for (GameTileBase* const tile : environment.getTileType<GameTileBase>())
			{
				tile->registerType(&environment);
			}

			for (InitializableTile* const tile : environment.getTileType<InitializableTile>())
			{
				tile->initialize(this);
			}

			worldProperties.worldId = resource->HeaderIntro.worldID;
		}

		virtual void onDraw()
		{
			environment.draw();

			for (PlayerBase* const player : players)
			{
				player->draw();
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			sf::Uint64 fullTimeValue = time.asMicroseconds() / 1000;

			do {
				fullTimeValue -= TIME_STEP;

				for (PlayerBase* const player : players)
				{
					player->collisionContainer.clear();
					
					simulatePlayers(
						player,
						fullTimeValue < 0
							? fullTimeValue + TIME_STEP
							: TIME_STEP
					);
				}
			} while (fullTimeValue > 0);

			environment.onLogic(time);
		}

		Resource::WorldId getWorldId() const
		{

		}

		const Environment* getEnvironment() const
		{
			return &environment;
		}

		const std::vector<PlayerBase*>& getPalyers() const
		{
			return players;
		}

	protected:
		Environment environment;
		std::vector<PlayerBase*> players;

	private:
		static const sf::Uint64 TIME_STEP = 1000;

		struct
		{
			Resource::WorldId worldId;

		} worldProperties;

		void simulatePlayers(PlayerBase* const player, const float timeValue)
		{
			applyGravity(player, timeValue);
			applyFriction(player, timeValue);
			moveTo(
				player,
				player->state.readProperties()->position
				* player->state.readProperties()->speed
				* timeValue);
		}

		void applyGravity(PlayerBase* const player, const float timeValue)
		{
			player->state.movement = player->state.readProperties()->movement 
				+ state.readProperties()->gravity 
				* timeValue 
				* player->state.readProperties()->speed;
		}

		void applyFriction(PlayerBase* const player, const float timeValue)
		{
			player->state.movement = player->state.readProperties()->movement
				* FrictionLoseOverTime(
					CalculateFrictionLose(
						player->state.readProperties()->friction
						+ state.readProperties()->friction,
						player->state.readProperties()->weight
					),
					timeValue);
		}
		
		void moveTo(PlayerBase* const player, sf::Vector2f target)
		{
			/*

				Very unoptimized

			*/

			struct
			{
				const CollisionType* type;
				CollidableTile* tile;

				CollisionEngine::CollisionInfo info;
				float distance = std::numeric_limits<float>::max();

			} closestCollision;

			closestCollision.tile = NULL; // == NOT_FOUND

			do
			{
				for (const CollisionType& collisionType : environment.getCollisionTypes())
				{
					const CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupCollisionContext(
						player->state.readProperties()->position,
						target,
						collisionType);

					for (CollidableTile* const tile : environment.getCollisionTileType(collisionType))
						if (CollisionEngine::FindCollision(
							&collisionContext,
							tile->getSize(),
							tile->getPosition()))
						{
							const CollisionEngine::CollisionInfo currentInfo = CollisionEngine::GetLastCollision();

							const sf::Vector2f currentDistanceVector = player->state.readProperties()->position - currentInfo.position;
							const float currentDistance = sqrtf(
								currentDistanceVector.x * currentDistanceVector.x + // abs not needed (-1 * -1) == 1
								currentDistanceVector.y * currentDistanceVector.y
							);

							if (currentDistance < closestCollision.distance)
							{
								closestCollision.distance = currentDistance;
								closestCollision.tile = tile;
								closestCollision.info = currentInfo;
								closestCollision.type = &collisionType;
							}
						}

					if (closestCollision.tile != NULL)
					{
						Collision collision;

						collision.info = closestCollision.info;
						collision.player = player;
						collision.target = target;

						target = closestCollision.tile->onCollision(
							*closestCollision.type, collision);
						player->collisionContainer.setCollision(
							collision.info.type, 
							closestCollision.tile);
					}
					else
					{
						player->state.position = target;

						break;
					}
				}

			} while (player->state.readProperties()->position != target);
		}
	};
}
