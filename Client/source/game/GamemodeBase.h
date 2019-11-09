#pragma once

#include <Client/source/game/UserConnection.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>

#include <Client/source/framework/Context.h>

#include <vector>

namespace Game
{
	// gamemode must be able to sync players
	// gamemode should only sync internal gamemode
	// values, not dependent on the creator
	class GamemodeBase
		:
		public GameState
	{
	public:
		virtual bool initialize() = 0;
		virtual void processLogic() = 0;

	};

	class GamemodeCreatorBase
		:
		public LazyGameState
	{
	public:
		// creator does not handle gamemode state
		virtual GamemodeBase* createGamemode() = 0;
	};

	class ClassicGamemode
		:
		public GamemodeBase
	{
	public:
		typedef std::vector<Resource::World*> WorldResources;

		ClassicGamemode(const WorldResources& worldResources)
			:
			worldResources(worldResources)
		{
		}

		bool initialize()
		{
			if (player == NULL)
			{
				Log::Error(L"Got empty player in classic gamemode");

				return false;
			}

			// or preload all worlds?
			loadWorld(worldResources[0]);

			return currentWorld->initialize();
		}

		void processLogic()
		{
			if (requestWorldSwitch)
			{
				if (currentWorld == NULL)
				{
					Log::Error(L"Got empty world in classic gamemode logic process");

					return;
				}

				for (int i = 0; i < worldResources.size(); ++i)
					if (worldResources[i]->HeaderIntro.worldID == currentWorld->getInformation()->worldId)
					{
						loadWorld(worldResources[
							i - 1 == worldResources.size() ? 0: i + 1
						]);
					}

				// req sync
			}

			currentWorld->processLogic();
		}

		void registerPlayer(PlayerBase* const player)
		{
			this->player = player;
		}

		World* getCurrentWorld() const
		{
			return currentWorld;
		}

		bool writeState(Resource::WritePipe* const writePipe)
		{
			Content content;

			if (currentWorld == NULL)
			{
				Log::Warning(L"Creating empty sync in classic gamemode");

				content.hasWorld = false;
				return writePipe->writeValue(&content);
			}

			content.hasWorld = true;
			content.world = currentWorld->getInformation()->worldId;

			return writePipe->writeValue(&content)
				&& currentWorld->writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe)
		{
			Content content;

			if (!readPipe->readValue(&content))
			{
				return false;
			}

			if (!content.hasWorld)
			{
				Log::Warning(L"Got empty sync in classic gamemode");

				delete currentWorld;
				currentWorld = NULL;

				return false;
			}

			if (currentWorld == NULL || // null check first
				currentWorld->getInformation()->worldId != content.world)
			{
				for (Resource::World* world : worldResources)
					if (world->HeaderIntro.worldID == content.world)
					{
						loadWorld(world);
						goto WORLD_FOUND;
					}

				Log::Error(L"Got invalid world in classic gamemode");
				return false;

			WORLD_FOUND:
			}

			return currentWorld->readState(readPipe);
		}

	private:
		struct Content
		{
			bool hasWorld;
			Resource::WorldId world;
		};

		void loadWorld(Resource::World* const worldResource)
		{
			if (currentWorld != NULL)
			{
				delete currentWorld;
			}

			currentWorld = new World(worldResource);
			currentWorld->addPlayer(player);
		}

		void registerExitTiles()
		{
			for (ExitableTile* tile : currentWorld->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.addListener(
					[this]()
					{
						requestWorldSwitch = true;
					});
			}
		}

		PlayerBase* player;

		bool requestWorldSwitch = false;
		World* currentWorld = NULL;
		const WorldResources& worldResources;
	};

	class ClassicGamemodeCreator
		:
		public GamemodeCreatorBase
	{
	public:
		GamemodeBase* createGamemode() override
		{
			return new ClassicGamemode(worlds);
		}

	private:
		std::vector<Resource::World*> worlds;
	},
}
