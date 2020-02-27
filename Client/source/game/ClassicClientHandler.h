#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/SimulatorAuthenticationHandler.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/game/net/ClassicSimClientMessage.h>
#include <Client/source/game/net/ClassicSimHostMessage.h>

#include <Client/source/net/DynamicClientHandler.h>

#include <Operator/source/net/ClassicSimulatorMessage.h>

namespace Game::Net
{
	class _ClassicClientHandler
		:
		public ::Net::DynamicClientHandler,
		public AuthenticationHandlerCallback
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
			const WorldResourceContainer& container)
			:
			DynamicClientHandler(connection),
			settings(settings),
			container(container)
		{
			addRequestHandler<AuthenticationHandler>(
				new AuthenticationHandler(
					this, 
					settings.authenticationTimeout)
			);

			/*
			
				1. wait for user
				2. authenticate user
				3. accept authentication
				// think about send request
				// after 5

				// think about if player should
				// be able to no play and be in
				// waiting mode (yes should it)
				4. send player data request
				   to operator
				5. wait for player
				   simulation request
				6. await player data if not
				   yet received
				7. create simulation with
				   playerdata
				8. running
			
			*/
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

		Operator::UserID userID = NULL;
		Status status = Status::Authenticating;
		
	/*	void onRequestFailed(const ::Net::Request::Reason reason) override
		{
		}
		*/

		void onAuthenticated(const Operator::UserID userID)
		{
			Log::Information(L"Client authenticated",
				userID, L"userID");

			status = Status::Waiting;
			this->userID = userID;

			removeRequestHandler<AuthenticationHandler>();
			// addRequestHandler<>

			// add get player data request
			// Operator::ConnectionHandler::PushRequest();
		}
	
		void onAuthenticationDenied() override
		{
			status = Status::Closing;
		}

		void accessOnRequestFailed(const ::Net::RequestFailure reason) override
		{
			accessSendMessage(
				Host::AuthenticationMessageID::InternalError,
				NULL);
			status = Status::Closing;
		}

		void onMessageSendFailed(const SendFailure reason) override
		{
			accessSendMessage(
				Host::AuthenticationMessageID::InternalError,
				NULL);
			status = Status::Closing;
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
