#include "NetDevice.h"

namespace
{
	// preallocated on stack
	Resource::PacketReadPipe pollPipe;
	Resource::PacketWritePipe pushPipe;

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
	bool ValidatePacketId();
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

	bool PollPacket(PacketContext* const context)
	{
		pollPipe.reset();
		const sf::Socket::Status status = udpsocket.receive(
			pollPipe.getPacket(), 
			context->address, 
			context->port);
		
		if (status != sf::Socket::Status::Done)
		{
			if (status != sf::Socket::Status::NotReady)
			{
				Log::Error(L"Got invalid result in pollpacket");
			}

			return false;
		}

		
		if (!ValidatePacketId())
		{
			return false;
		}
		
		if (!pollPipe.readValue((int*) &context->type))
		{
			return false;
		}

		return context->type > PacketContext::Invalid &&
			   context->type < PacketContext::_Length;
	}

	Resource::PacketReadPipe* GetReadPipe()
	{
		return &pollPipe;
	}

	bool ValidatePacketId()
	{
		PacketId id;
		if (!pollPipe.readValue((int*) &id))
		{
			return false;
		}

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
	
	void RegisterPacket(const PacketContext::Type type)
	{
		pushPipe.reset();

		const PacketId id = Device::Random::MakeRandom<PacketId>();

		pushPipe.writeValue(&id);
		pushPipe.writeValue((int*) &type);
	}

	bool PushPacket(
		const sf::IpAddress ip,
		const unsigned short port)
	{
		const sf::Socket::Status status = udpsocket.send(
			pushPipe.getPacket(),
			ip,
			port);

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

	bool PushReliablePacket(
		const sf::IpAddress ip, 
		const unsigned short port)
	{
	}

	Resource::PacketWritePipe* GetWritePipe()
	{
		return &pushPipe;
	}
}
