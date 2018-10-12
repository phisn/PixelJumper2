#pragma once

#include <Client/source/game/LocalPlayer.h>

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TimedTile.h>
#include <Client/source/game/tiles/TouchableTile.h>

#include <SFML/Graphics.hpp>

#include <vector>

namespace Game
{
	struct WorldSettings
	{
		const float speed = 0.1f;
	};

	class World
	{
	public:
		World();
		~World();

		void onLogic(
			sf::Time time)
		{
			for (Tile::Timed* tile : timed)
			{
				tile->onTime(time);
			}
		}

		void updatePlayer(
			_In_ LocalPlayer* player,
			const sf::Time time)
		{
			movePlayer(
				player,
				makeDestination(
					player, 
					time));

			if (player->getProperties()->isOnGround)
			{
				player->changeProperties()->movement *= 0.8f;
			}
			else
			{
				player->changeProperties()->movement *= 0.95f;
			}
		}

		void draw() const
		{
			for (Tile::Base* tile : tiles)
			{
				tile->draw();
			}
		}

		bool validate() const;
	private:
		const WorldSettings settings;

		void movePlayer(
			LocalPlayer* player,
			const sf::Vector2f destination)
		{
			Tile::Collision collision;

			for (Tile::Collidable* tile : collidable)
				if (tile->checkCollision(
						player->getPosition(),
						destination,
						&collision) &&
					tile->onCollision(
						collision,
						player))
				{
					return;
				}

			player->setPosition(destination);
		}

		sf::Vector2f makeDestination(
			const LocalPlayer* player,
			const sf::Time time)
		{
			return player->getPosition()
				+ player->getProperties()->movement
				* settings.speed
				* (float) time.asMicroseconds();
		}

		std::vector<Tile::Base*> tiles;

		std::vector<Tile::Touchable*> touchable;
		std::vector<Tile::Timed*> timed;
		std::vector<Tile::Collidable*> collidable;
	};
}
