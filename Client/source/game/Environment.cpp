#include "Environment.h"

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/tiletrait/DrawableTile.h>
#include <Client/source/game/tiletrait/DynamicTile.h>
#include <Client/source/game/tiletrait/InitializableTile.h>
#include <Client/source/game/tiletrait/StaticTile.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/shared/tiles/TileDescription.h>

namespace Game
{
	bool Environment::initialize(
		Resource::World* const resource)
	{
		Log::Section section(L"Environment Initialize");

		if (!initializeCreateAndRegister(resource))
		{
			return false;
		}

		if (!initializeVertex())
		{
			return false;
		}

		return true;
	}

	bool Environment::initializeCreateAndRegister(
		Resource::World* const resource)
	{
		for (int i = 0; i < resource->TileContainer.size(); ++i)
			//Resource::Tile& tileResource : resource->TileContainer)
		{
			Resource::Tile& tileResource = resource->TileContainer[i];

			GameTileBase* const tile = Shared::TileDescription::Find(
				tileResource.Header.id
			)->creation.createGameTile(&tileResource, i);

			if (tile == NULL) // TODO: still possible?
			{
				delete tile;
				Log::Error(L"Failed to create tile");
				return false;
			}

			registerTile<GameTileBase>(tile);
		}

		for (GameTileBase* const tile : getTileType<GameTileBase>())
		{
			tile->registerType(this);
		}

		/* Move to new World
			for (InitializableTile* const tile : getTileType<InitializableTile>())
			{
				tile->initialize();
			}
		*/

		return true;
	}

	bool Environment::initializeVertex()
	{

		for (StaticTile* const tile : getTileType<StaticTile>())
		{
			sf::VertexBuffer vertexBuffer(sf::PrimitiveType::Quads);

			if (!vertexBuffer.create(4))
			{
				Log::Error(L"Failed to create vertex buffer");

				return false;
			}

			sf::Vertex vertices[4] = { };

			for (int i = 0; i < 4; ++i)
			{
				vertices[i].color = tile->getColor();
			}

			vertices[0].position = tile->getPosition();
			vertices[1].position = tile->getPosition() + sf::Vector2f(tile->getSize().x, 0);
			vertices[2].position = tile->getPosition() + tile->getSize();
			vertices[3].position = tile->getPosition() + sf::Vector2f(0, tile->getSize().y);

			vertexBuffer.update((const sf::Vertex*) & vertices);

			vertexWorld.push_back(
				std::move(vertexBuffer)
			);
		}

		return true;
	}

	void Environment::draw(sf::RenderTarget* const target) const
	{
		for (const sf::VertexBuffer& vertexTile : vertexWorld)
		{
			target->draw(vertexTile);
		}

		for (DrawableTile* const tile : getTileType<DrawableTile>())
		{
			tile->onDraw(target);
		}
	}

	void Environment::onEvent(const sf::Event event)
	{
	}

	void Environment::onLogic(const sf::Time time)
	{
		for (DynamicTile* const tile : getTileType<DynamicTile>())
		{
			tile->onLogic(time);
		}
	}
}
