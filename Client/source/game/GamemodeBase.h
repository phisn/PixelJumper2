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
		virtual bool processLogic() = 0;

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
		
		ClassicGamemode(
			const WorldResources& worldResources)
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

			return loadWorld(worldResources[0]);
		}

		bool processLogic() override
		{
			if (requestWorldSwitch)
			{
				Log::Section section(L"Switching world in classic gamemode");

				if (!switchWorld())
				{
					Log::Error(L"Failed to initialize world");

					delete currentWorld;
					currentWorld = NULL;

					return false;
				}
			}

			currentWorld->processLogic();

			return true;
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
						if (!loadWorld(world))
						{
							return false;
						}

						goto WORLD_FOUND;
					}

				Log::Error(L"Got invalid world in classic gamemode");
				return false;

			WORLD_FOUND: __noop;
			}

			return currentWorld->readState(readPipe);
		}

	private:
		struct Content
		{
			bool hasWorld;
			Resource::WorldId world;
		};

		bool switchWorld()
		{
			for (int i = 0; i < worldResources.size(); ++i)
				if (worldResources[i]->HeaderIntro.worldID == currentWorld->getInformation()->worldId)
				{
					return loadWorld(worldResources[
						i - 1 == worldResources.size() ? 0 : i + 1
					]);
				}

			Log::Warning(
				L"Current world not found, "
				L"resource propably missing"
				L"because of creator sync. "
				L"Loading first");

			return loadWorld(worldResources[0]);
		}

		bool loadWorld(Resource::World* const worldResource)
		{
			if (currentWorld != NULL)
			{
				delete currentWorld;
			}

			currentWorld = new World(worldResource);
			currentWorld->addPlayer(player);

			return currentWorld->initialize();
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
		bool writeChangedState(Resource::WritePipe* const writePipe)
		{
			const int count = worlds.size();

			if (!writePipe->writeValue(&count))
			{
				return false;
			}

			for (Resource::World* const world : worlds)
				if (!world->save(writePipe))
				{
					return false;
				}

			return true;
		}

		bool readChangedState(Resource::ReadPipe* const readPipe)
		{
			int count;
			if (!readPipe->readValue(&count))
			{
				return false;
			}

			std::vector<Resource::World*> tempWorlds(count);

			for (int i = 0; i < count; ++i)
			{
				Resource::World* const world = tempWorlds[i] = new Resource::World();
				
				if (!world->make(readPipe))
				{
					for (Resource::World* const world : tempWorlds)
						delete world;

					return false;
				}
			}

			for (Resource::World* const world : worlds)
				delete world;

			worlds = std::move(tempWorlds);

			return true;
		}

		std::vector<Resource::World*> worlds;
	};
}
