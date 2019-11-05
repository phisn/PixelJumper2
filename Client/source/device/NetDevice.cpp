#include "NetDevice.h"

namespace
{
	sf::UdpSocket socket;

	Device::Net::PacketId lastPacketIds[4];
	int lastPacketIdIndex = 0;

	bool initialized = false;
}

namespace Device::Net
{
	bool ValidatePacketId(sf::Packet* const packet);
	bool Initialize(const unsigned short port)
	{
		if (const sf::Socket::Status status = socket.bind(port); status != sf::Socket::Done)
		{
			Log::Error(L"Failed to bind socket (c="
				+ std::to_wstring((int) status) + L")");

			return false;
		}

		socket.setBlocking(false);

		initialized = true;
		return true;
	}

	void Uninitialize()
	{
		socket.unbind();

		initialized = false;
	}

	bool Isinitialized()
	{
		return initialized;
	}

	bool PollPacket(Packet* const result)
	{
		const sf::Socket::Status status = socket.receive(
			result->packet, 
			result->address, 
			result->port);
		
		if (status != sf::Socket::Status::Done)
		{
			if (status != sf::Socket::Status::NotReady)
			{
				Log::Error(L"Got invalid result in pollpacket");
			}

			return false;
		}

		if (!ValidatePacketId(&result->packet))
		{
			return false;
		}
		
		result->packet >> (int&)result->type;

		return result->type > Packet::Invalid &&
			   result->type < Packet::_Length;
	}

	bool ValidatePacketId(sf::Packet* const packet)
	{
		PacketId id;
		*packet >> (PacketId&) id;

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
	
	void RegisterPacket(Packet* const packet, const Packet::Type type)
	{
		packet->packet.clear();

		packet->packet << Device::Random::MakeRandom<PacketId>();
		packet->packet << (packet->type = type);
	}

	bool PushPacket(Packet* const packet)
	{
		const sf::Socket::Status status = socket.send(
			packet->packet,
			packet->address,
			packet->port);

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
