#pragma once

#include "GameWorld.h"
#include "trait/ExitableTrait.h"

#include <future>
#include <mutex>
#include <vector>

namespace Game
{
	/*
	allow the client to use pushdelay BUT
	only as the first moves for a world.
	normally to possiblity of pushdelay would
	allow speed hacks as a slow view. but
	this does need to allow the player to
	use pushdelay whenever he wants. we only
	allow the player to use pushdelay at the
	begin of a world. because all frames are
	normally perfectly aligned we know as a
	simulation that a client is in a new world
	when we receive pushdelay and can react
	acordingly whenever this pushdelay is not
	appropiate
	*/

	typedef sf::Uint32 Tick;

	class ClassicSimulation
		:
		public GameState
	{
		typedef std::function<bool()> Task;

	public:
		enum class Status
		{
			Waiting,
			Running
		};

		ClassicSimulation(const Resource::WorldContainer& worldContainer)
			:
			worldContainer(worldContainer)
		{
		}

		bool processTick()
		{
			world->processLogic();
			++tick;

			if (tasks.size())
			{
				for (Task& task : tasks)
					if (!task())
					{
						tasks.clear();
						return false;
					}

				tasks.clear();
			}
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			if (!writePipe->writeValue(&world->getInformation()->worldId))
			{
				return false;
			}

			return world->writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			// synchronize when world is different
			Resource::WorldId worldID;
			if (!readPipe->readValue(&worldID))
			{
				return false;
			}

			// when worldid differes load real worldid
			// if loading fails return false
			if (world->getInformation()->worldId != worldID)
			{
				Log::Information(L"world differs from received state, loading new one",
					world->getInformation()->worldId, L"new_worldID",
					worldID, L"worldID");

				unloadWorld();

				if (!loadWorld(worldID))
				{
					return false;
				}

				world->addPlayer(player);
			}

			return world->readState(readPipe);
		}


		// has to be called before a world is pushed
		void changePlayer(Game::PlayerBase* const player)
		{
			// when world is not null then player
			// can not be null
			if (world != NULL)
			{
				world->removePlayer(this->player);

				// think about to set on correct position to
				// allow changes of player while playing?
				world->addPlayer(player);
			}

			this->player = player;
		}

		bool runWorld(Resource::WorldId worldID)
		{
			Log::Section section{ L"starting classic simulation",
				worldID, L"worldID" };

			if (status != Status::Waiting)
			{
				Log::Error(L"tried to run simulation in an invalid state",
					worldID, L"worldID");
			}

			if (player == NULL)
			{
				Log::Error(L"tried to run classic simulation without player");

				return false;
			}

			Log::Information(L"checked",
				player->getInformation().playerId, L"userID",
				player->getInformation().representationID, L"representationID");

			if (!loadWorld(worldID))
			{
				Log::Error(L"failed to load world");

				return false;
			}

			// preloadNextWorlds();
			world->addPlayer(player);

			return true;
		}

		const World* getWorld() const
		{
			return world;
		}

		Tick getTick() const
		{
			return tick;
		}

	protected:
		const Resource::WorldContainer& worldContainer;

		World* world = NULL;
		Game::PlayerBase* player = NULL;

		virtual bool loadWorld(const Resource::WorldId worldID)
		{
			for (std::future<bool>& loadingWorld : loadingWorlds)
				if (!loadingWorld.get())
				{
					return false;
				}

			unloadWorld();

			decltype(loadedWorlds)::iterator toLoad = loadedWorlds.find(worldID);
			if (toLoad == loadedWorlds.end())
			{
				Resource::WorldContainer::const_iterator iterator = worldContainer.find(worldID);

				if (iterator == worldContainer.end())
				{
					Log::Error(L"failed to create world, missing resource",
						worldID, L"worldID",
						worldContainer.size(), L"container_size");

					return false;
				}

				World* const toLoad_world = new World(iterator->second);
				if (!onInitializeWorld(toLoad_world))
				{
					delete toLoad_world;
					Log::Error(L"failed to create world, initialize failed",
						worldID, L"worldID");

					return true;
				}

				world = toLoad_world;
			}
			else
			{
				world = toLoad->second;
			}

			loadedWorlds[worldID] = world;

			for (Resource::WorldID worldID : world->getTargets())
				if (worldContainer.find(worldID) == worldContainer.end())
				{
					onTargetResourceMissing(worldID);
				}
				else
				{
					preloadWorld(worldID);
				}

			return true;
		}

		virtual void unloadWorld()
		{
			if (world)
				world->removePlayer(player);
		}

		virtual bool onInitializeWorld(World* const world)
		{
			if (!world->initialize())
			{
				return false;
			}

			for (ExitableTraitHandler* const handler : world->getEnvironment()->getExitableTraitTrait())
				handler->onExit.addListener(
					[this]()
					{
						tasks.push_back(
							[this]() -> bool
							{
								onWorldExit();
								return true;
							});
					});

			for (DynamicWorldExitHandler* const dynamicExit : world->getEnvironment()->getDynamicWorldExit())
				dynamicExit->onExit.addListener(
					[this](const DynamicWorldExitEvent& event)
					{
						tasks.push_back(
							[this, &event]() -> bool
						{
							return loadWorldDynamicTransition(event);
						});
					});

			return true;
		}


		bool preloadWorld(const Resource::WorldId worldID)
		{
			if (loadedWorlds.find(worldID) == loadedWorlds.end())
			{
				return true;
			}

			Resource::WorldContainer::const_iterator world_resource = worldContainer.find(worldID);

			if (world_resource == worldContainer.end())
			{
				Log::Error(L"missing resource in preloading world",
					worldID, L"worldID");

				return false;
			}

			loadingWorlds.push_back(std::async(
				std::launch::async,
				[this](Resource::World* const resource) -> bool
				{
					World* const world = new World(resource);

					if (!this->onInitializeWorld(world))
					{
						Log::Error(L"failed to initialize async world",
							resource->content.id, L"worldID");

						delete world;
						return false;
					}

					loadedWorlds[resource->content.id] = world;

					return true;
				},
				world_resource->second));

			return true;
		}

		virtual bool loadWorldDynamicTransition(const DynamicWorldExitEvent& event)
		{
			unloadWorld();

			if (loadWorld(event.targetWorld))
			{
				DynamicWorldEntryEvent entryEvent;
				entryEvent.offsetSource = event.offset;

				if (!world->addPlayerDynamicTransition(
					player,
					event.targetEntry,
					entryEvent))
				{
					Log::Error(L"failed to add player while dynamic transition",
						player->getInformation().playerId, L"playerID",
						world->getInformation()->worldId, L"worldID");

					onDynamicTransitionFailure();
					return false;
				}
			}
			else
			{
				Log::Error(L"failed to load world for dynamic transition",
					player->getInformation().playerId, L"playerID",
					world->getInformation()->worldId, L"worldID");

				onDynamicTransitionFailure();
				return false;
			}
		}

		const std::map<Resource::WorldID, World*>& getLoadedWorlds() const
		{
			return loadedWorlds;
		}

		// after this is called the simulation is
		// should be closed
		virtual void onWorldExit() = 0;

		// missing resource for a target world
		// currently not a problem but might get
		// one in future
		// [is actually a problem for simulator
		// because he usally has all resources]
		virtual void onTargetResourceMissing(Resource::WorldID worldID) = 0;

		virtual void onDynamicTransitionFailure() = 0;

	private:
		Status status = Status::Waiting;
		Tick tick;

		std::vector<Task> tasks;
		std::map<Resource::WorldID, World*> loadedWorlds;
		std::vector<std::future<bool>> loadingWorlds;
	};

