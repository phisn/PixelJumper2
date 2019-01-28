#include "World.h"

namespace Editor {
	_Ret_maybenull_
	Resource::World* Editor::World::convert(
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
}
	
bool Editor::World::convertTiles(
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

	sf::Vector2u worldSize;
		
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

		// rewrite
		sf::Uint32 x = tile.size.x + tile.position.x;
		if (worldSize.x < x)
		{
			worldSize.x = x;
		}

		sf::Uint32 y = tile.size.y + tile.position.y;
		if (worldSize.y < y)
		{
			worldSize.y = y;
		}
	}

	world->HeaderProperties.width = worldSize.x;
	world->HeaderProperties.height = worldSize.y;

	return groupedTiles.size() > 0; // TODO: pointless?
}
void Editor::World::sortTiles(TileGroups* const tileGroups) const
{
	for (TileBase* const tile : tiles)
	{
		for (std::vector<TileBase*>& group : *tileGroups)
			if (group.back()->equals(tile))
			{
				group.push_back(tile);

				goto Skip_New_Group_Creation;
			}

		tileGroups->emplace_back();
		tileGroups->back().push_back(tile);
	Skip_New_Group_Creation:;
	}
}
	
void Editor::World::groupTiles(
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