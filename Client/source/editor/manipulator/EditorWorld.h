#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/EditorTileBase.h>
#include <Client/source/editor/template/TileTemplate.h>
#include <Client/source/editor/template/TileTemplateFactory.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/TileResource.h>
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
			Editor::VectorTilePosition realPosition;

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
			TileTemplate* const tileTemplate);
		void replaceTileSafe(
			TileBase* const tile,
			TileBase* const newTile);
		void replaceTileUnsafe(
			TileBase* const tile,
			TileBase* const newTile);

		// slow
		void setTileSafe(TileBase* const newTile);
		// fast
		void setTileUnsafe(TileBase* const newTile);

		void deleteTile(TileBase* const tile);
		void removeTile(TileBase* const tile);

		_Ret_maybenull_
		Resource::World* convert(
			const sf::Uint32 worldID,
			const std::wstring authorName,
			const std::wstring mapName) const;

		bool adopt(const Resource::World* const world)
		{
			for (const Resource::Tile& resourceTile : world->TileContainer)
			{
				TileTemplate* const tileTemplate = TileTemplateFactory::GetTileTemplate(resourceTile.Header.id);

				for (int xoffset = 0; xoffset < resourceTile.Header.width; ++xoffset)
					for (int yoffset = 0; yoffset < resourceTile.Header.height; ++yoffset)
					{
						TileBase* const editorTile = tileTemplate->create(sf::Vector2f(
							resourceTile.Header.x + xoffset, 
							resourceTile.Header.y + yoffset
						));

						tiles.insert(editorTile);
						editorTile->adopt(resourceTile.Content);
					}
			}

			return true;
		}

		const std::set<TileBase*>& getTiles() const
		{
			return tiles;
		}

	private:
		bool convertTiles(
			Resource::World* const world) const;
		void sortTiles(
			TileGroups* const tileGroups) const;
		void groupTiles(
			GroupedTiles* const groupedTiles,
			TileGroups* const tileGroups) const;

		Tiles tiles;
	};

	inline void World::replaceTileTemplate(
		TileBase* const tile, 
		TileTemplate* const tileTemplate)
	{
		sf::Vector2f position = tile->getPosition();

		deleteTile(tile);
		setTileUnsafe(
			tileTemplate->create(tile->getPosition())
		); // do not replace with 'replaceTileUnsafe' ! order !
	}

	inline void World::replaceTileSafe(
		TileBase* const tile, 
		TileBase* const newTile)
	{
		newTile->setPosition(tile->getPosition());
		replaceTileUnsafe(tile, newTile);
	}

	inline void World::replaceTileUnsafe(
		TileBase* const tile, 
		TileBase* const newTile)
	{
		deleteTile(tile);
		setTileUnsafe(newTile);
	}

	// slow
	inline void World::setTileSafe(
		TileBase* const newTile)
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
	inline void World::setTileUnsafe(
		TileBase* const newTile)
	{
		tiles.insert(newTile);
	}

	inline void World::deleteTile(
		TileBase* const tile)
	{
		removeTile(tile);
		delete tile;
	}

	inline void World::removeTile(
		TileBase* const tile)
	{
		tiles.erase(tile);
	}
}
