#pragma once

#include "HostSimulatorAuthenticationHandler.h"
#include "HostClassicSessionHandler.h"
#include "HostClassicSelectionHandler.h"
#include "HostClassicSimulationHandler.h"
#include "ManagedUnregisterClientRequest.h"
#include "OperatorUnlockBuffer.h"

#include "GameCore/net/SimulatorContext.h"
#include "GameCore/net/SimulatorSettings.h"
#include "NetCore/ClientHandler.h"
#include "OperatorClient/request/ClassicHostRequest.h"
#include "ResourceCore/ClassicContextResource.h"

namespace Game
{
	struct ClassicHandlerCallbacks
		:
		public AuthenticationHandlerCallback,
		public ClassicSelectionHandlerCallback,
		public ClassicSimulatorHandlerCallback
	{
	};

	class ClassicClientHandler
		:
		public ClassicHandlerCallbacks,
		public ::Net::ClientHandler,
		public ::Net::RequestContainer,
		public Operator::ClassicHostRegisterClientRequest,
		public SimulatorContextCallback
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
			HSteamNetConnection connection,
			Settings settings,
			
			SimulatorContext& context,
			SimulatorSettings simulatorSettings,

			const Resource::ClassicContextResource& classicContext,
			const Resource::ClassicWorldContainer& classicWorldContainer,
			const Resource::WorldContainer& worldContainer)
			:
			ClientHandler(connection),
			settings(settings),

			context(context),
			simulatorSettings(simulatorSettings),

			classicContext(classicContext),
			classicWorldContainer(classicWorldContainer),
			worldContainer(worldContainer)
		{
			// initialize value for destructor
			resource.content.userID = NULL;

			addRequestHandler<AuthenticationHandler>(
				new AuthenticationHandler(
					this,
					settings.authenticationTimeout)
				);
		}

		~ClassicClientHandler()
		{
			Operator::Client::PopRequest((Operator::ClassicHostClientDataRequest*) this);

			if (isAuthenticated())
			{
				context.unregisterPlayer(resource.content.userID);

				Net::Client::OperatorClassicHost::UnregisterClientMessage* message =
					new Net::Client::OperatorClassicHost::UnregisterClientMessage;
				message->content.userID = resource.content.userID;

				Operator::ManagedUnregisterClientRequest* request =
					new Operator::ManagedUnregisterClientRequest;

				Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
					Net::Client::OperatorClassicHostID::UnregisterClient,
					message,
					request);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					Log::Error(L"failed to push unregisterclient request",
						(int) result, L"reason");

					delete request;
				}
			}

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
			processMessages();
			callHandlersUpdate();
		}

		virtual void processSimulation()
		{
			// no status check needed because it is
			// already done outside

			simulationHandler->processLogic();
		}

		void restoreOperatorState()
		{
			if (isAuthenticated())
			{
				Net::Client::OperatorClassicHost::RegisterClientMessage* operator_message =
					new Net::Client::OperatorClassicHost::RegisterClientMessage;
				operator_message->content.userID = resource.content.userID;

				Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
					Net::Client::OperatorClassicHostID::RegisterClient,
					operator_message,
					this);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					Log::Error(L"failed to send register client request in restore operator state",
						(int) result, L"reason");

					Net::Host::ClassicSession::InterruptSessionMessage message;
					message.content.reason = Net::Host::ClassicSession::InterruptSessionReason::RestoreOperatorStateFailed;

					sendMessage(
						Net::Host::ClassicSessionMessageID::InterruptSession,
						&message);

					status = Status::Closing;
				}
			}
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
		const SimulatorSettings simulatorSettings;

		const Resource::ClassicContextResource& classicContext;
		const Resource::ClassicWorldContainer& classicWorldContainer;
		const Resource::WorldContainer& worldContainer;

		SimulatorContext& context;

		Status status = Status::Authenticating;

		Resource::PlayerResource resource;
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
			Net::Host::ClassicSession::AddPlayerMessage message;
			message.player = player;
			
			sendMessage(
				Net::Host::ClassicSessionMessageID::AddPlayer,
				&message);
		}

		void playerUnregistered(const Operator::UserID userID) override
		{
			Net::Host::ClassicSession::RemovePlayerMessage message;
			message.content.userID = userID;

			sendMessage(
				Net::Host::ClassicSessionMessageID::RemovePlayer,
				&message);
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
			Resource::PlayerResource& resource,
			Resource::ClassicPlayerResource& classicResource) override
		{
			Log::Information(L"Client authenticated",
				resource.content.userID, L"userID");

			this->classicResource = std::move(classicResource);
			this->resource = std::move(resource);

			// unlock missing worlds
			for (Resource::WorldID worldID : classicContext.initialUnlockedWorlds)
				if (std::find(
						this->classicResource.unlockedWorlds.begin(),
						this->classicResource.unlockedWorlds.end(),
						worldID) 
					== this->classicResource.unlockedWorlds.end())
				{
					Simulator::UnlockBuffer::UnlockWorld(this->resource.content.userID, worldID);
					this->classicResource.unlockedWorlds.push_back(worldID);
				}

			// unlock missing representations
			for (Resource::RepresentationID representationID : classicContext.initialUnlockedRepresentations)
				if (std::find(
						this->classicResource.unlockedRepresentations.begin(),
						this->classicResource.unlockedRepresentations.end(),
						representationID)
					== this->classicResource.unlockedRepresentations.end())
				{
					Simulator::UnlockBuffer::UnlockRepresentation(this->resource.content.userID, representationID);
					this->classicResource.unlockedRepresentations.push_back(representationID);
				}

			delete removeRequestHandler<AuthenticationHandler>();
			addRequestHandler(new HostClassicSessionHandler
				{ 
					classicResource,
					classicContext 
				});

			::Net::Host::ClassicSession::InitializeSessionMessage message;
			message.players = context.getActivePlayers();
			message.content.settings = simulatorSettings;

			if (sendMessage(
					::Net::Host::ClassicSessionMessageID::InitializeSession,
					&message))
			{
				context.registerPlayer(&resource, this);

				selectionHandler = new ClassicSelectionHandler(
					this,
					classicResource,
					worldContainer);

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
			userInfo.Create(&resource);
			userInfo.representationID = bootInfo.representationID;

			simulationHandler = new ClassicSimulationHandler(
				this,
				context,
				worldContainer,
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
			delete removeRequestHandler<ClassicSimulationHandler>();

			// status = Status::Closing;

			// logging and error message sending is already
			// done by simulationhandler
		}


		void onSimulationClosed() override
		{
			delete removeRequestHandler<ClassicSimulationHandler>();

			// logging and error message sending is already
			// done by simulationhandler
		}

		void onSimulationFinished(Resource::WorldID worldID) override
		{
			Resource::ClassicWorldContainer::const_iterator world = classicWorldContainer.find(worldID);
			
			if (world == classicWorldContainer.end())
			{
				Log::Error(L"finished invalid world",
					resource.content.userID, L"userID",
					resource.username, L"username",
					worldID, L"worldID",
					classicWorldContainer.size(), L"worldcontainer_size");

				return;
			}

			Resource::WorldID unlockedWorld = world->second->content.worldID;

			Log::Information(L"unlocking", 
				resource.content.userID, L"userID",
				worldID, L"finished",
				unlockedWorld, L"unlocking");
			Simulator::UnlockBuffer::UnlockWorld(
				resource.content.userID,
				unlockedWorld);

			sendWorldUnlocked(unlockedWorld);
		}

	private:
		void sendWorldUnlocked(Resource::WorldID worldID)
		{
			Resource::WorldContainer::const_iterator world = worldContainer.find(worldID);

			if (world != worldContainer.cend())
			{
				Log::Error(L"failed to unlock world, resource not found",
					worldID, L"worldID",
					resource.content.userID, L"userID");
			}

			Net::Host::ClassicSession::WorldUnlockedMessage message;
			message.world = world->second;

			sendMessage(
				Net::Host::ClassicSessionMessageID::WorldUnlocked,
				&message);
		}

		// requests
	private:
		// Inherited via ClassicHostRegisterClientRequest
		void ClassicHostRegisterClientRequest::onRequestFailure(const Reason reason) override
		{
			Log::Error(L"failed to restore operator state, registerclient requestfailed",
				(int) reason, L"reason");

			Net::Host::ClassicSession::InterruptSessionMessage message;
			message.content.reason = Net::Host::ClassicSession::InterruptSessionReason::RestoreOperatorStateFailed;

			sendMessage(
				Net::Host::ClassicSessionMessageID::InterruptSession,
				&message);
			status = Status::Closing;
		}

		void onClientRegistered(Net::Host::OperatorClassicHost::ClientRegisteredMessage& message) override
		{
			// normally almost impossible to be invalid but you can never be sure
			assert(resource.content.userID == message.resource.content.userID);
			assert(resource.username == message.resource.username);
			assert(message.classicResource.unlockedRepresentations.size() == classicResource.unlockedRepresentations.size());
			assert(message.classicResource.unlockedWorlds.size() == classicResource.unlockedWorlds.size());

			Log::Information(L"operator state successfully restored");
		}
	
		// might never happen
		void onClientRegisterFailed(Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage& answer) override
		{
			switch (answer.content.reason)
			{
			case Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::UserRegisteredSomewhere:
				// user might try to abuse this situation
				onThreatIdentified(
					Net::Host::OperatorClassicHostID::ClientRegistrationFailed,
					L"at restore operator state, clientregister failed with reason \"UserRegisteredSomewhere\"",
					Net::ThreatLevel::Suspicious);

				break;
			}

			Log::Error(L"at restore operator, clientregister failed",
				(int) answer.content.reason, L"reason");

			Net::Host::ClassicSession::InterruptSessionMessage message;
			message.content.reason = Net::Host::ClassicSession::InterruptSessionReason::RestoreOperatorStateFailed;

			sendMessage(
				Net::Host::ClassicSessionMessageID::InterruptSession,
				&message);
			status = Status::Closing;
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
				(int)reason, L"reason");
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
				(int)level, L"level");
		}
};
}
