#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/WorldState.h>

#include <Client/source/game/GameTileBase.h>
#include <Client/source/game/GameTileFactory.h>

#include <Client/source/game/EnterableTile.h>
#include <Client/source/game/LeaveableTile.h>

#include <Client/source/resource/WorldResource.h>

#include <set>
#include <string>

namespace Game
{
	class GameWorld
	{
		GameWorld() = default;
	public:
		WorldState worldState;

		struct SortedTiles
		{
			std::set<GameTileBase*> moveableTiles;
			std::set<EnterableTile*> enterableTiles;
			std::set<LeaveableTile*> leaveableTiles;
		};

		static GameWorld* Create(
			Resource::World* const worldResource)
		{
			GameWorld* result = new GameWorld();

			result->Properties.size = sf::Vector2f(
				worldResource->HeaderProperties.width,
				worldResource->HeaderProperties.height
			);

			result->Properties.authorName = worldResource->HeaderAuth.authorName;
			result->Properties.mapName = worldResource->HeaderAuth.mapName;

			for (Resource::Tile& tileResource : worldResource->TileContainer)
			{
				result->tiles.push_back(
					GameTileFactory::Create(&tileResource)
				);
			}

			sf::Image image;
			image.create(
				worldResource->HeaderProperties.width,
				worldResource->HeaderProperties.height
			);

			for (int i = 0; i < result->tiles.size(); ++i)
			{
				if (result->tiles[i]->getTileProperties()->isStatic)
				{
					result->Visual.nonStaticTiles.insert(
						result->tiles[i]
					);
				}
				else
				{
					for (int x = 0; x < worldResource->TileContainer[i].Header.width; ++x)
						for (int y = 0; y < worldResource->TileContainer[i].Header.height; ++y)
						{
							image.setPixel(
								worldResource->TileContainer[i].Header.x + x,
								worldResource->TileContainer[i].Header.y + y,
								result->tiles[i]->getColor()
							);
						}
				}

				if (result->tiles[i]->getTileProperties()->isEnterable)
				{
					result->sortedTiles.enterableTiles.insert((EnterableTile*) result->tiles[i]);
				}

				if (result->tiles[i]->getTileProperties()->isLeaveable)
				{
					result->sortedTiles.leaveableTiles.insert((LeaveableTile*) result->tiles[i]);
				}
			}

			result->Visual.hasNonStatic = result->Visual.nonStaticTiles.size() > 0;

			result->Visual.texture.loadFromImage(image);
			result->Visual.sprite.setTexture(result->Visual.texture);

			return result;
		}

		Resource::World* convert() const
		{
			return NULL;
		}

		void onDraw() const
		{
			Device::Interface::GetScreen()->onDraw(&Visual.sprite);

			[[unlikely]] if (Visual.hasNonStatic) // unlikely for now
			{
				for (GameTileBase* const tile : Visual.nonStaticTiles)
				{
					tile->onDraw();
				}
			}
		}

		void onLogic(const sf::Time time) const
		{
			for (GameTileBase* const tile : tiles)
			{
				tile->onLogic(time);
			}
		}

		sf::Vector2f getSize() const;
		const std::wstring& getAuthorName() const;
		const std::wstring& getMapName() const;

		const SortedTiles& getSortedTiles() const;
		const std::vector<GameTileBase*>& getTiles() const;

	private:
		struct
		{
			sf::Vector2f size;

			std::wstring authorName;
			std::wstring mapName;
		} Properties;

		std::vector<GameTileBase*> tiles;
		SortedTiles sortedTiles;

		struct
		{
			sf::Texture texture;
			sf::Sprite sprite;
			
			std::set<GameTileBase*> nonStaticTiles;

			bool hasNonStatic;
		} Visual;
	};

	inline sf::Vector2f GameWorld::getSize() const
	{
		return Properties.size;
	}

	inline const std::wstring& GameWorld::getAuthorName() const
	{
		return Properties.authorName;
	}

	inline const std::wstring& GameWorld::getMapName() const
	{
		return Properties.mapName;
	}

	inline const GameWorld::SortedTiles& GameWorld::getSortedTiles() const
	{
		return sortedTiles;
	}

	inline const std::vector<GameTileBase*>& GameWorld::getTiles() const
	{
		return tiles;
	}

}
