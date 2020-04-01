#pragma once

#include "GameCore/net/SimulatorContext.h"
#include "NetCore/ClientHandler.h"
#include "OperatorClient/request/ClassicHostRequest.h"

#include "HostSimulatorAuthenticationHandler.h"
#include "HostClassicCommonHandler.h"
#include "HostClassicSelectionHandler.h"
#include "HostClassicSimulationHandler.h"

namespace Game::Net
{
	class ClassicClientHandler
		:
		public ::Net::ClientHandler,
		public ::Net::RequestContainer,
		public SimulatorContextCallback,

		public AuthenticationHandlerCallback,
		public ClassicSelectionHandlerCallback,
		public ClassicSimulatorHandlerCallback
	{
	public:
		struct Settings
		{
			// time after how many updates the current 
			// status will cause a hard timeout
			sf::Uint32 pendingTimeout = 2;
			sf::Uint32 runningTimeout = 4;

			sf::Uint32 authenticationTimeout;

			// tickratedifference represents the
			// difference between client tickrate and
			// the server tickrate. this tickrate
			// gets adjusted to prevent  
			int minTickrateDifference;
			int maxTickrateDifference;
		};

		enum class Status
		{
			Authenticating,

			// waiting users do not currently play a game
			// and can potentially be moved if space is needed
			Waiting,

			// running users do currently play a game and
			// should not be moved
			Running,

			// cause for closing is not specified
			// it can be an error, timeout, rejection
			// or different. if bad that it is already
			// reported as threat
			Closing
		};

		ClassicClientHandler(
			const HSteamNetConnection connection,
			const Settings settings,
			SimulatorContext& context,
			const WorldResourceContainer& container)
			:
			ClientHandler(connection),
			settings(settings),
			context(context),
			container(container)
		{
			addRequestHandler<AuthenticationHandler>(
				new AuthenticationHandler(
					this, 
					settings.authenticationTimeout)
			);
		}

		~ClassicClientHandler()
		{
			Operator::Client::PopRequest((Operator::ClassicHostClientDataRequest*) this);

			if (userID)
				context.unregisterPlayer(userID);

			if (selectionHandler)
			{
				// do not know if selectionhandler is still registered
				// and need to prevent multiple deletion from dynamicclienthandler
				// works because of structor stack
				removeRequestHandler<ClassicSelectionHandler>();

				delete selectionHandler;
			}
		}

		virtual void update()
		{
			if (!processMessages())
			{
				status = Status::Closing;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				sendMessage(::Net::CommonMessageID::InternalError);

				return;
			}

			callHandlersUpdate();
		}

		virtual void processSimulation()
		{
			// no status check needed because it is
			// already done outside

			simulationHandler->processLogic();
		}

		Status getStatus() const
		{
			return status;
		}

		bool isAuthenticated() const
		{
			return status == Status::Waiting
				|| status == Status::Running;
		}

	private:
		const Settings settings;
		const WorldResourceContainer& container;
		SimulatorContext& context;

		Status status = Status::Authenticating;

		Operator::UserID userID = NULL;
		std::string username;
		Resource::ClassicPlayerResource classicResource;

		// initiated after clientdata received after
		// authenticated
		// not deleted if not currently running
		ClassicSelectionHandler* selectionHandler = NULL;

		// stored as variable because a quick access is needed
		// for processSimulation
		// deleted if not currently running
		ClassicSimulationHandler* simulationHandler;

		void playerRegistered(Resource::PlayerResource* const player) override
		{
			// send message to player that a new player
			// was registered
			// need to add a common request handler to
			// both client and host
		}

		void playerUnregistered(const Operator::UserID userID) override
		{
			// send message to player that a player unregistered
		}

		void onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			callHandlersOnMessage(messageID, pipe);
		}

		ConnectionAccess* getConnectionAccess() override
		{
			return this;
		}

		// request handlers
	private:
		void onAuthenticated(
			const Operator::UserID userID,
			::Net::Host::ClassicRequestClientDataMessage* const answer) override
		{
			Log::Information(L"Client authenticated",
				userID, L"userID");

			this->userID = userID;
			classicResource = std::move(answer->resource);
			username = answer->username;

			delete removeRequestHandler<AuthenticationHandler>();
			addRequestHandler(new HostClassicCommonHandler);

			::Net::Host::InitializeClientMessage message;
			message.players = context.getActivePlayers();

			if (sendMessage(
					::Net::Host::ClassicCommonMessageID::InitializeClient,
					&message))
			{
				Resource::PlayerResource* const resource = new Resource::PlayerResource();
				resource->content.playerID = userID;
				resource->username = username;
				context.registerPlayer(resource, this);

				selectionHandler = new ClassicSelectionHandler(
					this,
					classicResource,
					container);

				addRequestHandler(selectionHandler);
				status = Status::Waiting;
			}
		}

		void onAuthenticationDenied() override
		{
			status = Status::Closing;
		}

		void onSimulationCreated(const SimulationBootInformation& bootInfo) override
		{
			Game::PlayerInformation userInfo;
			userInfo.representationID = bootInfo.representationID;
			userInfo.playerId = userID;
			userInfo.name = username;

			simulationHandler = new ClassicSimulationHandler(
				this,
				context,
				container,
				userInfo,
				bootInfo);

			if (!simulationHandler->initializeSimulation())
			{
				// already logged by simulationhandler
				// creation of simulation failed
				// deleting simulation and 'restoring' selectionhandler
				delete simulationHandler;
				return;
			}

			// not deleting selectionhandler because it is restored and
			// currently stored as 
			removeRequestHandler<ClassicSelectionHandler>();
			addRequestHandler<ClassicSimulationHandler>(
				simulationHandler);

			status = Status::Running;
		}

		void onSimulationFailed(const ClassicSimulation::WorldFailure reason) override
		{
			//status = Status::Closing;

			// logging and error message sending is already
			// done by simulationhandler
		}

		// general failure
	private:
		void accessOnRequestFailed(
			const ::Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			sendMessage(::Net::CommonMessageID::InternalError);
			status = Status::Closing;

			Log::Error(L"Request failed",
				messageID, L"messageID",
				(int) reason, L"reason");
		}

		void onMessageSendFailed(
			const ::Net::MessageID messageID, 
			const ::Net::SendFailure reason) override
		{
			// unable to let the user know what happened because
			// we already failed to transmit a message to him
			// we could send him an error message if the reason is
			// not send but we still have to prevent recrusion

			status = Status::Closing;

			Log::Error(L"Request failed",
				messageID, L"messageID",
				(int)reason, L"reason");
		}

		void onThreatIdentified(
			const sf::Uint32 identifier,
			const std::wstring& note,
			const ::Net::ThreatLevel level) override
		{
			Log::Warning(L"Threat identified (" + std::wstring(note) + L")",
				identifier, L"messageID",
				(int) level, L"level");
		}
	};
}
