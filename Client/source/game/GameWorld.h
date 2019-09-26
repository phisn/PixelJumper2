#pragma once


#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/tiletrait/CollidableTile.h>

#include <Client/source/game/Environment.h>
#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/WorldInformation.h>
#include <Client/source/game/WorldProperties.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/shared/tiles/TileDescription.h>

#include <vector>

namespace Game
{
	constexpr sf::Uint64 LogicTimeStep = 1'000;

	class World
	{
		const Resource::World* const resource;
		typedef std::vector<PlayerBase*> PlayerContainer;

	public: // init
		World(const Resource::World* const resource)
			:
			resource(resource),
			information(WorldInformation::Create(resource))
		{
			properties.loadDefault(information);
		}

		~World()
		{
		}

		bool initialize()
		{
			Log::Section section(L"Creating World");

			section.information(L"Map: " + resource->HeaderAuth.mapName);
			section.information(L"Author: " + resource->HeaderAuth.authorName);
			section.information(L"ID: " + std::to_wstring(resource->HeaderIntro.worldID));
			section.information(L"TileCount: " + std::to_wstring(resource->HeaderProperties.tileCount));
			section.information(L"Width: " + std::to_wstring(resource->HeaderProperties.width));
			section.information(L"Height: " + std::to_wstring(resource->HeaderProperties.height));

			if (!environment.initialize(resource))
			{
				section.error(L"Failed to initialize environment");
				return false;
			}

			if (!initializeTiles())
			{
				section.error(L"Failed to initialize tiles");
				return false;
			}

			return true;
		}

		const Environment* getEnvironment() const
		{
			return &environment;
		}

		const PlayerContainer& getPlayers() const
		{
			return players;
		}

		const WorldInformation* getInformation() const
		{
			return &information;
		}

	protected:
		Environment environment;
		WorldProperties properties;

	private:
		bool initializeTiles();

		const WorldInformation information;

	public: // player
		void addPlayer(PlayerBase* const player)
		{
			player->getProperties().loadDefault(information);
			players.push_back(player);

			properties.setPlayerCountValue(*properties.playerCount + 1);
		}

		void removePlayer(PlayerBase* const player)
		{
			PlayerContainer::const_iterator iterator = std::find(
				players.cbegin(), players.cend(), player
			);

			if (iterator != players.cend())
			{
				players.erase(iterator);

				properties.setPlayerCountValue(*properties.playerCount - 1);
				assert(*properties.playerCount >= 0);

				// uninitialize listner?
			}
		}
	private:
		PlayerContainer players;

	public: // access
		virtual void draw(sf::RenderTarget* const target)
		{
			environment.draw(target);

			for (PlayerBase* const player : players)
			{
				player->onDraw(target);
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > LogicTimeStep)
			{
				for (PlayerBase* const player : players)
				{
					processPlayer(player);
				}

				environment.onLogic(
					sf::microseconds(LogicTimeStep)
				);

				logicCounter -= LogicTimeStep;
			}

			for (PlayerBase* const player : players)
			{
				player->onLogic(time);
			}
		}

	private:
		sf::Uint64 logicCounter = 0;

	private: // player movement
		void processPlayer(PlayerBase* const player)
		{
			applyGravity(player);
			applyAirResistance(player);
			applyMovement(player);
		}

		void applyGravity(PlayerBase* const player)
		{
			player->properties.movement += *properties.gravity;
		}

		void applyAirResistance(PlayerBase* const player)
		{
			player->properties.movement *= WeightWithFriction(
				player->properties.weight,
				properties.airResistance);
		}

		void applyMovement(PlayerBase* const player)
		{
			player->collisionContainer.clear();

			struct
			{
				const CollisionType* type;
				CollidableTile* tile = NULL;
				CollisionEngine::CollisionInfo info;
				float distance = 0;

			} collisionData;

			sf::Vector2f offset = player->getProperties().movement;

			while (offset.x != 0 || offset.y != 0)
			{
				const sf::Vector2f target = offset + *player->properties.position;

				for (const CollisionType& collisionType : environment.getCollisionTypes())
				{
					const CollisionEngine::CollisionContext collisionContext =
						CollisionEngine::SetupCollisionContext(
							player->properties.position,
							target,
							collisionType
						);

					for (CollidableTile* const tile : environment.getCollisionTileType(collisionType))
						if (CollisionEngine::FindCollision(
								&collisionContext,
								tile->getSize(),
								tile->getPosition()))
						{
							const CollisionEngine::CollisionInfo collisionInfo =
								CollisionEngine::GetLastCollision();
							
							const sf::Vector2f collisionDistanceVector = *player->properties.position - collisionInfo.position;
							const float collisionDistance = sqrtf(
								collisionDistanceVector.x * collisionDistanceVector.x + // abs not needed (-1 * -1) == 1
								collisionDistanceVector.y * collisionDistanceVector.y
							);

							if (collisionDistance < collisionData.distance || collisionData.tile == NULL)
							{
								collisionData.distance = collisionDistance;
								collisionData.tile = tile;
								collisionData.info = collisionInfo;
								collisionData.type = &collisionType;
							}
						}
				}

				if (collisionData.tile)
				{
					Collision collision;

					collision.info = collisionData.info;
					collision.player = player;
					collision.target = target;
					// collision.timeValue = 

					offset = collisionData.tile->onCollision(
						*collisionData.type,
						collision);
					player->collisionContainer.setCollision(
						collision.info.type,
						collisionData.tile);

					collisionData.tile = NULL;
				}
				else
				{
					player->properties.position = target;
					
					break;
				}
			}
		}


	};
}
