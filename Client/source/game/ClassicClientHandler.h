#pragma once

#include <Client/source/game/SimulatorAuthenticationHandler.h>
#include <Client/source/game/ClassicSelectionRequestHandler.h>
#include <Client/source/game/ClassicSimulationRequestHandler.h>

#include <Client/source/net/DynamicClientHandler.h>
#include <Client/source/operator/request/ClassicHostRequest.h>

#include <Operator/source/net/ClassicSimulatorMessage.h>

namespace Game::Net
{
	class _ClassicClientHandler
		:
		public ::Net::DynamicClientHandler,
		public AuthenticationHandlerCallback,
		public Operator::ClassicClientDataRequest,
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

		_ClassicClientHandler(
			const HSteamNetConnection connection,
			const Settings settings,
			SimulatorContext& context,
			const WorldResourceContainer& container)
			:
			DynamicClientHandler(connection),
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

		~_ClassicClientHandler()
		{
			Operator::ConnectionHandler::PopRequest((Operator::ClassicClientDataRequest*) this);

			if (userID)
				context.unregisterPlayer(userID);
		}

		virtual void update()
		{
			if (!process())
			{
				status = Status::Closing;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				beginMessage(Host::AuthenticationMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			callHandlersUpdate();
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

		Operator::UserID userID = NULL;
		Status status = Status::Authenticating;

		std::string username;
		Resource::ClassicPlayerResource classicResource;

		void onAuthenticated(const Operator::UserID userID)
		{
			Log::Information(L"Client authenticated",
				userID, L"userID");

			status = Status::Waiting;
			this->userID = userID;

			removeRequestHandler<AuthenticationHandler>();

			Operator::Net::Client::RequestClientDataMessage message;
			message.userID = userID;

			if (!Operator::ConnectionHandler::PushRequest(
					Operator::Net::Client::ClassicHostID::RequestClientData,
					&message,
					(Operator::ClassicClientDataRequest*) this))
			{
				ExternalErrorMessage error;
				error.content.errorID = CommonErrorID::OperatorRequestFailed;
				error.content.messageID = Operator::Net::Client::ClassicHostID::RequestClientData;
				error.message = L"Failed to retrive clientdata";

				this->accessSendMessage(
					CommonMessageID::ExternalError,
					&error);

				status = Status::Closing;

				Log::Error(L"Failed to retrive client data from operator",
					userID, L"userID");
			}
		}
	
		void onAuthenticationDenied() override
		{
			status = Status::Closing;
		}

		void onClientDataReceived(Operator::Net::Host::RequestClientDataMessage* const answer) override
		{
			username = answer->username;
			classicResource = std::move(answer->resource);

			Host::InitializeClientMessage message;

			message.classicResource = &classicResource;
			message.username = username;
			message.players = context.getActivePlayers();
			
			if (accessSendMessage(
					Host::ClassicSelectionMessageID::InitializeClient,
					&message))
			{
				Resource::PlayerResource* const resource = new Resource::PlayerResource();
				resource->content.playerID = userID;
				resource->username = username;
				context.registerPlayer(resource, this);

				addRequestHandler<ClassicSelectionRequestHandler>(
					new ClassicSelectionRequestHandler(classicResource)
				);
			}
		}

		void onClientDataFailed(Operator::Net::Host::RequestClientDataFailedMessage* const message) override
		{

		}

		void playerRegistered(Resource::PlayerResource* const player) override
		{
			// send message to player that a new player
			// was registered
		}

		void playerUnregistered(const Operator::UserID userID) override
		{
			// send message to player that a player unregistered
		}

		// general failure
	private:
		void onRequestFailed(const Reason reason) override
		{
			ExternalErrorMessage message;
			message.content.errorID = CommonErrorID::OperatorRequestFailed;
			message.content.messageID = Operator::Net::Client::ClassicHostID::RequestClientData;
			message.message = L"Failed to retrive clientdata";

			this->accessSendMessage(
				CommonMessageID::ExternalError,
				&message);

			status = Status::Closing;

			Log::Error(L"Operator request failed",
				(int) reason, L"reason");
		}

		void accessOnRequestFailed(
			const Device::Net::MessageID messageID,
			const ::Net::RequestFailure reason) override
		{
			accessSendMessage(
				Host::AuthenticationMessageID::InternalError,
				NULL);
			status = Status::Closing;

			Log::Error(L"Request failed",
				messageID, L"messageID",
				(int) reason, L"reason");
		}

		void onMessageSendFailed(
			const Device::Net::MessageID messageID, 
			const SendFailure reason) override
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

		void onInvalidMessageID(const Device::Net::MessageID messageID) override
		{
			status = Status::Closing;

			Log::Error(L"Got invalid messageID");
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

	class ClassicClientHandler
		:
		public ::Net::DynamicClientHandler
	{
	public:
		struct Settings
		{
			// time after how many updates the current 
			// status will cause a hard timeout
			sf::Uint32 pendingTimeout = 2;
			sf::Uint32 runningTimeout = 4;

			// tickratedifference represents the
			// difference between client tickrate and
			// the server tickrate. this tickrate
			// gets adjusted to prevent  
			int minTickrateDifference;
			int maxTickrateDifference;
		};

		ClassicClientHandler(
			const HSteamNetConnection connection,
			const Settings settings,
			const WorldResourceContainer& container)
			:
			simulation(container),
			settings(settings),
			container(container),
			DynamicClientHandler(connection)
		{
			playerResource = new Resource::PlayerResource();
			playerResource->username = L"username";
			playerResource->content.playerID = 0;

			player = new VirtualPlayer(Game::PlayerInformation::Create(playerResource));
			simulation.setPlayer(player);
		}

		/*void update() override
		{
			HostAuthenticationHandler::update();
		}*/

		void processLogic()
		{
			if (player->hasUpdate())
			{
				if (!simulation.processLogic())
				{
					// simulation has error
				}
			}
			else
			{
				++missingFrames;
			}
		}

	private:
		const Settings settings;
		const WorldResourceContainer& container;

		ClassicSimulation simulation;
		VirtualPlayer* player = NULL;
		Resource::PlayerResource* playerResource;

		sf::Uint64 missingFrames = 0;

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			/*if (status == Connecting)
			{
				HostAuthenticationHandler::onMessage(messageID, pipe);
				return;
			}

			switch (messageID)
			{
			case Client::ClassicSimMessageID::PrepareSimulation:
			{
				Client::PrepareSimulationMessage message;

				if (!loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onPrepareSimulation(message);
				break;
			}
			case Client::ClassicSimMessageID::PushMovement:
			{
				Client::PushMovementMessage message;

				if (!loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onPushMovement(message);
				break;
			}
			default:

				break;
			*/
		}

		void onPrepareSimulation(const Client::PrepareSimulationMessage& request)
		{
			if (simulation.getStatus() == ClassicSimulation::Status::Running)
			{
				sendRejectSimulationRequestMessage(Host::RejectSimulationRequestMessage::SimulationAlreadyRunning);
			}

			WorldResourceContainer::const_iterator iterator = container.find(request.world);
			
			if (iterator == container.cend())
			{
				sendRejectSimulationRequestMessage(Host::RejectSimulationRequestMessage::InvalidWorldID);
			}

			const ClassicSimulation::WorldFailure result = simulation.runWorld(request.world);

			if (result != ClassicSimulation::WorldFailure::Success)
			{
				Log::Error(L"Failed to start simulation",
					(int) result, L"result");

				sendRejectSimulationRequestMessage(Host::RejectSimulationRequestMessage::SimulationRunFailed);
			}
		}

		void sendRejectSimulationRequestMessage(Host::RejectSimulationRequestMessage::Reason reason)
		{
			Host::RejectSimulationRequestMessage message;
			message.reason = reason;

			/*sendCommonMessage(
				Host::ClassicalConnectionMessageID::RejectSimulationRequest,
				&message);*/
		}

		void onPushMovement(const Client::PushMovementMessage& request)
		{
			for (const FrameStatus& frameStatus : request.packetFrameStatus.frames)
				player->pushUpdate(frameStatus);

			while (missingFrames > 0 && player->hasUpdate())
			{
				if (!simulation.processLogic())
				{
					// simulation has error
				}

				--missingFrames;
			}
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			SteamNetConnectionInfo_t info;
			if (networkInterface->GetConnectionInfo(connection, &info))
			{
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int) level, L"level",
					(int) messageID, L"messageID",
					(int) info.m_addrRemote.GetIPv4(), L"ipv4",
					(int) info.m_addrRemote.m_port, L"port"); 
			}
			else
			{
				Log::Error(L"Invalid connection while threat is identified");
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int) level, L"level",
					(int) messageID, L"messageID",
					(int) connection, L"connection");
			}
		}

		/*void onClientConnected() override
		{
			// request playerdata from operator
		}*/
	};
}
