#pragma once

#include <Client/source/net/DynamicClientConnection.h>

#include <Client/source/game/ClientAuthenticationHandler.h>
#include <Client/source/game/ClientClassicCommonHandler.h>
#include <Client/source/game/ClientClassicSelectionHandler.h>
#include <Client/source/game/ClientClassicSimulationHandler.h>

#include <Client/source/game/net/ClassicSelectionMessage.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
#include <Client/source/game/net/SimulatorAuthenticationMessage.h>

#include <Operator/source/Common.h>

namespace Game::Net
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
		public ::Net::DynamicClientConnection,

		public ClientAuthenticationHandlerCallback,
		public ClientClassicSelectionHandlerCallback,
		public ClientClassicSimulationHandlerCallback
	{
	public:
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

	protected:
		Resource::ClassicPlayerResource classicPlayerResource;
		std::string username;

		virtual ClientAuthenticationHandler* createAuthenticationHandler(
			ClientAuthenticationHandlerCallback* const callback,
			const sf::Uint32 timeout) = 0;
		virtual ClientClassicCommonHandler* createCommonHandler() = 0;
		virtual ClientClassicSelectionHandler* createSelectionHandler(ClientClassicSelectionHandlerCallback* const callback) = 0;
		virtual ClientClassicSimulationHandler* createSimulationHandler(
			ClientClassicSimulationHandlerCallback* const callback,
			const SimulationBootInformation info,
			const WorldResourceContainer& worldContainer) = 0;

		virtual void onConnectionLost(const int reason) override
		{
			// think about allowing lost connections for 
			// about an hour to stay and reconnect if possible
		}

		virtual void onConnectionClosed(const int reason) override
		{

		}

	private:
		ClassicConnectionInformation connectionInfo;
		WorldResourceContainer worldContainer;

		// saved to prevent repetitive creation of selection
		// handler
		// created after authentication
		// has to manually deleted if not currently running
		ClientClassicSelectionHandler* selectionHandler = NULL;

		// stored as variable to allow quick access though
		// processsimulation
		ClientClassicSimulationHandler* simulatorHandler;

		void onConnectionOpen() override
		{
			Net::Client::AuthenticationMessage message;

			message.key = connectionInfo.key;
			message.userID = connectionInfo.userID;

			if (accessSendMessage(
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

		void onAuthenticated(Host::AuthenticationAcceptedMessage* const answer) override
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
			const Host::AcceptSimulationRequestMessage& message) override
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

			ClientClassicSimulationHandler* const simulationHandler = createSimulationHandler(this, info, worldContainer);

			if (!simulationHandler->initializeSimulation())
			{
				delete simulationHandler;
				return;
			}

			addRequestHandler(simulationHandler);
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + std::wstring(note) + L")",
				messageID, L"messageID",
				(int)level, L"level");
		}
	};
}
