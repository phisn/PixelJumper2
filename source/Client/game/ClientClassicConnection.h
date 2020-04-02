#pragma once

#include "ClientAuthenticationHandler.h"
#include "ClientClassicCommonHandler.h"
#include "ClientClassicSelectionHandler.h"
#include "ClientClassicSimulationHandler.h"

#include "GameCore/net/ClassicSelectionMessage.h"
#include "GameCore/net/ClassicSimulatorMessage.h"
#include "GameCore/net/SimulatorAuthenticationMessage.h"

#include "NetCore/Client.h"
#include "OperatorClient/OperatorClient.h"

namespace Game
{
	struct ClassicConnectionInformation
	{
		sf::Uint32 authenticationTimeout;

		// host key and address
		Operator::ConnectionKey key;
		SteamNetworkingIPAddr address;

		// player userid
		Operator::UserID userID;
	};

	class ClientClassicConnection
		:
		public Net::ClientBase,
		public Net::RequestContainer,

		public ClientAuthenticationHandlerCallback,
		public ClientClassicSelectionHandlerCallback,
		public ClientClassicSimulationHandlerCallback
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

			if (simulationRunning)
				simulatorHandler->onLogic(time);

			if (logicCounter > nextUserProcess)
			{
				if (!processMessages())
				{
					onProcessFailed();
					return;
				}

				callHandlersUpdate();
				nextUserProcess = logicCounter + settings.tickrate;
			}
		}

	protected:
		const Settings settings;

		Resource::ClassicPlayerResource classicPlayerResource;
		std::string username;

		virtual ClientAuthenticationHandler* createAuthenticationHandler(
			ClientAuthenticationHandlerCallback* const callback,
			const sf::Uint32 timeout) = 0;
		virtual ClientClassicCommonHandler* createCommonHandler() = 0;
		virtual ClientClassicSelectionHandler* createSelectionHandler(
			ClientClassicSelectionHandlerCallback* const callback) = 0;
		virtual ClientClassicSimulationHandler* createSimulationHandler(
			ClientClassicSimulationHandlerCallback* const callback,
			const SimulationBootInformation info,
			const WorldResourceContainer& worldContainer) = 0;

		virtual void onProcessFailed() = 0;

		// common needs to add here
		WorldResourceContainer worldContainer;

	private:
		ClassicConnectionInformation connectionInfo;

		// saved to prevent repetitive creation of selection
		// handler
		// created after authentication
		// has to manually deleted if not currently running
		ClientClassicSelectionHandler* selectionHandler = NULL;

		// stored as variable to allow quick access though
		// processsimulation
		ClientClassicSimulationHandler* simulatorHandler;

		sf::Uint64 logicCounter = 0,
			nextUserProcess = 0;

		bool simulationRunning = false;

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

		ConnectionAccess* getConnectionAccess() override
		{
			return this;
		}

		void onConnectionOpened() override
		{
			Net::Client::AuthenticationMessage message;

			message.key = connectionInfo.key;
			message.userID = Operator::Client::GetUserID();

			if (sendMessage(
					Net::Client::AuthenticationMessageID::Authenticate,
					&message))
			{
				addRequestHandler(
					createAuthenticationHandler(
						this, 
						connectionInfo.authenticationTimeout)
				);
			}
		}

		void onAuthenticated(Net::Host::AuthenticationAcceptedMessage* const answer) override
		{
			Log::Information(L"Authenticated");

			delete removeRequestHandler<ClientAuthenticationHandler>();

			classicPlayerResource = std::move(*answer->resource);
			username = std::move(*answer->username);

			addRequestHandler(createCommonHandler());

			selectionHandler = createSelectionHandler(this);
			addRequestHandler(selectionHandler);
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

			simulatorHandler = createSimulationHandler(
				this, 
				info, 
				worldContainer);

			if (!simulatorHandler->initializeSimulation())
			{
				delete simulatorHandler;
				return;
			}

			addRequestHandler(simulatorHandler);
			simulationRunning = true;
		}

		void onThreatIdentified(
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
