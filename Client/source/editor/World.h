#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/TileBase.h>
#include <Client/source/editor/template/TileTemplate.h>

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/WorldResource.h>

#include <limits>
#include <set>
#include <vector>

#define RTILE_TYPE_MAX(var) std::numeric_limits< decltype( decltype(Resource::Tile::Header):: ## var )>::max()

namespace Editor
{
	class World
	{
		struct GroupedTile
		{
			Resource::VectorTileSize size;
			Resource::VectorTilePosition position;

			Editor::TileBase* tile;
		};

		typedef std::set<Editor::TileBase*> Tiles;
		typedef std::vector<
			std::vector<Editor::TileBase*>
		> TileGroups;
		typedef std::vector<GroupedTile> GroupedTiles;
	public:
		void replaceTileTemplate(
			TileBase* const tile,
			TileTemplate* const tileTemplate)
		{
			VectorTilePosition position = tile->getPosition();

			deleteTile(tile);
			setTileUnsafe(
				tileTemplate->create( tile->getPosition() )
			); // do not replace with 'replaceTileUnsafe' ! order !
		}

		void replaceTileSafe(
			TileBase* const tile,
			TileBase* const newTile)
		{
			newTile->setPosition( tile->getPosition() );
			replaceTileUnsafe(tile, newTile);
		}

		void replaceTileUnsafe(
			TileBase* const tile,
			TileBase* const newTile)
		{
			deleteTile(tile);
			setTileUnsafe(newTile);
		}

		// slow
		void setTileSafe(TileBase* const newTile)
		{
			for (TileBase* const tile : tiles)
				if (newTile->getPosition() == tile->getPosition())
				{
					replaceTileUnsafe(tile, newTile);
					return;
				}

			setTileUnsafe(newTile);
		}

		// fast
		void setTileUnsafe(TileBase* const newTile)
		{
			tiles.insert(newTile);
		}

		void deleteTile(TileBase* const tile)
		{
			removeTile(tile);
			delete tile;
		}

		void removeTile(TileBase* const tile)
		{
			tiles.erase(tile);
		}

		_Ret_maybenull_
		Resource::World* convert(
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
				+ std::to_wstring( tiles.size() )
				+ L"' tiles");

			if ( !convertTiles(world) )
			{
				delete world;
				world = NULL;
			}

			return world;
		}

		const std::set<TileBase*>& getTiles() const
		{
			return tiles;
		}

	private:
		bool convertTiles(Resource::World* const world) const
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
		 
		void sortTiles(TileGroups* const tileGroups) const
		{
			for (TileBase* const tile : tiles)
			{
			NEXT_TILE:
				for (std::vector<TileBase*>& group : *tileGroups)
					if ( group.back()->equals(tile) )
					{
						group.push_back(tile);

						goto NEXT_TILE; // continue replacement
					}

				tileGroups->emplace_back();
				tileGroups->back().push_back(tile);
			}
		}

		void groupTiles(
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
					groupedTile->position = Resource::VectorTilePosition( tile->getPosition() );
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

		Tiles tiles;
	};
}