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

			for (LocalPlayer* const player : asLocalPlayers())
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

			for (LocalPlayer* const player : asLocalPlayers())
			{
				player->onLogic(time);
			}

			properties.update();
		}

	private:
		std::vector<LocalPlayer*>& asLocalPlayers()
		{
			return (std::vector<LocalPlayer*>&) players;
		}

		sf::Uint64 logicCounter = 0;
	};
}
