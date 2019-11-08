#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/IndependentSimulator.h>
#include <Client/source/game/UserConnection.h>

namespace Game::Net
{
	class NetOperator
	{
	public:
		struct Settings
		{
			int tickRate; // in Game::LogicTimeStep (ms)
		};

		struct Message
			:
			public Resource::ResourceBase
		{
			enum Type
			{
				Load,
				Confirm,
				Connect,
				Disconnect

			} type;

			Resource::ConnectionResource connection;

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

		bool initialize(
			Simulator* const simulator,
			const unsigned short port)
		{
			return Device::Net::Isinitialized()
				&& Device::Net::Initialize(port)
				&& simulator->initialize();
		}

		void onLogic(const sf::Time time)
		{
			while (Device::Net::PollPacket(&packet))
			{
				switch (packet.type)
				{
				case Device::Net::PacketContext::Command:
					onCommand();

					break;
				case Device::Net::PacketContext::OperatorMessage:
					onOperatorMessage();

					break;
				case Device::Net::PacketContext::GameMessage:
					onGameMessage();

					break;
				}
			}

			logicCounter += time;
			if (logicCounter.asMicroseconds() > Game::LogicTimeStep * settings.tickRate)
			{
				processLogic();
			}

			for (decltype(connections)::iterator i = connections.begin();
				i != connections.end(); ++i)
			{
				RemoteConnection* const connection = *i;

				if (connection->getStatus() != RemoteConnection::Connected)
				{
					simulator->onConnectionRemoved(*i);

					connections.erase(i);
					delete connection;
				}
			}
		}

		const Settings& getSettings() const
		{
			return settings;
		}

	protected:
		virtual void processLogic() = 0;

		virtual void onCommand() = 0;
		virtual void onGameMessage() = 0;

		virtual void onOperatorMessage()
		{
			Message message;

			Resource::ReadPipe* const pipe = Device::Net::GetReadPipe();
			if (!message.make(pipe))
			{
				// ...
				return;
			}

			switch (message.type)
			{
			case Message::Connect:


				break;
			case Message::Acknowledgement:

			case Message::Disconnect:

			}
		}

		Simulator* simulator;

		std::vector<RemoteConnection*> connections;
		Device::Net::PacketContext packet;

	private:
		const Settings settings;

		sf::Time logicCounter;
	};

	class LocalHostOperator
	{
	public:

	private:
		void onCommand()
		{
		}

		void onOperatorMessage()
		{
		}

		void onGameMessage()
		{
		}
	};
}
