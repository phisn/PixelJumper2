#include "EditorWorld.h"

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

	// World size cant be larger than tile position (ignore size)
	TilePosition worldWidth = 0, worldHeight = 0;
	for (const GroupedTile& tile : groupedTiles)
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
		const sf::Uint32 totalTileWidth = tile.size.x + tile.position.x;
		if (worldWidth < totalTileWidth)
		{
			worldWidth = totalTileWidth;
		}

		const sf::Uint32 totalTileHeight = tile.size.y + tile.position.y;
		if (worldHeight < totalTileHeight)
		{
			worldHeight = totalTileHeight;
		}
	}

	world->HeaderProperties.width = worldWidth;
	world->HeaderProperties.height = worldHeight;

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

	struct LocalTileStorage
	{
		TileBase* tile;
		mutable bool consumed;
	};

	// start smallest from 0 in resource
	Editor::VectorTilePosition smallestTilePosition = tileGroups->begin()->begin().operator*()->getPosition();
	for (const std::vector<TileBase*>& tileGroup : *tileGroups)
	{
		Editor::VectorTilePosition smallestInGroup = std::min_element(tileGroup.begin(), tileGroup.end(),
			[&totalTileCount](const TileBase* const tile1, const TileBase* const tile2)
		{
				++totalTileCount;

				return tile1->getPosition().x == tile2->getPosition().x
					? tile1->getPosition().y < tile2->getPosition().y
					: tile1->getPosition().x < tile2->getPosition().x;
		}).operator*()->getPosition();

		if (smallestTilePosition.x == smallestInGroup.x
				? smallestTilePosition.y > smallestInGroup.y
: smallestTilePosition.x > smallestInGroup.x)
		{
		smallestTilePosition = smallestInGroup;
		}
	}

	for (const std::vector<TileBase*>& tileGroup : *tileGroups)
	{
		std::set<LocalTileStorage, bool(*)(const LocalTileStorage, const LocalTileStorage)> orderedTiles(
			[](const LocalTileStorage tile1, const LocalTileStorage tile2)
			{
				return tile1.tile->getPosition().x == tile2.tile->getPosition().x
					? tile1.tile->getPosition().y < tile2.tile->getPosition().y
					: tile1.tile->getPosition().x < tile2.tile->getPosition().x;
			});

		for (TileBase* tile : tileGroup)
		{
			orderedTiles.insert({ tile, false });
		}

		GroupedTile* current = NULL;
		for (decltype(orderedTiles)::iterator tile = orderedTiles.begin()
			; tile != orderedTiles.end(); ++tile)
		{
		AQUIRE_NEW_GROUP:
			if (tile->consumed)
			{
				continue;
			}

			if (current == NULL)
			{
				groupedTiles->emplace_back();
				current = &groupedTiles->back();

				current->position = Resource::VectorTilePosition(
					tile->tile->getPosition() - smallestTilePosition
				);
				current->realPosition = tile->tile->getPosition();
				current->size = { 1, 1 };
				current->tile = tile->tile;

				continue;
			}

			// extend vertical
			if (current->realPosition.x == tile->tile->getPosition().x &&					// same width
				current->realPosition.y + current->size.y == tile->tile->getPosition().y)	// exact height
			{
				current->size.y++;
				tile->consumed = true;

				continue;
			}

			// extend horizontal
			decltype(orderedTiles)::iterator currentTile = tile;
			while (true)
			{
				while (currentTile != orderedTiles.end() // reached end
					&& current->realPosition.x + current->size.x != currentTile->tile->getPosition().x)
				{
					++currentTile;
				}  // goto expected x

				while (currentTile != orderedTiles.end() // reached end
					&& current->realPosition.x + current->size.x == currentTile->tile->getPosition().x
					&& current->realPosition.y != currentTile->tile->getPosition().y)
				{
					++currentTile;
				} // goto expected y

				// reached end or invalid width
				if (currentTile == orderedTiles.end())
				{
					current = NULL;
					goto AQUIRE_NEW_GROUP;
				}

				// remark to set all consumed
				decltype(orderedTiles)::iterator currentRowBegin = currentTile;

				{	// check whole row if exists
					int offset = 0;

					while (true)
					{
						if (current->realPosition.x + current->size.x != currentTile->tile->getPosition().x)
						{
							current = NULL;
							goto AQUIRE_NEW_GROUP;
						}

						if (current->realPosition.y + offset != currentTile->tile->getPosition().y)
						{
							current = NULL;
							goto AQUIRE_NEW_GROUP; // failed
						}

						++currentTile;

						if (++offset >= current->size.y)
						{
							break;
						}

						if (currentTile == orderedTiles.end())
						{
							current = NULL;
							goto AQUIRE_NEW_GROUP;
						}
					}
				}

				// success -> set all consumed
				for (int i = 0; i < current->size.y; ++i)
				{
					(currentRowBegin++)->consumed = true;
				}

				++current->size.x;
			}

			// unreachable
		}
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
