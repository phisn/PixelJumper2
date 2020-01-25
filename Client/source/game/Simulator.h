#pragma once

#include <Client/source/game/GameWorld.h>
#include <Client/source/game/Simulation.h>

namespace Game
{
	class Simulator
		:
		public GameState
	{
	public:
		virtual bool initialize() = 0;
		virtual void onLogic(const sf::Time time) = 0;
	};

	class HostClassicSimulator
		:
		public Simulator
	{
		struct User
		{
			RemoteConnection* connection;
			ClassicSimulation* simulation;

			sf::Uint64 tickCountBegin;
		};

		typedef std::vector<User> UserContainer;
		typedef std::map<Resource::WorldId, Resource::World*> ResourceContainer;

	public:
		struct Settings
		{
			sf::Uint64 tickrate = 100'000; // 100ms
		};

		HostClassicSimulator(const Settings settings = Settings{ })
			:
			settings(settings)
		{
		}

		virtual void onLogic(const sf::Time time) override
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > nextGameProcess)
			{
				for (User& user : users)
					if (user.simulation->getStatus() == Simulation::Status::Running)
					{
						user.simulation->processLogic();
					}

				nextGameProcess += LogicTimeStep;
			}

			if (logicCounter > nextUserProcess)
			{
				UserContainer::iterator user = users.begin();
				while (user != users.end())
				{
					if (user->connection->getStatus() != RemoteConnection::Status::Connected && 
						onUnexpectedConnectionStatus(&*user))
					{
						user = users.erase(user);
						continue;
					}

					if (user->simulation->getStatus() != Simulation::Status::Running &&
						onUnexpectedSimulationStatus(&*user))
					{
						user = users.erase(user);
						continue;
					}

					// sync user
				}

				nextUserProcess = logicCounter + settings.tickrate;
			}
		}

		virtual bool pushConnection(RemoteConnection* const connection)
		{
			User user;
			
			user.connection = connection;
			user.simulation = new ClassicSimulation(resources, connection);
			user.tickCountBegin = tickCount;

			if (!user.simulation->initialize())
			{
				delete user.simulation;
				return false;
			}

			users.push_back(user);
		}

		virtual void removeConnection(RemoteConnection* const connection)
		{
			decltype(users)::const_iterator user = std::find_if(
				users.cbegin(), 
				users.cend(),
				[connection](const User& user) -> bool
				{
					return user.connection == connection;
				});

			if (user != users.cend())
			{
				delete user->simulation;
				users.erase(user);
			}
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
		}

	private:
		User* findUser(UserConnection* const connection)
		{
			return findUser(connection->getInformation().playerId);
		}

		User* findUser(Resource::PlayerID playerID)
		{
			for (User& user : users)
				if (user.connection->getInformation().playerId == playerID)
				{
					return &user;
				}

			return NULL;
		}

		// return == remove user
		virtual bool onUnexpectedConnectionStatus(User* const user)
		{
		}

		// return == remove user
		virtual bool onUnexpectedSimulationStatus(User* const user)
		{
		}

		const Settings settings;

		sf::Uint64 logicCounter = 0,
				nextGameProcess = 0,
				nextUserProcess = 0;

		sf::Uint64 tickCount = 0;

		ResourceContainer resources;
		UserContainer users;
	};

	class LocalClassicTestSimulator
		:
		public Simulator
	{
	public:
		typedef std::map<Resource::WorldId, Resource::World*> WorldResources;

		LocalClassicTestSimulator(
			LocalConnection* const connection,
			const WorldResources& worlds)
			:
			connection(connection),
			worlds(worlds)
		{
			simulation = new VisualClassicSimulation(worlds, connection);
		}

		bool initialize() override
		{
			return simulation->initialize()
				&& simulation->pushWorld((worlds.begin())->second->content.id);
		}

		void draw(sf::RenderTarget* const target)
		{
			connection->enableView(target);
			simulation->draw(target);
			connection->getLocalPlayer()->onDraw(target);
		}

		void onLogic(const sf::Time time) override
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > LogicTimeStep)
			{
				simulation->processLogic();

				logicCounter -= LogicTimeStep;
			}

			connection->getLocalPlayer()->onLogic(time);
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			return true;
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			return true;
		}

	private:
		sf::Uint64 logicCounter = 0;
		WorldResources worlds;

		VisualClassicSimulation* simulation;
		LocalConnection* const connection;
	};
}
