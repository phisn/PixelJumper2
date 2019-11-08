#pragma once

#include <Client/source/game/GamemodeBase.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/VirtualPlayer.h>

namespace Game
{
	namespace Net
	{
		class Simulation
			:
			public GameState
		{
		public:
			// order by importance
			enum Status
			{
				Running,	// has player(s)
				Dead,		// has no player
				Error		// error
			};

			virtual ~Simulation()
			{
			}
			
			virtual bool initialize(GamemodeBase* const gamemode)
			{
				return gamemode->initialize();
			}

			virtual void onLogic(const sf::Time time)
			{
				logicCounter += time.asMicroseconds();

				if (logicCounter > LogicTimeStep)
				{
					do
					{
						if (processLogic())
						{
							break;
						}

						logicCounter -= LogicTimeStep;
					} while (logicCounter > LogicTimeStep);
				}
			}

			// last possiblity to handly removal 
			// of connection
			virtual void onConnectionRemoved(UserConnection* const connection) = 0;

			virtual void kill()
			{
				adjustStatus(Simulation::Status::Dead);
			}

			Status getStatus() const
			{
				return status;
			}

			bool writeState(Resource::WritePipe* const writePipe)
			{
				return gamemode->writeState(writePipe);
			}

			bool readState(Resource::ReadPipe* const readPipe)
			{
				return gamemode->readState(readPipe);
			}

		protected:
			virtual bool processLogic() = 0;

			sf::Uint64 logicCounter;
			GamemodeBase* gamemode;

			void adjustStatus(const Status status)
			{
				if (this->status < status)
					this->status = status;
			}

		private:
			Status status = Running;
		};

		class DedicatedLocalSimulation
			:
			public Simulation
		{
		public:
			DedicatedLocalSimulation(LocalConnection* const connection)
				:
				connection(connection)
			{
			}

			void onConnectionRemoved(UserConnection* const connection)
			{
				assert(connection != this->connection);
			}

		private:
			bool processLogic() override
			{
				return false;
			}

			LocalConnection* const connection;
		};

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

		class Simulator
			:
			public GameState
		{
		public:
			typedef std::vector<Simulation*> SimulationContainer;
			typedef SimulationContainer::iterator SimulationIterator;

			Simulator(GamemodeCreatorBase* const gamemodeCreator)
				:
				gamemodeCreator(gamemodeCreator)
			{
			}
			
			virtual bool initialize() = 0;

			virtual void onLogic(const sf::Time time)
			{
				for (SimulationIterator i = simulations.begin(); i != simulations.end(); ++i)
				{
					processSimulationLogic(*i, time);

					switch ((*i)->getStatus())
					{
					case Simulation::Status::Dead:
						onSimulationDead(i);

						break;
					case Simulation::Status::Error:
						onSimulationError(i);

						break;
					}
				}
			}

			virtual bool pushConnection(UserConnection* const connection) = 0;
			virtual void onConnectionRemoved(UserConnection* const connection)
			{
				for (Simulation* const simulation : simulations)
				{
					simulation->onConnectionRemoved(connection);
				}
			}

		protected:
			virtual void processSimulationLogic(
				Simulation* const simulation,
				const sf::Time time)
			{
				simulation->onLogic(time);
			}

			virtual void onSimulationDead(SimulationIterator simulation)
			{
				handleCommonSimulationFault(simulation);
			}

			virtual void onSimulationError(SimulationIterator simulation)
			{
				handleCommonSimulationFault(simulation);
			}

			virtual void handleCommonSimulationFault(SimulationIterator simulation)
			{
				Simulation* const simulationPtr = *simulation;

				simulations.erase(simulation);
				delete simulationPtr;
			}

			virtual bool pushSimulation(Simulation* const simulation)
			{
				if (simulation->initialize(gamemodeCreator->createGamemode()))
				{
					simulations.push_back(simulation);
					return true;
				}
				else
				{
					delete simulation;
					return false;
				}
			}

		private:
			SimulationContainer simulations;
			GamemodeCreatorBase* const gamemodeCreator;
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