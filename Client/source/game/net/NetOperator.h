#pragma once

#include <Client/source/game/net/NetGameBase.h>
#include <Client/source/game/UserConnection.h>

namespace Game::Net
{
	class NetOperator
		:
		public NetGameBase
	{
	public:
		struct Settings
		{
			int tickRate; // in Game::LogicTimeStep (ms)
		};

		NetOperator(const Settings settings)
			:
			settings(settings)
		{
		}

		void onLogic(const sf::Time time) override
		{
			NetGameBase::onLogic(time);

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

		virtual void onNetMessagePacket()
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

		virtual void onConnectionMessagePacket()
		{

		}

		virtual void onCommandPacket()
		{
		}

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
