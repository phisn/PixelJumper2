#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/HostAuthenticationHandler.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/game/net/ClassicSimClientMessage.h>
#include <Client/source/game/net/ClassicSimHostMessage.h>

namespace Game::Net
{
	class HostClassicSimulation
		:
		public HostAuthenticationHandler
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

		HostClassicSimulation(
			const Settings settings,
			const WorldResourceContainer& container)
			:
			simulation(container),
			settings(settings),
			container(container)
		{
			playerResource = new Resource::PlayerResource();
			playerResource->username = L"username";
			playerResource->content.playerID = 0;

			player = new VirtualPlayer(Game::PlayerInformation::Create(playerResource));
			simulation.setPlayer(player);
		}

		void update() override
		{
			HostAuthenticationHandler::update();
		}

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
			if (status == Connecting)
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
			}
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

			sendCommonMessage(
				Host::ClassicalConnectionMessageID::RejectSimulationRequest,
				&message);
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
					(int) messageID,
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

		void onClientConnected() override
		{
			// request playerdata from operator
		}
	};
}
