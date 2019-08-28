#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/GamePlayer.h>

#include <Client/source/game/WorldInformation.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/shared/tiles/TileDescription.h>

#include <vector>

namespace Game
{
	struct WorldState
	{
		sf::Uint16 playerCount;
		
		sf::Vector2f gravity;
		float airResistance;

		sf::Uint32 milisecondsPassed;
	};

	class WorldProperties
		:
		public GameState
	{
		friend class World;
	public:
		WorldProperties()
		{
			playerCount = 0;
			milisecondsPassed = 0;
		}
		
		// setPlayerCountValue
		ReadOnlyProperty<sf::Uint16, WorldProperties> playerCount;

		Property<sf::Vector2f> gravity;
		Property<float> airResistance;

		Property<sf::Uint32> milisecondsPassed;

		void loadDefault(const WorldInformation info)
		{
			airResistance = info.defaultWorldProperties.airResistance;
			gravity = info.defaultWorldProperties.gravity;

			update();
		}

		void update()
		{
			playerCount.update();
			gravity.update();
			airResistance.update();
			milisecondsPassed.update();
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			WorldState rpp = createState();
			return writePipe->writeValue(&rpp);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			WorldState rpp;

			if (!readPipe->readValue(&rpp))
			{
				return false;
			}

			playerCount = rpp.playerCount;
			gravity = rpp.gravity;
			airResistance = rpp.airResistance;
			milisecondsPassed = rpp.milisecondsPassed;

			update(); // TODO: ?

			return true;
		}

		WorldState createState() const
		{
			WorldState rpp;

			rpp.playerCount = playerCount;
			rpp.gravity = gravity;
			rpp.airResistance = airResistance;
			rpp.milisecondsPassed = milisecondsPassed;

			return rpp;
		}

	private:
		void setPlayerCountValue(const sf::Uint16 value)
		{
			playerCount = value;
		}
	};

	class World
	{
		const Resource::World* const resource;

		typedef std::vector<PlayerBase*> PlayerContainer;

	public:
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

		virtual void draw(sf::RenderTarget* const target)
		{
			environment.draw(target);

			for (PlayerBase* const player : players)
			{
				player->onDraw(target);
			}
		}

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
			}
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
		PlayerContainer players;

		Environment environment;
		WorldProperties properties;

	private:
		bool initializeTiles();

		const WorldInformation information;
	};
}
