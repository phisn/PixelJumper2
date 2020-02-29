#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/GameWorld.h>

#include <Client/source/game/ClassicClientHandler.h>
#include <Client/source/game/SimulatorContext.h>
#include <Client/source/game/UserConnection.h>

/*
	classic process:
		1. authentication with simulatorauthentication
		2. getplayerdata request is sent to operator and
		   waiting for result
		3. classicsimulation is pushed handles all common 
		   simulator requests and allows the creation of a
		   simulation
		4. simulation is created
*/

namespace Game::Net
{
	// need something like a host structure
	// while the host self has all connections
	// the client wont be connected to all
	// clients. he will have a list of all player
	// informations? and their current position
	// can optionally request more player data

	class HostClassicSimulator
		:
		public Device::Net::Server,
		public GameState
	{
	public:
		struct Settings
		{
			sf::Uint64 tickrate = 100'000; // 100ms
			unsigned short port = 9928;
			int maxClients = 128;
		};

		HostClassicSimulator(const Settings settings = Settings{ })
			:
			settings(settings)
		{
		}

		bool initialize()
		{
			if (!Server::initialize(settings.port))
			{
				Log::Error(L"Failed to initialize server");
				return false;
			}

			return true;
		}

		virtual void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > nextGameProcess)
			{

				nextGameProcess += LogicTimeStep;
			}

			if (logicCounter > nextUserProcess)
			{

			}
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
		std::vector<ClassicClientHandler*> connections;

		SimulatorContext context;
		WorldResourceContainer resources;

		bool askClientConnect(SteamNetworkingIPAddr* const ipAddress) override
		{
			Log::Information(L"New client connects",
				ipAddress->GetIPv4(), L"ip",
				ipAddress->m_port, L"port");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{

		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
		}
	};

	/*class LocalClassicTestSimulator
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
			simulation = new VisualClassicSimulation(worlds);
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
	*/
}
