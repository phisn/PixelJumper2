#pragma once

#include "ClassicClientHandler.h"
#include "device/CoreDevice.h"

#include "NetCore/NetCore.h"
#include "GameCore/GameWorld.h"
#include "GameCore/net/SimulatorContext.h"
#include "GameCore/net/SimulatorSettings.h"

// #include <Client/source/game/UserConnection.h>

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

namespace Game
{
	// need something like a host structure
	// while the host self has all connections
	// the client wont be connected to all
	// clients. he will have a list of all player
	// informations? and their current position
	// can optionally request more player data

	class HostClassicSimulator
		:
		public ::Net::ServerInterface,
		public GameState,
		public Operator::RegisterClassicHostRequest
	{
	public:
		struct HostSettings
		{
			unsigned short port = 9927;
			sf::Time interval = sf::milliseconds(100);
		};

		HostClassicSimulator(
			HostSettings settings = HostSettings{ },
			SimulatorSettings simulatorSettings = SimulatorSettings{ })
			:
			clientSettings{ },
			settings(settings),
			simulatorSettings(simulatorSettings)
		{
			Operator::Client::ClosedConnectionNotifier.addListener(
				[](int reason)
				{

				});

			Operator::Client::LostConnectionNotifier.addListener(
				[](int reason)
				{

				});

			Operator::Client::OpenConnectionNotifier.addListener(
				[this](bool authenticated)
				{
					if (authenticated)
					{
						initializeOperatorConnection();
					}
					else
					{
						Device::Core::Shutdown();
					}
				});
		}

		bool initialize()
		{
			Log::SectionHost section{ L"starting server" };

			if (!ServerInterface::initialize(settings.port))
			{
				Log::Error(L"Failed to initialize server");
				return false;
			}

			return true;
		}

		virtual void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();
			while (logicCounter > LogicTimeStep)
			{
				for (ClassicClientHandler* const clientHandler : connections)
					if (clientHandler->getStatus() == ClassicClientHandler::Status::Running)
					{
						clientHandler->processSimulation();
					}

				logicCounter -= LogicTimeStep;
			}

			decltype(connections)::iterator iterator = connections.begin();
			while (iterator != connections.end())
			{
				ClassicClientHandler* const handler = *iterator;

				if (handler->getStatus() == ClassicClientHandler::Status::Closing)
				{
					iterator = connections.erase(iterator);
				}
				else
				{
					handler->update();
					++iterator;
				}
			}
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			return true;
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			return true;
		}

		void pushResource(Resource::World* const world)
		{
			resources[world->content.id] = world;
		}

		const HostSettings settings;
		const SimulatorSettings simulatorSettings;

	private:
		const ClassicClientHandler::Settings clientSettings;

		sf::Uint64 logicCounter = 0;
		sf::Uint64 tickCount = 0;

		// last element represents empty connection
		// to accept new tcpsockets
		std::vector<ClassicClientHandler*> connections;

		SimulatorContext context;
		WorldResourceContainer resources;

		bool askClientConnect(SteamNetConnectionStatusChangedCallback_t* const event) override
		{
			Log::Information(L"New client connects",
				::Net::ConvertIPAddress(&event->m_info.m_addrRemote), L"address");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client connected");

			connections.push_back(new ClassicClientHandler(
				connection,
				clientSettings,
				context,
				simulatorSettings,
				resources));
		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
			Log::Information(L"client disconnected");

			removeClient(connection);
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
			// think about allowing lost connections for 
			// about an hour to stay and reconnect if possible

			Log::Information(L"client lost");

			removeClient(connection);
		}

		void removeClient(const HSteamNetConnection connection)
		{
			decltype(connections)::iterator iterator = std::find_if(
				connections.begin(),
				connections.end(),
				[connection](const ClassicClientHandler* const handler)
				{
					return handler->getConnection() == connection;
				});

			if (iterator != connections.end())
				connections.erase(iterator);
		}

		void initializeOperatorConnection()
		{
			Net::Client::RegisterClassicHostMessage* message =
				new Net::Client::RegisterClassicHostMessage;

			message->maxPlayers = 100;
			message->port = 9927;

			Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
				Net::Client::OperatorCommonMessageID::RegisterClassicHost,
				message, this);

			if (result != Operator::Client::PushRequestFailure::Success)
			{
				Log::Error(L"failed to push registerclassichost request",
					(int) result, L"reason");

				Device::Core::Shutdown();
			}
		}

		void onRegisteredClassicHost() override
		{
			Log::Information(L"simulator successfully registered");

			// might be restored session after operator restart
			// restore all clienthandler states
			for (ClassicClientHandler* client : connections)
			{
				client->restoreOperatorState();
			}
		}

		void onRegisterClassicHostRejected() override
		{
			Log::Error(L"register classic host request rejected, insufficent priviliges");
			Device::Core::Shutdown();
		}

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Error(L"register classic host request failed",
				(int) reason, L"reason");
			Device::Core::Shutdown();
		}
	};
}
