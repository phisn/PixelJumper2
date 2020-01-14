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

		bool hasPlayer(const Resource::PlayerID player)
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


		virtual bool retrivePlayerResource(
			Resource::PlayerResource* const resource,
			const Resource::PlayerID player) = 0;

		Simulator* simulator;

		std::vector<RemoteConnection*> connections;

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
