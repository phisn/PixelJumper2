#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

namespace Game
{
	class RemoteConnection
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

		enum Status
		{
			Running,
			Pending,
			Disconnected,
			Timeout,
			Error
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
			socket.setBlocking(false);
		}

		bool initialize()
		{
		}

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

		bool update()
		{
			switch (socket.receive(buffer))
			{
			case sf::Socket::Status::Disconnected:
				adjustStatus(Disconnected);

				return false;
			case sf::Socket::Status::Done:
				handlePacket();

				break;
			case sf::Socket::Status::Error:
				adjustStatus(Error);

				return false;
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

				return false;
			}

			return true;
		}

		void sendError(const std::wstring message, const sf::Uint32 code)
		{
		}

		Status getStatus() const
		{
			return status;
		}

		sf::TcpSocket& getSocket()
		{
			return socket;
		}

	protected:
		void handlePacket()
		{

		}

		void adjustStatus(const Status status)
		{
			if (this->status < status)
				this->status = status;
		}

		const Settings settings;

		Status status;

		sf::TcpSocket socket;
		sf::Packet buffer;

		ClassicSimulation* simulation = NULL;
		VirtualPlayer* player = NULL;
	};
}
