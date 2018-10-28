#pragma once

#include <vector>

#include <Client/source/editor/tile/Wall.h>

#include <Client/source/game/World.h>

namespace Editor
{
	struct Selection
	{
		enum class Type
		{
			Random,
			EqualType,
		} type;

		std::vector<TileBase*> tiles;
	};

	class TileGrid
	{
	public:
		~TileGrid()
		{
			for (TileBase* tile : tiles)
			{
				delete tile;
			}
		}

		void select(
			sf::Vector2f offset,
			sf::Vector2f size,
			Selection* selection)
		{
			Game::Tile::Id lastId = Game::Tile::Id::InvalidTile;

			selection->tiles.clear();
			selection->type = Selection::Type::EqualType;

			for (TileBase* tile : tiles)
				if (tile->position.x + tile->size.x > offset.x &&
					tile->position.y + tile->size.y > offset.y &&
					
					tile->position.x < offset.x + size.x &&
					tile->position.y < offset.y + size.y)
				{
					if (selection->type == Selection::Type::EqualType)
					{
						if (lastId == Game::Tile::Id::InvalidTile)
						{
							lastId = tile->getId();
						}
						else
						{
							if (lastId != tile->getId())
							{
								selection->type = Selection::Type::Random;
							}
						}
					}

					selection->tiles.push_back(tile);
				}
		}

		Game::WorldSettings* createWorldSettings(
			const std::wstring author,
			const std::wstring name,
			const sf::Vector2f size)
		{
			Game::WorldSettings* world = new Game::WorldSettings();

			world->author = author;
			world->name = name;
			world->size = size;

			for (TileBase* tile : tiles)
			{
				world->tiles.push_back(tile->create());
			}

			return world;
		}
	private:
		std::vector<TileBase*> tiles;
	};
}