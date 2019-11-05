#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/IndependentSimulator.h>
#include <Client/source/game/UserConnection.h>

namespace Game
{
	class NetOperator
	{
	public:
		struct Settings
		{
			int tickRate; // in Game::LogicTimeStep (ms)
		};

		struct Message
		{
			enum Type
			{
				Connect,
				Disconnect
			} type;
		};

		~NetOperator()
		{
			if (Device::Net::Isinitialized())
			{
				Device::Net::Uninitialize();
			}
		}

		NetOperator(const Settings settings)
			:
			settings(settings)
		{
		}

		bool initialize(const unsigned short port)
		{
			if (Device::Net::Isinitialized())
			{
				return false;
			}

			if (!Device::Net::Initialize(port))
			{
				return false;
			}
		}

		void onLogic(const sf::Time time)
		{
			while (Device::Net::PollPacket(&packet))
			{
				switch (packet.type)
				{
				case Device::Net::Packet::Command:


					break;
				case Device::Net::Packet::OperatorMessage:


					break;
				case Device::Net::Packet::GameMessage:
					RemoteConnection::Message message;

					packet.packet >> message.playerId;
					packet.packet >> (int&) message.type;
					message.packet = packet.packet;

					for (RemoteConnection* const connection : connections)
						if (connection->getIpAddress() == packet.address &&
							connection->getInformation().playerId == message.playerId)
						{


							break;
						}

					break;
				}
			}



			logicCounter += time;
			if (logicCounter.asMicroseconds() > Game::LogicTimeStep * settings.tickRate)
			{
				// sync
			}
		}

	private:
		const Settings settings;

		Device::Net::Packet packet;
		std::vector<RemoteConnection*> connections;

		sf::Time logicCounter;
	};
}
