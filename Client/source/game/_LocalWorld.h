#pragma once

#include <Client/source/game/GameWorld.h>
#include <Client/source/game/LocalPlayer.h>

namespace Game
{
	class LocalWorld
		:
		public World
	{
	public:
		using World::World;

		virtual void draw(sf::RenderTarget* const target)
		{
			environment.draw(target);

			for (LocalPlayer* const player : players)
			{
				player->onDraw(target);
			}
		}

		virtual void onLogic(const sf::Time time)
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > LogicTimeStep)
			{
				for (PlayerType* const player : players)
				{
					World::processPlayer(player);
				}

				environment.onLogic(
					sf::microseconds(LogicTimeStep)
				);

				logicCounter -= LogicTimeStep;
			}

			for (LocalPlayer* const player : players)
			{
				player->onLogic(time);
			}

			properties.update();
		}

	private:
		std::vector<LocalPlayer*> players;

		sf::Uint64 logicCounter = 0;
	};
}
