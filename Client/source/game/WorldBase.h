#pragma once

#include <Client/source/game/GameTileFactory.h>
#include <Client/source/game/Environment.h>
#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/WorldState.h>

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/InitializableTile.h>

#include <Client/source/resource/WorldResource.h>

#include <Client/source/logger/Logger.h>

#include <vector>

namespace Game
{
	class WorldBase
	{
	public:
		WorldState state;

		bool initialize(
			Resource::World* const resource)
		{
			Log::Section section(L"Creating world from WorldResource");

			section.information(L"Map: " + resource->HeaderAuth.mapName);
			section.information(L"Author: " + resource->HeaderAuth.authorName);
			section.information(L"ID: " + std::to_wstring(resource->HeaderIntro.worldID));
			section.information(L"TileCount: " + std::to_wstring(resource->HeaderProperties.tileCount));
			section.information(L"Width: " + std::to_wstring(resource->HeaderProperties.width));
			section.information(L"Height: " + std::to_wstring(resource->HeaderProperties.height));

			worldProperties.authorName = resource->HeaderAuth.authorName;
			worldProperties.mapName = resource->HeaderAuth.mapName;

			for (Resource::Tile& tileResource : resource->TileContainer)
			{
				GameTileBase* const tile = GameTileFactory::Create(&tileResource);

				if (tile == NULL)
				{
					delete tile;
					section.error(L"Failed to create tile");
					return false;
				}

				environment.registerTile<GameTileBase>(tile);
			}

			for (GameTileBase* const tile : environment.getTileType<GameTileBase>())
			{
				tile->registerType(&environment);
			}

			for (InitializableTile* const tile : environment.getTileType<InitializableTile>())
			{
				tile->initialize(this);
			}

			worldProperties.worldId = resource->HeaderIntro.worldID;

			if (!environment.initialize(resource))
			{
				return false;
			}

			return true;
		}

		virtual void draw()
		{
			environment.draw();

			for (PlayerBase* const player : players)
			{
				player->draw();
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			sf::Int64 fullTimeValue = time.asMicroseconds() / 1000;

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

		void addPlayer(PlayerBase* const player)
		{
			players.push_back(player);

			player->state.speed = 0.0012f;
			player->state.weight = 1.f;
			player->state.friction = 0.f;
			player->state.position = { 2, 0 };

			player->initializeFromState();
		}

		Resource::WorldId getWorldId() const
		{
			return worldProperties.worldId;
		}

		const Environment* getEnvironment() const
		{
			return &environment;
		}

		const std::vector<PlayerBase*>& getPlayers() const
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
			std::wstring authorName;
			std::wstring mapName;

			Resource::WorldId worldId;
			sf::Vector2f size;

		} worldProperties;

		void simulatePlayers(
			PlayerBase* const player, 
			const float timeValue)
		{
			applyGravity(player, timeValue);
			applyAirResistance(player, timeValue);

			moveTo(player, timeValue);
		}

		void applyGravity(PlayerBase* const player, const float timeValue)
		{
			player->state.movement = player->state.readProperties()->movement 
				+ state.readProperties()->gravity 
				* timeValue 
				* player->state.readProperties()->speed;
		}

		void applyAirResistance(
			PlayerBase* const player,
			const float timeValue)
		{
			player->state.movement = player->state.readProperties()->movement
				* CalculateFrictionLose(
					state.readProperties()->airResistance,
					player->state.readProperties()->weight
				);
		}
		
		void moveTo(PlayerBase* const player, const float timeValue)
		{
			sf::Vector2f targetOffset = player->state.readProperties()->movement
				* player->state.readProperties()->speed
				* timeValue;

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

			closestCollision.tile = NULL;

			do
			{
				const sf::Vector2f currentTarget = player->state.readProperties()->position + targetOffset;

				for (const CollisionType& collisionType : environment.getCollisionTypes())
				{
					const CollisionEngine::CollisionContext collisionContext = CollisionEngine::SetupCollisionContext(
						player->state.readProperties()->position,
						currentTarget,
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
				}

				if (closestCollision.tile != NULL)
				{
					Collision collision;

					collision.info = closestCollision.info;
					collision.player = player;
					collision.target = currentTarget;

					const sf::Vector2f deltaCollision = player->state.readProperties()->position 
						- closestCollision.info.position;
					/*
					if (deltaCollision.x != 0)
					{
						collision.timeValue = timeValue - (deltaCollision.x / targetOffset.x) * timeValue;
					}
					else if (deltaCollision.y != 0)
					{
						collision.timeValue = timeValue - (deltaCollision.y / targetOffset.y) * timeValue;
					}
					else
					{
						collision.timeValue = 0;
					}*/

					collision.timeValue = timeValue;

					targetOffset = closestCollision.tile->onCollision(
						*closestCollision.type, collision);
					player->collisionContainer.setCollision(
						collision.info.type,
						closestCollision.tile);


					// replace
					closestCollision.tile = NULL;
					closestCollision.distance = std::numeric_limits<float>::max();
				}
				else
				{
					player->state.position = currentTarget;

					break;
				}

			} while (targetOffset.x != 0 || targetOffset.y != 0);
		}
	};
}
