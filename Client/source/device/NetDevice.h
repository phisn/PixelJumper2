#pragma once

#include <Client/source/device/RandomDevice.h>
#include <Client/source/logger/Logger.h>

#include <SFML/Network.hpp>

namespace Device::Net
{
	typedef sf::Uint64 PacketId;

	struct Packet
	{
		enum Type
		{
			Invalid  = 0,

			Command,
			OperatorMessage,
			GameMessage,

			_Length
		} type;

		unsigned short port;
		sf::IpAddress address;
		
		sf::Packet packet;
	};

	bool Initialize(const unsigned short port);
	void Uninitialize();

	bool Isinitialized();

	// packets have / must have reliable data
	bool PollPacket(Packet* const packet);

	// automatically clears packet
	void RegisterPacket(Packet* const packet, const Packet::Type type);
	bool PushPacket(Packet* const packet);
}
