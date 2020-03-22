#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/EditorTileBase.h>
#include <Client/source/editor/tile/TileTemplate.h>
#include <Client/source/editor/tile/TileTemplateFactory.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/TileResource.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/shared/ValueDefinitions.h>

#include <limits>
#include <set>
#include <vector>

namespace Editor
{
	class World
	{
		struct GroupedTile
		{
			Editor::VectorTilePosition realPosition;

			Resource::VectorTileSize size;
			Resource::VectorTilePosition position;

			int instancePosition;
		};

		typedef std::vector<GroupedTile> GroupedTiles;

		typedef std::set<Editor::TileBase*> Tiles;
		typedef std::vector<Editor::TileBase*> TileGroup;
		typedef std::vector<TileGroup> TileGroups;

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
			const Resource::PlayerID playerID) const;

		bool adopt(const Resource::World* const world)
		{
			for (const Resource::Tile& tile : world->tiles)
			{
				const Resource::TileInstanceWrapper* tileInstance = &world->tileInstances[tile.content.instanceIndex];
				TileTemplate* const tileTemplate = TileTemplateFactory::GetTileTemplate(tileInstance->getID());

				for (int xoffset = 0; xoffset < tile.content.width; ++xoffset)
					for (int yoffset = 0; yoffset < tile.content.height; ++yoffset)
					{
						TileBase* const editorTile = tileTemplate->create(sf::Vector2f(
							tile.content.x + xoffset,
							tile.content.y + yoffset
						));

						tiles.insert(editorTile);
						editorTile->adopt(tileInstance);
					}
			}

			return true;
		}

		const std::set<TileBase*>& getTiles() const
		{
			return tiles;
		}

	private:
		void convertTiles(Resource::World* const world) const;
		void prepareTiles(
			Resource::World* const world,
			GroupedTiles* const groupedTiles) const;
		void sortTiles(TileGroups* const tileGroups) const;
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
