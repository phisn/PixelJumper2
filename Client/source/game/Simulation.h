#pragma once

#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>
#include <Client/source/game/tiletrait/TransitiveTile.h>

#include <Client/source/framework/Context.h>

#include <future>
#include <vector>

namespace Game
{
	typedef std::map<Resource::WorldId, Resource::World*> WorldResourceContainer;

	class Simulation
		:
		public GameState
	{
	public:
		virtual ~Simulation()
		{
		}

		virtual bool initialize() = 0;
		virtual bool processLogic() = 0;

		virtual void pushPlayer(PlayerBase* const player) = 0;
		virtual void popPlayer(PlayerBase* const player) = 0;
	};

	class _ClassicSimulation
	{
	public:
		typedef std::future<World*> LoadingWorld;
		typedef std::vector<LoadingWorld> LoadingWorldContainer;
		typedef std::vector<World*> WorldContainer;

		enum Status
		{
			Waiting,
			Running,
			Error
		};

		_ClassicSimulation(const WorldResourceContainer& worldResources)
			:
			worldResources(worldResources)
		{
		}

		Status getStatus()
		{
			return status;
		}

	protected:
		Game::PlayerBase* player = NULL;
		World* world;

		enum class WorldFailure
		{
			Success,
			PlayerEmpty,
			PreloadingFailed,
			ResourceNotFound,
			MissingResource,
			InitializeFailed
		};

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

			return loadWorld(worldID);
		}

		std::future<WorldFailure> prepareWorld(const Resource::WorldId worldID)
		{
			return std::async(
				std::launch::async,
				[worldID]() -> WorldFailure
				{

				});
		}

		void processLogic()
		{
		}

		virtual bool onInitializeWorld(World* const world)
		{
			return world->initialize();
		}

		virtual void onBeginWorld(World* const world) = 0;

	private:
		const WorldResourceContainer& worldResources;
		Status status = Waiting;
		WorldFailure failure = WorldFailure::Success;

		WorldContainer worlds;
		LoadingWorldContainer loadingWorlds;

		// tasks from different execution points
		// that have to be executed later
		std::vector<std::function<void()>> tasks;

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
			for (TransitiveTile* tile : world->getEnvironment()->getTileType<TransitiveTile>())
				if (std::find_if(
						worlds.begin(),
						worlds.end(),
					[tile](World* const world)
					{
						return world->getInformation()->worldId == tile->getTarget();

					}) == worlds.end())
				{
					if (!preloadNextWorld(tile->getTarget()))
						return false;
				}

