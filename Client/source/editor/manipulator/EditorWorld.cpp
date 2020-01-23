#include "EditorWorld.h"

_Ret_maybenull_
Resource::World* Editor::World::convert(
	const sf::Uint32 worldID,
	const Resource::PlayerID playerID) const
{
	Log::Section section(L"Converting World");

	if (worldID == NULL)
	{
		Log::Error(L"Got invalid worldID: (NULL)");

		return NULL;
	}

	Resource::World* world = new Resource::World();

	world->content.id = worldID;
	world->content.author = playerID;

	world->content.defaultPlayerProperties.inputForce = 1.f;
	world->content.defaultPlayerProperties.inputReduce = 2.f;
	world->content.defaultPlayerProperties.mass = 1.f;
	world->content.defaultPlayerProperties.movement = { 0, 0 };
	world->content.defaultPlayerProperties.position = { 0, 0 };
	world->content.defaultPlayerProperties.viewFollow = true;
	world->content.defaultPlayerProperties.viewRotation = 0.f;
	world->content.defaultPlayerProperties.viewWindow = { 0.f, 0.f, 100.f, 100.f };

	world->content.defaultProperties.airResistance = 1.f;
	world->content.defaultProperties.gravity = { 0.f, 1.f };

	Log::Information(
		L"Converting '"
		+ std::to_wstring(tiles.size())
		+ L"' tiles");

	convertTiles(world);

	return world;
}
	
void Editor::World::convertTiles(Resource::World* const world) const
{
	GroupedTiles groupedTiles;
	prepareTiles(world, &groupedTiles);

	world->tiles.reserve(groupedTiles.size());

	// World size cant be larger than tile position (ignore size)
	TilePosition worldWidth = 0, worldHeight = 0;
	for (const GroupedTile& tile : groupedTiles)
	{
		world->tiles.emplace_back();
		Resource::Tile* resourceTile = &world->tiles.back();

		resourceTile->content.width = tile.size.x;
		resourceTile->content.height = tile.size.y;

		resourceTile->content.x = tile.position.x;
		resourceTile->content.y = tile.position.y;

		resourceTile->content.instanceIndex = tile.instancePosition;

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

	world->content.width = worldWidth;
	world->content.height = worldHeight;
}

void Editor::World::prepareTiles(
	Resource::World* const world,
	GroupedTiles* const groupedTiles) const
{
	TileGroups tileGroups;
	sortTiles(&tileGroups);

	world->tileInstances.reserve(tileGroups.size());
	for (int i = 0; i < tileGroups.size(); ++i)
	{
		TileBase* const tile = tileGroups[i][0];

		world->tileInstances.emplace_back(tile->getTileId());
		tile->assignInstance(&world->tileInstances.back());
	}

	groupTiles(groupedTiles, &tileGroups);
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

	// start smallest from 0 in resource
	// TODO: rewrite find smallest (quick and easy)
	Editor::VectorTilePosition smallestTilePosition = (VectorTilePosition) tileGroups->begin()->begin().operator*()->getPosition();
	for (const std::vector<TileBase*>& tileGroup : *tileGroups)
	{
		const Editor::TilePosition smallestWidthInGroup = std::min_element(tileGroup.begin(), tileGroup.end(),
			[&totalTileCount](const TileBase* const tile1, const TileBase* const tile2)
		{
			return tile1->getPosition().x < tile2->getPosition().x;

		}).operator*()->getPosition().x;

		const Editor::TilePosition smallestHeightInGroup = std::min_element(tileGroup.begin(), tileGroup.end(),
			[&totalTileCount](const TileBase* const tile1, const TileBase* const tile2)
		{
			return tile1->getPosition().y < tile2->getPosition().y;

		}).operator*()->getPosition().y;

		if (smallestWidthInGroup < smallestTilePosition.x)
		{
			smallestTilePosition.x = smallestWidthInGroup;
		}

		if (smallestHeightInGroup < smallestTilePosition.y)
		{
			smallestTilePosition.y = smallestHeightInGroup;
		}
	}

	struct LocalTileStorage
	{
		TileBase* tile;
		mutable bool consumed;
	};

	for (int instance = 0; instance < tileGroups->size(); ++instance)
	{
		// prepare sorted tiles
		std::set<LocalTileStorage, bool(*)(const LocalTileStorage, const LocalTileStorage)> orderedTiles(
			[](const LocalTileStorage tile1, const LocalTileStorage tile2)
			{
				return tile1.tile->getPosition().x == tile2.tile->getPosition().x
					? tile1.tile->getPosition().y < tile2.tile->getPosition().y
					: tile1.tile->getPosition().x < tile2.tile->getPosition().x;
			});

		for (TileBase* const tile : (*tileGroups)[instance])
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
					(VectorTilePosition) tile->tile->getPosition() - smallestTilePosition
				);
				current->realPosition = (VectorTilePosition) tile->tile->getPosition();
				current->size = { 1, 1 };
				current->instancePosition = instance;

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
		L"Tile Groups: "
		+ std::to_wstring(tileGroups->size())
	);
	Log::Information(
		L"Grouped Tiles: "
		+ std::to_wstring(groupedTiles->size())
	);
}
