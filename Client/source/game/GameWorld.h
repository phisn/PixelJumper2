#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/GamePlayer.h>

#include <Client/source/game/WorldInformation.h>

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

		void removePlayer(PlayerBase * const player)
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
					player->process();
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
		}

		void applyAirResistance(PlayerBase* const player)
		{
		}

		void applyMovement(PlayerBase* const player)
		{
		}
	};
}
