#pragma once

#include <Client/source/game/VirtualPlayer.h>
#include <Client/source/game/GameWorld.h>

namespace Game
{
	struct VirtualWorldSettings
	{
		sf::Time softTimeout = sf::milliseconds(500);	// allowed lag
		int hardTimeoutCount = 4; // 2 sec				// timout after N lags
	};

	class DedicatedVirtualWorld
		:
		public World
	{
	public:
		DedicatedVirtualWorld(
			Resource::World* const resource,
			const VirtualWorldSettings settings,
			VirtualPlayer* const player)
			:
			World(resource),
			settings(settings),
			player(player)
		{
		}

		~DedicatedVirtualWorld()
		{
			delete player;
		}

		void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			if (logicCounter > LogicTimeStep)
			{
				if (!player->hasUpdate() && handleNoUpdate())
				{
					return;
				}

				do
				{
					World::processPlayer(player);

					environment.onLogic(
						sf::microseconds(LogicTimeStep)
					);

					logicCounter -= LogicTimeStep;
				} 
				while (logicCounter > LogicTimeStep);
			}

			properties.update();
		}
		
	private:
		bool handleNoUpdate()
		{
			if (logicCounter > settings.softTimeout.asMicroseconds())
			{
				++softTimeoutCounter;
				// enforce sync
			}
			else
			{
				return true;
			}

			if (softTimeoutCounter >= settings.hardTimeoutCount)
			{
				// timeout
			}
			else
			{
				for (int i = 0; i < (logicCounter / LogicTimeStep); ++i)
					player->pushEmptyUpdate();
			}

			return false;
		}

		VirtualPlayer* const player;
		const VirtualWorldSettings settings;

		int softTimeoutCounter = 0;
		sf::Uint64 logicCounter = 0;
	};
}
