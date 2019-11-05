#pragma once

#include <Client/source/game/GamemodeBase.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/VirtualPlayer.h>

namespace Game
{
	namespace Net
	{
		class DedicatedSimulation
			:
			public GameState
		{
		public:
			enum Status
			{ // order by importance
				Running,
				Disconnect,
				Timeout,
				Error
			};

			virtual ~DedicatedSimulation()
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

			virtual UserConnection* getConnection() const = 0;

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

			void adjustState(const Status state)
			{
				if (this->status < state)
					this->status = state;
			}

		private:
			Status status = Running;
		};

		class DedicatedLocalSimulation
			:
			public DedicatedSimulation
		{
		public:
			DedicatedLocalSimulation(LocalConnection* const connection)
				:
				connection(connection)
			{
			}

			UserConnection* getConnection() const
			{
				return connection;
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
			public DedicatedSimulation
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

			UserConnection* getConnection() const
			{
				return connection;
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
					Log::Error(L"User (pid=" 
						+ std::to_wstring(connection->getInformation().playerId)
						+ L", name="
						+ connection->getInformation().name
						+ L") timed out");

					adjustState(Timeout);

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

		// abstraction for simulator?
		class IndependentSimulator
		{
		public:
			IndependentSimulator(GamemodeCreatorBase* const gamemodeCreator)
				:
				gamemodeCreator(gamemodeCreator)
			{
			}

			void onLogic(const sf::Time time)
			{
				for (decltype(simulations)::iterator i = simulations.begin();
					i != simulations.end(); ++i)
				{
					DedicatedSimulation* const simulation = *i;

					simulation->onLogic(time);

					switch (simulation->getStatus())
					{
					case DedicatedSimulation::Status::Running:
						break;

					default:
						simulations.erase(i);
						delete simulation;

						break;
					}
				}
			}
			
			bool createSimulation(UserConnection* const connection)
			{
				DedicatedSimulation* const simulation = initiateSimulation(connection);

				if (!simulation->initialize(gamemodeCreator->createGamemode()))
				{
					delete simulation;
					return false;
				}

				simulations.push_back(simulation);
				return true;
			}
			
		private:
			DedicatedSimulation* initiateSimulation(UserConnection* const connection)
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

			std::vector<DedicatedSimulation*> simulations;
			GamemodeCreatorBase* const gamemodeCreator;
			DedicatedRemoteSimulation::Settings simulationSettings;
		};

		class NetOperator
		{
		public:
			/*
			
				Handles a independent simulator and connecting players.
				also loads their playerinformation from local base
			
			*/
		};
	}
}