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
	// in microseconds
	constexpr sf::Uint64 LogicTimeStep = 1'000;

	class World
		:
		public GameState
	{
	protected:
		typedef PlayerBase PlayerType;
		typedef std::vector<PlayerType*> PlayerContainer;

		const Resource::World* const resource;

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

			section.information(L"Map: " + resource->content.author);
			section.information(L"ID: " + std::to_wstring(resource->content.id));
			section.information(L"TileCount: " + std::to_wstring(resource->tiles.size()));
			section.information(L"Width: " + std::to_wstring(resource->content.width));
			section.information(L"Height: " + std::to_wstring(resource->content.height));

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

		bool initializeGraphics()
		{
			// maybe initialize some tiles individually if needed
			// (feature as tiletrait)
			return environment.initializeGraphics();
		}

		void processLogic()
		{
			for (PlayerBase* const player : players)
			{
				processPlayer(player);
			}

			environment.processLogic();

			properties.increaseTickCount();
			properties.update();
		}

		bool writeState(Resource::WritePipe* const writePipe)
		{
			if (!properties.writeState(writePipe))
			{
				return false;
			}

			for (PlayerBase* const player : players)
				if (!player->writeState(writePipe))
				{
					return false;
				}

			return true;
		}

		bool readState(Resource::ReadPipe* const readPipe)
		{
			if (!properties.readState(readPipe))
			{
				return false;
			}

			for (PlayerBase* const player : players)
				if (!player->readState(readPipe))
				{
					return false;
				}

			return true;
		}

		const Environment* getEnvironment() const
		{
			return &environment;
		}

		WorldProperties& getProperties()
		{
			return properties;
		}

		const WorldProperties& getProperties() const
		{
			return properties;
		}

		const WorldInformation* getInformation() const
		{
			return &information;
		}

	protected:
		const WorldInformation information;

		Environment environment;
		WorldProperties properties;

	private:
		bool initializeTiles();

	public: // player
		void addPlayer(PlayerType* const player)
		{
			player->getProperties().loadDefault(information);
			players.push_back(player);

			properties.setPlayerCount(*properties.playerCount + 1);
		}

		void removePlayer(PlayerType* const player)
		{
			PlayerContainer::const_iterator iterator = std::find(
				players.cbegin(), players.cend(), player
			);

			if (iterator != players.cend())
			{
				players.erase(iterator);

				properties.setPlayerCount(*properties.playerCount - 1);
				assert(*properties.playerCount >= 0);

				// uninitialize listener?
				
			}
		}

	protected:
		PlayerContainer players;

		void processPlayer(PlayerType* const player)
		{
			player->onInternalUpdate(); // before or after process?

			applyGravity(player);
			applyResistance(player);
			applyMovement(player);
		}
		
	private:
		void applyGravity(PlayerType* const player)
		{
			player->properties.movement += *properties.gravity;
		}

		void applyResistance(PlayerType* const player)
		{
			player->properties.movement *= *properties.airResistance;
		}

		void applyMovement(PlayerType* const player)
		{
			player->collisionContainer.reduceFrame();

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
					collision.player = (PlayerBase*) player;
					collision.target = target;

					offset = collisionData.tile->onCollision(
						*collisionData.type,
						collision);

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
