#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/AuthenticationHandler.h>
#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

namespace Game::Net
{
	class RemoteConnection
		:
		public AuthenticationHandler
	{
	public:
		struct Error
		{
			enum : sf::Uint32
			{
				_Begin = 0xa0,
				InvalidSimulationState
			};
		};

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

		RemoteConnection(const Settings settings)
			:
			settings(settings)
		{
		}

		bool initialize()
		{
		}

		// called on every 
		bool processLogic()
		{
			if (simulation->getStatus() == ClassicSimulation::Running)
			{
				if (player->hasUpdate())
				{
					if (simulation->processLogic())
					{
					}
				}
			}
		}

		void update() override
		{
			switch (status)
			{
			case Connecting:
			case Connected:

			default:
				// update should neither be called with
				// disconnecting nor disconnected
				Log::Error(L"Got invalid status in update");

				break;
			}

			switch (simulation->getStatus())
			{
			case ClassicSimulation::Status::Running:
			case ClassicSimulation::Status::Shutdown:
			case ClassicSimulation::Status::Error:

			default:
				// missing * cant happen in remote connection
				// results in error

				/*
				sendError(
					Log::Convert(L"Got invalid simulation status",
						(int) simulation->getStatus(), L"status"),
					Error::InvalidSimulationState);
					*/

			}
		}

	private:
		const Settings settings;

		ClassicSimulation* simulation = NULL;
		VirtualPlayer* player = NULL;

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

		void onClientConnected() override;
	};
}
