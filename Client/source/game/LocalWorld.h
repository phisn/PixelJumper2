#pragma once

#include <Client/source/game/GameWorld.h>

namespace Game
{
	class LocalWorld
		:
		public World<LocalPlayer>
	{
	public: // access
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
				processLogic();
				logicCounter -= LogicTimeStep;
			}

			for (LocalPlayer* const player : players)
			{
				player->onLogic(time);
			}

			properties.update();
		}

	private:
		sf::Uint64 logicCounter = 0;
	};
}
