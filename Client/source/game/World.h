#pragma once

#include <Client/source/game/LocalPlayer.h>
#include <Client/source/game/WorldManager.h>

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TimedTile.h>
#include <Client/source/game/tiles/TouchableTile.h>

#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>

namespace Game
{
	class World
	{
	public:
		~World()
		{
			delete settings;
		}

		bool initialize(
			_In_ WorldSettings* settings)
		{
			this->settings = settings;

			if (!validate())
			{
				return false;
			}

			worldImage.create(
				settings->size.x,
				settings->size.y);

			for (Tile::Base* tile : settings->tiles)
			{
				if (CONTAINS_ENUM(tile->getType(), Tile::Type::Collidable))
				{
					collidable.push_back(
						(Tile::Collidable*) tile);
				}

				if (CONTAINS_ENUM(tile->getType(), Tile::Type::Timed))
				{
					timed.push_back(
						(Tile::Timed*) tile);
				}

				if (CONTAINS_ENUM(tile->getType(), Tile::Type::Static))
				{
					worldImage.setPixel(
						tile->getPosition().x,
						tile->getPosition().y,
						tile->getColor());
				}
			}

			return true;
		}

		void initializePlayer(
			_In_ LocalPlayer* player)
		{
			player->getView()->setSize(settings->size);
			player->setPosition(sf::Vector2f(0, 0));
		}

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
				player->getMovement()->nextPositionOffset(time)
			);
		}

		void onDraw() const
		{
			for (Tile::Base* tile : settings->tiles)
			{
				tile->onDraw();
			}
		}

		bool validate() const;
	private:
		sf::Image worldImage;
		WorldSettings* settings;

		void movePlayer(
			LocalPlayer* player,
			const sf::Vector2f offset)
		{
			sf::Vector2f destination =
				player->getPosition() + offset;

			Tile::Collidable* bestTile;
			Tile::Collision bestCollision;
			Tile::Collision collision;

			bool collided;
			while (collision.remaining != 0.0f)
			{
				collided = false;

				bestTile = NULL;
				collision.remaining = 0.0f;

				for (Tile::Collidable* tile : collidable)
				{
					if (tile->checkCollision(
						player->getPosition(),
						destination,
						&collision) && !bestTile ||
							sqrtf(
								pow(destination.x - collision.position.x, 2.f) + 
								pow(destination.y - collision.position.y, 2.f)
							) >
							sqrtf(
								pow(destination.x - bestCollision.position.x, 2.f) +
								pow(destination.y - bestCollision.position.y, 2.f)
							)
						)
						
					{
						bestCollision = collision;
						bestTile = tile;
					}
				}

				if (bestTile)
				{
					collided = bestTile->onCollision(
						bestCollision,
						player,
						&destination);
				}
			}

			if (!collided)
			{
				player->setPosition(destination);
			}
		}

		std::vector<Tile::Timed*> timed;
		std::vector<Tile::Collidable*> collidable;
	};
}
