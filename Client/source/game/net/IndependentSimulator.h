#pragma once

#include <Client/source/game/GamemodeBase.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/VirtualPlayer.h>

namespace Game
{
	namespace Net
	{
		class Simulation
		{
		public:
			virtual bool initialize(GamemodeBase* const gamemode)
			{
				return gamemode->initialize();
			}

			void onLogic(const sf::Time time)
			{
				logicCounter += time.asMicroseconds();

				if (logicCounter > LogicTimeStep)
				{
					do
					{
						if (!processLogic())
						{
							break;
						}

						logicCounter -= LogicTimeStep;
					} 
					while (logicCounter > LogicTimeStep);
				}
			}

		protected:
			// return == successfull process
			virtual bool processLogic() = 0;

			sf::Uint64 logicCounter;
			GamemodeBase* gamemode;
		};

		class VirtualSimulation
			:
			public Simulation
		{
		public:
			struct Settings
			{
				sf::Time softTimeout = sf::milliseconds(500);	// allowed lag
				int hardTimeoutCount = 4; // 2 sec				// timout after N lags
			};

		private:
			bool processLogic() override
			{
				if (player->hasUpdate())
				{
					return true;
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
					// timeout

					return true;
				}
				
				for (int i = 0; i < (logicCounter / LogicTimeStep); ++i)
					player->pushEmptyUpdate();

				return false;
			}

			int softTimeoutCounter = 0;

			World* world;
			VirtualPlayer* player;
			Settings settings;
		};

		class IndependentSimulator
		{
		public:
			IndependentSimulator()
			{
			}

			void onLogic(const sf::Time time)
			{
				for (Simulation* const simulation : simulations)
				{
					simulation->connection->onLogic(time);
					simulation->world->onLogic(time);
				}
			}
			
			World* createSimulation(
				GamemodeBase* const gamemode,
				UserConnection* const connection)
			{
			}
			
		private:
			std::vector<Simulation*> simulations;
		};
	}
}