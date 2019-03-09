#include "GameWorld.h"
#include <Client/source/game/tiletrait/StaticTile.h>

namespace Game
{

	GameWorld* GameWorld::Create(Resource::World * const worldResource)
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

			result->insertTile<GameTileBase>(tile);
			tile->registerType(this);
		}

		sf::Image image;
		image.create(
			worldResource->HeaderProperties.width,
			worldResource->HeaderProperties.height
		);

		for (StaticTile* const tile : result-><StaticTile>())
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

}
