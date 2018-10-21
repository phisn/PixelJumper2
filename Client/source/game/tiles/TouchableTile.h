#pragma once

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TimedTile.h>

#include <SFML/System/Time.hpp>

namespace Game
{
	namespace Tile
	{
		class Touchable
			:
			public Collidable,
			public Timed
		{
		protected:
			Touchable(
				const BaseSettings settings,
				const sf::Time resetTime)
				:
				Collidable(settings),
				resetTime(resetTime)
			{
				passedTime = { };
			}

			virtual void onTouch() = 0;

		private:
			bool onCollision(
				const Collision collision,
				LocalPlayer* player) override
			{
				if (passedTime > resetTime)
				{
					sf::FileInputStream
					onTouch();
				}

				return false;
			}

			void onTime(
				sf::Time time)
			{
				if (passedTime <= resetTime)
				{
					passedTime += time;
				}
			}

			sf::Time passedTime;
			const sf::Time resetTime;
		};
	}
}
