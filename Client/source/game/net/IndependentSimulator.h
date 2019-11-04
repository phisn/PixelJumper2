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
			enum State
			{ // order by importance
				Running,
				Disconnect,
				Timeout,
				Error
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

			State getState() const
			{
				return state;
			}

		protected:
			virtual bool processLogic() = 0;

			sf::Uint64 logicCounter;
			GamemodeBase* gamemode;

			void adjustState(const State state)
			{
				if (this->state < state)
					this->state = state;
			}

		private:
			State state = Running;
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

			bool initialize(GamemodeBase* const gamemode) override
			{
				connection->initialize();
				return Simulation::initialize(gamemode);
			}

		private:
			bool processLogic() override
			{
				return false;
			}

			LocalConnection* const connection;
		};

		class DedicatedVirtualSimulation
			:
			public Simulation
		{
		public:
			struct Settings
			{
				sf::Time softTimeout = sf::milliseconds(500);	// allowed lag
				int hardTimeoutCount = 4; // 2 sec				// timout after N lags
			};

			DedicatedVirtualSimulation(const Settings settings)
				:
				settings(settings)
			{
			}

			bool initialize(GamemodeBase* const gamemode) override
			{

				return Simulation::initialize(gamemode);
			}

		private:
			bool processLogic() override
			{
				if (player->hasUpdate())
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
						+ std::to_wstring(player->getInformation().playerId)
						+ L", name="
						+ player->getInformation().name
						+ L") timed out");

					adjustState(Timeout);

					return true;
				}
				
				for (int i = 0; i < (logicCounter / LogicTimeStep); ++i)
					player->pushEmptyUpdate();

				return false;
			}

			int softTimeoutCounter = 0;

			UserConnection* connection;
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
					Simulation* const simulation = *i;

					simulation->onLogic(time);

					switch (simulation->getState())
					{
					case Simulation::State::Running:
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
				Simulation* const simulation = initiateSimulation(connection);

				if (!simulation->initialize(gamemodeCreator->createGamemode()))
				{
					delete simulation;
					return false;
				}

				simulations.push_back(simulation);
				return true;
			}
			
		private:
			Simulation* initiateSimulation(UserConnection* const connection)
			{
				switch (connection->type)
				{
				case UserConnection::Local:
					return new DedicatedLocalSimulation((LocalConnection*) connection);

				case UserConnection::Remote:
					return new DedicatedVirtualSimulation((RemoteConnection*)connection);

				case UserConnection::Ghost:
					return new DedicatedVirtualSimulation((GhostConnection*)connection);

				}

				// not possible
			}

			std::vector<Simulation*> simulations;
			GamemodeCreatorBase* const gamemodeCreator;
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