	class VisualClassicSimulation
		:
		public ClassicSimulation
	{
	public:
		using ClassicSimulation::ClassicSimulation;

		void draw(sf::RenderTarget* const target)
		{
			world->getEnvironment()->draw(target);
		}

	private:
		bool onInitializeWorld(World* const world) override
		{
			return ClassicSimulation::onInitializeWorld(world)
				&& world->initializeGraphics();
		}
	};

	/*



	class ClassicSimulation
		:
		public GameState
	{
	public:
		typedef std::future<World*> LoadingWorld;
		typedef std::vector<LoadingWorld> LoadingWorldContainer;
		typedef std::vector<World*> InternalWorldContainer;

		enum Status
		{
			Waiting,
			Running,
			Finished,
			Error
		};

		enum class WorldFailure
		{
			Success,

			DynamicEntryNotFound,
			InitializeFailed,
			MissingResource,
			PlayerEmpty,
			PreloadingFailed,
			ResourceNotFound
		};

		typedef std::function<WorldFailure()> Task;

		ClassicSimulation(const Resource::WorldContainer& worldResources)
			:
			worldResources(worldResources)
		{
		}

		virtual WorldFailure processTick()
		{
			world->processLogic();
			++tick;

			// prevent repetetive deletion of tasks
			if (!tasks.empty())
			{
				for (const Task& task : tasks)
				{
					WorldFailure result = task();

					if (result != WorldFailure::Success)
					{
						Log::Error(L"Simulation task failed",
							(int)result, L"reason");

						status = Error;
						return result;
					}
				}

				tasks.clear();
			}

			return WorldFailure::Success;
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			if (!writePipe->writeValue(&tick) ||
				!writePipe->writeValue(&world->getInformation()->worldId))
			{
				return false;
			}

			return world->writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			// synchronize when world is different
			Resource::WorldId worldID;
			if (!readPipe->readValue(&tick) ||
				!readPipe->readValue(&worldID))
			{
				return false;
			}

			// when worldid differes load real worldid
			// if loading fails return false
			if (world->getInformation()->worldId != worldID &&
				loadWorld(worldID) != WorldFailure::Success)
			{
				return false;
			}

			return world->readState(readPipe);
		}


		// has to be called before a world is pushed
		void setPlayer(Game::PlayerBase* const player)
		{
			// when world is not null then player
			// can not be null
			if (world != NULL)
			{
				world->removePlayer(this->player);

				// think about to set on correct position to
				// allow changes of player while playing?
				world->addPlayer(player);
			}

			this->player = player;
		}

		WorldFailure runWorld(Resource::WorldId worldID)
		{
			if (player == NULL)
			{
				return WorldFailure::PlayerEmpty;
			}

			WorldFailure result = loadWorld(worldID);

			if (result != WorldFailure::Success)
			{
				status = Running;
			}

			return result;
		}

		Status getStatus()
		{
			return status;
		}

		const World* getWorld() const
		{
			return world;
		}

		sf::Uint32 getTick() const
		{
			return tick;
		}

	protected:
		const Resource::WorldContainer& worldResources;

		World* world = NULL;
		Game::PlayerBase* player = NULL;

		virtual bool onInitializeWorld(World* const world)
		{
			if (!world->initialize())
			{
				return false;
			}

			for (ExitableTraitHandler* const handler : world->getEnvironment()->getExitableTraitTrait())
				handler->onExit.addListener(
					[this]()
					{
						tasks.push_back(
							[this]() -> WorldFailure
							{
								status = Status::Finished;
								onWorldExit();
								return WorldFailure::Success;
							});
					});

			for (DynamicWorldExitHandler* const dynamicExit : world->getEnvironment()->getDynamicWorldExit())
				dynamicExit->onExit.addListener(
					[this](const DynamicWorldExitEvent& event)
					{
						tasks.push_back(
							[this, &event]()->WorldFailure
						{
							return loadWorldDynamicTransition(event);
						});
					});

			return true;
		}

		virtual void onWorldLoad(World* loadingWorld) = 0;
		// after this is called the simulation is
		// should be closed
		virtual void onWorldExit() = 0;

	private:
		Status status = Waiting;
		WorldFailure failure = WorldFailure::Success;

		InternalWorldContainer worlds;
		LoadingWorldContainer loadingWorlds;

		// similar to tick in world but world
		// independent to be used to synchronize
		// simulator states
		sf::Uint32 tick = 0;

		// tasks from different execution points
		// that have to be executed later
		std::vector<Task> tasks;

		WorldFailure loadWorld(const Resource::WorldId worldID)
		{
			unloadWorld();
			WorldFailure result = enforceWorld(worldID);

			if (result != WorldFailure::Success)
			{
				return result;
			}

			preloadNextWorlds();

			world->addPlayer(player);
			return WorldFailure::Success;
		}

		void unloadWorld()
		{
			if (world)
				world->removePlayer(player);
		}

		// false if some resource was not found
		bool preloadNextWorlds()
		{
			for (const Resource::WorldId worldID : world->getTargets())
				if (std::find_if(
					worlds.begin(),
					worlds.end(),
					[worldID](World* const world)
					{
						return world->getInformation()->worldId == worldID;

					}) == worlds.end())
				{
					if (!preloadNextWorld(worldID))
						return false;
				}

					return true;
		}

		// false if some resource was not found
		bool preloadNextWorld(const Resource::WorldId worldID)
		{
			Resource::WorldContainer::const_iterator resource = worldResources.find(worldID);

			if (resource == worldResources.end())
			{
				return false;
			}

			loadingWorlds.push_back(
				std::async(
					std::launch::async,
					[this](Resource::World* const resource) -> World*
					{
						World* const world = new World(resource);

						if (!this->onInitializeWorld(world))
						{
							delete world;
							return NULL;
						}

						return world;
					},
					resource->second));

			return true;
		}

		WorldFailure enforceWorld(const Resource::WorldId worldID)
		{
			switch (processPreloadedWorlds(worldID))
			{
			case PreloadingResult::Error:
				return WorldFailure::PreloadingFailed;

			case PreloadingResult::Found:
				onWorldLoad(world);

				return WorldFailure::Success;
			}

			for (World* const world : worlds)
				if (world->getInformation()->worldId == worldID)
				{
					this->world = world;
					onWorldLoad(world);

					return WorldFailure::Success;
				}

			const WorldFailure result = createWorld(worldID);

			if (result == WorldFailure::Success)
			{
				onWorldLoad(world);
			}

			return result;
		}

		enum class PreloadingResult
		{
			Found,
			NotFound,
			Error
		};

		PreloadingResult processPreloadedWorlds(const Resource::WorldId worldID)
		{
			PreloadingResult result = PreloadingResult::NotFound;

			for (LoadingWorld& loadingWorld : loadingWorlds)
			{
				// can potentially be that loading world is yet
				// not downloaded. this will cause an lag and potentially
				// cause a timeout. because this should never happen
				// because the world sizes and the normaly loading time
				// are extremly tiny
				World* const loadedWorld = loadingWorld.get();

				if (loadedWorld == NULL)
				{
					return PreloadingResult::Error;
				}

				if (loadedWorld->getInformation()->worldId == worldID)
				{
					world = loadedWorld;
					result = PreloadingResult::Found;
				}

				worlds.push_back(loadedWorld);
			}

			return result;
		}

		WorldFailure loadWorldDynamicTransition(const DynamicWorldExitEvent& event)
		{
			unloadWorld();
			WorldFailure result = enforceWorld(event.targetWorld);

			if (result == WorldFailure::Success)
			{
				DynamicWorldEntryEvent entryEvent;
				entryEvent.offsetSource = event.offset;

				if (!world->addPlayerDynamicTransition(
					player,
					event.targetEntry,
					entryEvent))
				{
					result = WorldFailure::DynamicEntryNotFound;
				}
			}

			return result;
		}

		WorldFailure createWorld(const Resource::WorldId worldID)
		{
			Resource::WorldContainer::const_iterator iterator = worldResources.find(worldID);

			if (iterator == worldResources.end())
			{
				return WorldFailure::MissingResource;
			}

			World* const world = new World(iterator->second);

			if (!onInitializeWorld(world))
			{
				delete world;
				return WorldFailure::InitializeFailed;
			}

			worlds.push_back(world);
			this->world = world;
			return WorldFailure::Success;
		}

	};

	class VisualClassicSimulation
		:
		public ClassicSimulation
	{
	public:
		using ClassicSimulation::ClassicSimulation;

		void draw(sf::RenderTarget* const target)
		{
			world->getEnvironment()->draw(target);
		}

	private:
		bool onInitializeWorld(World* const world) override
		{
			return ClassicSimulation::onInitializeWorld(world)
				&& world->initializeGraphics();
		}
	};

	/*
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
	*/
}
