#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	class Movement
	{
		const float TIME_FACTOR = 1000.f * 1000.f;
	public:

		void change(
			const sf::Time time,
			const sf::Vector2f direction)
		{
			movement += doTime(time) * direction;
		}

		void onLogic(
			const sf::Time time)
		{
			if (!onGround)
			{
				change(time, { 0.f, 0.02f });
			}

			if (movement.x)
			{
				onResistance(
					time,
					&movement.x,
					onGround 
					? groundResistance
					: airResistance);
			}

			if (movement.y)
			{
				onResistance(
					time,
					&movement.y,
					airResistance);
			}
		}

		bool isOnGround() const
		{
			return onGround;
		}

	private:
		bool onGround = false;
		float 
			airResistance = 0.05f,
			groundResistance = 0.f;

		void onResistance(
			const sf::Time time,
			float* movement,
			const float factor)
		{
			const float change = doTime(time)
				* *movement
				* factor;

			if (fabs(change) > fabs(*movement))
			{
				*movement = 0.f;
			}
			else
			{
				*movement -= change;
			}
		}

		float doTime(
			const sf::Time time) const
		{
			return (float) time.asMicroseconds() / TIME_FACTOR * timeSpeed;
		}

		sf::Vector2f movement;
		const float timeSpeed;
	};
}
