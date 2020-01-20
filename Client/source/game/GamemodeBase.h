#pragma once

#include <Client/source/game/UserConnection.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>
#include <Client/source/game/tiletrait/TransitiveTile.h>

#include <Client/source/framework/Context.h>

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
	class TestGamemode
		:
		public GamemodeBase
	{
	public:
		TestGamemode(
			Resource::World* const world,
			PlayerBase* const player)
			:
			resource(world),
			player(player)
		{
		}

		~TestGamemode()
		{
			if (world)
				delete world;
		}

		bool initialize() override
		{
			world = new World(resource);
			world->addPlayer(player);

			return world->initialize() && world->initializeGraphics();
		}

		bool processLogic() override
		{
			world->processLogic();
		}

		bool writeState(Resource::WritePipe* const writePipe)
		{
			return true;
		}

		bool readState(Resource::ReadPipe* const readPipe)
		{
			return true;
		}

	private:
		World* world = NULL;

		Resource::World* const resource;
		PlayerBase* const player;
	};

	class TestGamemodeCreator
		:
		public GamemodeCreatorBase
	{
	public:
		TestGamemodeCreator(
			Resource::World* const world,
			LocalConnection* const connection)
			:
			world(world),
			connection(connection)
		{
		}

		GamemodeBase* createGamemode() override
		{
			return new TestGamemode(world, connection->getLocalPlayer());
		}

		bool writeChangedState(Resource::WritePipe* const writePipe)
		{
			return true;
		}

		bool readChangedState(Resource::ReadPipe* const readPipe)
		{
			return true;
		}

	private:
		Resource::World* const world;
		LocalConnection* const connection;
	};
	*/
	class ClassicSimulation
		:
		public Simulation
	{
	public:
		typedef std::map<Resource::WorldId, Resource::World*> ResourceContainer;
		typedef std::map<Resource::WorldId, World*> WorldContainer;

		ClassicSimulation()
		{
		}

		bool initialize() override
		{
		}

		bool processLogic() override
		{
			return true;
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
		}

	private:
		bool registerExitTiles()
		{
			for (ExitableTile* tile : currentWorld->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.addListener(
					[this]()
					{
					});
			}

			for (TransitiveTile* tile : currentWorld->getEnvironment()->getTileType<TransitiveTile>())
			{
				tile->onTransition.addListener(
					[this](const Resource::WorldId worldID)
					{
						decltype(loadedWorlds)::iterator world = loadedWorlds.find(worldID);

						if (world == loadedWorlds.cend())
						{
							Log::Error("World for transition was not loaded, fatal error");
						}

						world->second
					});

				if (loadedWorlds.find(tile->getTarget()) == loadedWorlds.cend())
				{
					if (loadedWorldResources.find(tile->getTarget()) == loadedWorldResources.cend())
					{
						return false;
					}
					else
					{
						World* const world = new World(loadedWorldResources[tile->getTarget()]);

						if (!world->initialize())
						{
							delete world;
							return false;
						}

						loadedWorlds[tile->getTarget()] = world;
					}
				}
			}
		}

		Resource::WorldId currentWorldID;
		World* currentWorld;

		std::map<Resource::WorldId, Resource::World*> loadedWorldResources;
		WorldContainer loadedWorlds;
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
