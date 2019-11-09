#pragma once

#include <Client/source/game/net/NetGameBase.h>

namespace Game::Net
{
	class NetClient
		:
		public NetGameBase
	{
	public:
		bool connect(const sf::IpAddress ipAddress)
		{

		}

	private:
		bool initialize(
			Simulator* const simulator,
			const unsigned short port)
		{
			return Device::Net::Isinitialized()
				&& Device::Net::Initialize(port)
				&& simulator->initialize();
		}

		virtual void onNetMessagePacket()
		{
			ClientMessage message;

			Resource::ReadPipe* const pipe = Device::Net::GetReadPipe();
			if (!message.make(pipe))
			{
				// ...
				return;
			}

			(&message);
		}

		virtual void onConnectionMessagePacket()
		{

		}

		virtual void onCommandPacket()
		{
		}
	};
}

