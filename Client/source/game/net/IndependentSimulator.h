#pragma once

#include <Client/source/game/Simulation.h>
#include <Client/source/game/VirtualPlayer.h>

namespace Game
{
	namespace Net
	{
		// move all logic into (remote) connection
		class DedicatedRemoteSimulation
			:
			public Simulation
		{
		public:
			struct Settings
			{
				sf::Time softTimeout = sf::milliseconds(500);	// allowed lag
				int hardTimeoutCount = 4; // 2 sec				// timout after N lags
			};

			DedicatedRemoteSimulation(
				RemoteConnection* const connection,
				const Settings settings)
				:
				connection(connection),
				settings(settings)
			{
			}

			void onConnectionRemoved(UserConnection* const connection)
			{
				if (connection == this->connection)
				{
					kill();
				}
			}

		private:
			bool processLogic() override
			{
				if (connection->getVirtualPlayer()->hasUpdate())
				{
					return false;
				}

				return handleNoUpdate();
			}

			bool handleNoUpdate()
			{
				if (logicCounter <= settings.softTimeout.asMicroseconds())
				{
					return true;
				}

				// enforce sync

				if (++softTimeoutCounter >= settings.hardTimeoutCount)
				{
					assert(connection->getStatus() != RemoteConnection::Timeout);

					Log::Error(L"User (pid="
						+ std::to_wstring(connection->getInformation().playerId)
						+ L", name="
						+ connection->getInformation().name
						+ L") timed out");

					connection->adjustStatus(RemoteConnection::Timeout);

					return true;
				}

				for (int i = 0; i < (logicCounter / LogicTimeStep); ++i)
					connection->getVirtualPlayer()->pushEmptyUpdate();

				return false;
			}

			int softTimeoutCounter = 0;

			RemoteConnection* connection;
			const Settings settings;
		};

		class DedicatedSimulator
			:
			public Simulator
		{
		public:
			DedicatedSimulator(
				const DedicatedRemoteSimulation::Settings simulationSettings,
				GamemodeCreatorBase* const gamemodeCreator)
				:
				Simulator(gamemodeCreator),
				simulationSettings(simulationSettings)
			{
			}

			bool initialize() override
			{
				return true;
			}
			
			bool pushConnection(UserConnection* const connection) override
			{
				return pushSimulation(initiateSimulation(connection));
			}
			
		private:
			virtual bool pushSimulation(Simulation* const simulation) = 0;

			Simulation* initiateSimulation(UserConnection* const connection)
			{
				switch (connection->type)
				{
				case UserConnection::Local:
					return new DedicatedLocalSimulation(
						(LocalConnection*) connection);

				case UserConnection::Remote:
					return new DedicatedRemoteSimulation(
						(RemoteConnection*) connection, simulationSettings);

				case UserConnection::Ghost:
					// return new DedicatedRemoteSimulation((GhostConnection*)connection);
					assert(false);
				}
					
				// not possible
			}

			const DedicatedRemoteSimulation::Settings simulationSettings;
		};
	}
}