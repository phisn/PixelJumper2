#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/game/WorldState.h>

#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/game/GameTileContainer.h>
#include <Client/source/game/GameTileFactory.h>

#include <Client/source/resource/WorldResource.h>

namespace Game
{
	class GameWorld
	{
		GameWorld() = default;
	public:
		WorldState worldState; // TODO: Open?

		static GameWorld* Create(
			Resource::World* const worldResource)
		{
			Log::Section section(L"Creating world from WorldResource");
			
			section.information(L"Map: " + worldResource->HeaderAuth.mapName);
			section.information(L"Author: " + worldResource->HeaderAuth.authorName);
			section.information(L"ID: " + std::to_wstring(worldResource->HeaderIntro.worldID));
			section.information(L"TileCount: " + std::to_wstring(worldResource->HeaderProperties.tileCount));
			section.information(L"Width: " + std::to_wstring(worldResource->HeaderProperties.width));
			section.information(L"Height: " + std::to_wstring(worldResource->HeaderProperties.height));

			GameWorld* result = new GameWorld();

			result->Properties.size = sf::Vector2f(
				worldResource->HeaderProperties.width,
				worldResource->HeaderProperties.height
			);

			result->Properties.authorName = worldResource->HeaderAuth.authorName;
			result->Properties.mapName = worldResource->HeaderAuth.mapName;

			for (Resource::Tile& tileResource : worldResource->TileContainer)
			{
				GameTileBase* const tile = GameTileFactory::Create(&tileResource);

				if (tile == NULL)
				{
					section.error(L"Failed to create tile");

					return NULL;
				}

				result->tileContainer.insertTile(tile);
			}

			sf::Image image;
			image.create(
				worldResource->HeaderProperties.width,
				worldResource->HeaderProperties.height
			);

			for (StaticTile* const tile : result->tileContainer.getSortedTiles().staticTiles)
			{
				const sf::Vector2i target = sf::Vector2i(tile->getPosition() + tile->getSize());

				[[unlikely]] if (target.x > worldResource->HeaderProperties.width ||
					target.y > worldResource->HeaderProperties.height ||
					target.x < 0 || target.y < 0)
				{
					section.error(L"Tile is out of bounds");

					return NULL;
				}

				for (int x = tile->getPosition().x; x < target.x; ++x)
					for (int y = tile->getPosition().y; y < target.y; ++y)
					{
						image.setPixel(x, y, tile->getColor());
					}
			}

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

			for (DynamicTile* const tile : tileContainer.getSortedTiles().dynamicTiles)
			{
				tile->onDraw();
			}
		}

		void onLogic(const sf::Time time) const
		{
			for (DynamicTile* const tile : tileContainer.getSortedTiles().dynamicTiles)
			{
				tile->onLogic(time);
			}
		}

		const TileContainer* getTileContainer() const
		{
			return &tileContainer;
		}

		sf::Vector2f getSize() const;
		const std::wstring& getAuthorName() const;
		const std::wstring& getMapName() const;
	private:
		struct
		{
			sf::Vector2f size;

			std::wstring authorName;
			std::wstring mapName;
		} Properties;

		TileContainer tileContainer;

		struct
		{
			sf::Texture texture;
			sf::Sprite sprite;
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
}
