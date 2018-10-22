#pragma once

#include <Client/source/game/LocalPlayer.h>

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TimedTile.h>
#include <Client/source/game/tiles/TouchableTile.h>

#include <SFML/Graphics.hpp>

#include <vector>
#include <iostream>

namespace Game
{
	struct WorldSettings
	{
		~WorldSettings()
		{
			for (Tile::Base* tile : tiles)
			{
				delete tile;
			}
		}

		int difficulty;
		float speed;

		sf::Vector2f begin;
		sf::Vector2f size;

		std::wstring name;
		std::wstring creator;
		
		std::vector<
			Tile::Base*> tiles;
	};

	class World
	{
	public:
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
			player->setPosition(settings->begin);
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
			const sf::Vector2f destination =
				player->getPosition() + offset;
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

		std::vector<Tile::Timed*> timed;
		std::vector<Tile::Collidable*> collidable;
	};
}
