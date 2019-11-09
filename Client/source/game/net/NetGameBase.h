#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/game/net/IndependentSimulator.h>

namespace Game::Net
{
	struct OperatorMessage
		:
		public Resource::ResourceBase
	{
		enum Type
		{
			Connect,
			Ack,
			Disconnect,

			Extended

		} type;

		Resource::ConnectionResource connection;

		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&type)
				&& connection.make(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&type)
				&& connection.save(pipe);
		}

		bool setup() override
		{
			return true;
		}
	};

	class ClientMessage
		:
		public Resource::ResourceBase
	{
	public:

	private:
		bool make(Resource::ReadPipe* const pipe) override
		{
		}

		bool save(Resource::WritePipe* const pipe) override
		{
		}

		bool setup() override
		{
			return true;
		}
	};

	class NetGameBase
	{
	public:
		virtual ~NetGameBase()
		{
			if (Device::Net::Isinitialized())
			{
				Device::Net::Uninitialize();
			}
		}

		virtual bool initialize(
			Simulator* const simulator,
			const unsigned short port)
		{
			return Device::Net::Isinitialized()
				&& Device::Net::Initialize(port)
				&& simulator->initialize();
		}

		virtual void onLogic(const sf::Time time)
		{
			while (Device::Net::PollPacket(&packet))
			{
				switch (packet.type)
				{
				case Device::Net::PacketContext::NetMessage:
					onNetMessagePacket();

					break;
				case Device::Net::PacketContext::ConnectionMessage:
					onConnectionMessagePacket();

					break;
				case Device::Net::PacketContext::Command:
					onCommandPacket();

					break;
				}
			}

			simulator->onLogic(time);
		}

	protected:
		virtual void onNetMessagePacket() = 0;
		virtual void onConnectionMessagePacket() = 0;
		virtual void onCommandPacket() = 0;

		Simulator* simulator;

		Device::Net::PacketContext packet;
	};
}
