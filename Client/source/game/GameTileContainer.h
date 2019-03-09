#pragma once

#include <Client/source/game/tiletrait/CollidableTile.h>
#include <Client/source/game/tiletrait/DynamicTile.h>
#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/game/tiletrait/StaticTile.h>
#include <Client/source/game/tiletrait/SynchronizableTile.h>

#include <map>
#include <vector>

namespace Game
{
	class TileContainer
	{
	public:

	private:
		std::unordered_map<
			size_t, 
			std::vector<GameTileBase*>
		> tiles;
	};

	class _TileContainer
	{
	public:
		struct SortedTiles
		{
			std::vector<StaticTile*> staticTiles;
			std::map<
				CollisionType, 
				std::vector<CollidableTile*>
			> collidableTiles;
			std::vector<DynamicTile*> dynamicTiles;
			std::vector<SynchronizableTile*> synchronizableTiles;
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
			tile->registerType(this);
		}

		void insertCollidable(CollidableTile* const tile, CollisionType type)
		{
			sortedTiles.collidableTiles[type].push_back(tile);
		}

		void insertDynamic(DynamicTile* const tile)
		{
			sortedTiles.dynamicTiles.push_back(tile);
		}

		void insertStatic(StaticTile* const tile)
		{
			sortedTiles.staticTiles.push_back(tile);
		}

		void insertSynchronizable(SynchronizableTile* const tile)
		{
			sortedTiles.synchronizableTiles.push_back(tile);
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
