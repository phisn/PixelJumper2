#pragma once

#include "GameWorld.h"
#include "trait/ExitableTrait.h"

#include <future>
#include <vector>

namespace Game
{
	struct SimulationBootInformation
	{
		Resource::WorldId worldID;
		Resource::RepresentationID representationID;
	};

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

		std::future<WorldFailure> prepareWorld(const Resource::WorldId worldID)
		{
			return std::async(
				std::launch::async,
				[worldID]() -> WorldFailure
				{

				});
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
