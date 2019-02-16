#pragma once

#include <Client/source/game/CollidableTile.h>
#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/InitializableTile.h>
#include <Client/source/game/StaticTile.h>

#include <map>
#include <vector>

namespace Game
{
	class TileContainer
	{
	public:
		struct SortedTiles
		{
			std::vector<GameTileBase*> staticTiles;
			std::multimap<CollisionType, CollidableTile*> collidableTiles;
			std::vector<InitializableTile*> initializableTiles;
		};

		~TileContainer()
		{
			for (GameTileBase* const tile : tiles)
			{
				delete tile;
			}
		}

		void insertTile(GameTileBase* const tile)
		{
			tiles.push_back(tile);

			if (tile->getTileProperties()->isCollidable)
			{

			}

			if (tile->getTileProperties()->isStatic)
			{
				sortedTiles.staticTiles.push_back(tile);
			}

			if (tile->getTileProperties()->isInitializable)
			{

			}
		}

		const std::vector<GameTileBase*>& getTiles() const;
		const SortedTiles& getSortedTiles() const;
	private:
		std::vector<GameTileBase*> tiles;

		SortedTiles sortedTiles;
	};

	inline const std::vector<GameTileBase*>& TileContainer::getTiles() const
	{
		return tiles;
	}

	inline const TileContainer::SortedTiles& TileContainer::getSortedTiles() const
	{
		return sortedTiles;
	}
}