			return true;
		}

		// false if some resource was not found
		bool preloadNextWorld(const Resource::WorldId worldID)
		{
			WorldResourceContainer::const_iterator resource = worldResources.find(worldID);

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

						for (ExitableTile* const tile : world->getEnvironment()->getTileType<ExitableTile>())
							tile->onExit.addListener(
								[this]()
								{
									tasks.push_back(
										[this]()
										{
										});
								});

						for (TransitiveTile* const tile : world->getEnvironment()->getTileType<TransitiveTile>())
							tile->onTransition.addListener(
								[this, tile](const TransitiveTile::Event& event)
								{
									tasks.push_back(
										[this, tile]()
										{
										});
								});
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
				return WorldFailure::Success;

			}

			for (World* const world : worlds)
				if (world->getInformation()->worldId == worldID)
				{
					this->world = world;
					return WorldFailure::Success;
				}

			return createWorld(worldID);
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

		WorldFailure tansitiveLoadWorld(const Resource::WorldId worldID)
		{
			unloadWorld();
			const WorldFailure failure = enforceWorld(worldID);

			if (failure == WorldFailure::Success)
			{
				world->addTransitivePlayer(
					);
			}

			return failure;
		}

		WorldFailure createWorld(const Resource::WorldId worldID)
		{
			WorldResourceContainer::const_iterator iterator = worldResources.find(worldID);

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

	/*

		A simulation consists of worlds which can be connected
		transitivly with each other meaning the player can switch
		between them without interrupts.
		A loaded world will preload all transitivly connected
		worlds to prevent any interrupts by loading. preloaded
		worlds will be loaded asynchronous and evaluated when
		needed. an error in loading a world will be first discovered
		when the world is loaded. when the worlds are evaluated
		first all worlds are moved to loadedworlds then the targeted
		worlds will be loaded and possibly new worlds moved into
		preloadedworlds.
		it is important to reset all worlds when the player resets
		or dies to prevent an overallocation of resources. that
		either mean that there will only be a low count of active
		worlds when the player often respawns and dies or that there
		will be currently many loaded worlds

	*/
	class ClassicSimulation
		:
		public Simulation
	{
	public:
		typedef std::future<World*> WorldPreloader;
		typedef std::map<Resource::WorldId, Resource::World*> ResourceContainer;
		typedef std::map<Resource::WorldId, World*> WorldContainer;

		enum Status
		{
			Running,
			MissingPlayer,   // waiting for player being added
			MissingWorld,    // waiting for world being run
			MissingResource, // waiting for missing resource
			Shutdown,        // manually shutdown
			Error            // fault in load world or resource
		};

		ClassicSimulation(const ResourceContainer& resourceContainer)
			:
			loadedWorldResources(resourceContainer)
		{
		}

		bool initialize() override
		{
			return true;
		}

		bool pushWorld(Resource::WorldId const world)
		{
			return commonLoadWorld(world);
		}

		bool processLogic() override
		{
			currentWorld->processLogic();

			if (transitiveSwitchRequest)
			{
				transitiveLoadWorld(transitiveSwitchEvent);
				transitiveSwitchRequest = false;
			}

			return true;
		}

		void pushPlayer(PlayerBase* const player) override
		{
			if (this->player != NULL)
			{
				currentWorld->removePlayer(player);
			}

			this->player = player;

			if (currentWorldID)
			{
				commonLoadWorld(currentWorldID);
			}
			else
			{
				adjustStatus(MissingWorld);
			}
		}
		
		void popPlayer(PlayerBase* const player) override
		{
			if (this->player != player)
			{
				const Resource::PlayerID existing = this->player
					? this->player->getInformation().playerId 
					: 0;

				Log::Error(L"Tried to remove invaid player in simulation",
					existing, L"existing",
					player->getInformation().playerId, L"remove");
			}
			else
			{
				currentWorld->removePlayer(player);
				this->player = NULL;
				adjustStatus(MissingPlayer);
			}
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			if (!writePipe->writeValue(&currentWorld->getInformation()->worldId))
			{
				return false;
			}

			return currentWorld->writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			// synchronize when world is different
			Resource::WorldId worldID;
			if (!readPipe->readValue(&worldID))
			{
				return false;
			}

			if (currentWorld->getInformation()->worldId != worldID && !commonLoadWorld(worldID))
			{
				return false;
			}

			return currentWorld->readState(readPipe);
		}

		const World* getCurrentWorld() const
		{
			return currentWorld;
		}

		Status getStatus() const
		{
			return status;
		}

		sf::Uint64 getTickCount() const
		{
			return currentWorld->getProperties().tickCount.getValue();
		}

	protected:
		virtual bool initializeWorld(World* const world)
		{
			return world->initialize();
		}

	private:
		bool processTileDependencies()
		{
			for (ExitableTile* const tile : currentWorld->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.addListener(
					[this]()
					{
						// ...

					}, GameEventIdentifier::ClassicSimulation);
			}

			// preload transtive tiles because environment uses maps
			const std::vector<TransitiveTile*>& transitiveTiles = currentWorld->getEnvironment()->getTileType<TransitiveTile>();

			// prepare preloading size to prevent resize
			preloadingWorlds.reserve(transitiveTiles.size());

			for (TransitiveTile* tile : transitiveTiles)
			{
				tile->onTransition.addListener(
					[this, tile](const TransitiveTile::Event& event)
					{
						transitiveSwitchRequest = true;
						transitiveSwitchEvent = event;

					}, GameEventIdentifier::ClassicSimulation);

				if (!preloadWorld(tile->getTarget()))
				{
					return false;
				}
			}

			return true;
		}

		void removeTileDependencies()
		{
			for (ExitableTile* const tile : currentWorld->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.popListener(GameEventIdentifier::ClassicSimulation);
			}

			for (TransitiveTile* const tile : currentWorld->getEnvironment()->getTileType<TransitiveTile>())
			{
				tile->onTransition.popListener(GameEventIdentifier::ClassicSimulation);
			}
		}

		bool preloadWorld(const Resource::WorldId worldID)
		{
			if (loadedWorlds.find(worldID) == loadedWorlds.cend())
			{
				ResourceContainer::const_iterator worldResource = loadedWorldResources.find(worldID);

				// ignore missing here to hope for getting it later
				if (worldResource != loadedWorldResources.cend())
				{
					preloadingWorlds.push_back(
						std::async(
							std::launch::async,
							[this](Resource::World* const worldResource) -> World*
							{
								World* const world = new World(worldResource);

								if (!initializeWorld(world))
								{
									delete world;
									return NULL;
								}

								return world;
							}, worldResource->second));
				}
			}

			return true;
		}

		bool commonLoadWorld(const Resource::WorldId worldID)
		{
			if (prepareWorld(worldID))
			{
				currentWorld->addPlayer(player);
				return true;
			}

			return false;
		}

		bool transitiveLoadWorld(const TransitiveTile::Event& event)
		{
			const Resource::WorldId source = currentWorld->getInformation()->worldId;

			if (prepareWorld(event.target))
			{
				currentWorld->addTransitivePlayer(
					player,
					event.sourceOffset, 
					source);

				return true;
			}

			return false;
		}

		bool prepareWorld(const Resource::WorldId worldID)
		{
			currentWorldID = worldID;

			World* const world = getWorld(worldID);
			if (currentWorld)
			{
				removeTileDependencies();
				currentWorld->removePlayer(player);
			}

			currentWorld = world;
			if (!processTileDependencies())
			{
				return false;
			}

			return true;
		}

		World* getWorld(const Resource::WorldId worldID)
		{
			if (World* world = processPreloadedWorlds(worldID); world)
			{
				return world;
			}

			decltype(loadedWorlds)::const_iterator world = loadedWorlds.find(worldID);
			if (world == loadedWorlds.cend())
			{
				return createWorld(worldID);
			}

			return world->second;
		}

		World* createWorld(const Resource::WorldId worldID)
		{
			ResourceContainer::const_iterator worldResource = loadedWorldResources.find(worldID);
			if (worldResource == loadedWorldResources.cend())
			{
				adjustStatus(Status::MissingResource);
				return NULL;
			}

			World* const world = new World(worldResource->second);
			if (!initializeWorld(world))
			{
				delete world;
				adjustStatus(Status::Error);
				return NULL;
			}
			loadedWorlds[worldID] = world;

			return world;
		}

		World* processPreloadedWorlds(const Resource::WorldId worldID)
		{
			World* world = NULL;

			for (WorldPreloader& preloader : preloadingWorlds)
			{
				World* const preloadedWorld = preloader.get();
				const Resource::WorldId preloadedWorldID = preloadedWorld->getInformation()->worldId;

				if (preloadedWorldID == worldID)
				{
					world = preloadedWorld;
				}

				loadedWorlds[preloadedWorldID] = preloadedWorld;
			}

			preloadingWorlds.clear();

			return world;
		}

		void adjustStatus(const Status status)
		{
			if (this->status < status)
				this->status = status;
		}

		Status status = MissingPlayer;

		TransitiveTile::Event transitiveSwitchEvent;
		bool transitiveSwitchRequest = false;

		PlayerBase* player;

		Resource::WorldId currentWorldID;
		World* currentWorld = NULL;

		const ResourceContainer& loadedWorldResources;
		WorldContainer loadedWorlds;
		std::vector<WorldPreloader> preloadingWorlds;
	};

	class VisualClassicSimulation
		:
		public ClassicSimulation
	{
	public:
		using ClassicSimulation::ClassicSimulation;

		void draw(sf::RenderTarget* const target)
		{
			getCurrentWorld()->getEnvironment()->draw(target);
		}

	private:
		bool initializeWorld(World* const world)
		{
			return ClassicSimulation::initializeWorld(world) 
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
