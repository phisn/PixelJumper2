#pragma once

#include <Client/source/device/RandomDevice.h>
#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/PacketPipe.h>

#include <SFML/Network.hpp>

namespace Device::Net
{
	typedef sf::Uint64 PacketId;

	struct NetResource
	{

	};

	class PacketContext
	{
	public:
		enum Type
		{
			Invalid  = 0,

			OperatorMessage,
			ConnectionMessage,

			Command,

			_Length
		} type;

		unsigned short port;
		sf::IpAddress address;
	};

	bool Initialize(const unsigned short port);
	void Uninitialize();

	bool Isinitialized();

	// packets have / must have reliable data
	bool PollPacket(PacketContext* const context);
	Resource::PacketReadPipe* GetReadPipe();

	// automatically clears packet
	void RegisterPacket(const PacketContext::Type type);
	bool PushPacket(
		const sf::IpAddress ip, 
		const unsigned short port);
	Resource::PacketWritePipe* GetWritePipe();
}
