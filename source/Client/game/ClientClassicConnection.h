#pragma once

#include "ClientAuthenticationHandler.h"
#include "ClientClassicSelectionHandler.h"
#include "ClientClassicSessionHandler.h"
#include "ClientClassicSimulationHandler.h"

#include "GameCore/net/ClassicSelectionMessage.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "GameCore/net/SimulatorAuthenticationMessage.h"

#include "NetCore/Client.h"
#include "OperatorClient/OperatorClient.h"

namespace Game
{
	struct ClassicHandlerCallbacks
		:
		public ClientAuthenticationHandlerCallback,
		public ClientClassicSelectionHandlerCallback,
		public ClientClassicSessionHandlerCallback,
		public ClientClassicSimulationHandlerCallback
	{
	};

	struct ClassicConnectionInformation
	{
		sf::Uint32 authenticationTimeout;

		// host key and address
		Operator::ConnectionKey key;
		SteamNetworkingIPAddr address;
	};

	struct ClassicConnectionContext
	{
		std::vector<Resource::PlayerResource> players;
		Resource::PlayerResource player;
	};

	class ClientClassicConnection
		:
		public Net::RequestContainer,
		private Net::ClientBase,
		public ClassicHandlerCallbacks
	{
	public:
		struct Settings
		{
			sf::Uint64 tickrate = 25'000;
		};

		ClientClassicConnection(const Settings settings = { })
			:
			settings(settings)
		{
		}

		~ClientClassicConnection()
		{
			if (selectionHandler)
			{
				// do not know if selectionhandler is still registered
				// and need to prevent multiple deletion from dynamicclienthandler
				// works because of structor stack
				removeRequestHandler<ClientClassicSelectionHandler>();
				delete selectionHandler;
			}

			for (const WorldResourceContainer::value_type& world : worldContainer)
			{
				delete world.second;
			}
		}

		bool initialize(const ClassicConnectionInformation& info)
		{
			this->connectionInfo = info;
			return this->connect(info.address);
		}

		virtual void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			if (simulatorHandler)
				simulatorHandler->onLogic(time);

			if (logicCounter > settings.tickrate)
			{
				processMessages();
				callHandlersUpdate();

				logicCounter = 0;
			}
		}

	protected:
		const Settings settings;

		Resource::ClassicPlayerResource classicPlayerResource;

		///////////////////////////////////////////////////////////
		std::string username;
		Operator::UserID userID;
		///////////////////////////////////////////////////////////

		virtual ClientAuthenticationHandler* createAuthenticationHandler(const ClientAuthenticationHandlerArguments& arguments) = 0;
		virtual ClientClassicSelectionHandler* createSelectionHandler() = 0;
		virtual ClientClassicSessionHandler* createSessionHandler(const ClientClassicSessionHandlerArguments& arguments) = 0;
		virtual ClientClassicSimulationHandler* createSimulationHandler(const ClientClassicSimulationHandlerArguments& arguments) = 0;

		// common needs to add here
		WorldResourceContainer worldContainer;

	private:
		ClassicConnectionInformation connectionInfo;

		sf::Uint64 logicCounter = 0;

		// saved to prevent repetitive creation of selection
		// handler. created after authentication
		// has to manually deleted if not currently running
		ClientClassicSelectionHandler* selectionHandler = NULL;

		// stored as variable to allow quick access though
		// processsimulation. might be null
		ClientClassicSimulationHandler* simulatorHandler = NULL;

		// sessionhandler needed for simulationHandler to
		// find playernames by playerID
		// after authentication not null
		ClientClassicSessionHandler* sessionHandler = NULL;

		ConnectionAccess* getConnectionAccess() override
		{
			return this;
		}

		void onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			if (!callHandlersOnMessage(messageID, pipe))
			{
				onThreatIdentified(
					messageID,
					L"invalid messageid on onmessage",
					Net::ThreatLevel::Suspicious);
			}
		}

		void onConnectionOpened() override
		{
			ClientBase::onConnectionOpened();

			Net::Client::AuthenticationMessage message;

			message.content.key = connectionInfo.key;
			message.content.userID = Operator::Client::GetUserID();

			if (sendMessage(
					Net::Client::AuthenticationMessageID::Authenticate,
					&message))
			{
				ClientAuthenticationHandlerArguments arguments;
				arguments.callback = this;
				arguments.timeout = connectionInfo.authenticationTimeout;

				addRequestHandler(createAuthenticationHandler(arguments));
			}
		}

		void onAuthenticated(Net::Host::AuthenticationAcceptedMessage* const answer) override
		{
			delete removeRequestHandler<ClientAuthenticationHandler>();

			Log::Information(L"Authenticated",
				answer->username, L"username",
				answer->resource->unlockedRepresentations.size(), L"unlocked_repr",
				answer->resource->unlockedWorlds.size(), L"unlocked_wrld");

			classicPlayerResource = std::move(*answer->resource);
			username = std::move(*answer->username);

			ClientClassicSessionHandlerArguments sessionHandlerArguments;
			sessionHandlerArguments.callback = this;

			addRequestHandler(sessionHandler = createSessionHandler(sessionHandlerArguments));
			addRequestHandler(selectionHandler = createSelectionHandler());
		}

		void onSimulationCreated(
			const SimulationBootInformation& info,
			const Net::Host::AcceptSimulationRequestMessage& message) override
		{
			for (Resource::World* const world : message.initialWorlds)
			{
				WorldResourceContainer::const_iterator iterator = worldContainer.find(world->content.id);

				if (iterator != worldContainer.cend())
				{
					delete iterator->second;
				}

				worldContainer[world->content.id] = world;
			}

			ClientClassicSimulationHandlerArguments arguments =
			{
				this, worldContainer, 
				sessionHandler->getPlayers(),
				username, userID, info
			};
			
			simulatorHandler = createSimulationHandler(arguments);
			if (!simulatorHandler->initializeSimulation())
			{
				Log::Error(L"Failed to initialize simulation, aborting",
					userID, L"userID",
					username, L"username",
					info.worldID, L"worldID",
					info.representationID, L"reprID");

				delete simulatorHandler;
				sendMessage(Net::Client::ClassicSimulatorMessageID::SimulationFailure);

				return;
			}

			addRequestHandler(simulatorHandler);
		}

		void onSimulationFailure() override
		{
			delete removeRequestHandler<ClientClassicSimulationHandler>();
			// ...?
		}

		void onSimulationClosed() override
		{
			delete removeRequestHandler<ClientClassicSimulationHandler>();
		}

		virtual void onThreatIdentified(
			const sf::Uint32 identifier,
			const std::wstring& note,
			const Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + std::wstring(note) + L")",
				identifier, L"messageID",
				(int) level, L"level");
		}
	};
}
