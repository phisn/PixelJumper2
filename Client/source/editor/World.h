#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/TileBase.h>
#include <Client/source/editor/template/TileTemplate.h>

#include <Client/source/game/World.h>

#include <vector>

namespace Editor
{
	class World
	{
	public:
		~World()
		{
			for (TileBase* tile : tiles)
			{
				delete tile;
			}
		}

		Game::WorldSettings* createSettings(
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

		void addTile(
			TileBase* const tile)
		{
			tiles.push_back(tile);
		}

		void removeTile(
			TileBase* const tile)
		{
			for (int i = 0; i < tiles.size(); ++i)
				if (tiles[i] == tile)
				{
					if (i + 1 != tiles.size())
					{
						tiles[i] = tiles.back();
					}

					tiles.pop_back();
					
					delete tile;
					return;
				}
		}

		void removeAllTiles()
		{
			tiles.clear();
		}

		const std::vector<TileBase*>& getTiles() const
		{
			return tiles;
		}
	private:
		std::vector<TileBase*> tiles;
	};
}