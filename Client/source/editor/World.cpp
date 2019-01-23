#include "World.h"

namespace Editor
{
	Resource::World* World::convert(
		const sf::Uint32 worldID, 
		const std::wstring authorName, 
		const std::wstring mapName) const
	{
		Log::Section section(L"Converting World");

		if (worldID == NULL)
		{
			Log::Error(L"Got invalid worldID: (NULL)");

			return NULL;
		}

		Resource::World* world = new Resource::World();

		world->HeaderIntro.worldID = worldID;

		world->HeaderAuth.authorName = authorName;
		world->HeaderAuth.mapName = mapName;

		Log::Information(
			L"Converting '"
			+ std::to_wstring(tiles.size())
			+ L"' tiles");

		if (!convertTiles(world))
		{
			delete world;
			world = NULL;
		}

		return world;
	}
	
	bool World::convertTiles(
		Resource::World* const world) const
	{
		GroupedTiles groupedTiles;

		{
			TileGroups tileGroups;

			sortTiles(
				&tileGroups);
			groupTiles(
				&groupedTiles,
				&tileGroups);
		}

		for (GroupedTile& tile : groupedTiles)
		{
			if (tile.position.x > RTILE_TYPE_MAX(x) ||
				tile.position.y > RTILE_TYPE_MAX(y))
			{
				return false;
			}

			if (tile.size.x > RTILE_TYPE_MAX(width) ||
				tile.size.y > RTILE_TYPE_MAX(height))
			{
				return false;
			}

			world->TileContainer.emplace_back();
			Resource::Tile* resourceTile = &world->TileContainer.back();

			resourceTile->Content = tile.tile->create(
				tile.size,
				tile.position);

			resourceTile->Header.width = tile.size.x;
			resourceTile->Header.height = tile.size.y;

			resourceTile->Header.x = tile.position.x;
			resourceTile->Header.y = tile.position.y;
		}

		return groupedTiles.size() > 0; // TODO: pointless?
	}
	void World::sortTiles(TileGroups* const tileGroups) const
	{
		for (TileBase* const tile : tiles)
		{
		NEXT_TILE:
			for (std::vector<TileBase*>& group : *tileGroups)
				if (group.back()->equals(tile))
				{
					group.push_back(tile);

					goto NEXT_TILE; // continue replacement
				}

			tileGroups->emplace_back();
			tileGroups->back().push_back(tile);
		}
	}
	
	void World::groupTiles(
		GroupedTiles* const groupedTiles, 
		TileGroups* const tileGroups) const
	{
		int totalTileCount = 0; // for logging

		for (const std::vector<TileBase*>& tileGroup : *tileGroups)
		{
			// for tests replace later
			for (TileBase* const tile : tileGroup)
			{
				++totalTileCount;

				groupedTiles->emplace_back();
				GroupedTile* groupedTile = &groupedTiles->back();

				groupedTile->tile = tile;

				groupedTile->size = { 1u, 1u };
				groupedTile->position = Resource::VectorTilePosition(tile->getPosition());
			}
			// -----------------------

			// group and push to tileGroups
		}


		Log::Information(
			L"Single Tiles: "
			+ std::to_wstring(totalTileCount)
		);
		Log::Information(
			L"Grouped Tiles: "
			+ std::to_wstring(groupedTiles->size())
		);
		Log::Information(
			L"Tile Groups: "
			+ std::to_wstring(tileGroups->size())
		);
	}
}
