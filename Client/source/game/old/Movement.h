#pragma once

#include <SFML/Graphics.hpp>

namespace Game
{
	class Movement
	{
		const float TIME_FACTOR = 1000.f * 1000.f;
	public:
		sf::Vector2f nextPositionOffset(
			const sf::Time time) const
		{
			return movement * doTime(time);
		}

		void change(
			const sf::Time time,
			const sf::Vector2f direction)
		{
			movement += doTime(time) * direction;
		}

		void jump(
			const float factor)
		{
			movement.y = factor;
		}

		void onLogic(
			const sf::Time time,
			const bool x_changed)
		{
			if (!onGround)
			{
				change(time, { 0.f, 0.2f });
			}

			if (movement.x)
			{
				onResistance(
					time,
					&movement.x,
					onGround 
					? groundResistance
					: airResistance,
					!x_changed * slide);
			}

			if (movement.y > 0.0f)
			{
				onResistance(
					time,
					&movement.y,
					0.05f,
					0.f);
			}
		}

		void muliMovement(
			const sf::Vector2f direction)
		{
			movement.x *= direction.x;
			movement.y *= direction.y;
		}

		bool isOnGround() const
		{
			return onGround;
		}

		void releaseGround()
		{
			onGround = false;
		}

		void setOnGround(
			const float resistance)
		{
			groundResistance = resistance;
			onGround = true;
		}

	private:
		bool onGround = false;
		float 
			airResistance = 0.2f,
			groundResistance = 0.f;
		const float slide = 0.1f;

		void onResistance(
			const sf::Time time,
			float* movement,
			const float factor,
			const float slide)
		{
			const float change_c = 
				copysignf(
					doTime(time) * slide,
					*movement);

			const float change = doTime(time)
				* *movement
				* factor;

			if (fabs(change) > fabs(*movement) + fabs(change_c))
			{
				*movement = 0.f;
			}
			else
			{
				*movement -= change + change_c;
			}
		}

		float doTime(
			const sf::Time time) const
		{
			return (float) time.asMicroseconds() / TIME_FACTOR * timeSpeed;
		}

		sf::Vector2f movement;
		const float timeSpeed = 15.f;
	};
}
