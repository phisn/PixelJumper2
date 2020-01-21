#pragma once

#include <Client/source/game/UserConnection.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>
#include <Client/source/game/tiletrait/TransitiveTile.h>

#include <Client/source/framework/Context.h>

#include <future>
#include <vector>

namespace Game
{
	class Simulation
		:
		public GameState
	{
	public:
		// order by importance
		enum Status
		{
			Running,	// has player(s)
			Idle,		// wating for simulator resources
			Dead,		// has no player
			Error		// error
		};

		virtual ~Simulation()
		{
		}

		Simulation(UserConnection* const connection)
			:
			connection(connection)
		{
		}

		virtual bool initialize() = 0;
		virtual bool processLogic() = 0;

		virtual void kill()
		{
			adjustStatus(Simulation::Status::Dead);
		}


		Status getStatus() const
		{
			return status;
		}

		UserConnection* getConnection() const
		{
			return connection;
		}

	protected:
		virtual void adjustStatus(const Status status)
		{
			if (this->status < status)
				this->status = status;
		}

		UserConnection* const connection;

	private:
		Status status = Running;
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

		ClassicSimulation(
			const ResourceContainer& resourceContainer,
			UserConnection* const connection)
			:
			Simulation(connection),
			loadedWorldResources(resourceContainer)
		{
		}

		bool pushWorld(Resource::WorldId const world)
		{
			return loadWorld(world);
		}

		bool processLogic() override
		{
			if (!connection->processLogic())
			{
				return false;
			}

			currentWorld->processLogic();

			return true;
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

			if (currentWorld->getInformation()->worldId != worldID && !loadWorld(worldID))
			{
				return false;
			}

			return currentWorld->readState(readPipe);
		}

		sf::Uint64 getTickCount() const
		{
			return currentWorld->getProperties().tickCount.getValue();
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
					[this](const Resource::WorldId worldID)
					{
						decltype(loadedWorlds)::iterator world = loadedWorlds.find(worldID);

						if (world == loadedWorlds.cend())
						{
							Log::Error("World for transition was not loaded, fatal error");
						}

						// ...

					}, GameEventIdentifier::ClassicSimulation);

				if (!preloadWorld(tile->getTarget()))
				{
					return false;
				}
			}
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
							[&worldResource]() -> World*
							{
								World* const world = new World(worldResource->second);

								if (!world->initialize())
								{
									delete world;
									return NULL;
								}

								return world;
							}));
				}
			}

			return true;
		}

		bool loadWorld(const Resource::WorldId worldID)
		{
			currentWorldID = worldID;

			World* world = processPreloadedWorlds(worldID);
			if (world == NULL)
			{
				ResourceContainer::const_iterator worldResource = loadedWorldResources.find(worldID);

				if (worldResource == loadedWorldResources.cend())
				{
					adjustStatus(Status::Idle);
					return false;
				}

				world = new World(worldResource->second);
				if (!world->initialize())
				{
					adjustStatus(Status::Error);
					return false;
				}
			}

			removeTileDependencies();
			currentWorld = world;

			if (!processTileDependencies())
			{
				return false;
			}

			currentWorld->addPlayer(connection->getPlayer());
			return true;
		}

		World* processPreloadedWorlds(const Resource::WorldId worldID)
		{
			World* world = NULL;

			for (WorldPreloader& preloader : preloadingWorlds)
			{
				World* const preloadedWorld = preloader.get();
				const Resource::WorldId preloadedWorldID = world->getInformation()->worldId;

				if (preloadedWorldID == worldID)
				{
					world = preloadedWorld;
				}

				loadedWorlds[preloadedWorldID] = preloadedWorld;
			}

			return world;
		}

		Resource::WorldId currentWorldID;
		World* currentWorld;

		const ResourceContainer& loadedWorldResources;
		WorldContainer loadedWorlds;

		std::vector<Resource::WorldId> missingResources;
		std::vector<WorldPreloader> preloadingWorlds;
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
