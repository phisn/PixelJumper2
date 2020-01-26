#pragma once

#include <Client/source/game/GameWorld.h>
#include <Client/source/game/Simulation.h>

namespace Game
{
	class Operator
		:
		public GameState
	{
	public:
		virtual bool initialize() = 0;
		virtual void onLogic(const sf::Time time) = 0;
	};

	// need something like a host structure
	// while the host self has all connections
	// the client wont be connected to all
	// clients. he will have a list of all player
	// informations? and their current position
	// can optionally request more player data

	class HostOperator
		:
		public Operator
	{
		typedef std::map<Resource::WorldId, Resource::World*> ResourceContainer;

	public:
		struct Settings
		{
			sf::Uint64 tickrate = 100'000; // 100ms
			unsigned short port = 9928;
			int maxClients = 128;

			RemoteConnection::Settings connectionSettings;
		};

		HostOperator(const Settings settings = Settings{ })
			:
			settings(settings)
		{
			listener.setBlocking(false);
		}

		bool initialize()
		{
			const sf::Socket::Status status = listener.listen(settings.port);

			if (status != sf::Socket::Status::Done)
			{
				Log::Error(L"Failed to set host socket to listen",
					(int) status, L"status",
					settings.port, L"port");
				
				return false;
			}

			return true;
		}

		virtual void onLogic(const sf::Time time) override
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > nextGameProcess)
			{

				nextGameProcess += LogicTimeStep;
			}

			if (logicCounter > nextUserProcess)
			{

			}

			while (listener.accept(connections.back()->getSocket()))
			{
				if (connections.back()->initialize())
					connections.push_back(
						new RemoteConnection(settings.connectionSettings)
					);
			}
		}

		virtual bool pushConnection(RemoteConnection* const connection)
		{
		}

		virtual void removeConnection(RemoteConnection* const connection)
		{
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
		}

	private:
		const Settings settings;

		sf::Uint64 logicCounter = 0,
				nextGameProcess = 0,
				nextUserProcess = 0;

		sf::Uint64 tickCount = 0;

		// last element represents empty connection
		// to accept new tcpsockets
		std::vector<RemoteConnection*> connections;
		ResourceContainer resources;

		sf::TcpListener listener;
	};

	class LocalClassicTestSimulator
		:
		public Operator
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
