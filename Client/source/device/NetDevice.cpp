#include "NetDevice.h"

namespace
{
	sf::UdpSocket udpsocket;

	Device::Net::PacketId lastPacketIds[4];
	int lastPacketIdIndex = 0;

	bool initialized = false;
}

// Client members
namespace Device::Net
{
}

// Functions
namespace Device::Net
{
	bool Initialize(const unsigned short port)
	{
		if (const sf::Socket::Status status = udpsocket.bind(port); status != sf::Socket::Done)
		{
			Log::Error(L"Failed to bind socket (c="
				+ std::to_wstring((int) status) + L")");

			return false;
		}

		udpsocket.setBlocking(false);

		initialized = true;
		return true;
	}

	void Uninitialize()
	{
		udpsocket.unbind();

		initialized = false;
	}

	bool Isinitialized()
	{
		return initialized;
	}

	bool PollPacket(MessageRead* const message)
	{
		if (const sf::Socket::Status status = message->pullPacket(&udpsocket); status != sf::Socket::Status::Done)
		{
			// nonblocking - no packet in buffer
			if (status != sf::Socket::Status::NotReady)
			{
				Log::Error(L"Got invalid result in pollpacket");
			}
		
			return false;
		}

		if (!ValidatePacketId(message->getPacketId()))
		{
			// no log needed because udp
			// double packets are common
			// and cant be handled

			return false;
		}

		return true;
	}

	bool ValidatePacketId(const PacketId id)
	{
		// unrolled loop
		if (id == lastPacketIds[0] ||
			id == lastPacketIds[1] ||
			id == lastPacketIds[2] ||
			id == lastPacketIds[3])
		{
			return false;
		}

		lastPacketIds[lastPacketIdIndex++] = id;
		return true;
	}

	bool PushPacket(
		MessageWrite* const message,
		const Target target)
	{
		const sf::Socket::Status status = message->pushPacket(&udpsocket, target);
		
		if (status == sf::Socket::Error)
		{
			Log::Error(L"Failed to push packet");

			return false;
		}

		if (status != sf::Socket::Done)
		{
			Log::Error(L"Failed to push packet (s="
				+ std::to_wstring(status) + L")");

			return false;
		}

		return true;
	}
}
