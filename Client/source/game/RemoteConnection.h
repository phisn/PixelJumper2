#pragma once

#include <Client/source/game/AuthenticationHandler.h>
#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

#include <Client/source/game/net/ClassicSimClientMessage.h>
#include <Client/source/game/net/ClassicSimHostMessage.h>

namespace Game::Net
{
	/*
	
		Welten komplett dynamisieren?
		-> transitiv wohin man will
		-> vorher nicht unbedingt erkennbar wohin es geht

		Spieler gibt es nicht mehr als resource

		Überlegen wie man asynchrone abfragen handhabt
	
	*/

	class ClassicSimulationHandler
		:
		public AuthenticationHandler
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

		ClassicSimulationHandler(
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

			simulation.pushPlayer(player);
		}

		void update() override
		{
			AuthenticationHandler::update();
		}

	private:
		const Settings settings;
		const WorldResourceContainer& container;

		ClassicSimulation simulation;
		VirtualPlayer* player = NULL;

		Resource::PlayerResource* playerResource;

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			if (status == Connecting)
			{
				AuthenticationHandler::onMessage(messageID, pipe);
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
			default:

				break;
			}
		}

		void onPrepareSimulation(const Client::PrepareSimulationMessage& request)
		{
			if (simulation.getStatus() == ClassicSimulation::Status::Running)
			{
				Host::RejectSimulationRequestMessage message;
				message.reason = Host::RejectSimulationRequestMessage::SimulationAlreadyRunning;

				sendCommonMessage(
					Host::ClassicalConnectionMessageID::RejectSimulationRequest,
					&message);
			}

			WorldResourceContainer::const_iterator iterator = container.find(request.world);
			
			if (iterator == container.cend())
			{
				Host::RejectSimulationRequestMessage message;
				message.reason = Host::RejectSimulationRequestMessage::InvalidWorldID;

				sendCommonMessage(
					Host::ClassicalConnectionMessageID::RejectSimulationRequest,
					&message);
			}

			if (!simulation.pushWorld(request.world))
			{
				simulation.
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
