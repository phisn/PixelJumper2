#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/IndependentSimulator.h>
#include <Client/source/game/UserConnection.h>

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
		}

		bool save(Resource::WritePipe* const pipe) override
		{
		}

		bool setup() override
		{
			return true;
		}
	};

	class NetOperator
	{
	public:
		struct Settings
		{
			int tickRate; // in Game::LogicTimeStep (ms)
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
				case Device::Net::PacketContext::ConnectionMessage:
					onConnectionMessage();

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

		bool hasPlayer(const Resource::PlayerId player)
		{
			for (RemoteConnection* const connection : connections)
				if (connection->getInformation().playerId == player)
				{
					return true;
				}

			return false;
		}

		const Settings& getSettings() const
		{
			return settings;
		}

	protected:
		virtual void processLogic() = 0;

		void handleCommandPacket()
		{
		}

		void handleGameMessagePacket()
		{
		}

		void handleOperatorMessagePacket()
		{
			OperatorMessage message;

			Resource::ReadPipe* const pipe = Device::Net::GetReadPipe();
			if (!message.make(pipe))
			{
				// ...
				return;
			}

			onOperatorMessage(&message);
		}
		
		virtual void onCommand() = 0;
		virtual void onConnectionMessage() = 0;

		virtual void onOperatorMessage(OperatorMessage* const message)
		{
			switch (message->type)
			{
			case OperatorMessage::Connect:
			{
				Resource::PlayerResource resource;

				if (!retrivePlayerResource(
						&resource,
						message->connection.HeaderIntro.playerId))
				{
					// ...
				}

				if (!hasPlayer(message->connection.HeaderIntro.playerId))
				{
					RemoteConnection* const connection = new RemoteConnection(packet.address, PlayerInformation::Create(&resource));

					if (!simulator->pushConnection(connection))
					{
						// ...
					}

					connections.push_back(connection);
				}

				Device::Net::RegisterPacket(packet.type);

				Resource::ResourceBase* const client = NULL;
				if (!client->save(Device::Net::GetWritePipe()))
				{
					// ... delete last connection
				}

				if (!Device::Net::PushPacket(
					packet.address,
					packet.port))
				{
					// ... delete last connection
				}

				// success
			}
				break;
			case OperatorMessage::Ack:

				break;
			case OperatorMessage::Disconnect:
				connections[0]->adjustStatus(RemoteConnection::Disconnected);

				break;
			}
		}

		virtual bool retrivePlayerResource(
			Resource::PlayerResource* const resource,
			const Resource::PlayerId player) = 0;

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
