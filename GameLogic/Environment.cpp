#include "Environment.h"

#include "tiletrait/DrawableTile.h"
#include "tiletrait/DynamicTile.h"
#include "tiletrait/InitializableTile.h"
#include "tiletrait/StaticTile.h"

#include "GameElementBase.h"

#include "Logger/Logger.h"

namespace Game
{
	bool Environment::initialize(
		const Resource::World* const resource)
	{
		Log::Section section(L"initialize Environment");

		// vertex init moved
		return initializeCreateAndRegister(resource);
	}

	bool Environment::initializeCreateAndRegister(const Resource::World* const resource)
	{
		for (int i = 0; i < resource->tiles.size(); ++i)
		{
			const Resource::Tile& tileResource = resource->tiles[i];
			resource->components[tileResource.componentIndex].;

			/*
				const Resource::TileInstanceWrapper* const tileInstance = &resource->components[tileResource.content.instanceIndex];

				GameElementBase* const tile = Shared::TileDescription::Find(
					tileInstance->getID()
				)->creation.createGameTile(&tileResource, tileInstance, i);
			*/

			if (tile == NULL)
			{
				Log::Error(L"Failed to create tile");
				return false;
			}

			registerTile<GameElementBase>(tile);
		}

		for (GameElementBase* const tile : getTileType<GameElementBase>())
		{
			tile->registerComponents(this);
		}

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
				vertices[i].color = tile->color;
			}

			vertices[0].position = tile->content.position;
			vertices[1].position = tile->content.position + sf::Vector2f(tile->content.size.x, 0);
			vertices[2].position = tile->content.position + tile->content.size;
			vertices[3].position = tile->content.position + sf::Vector2f(0, tile->content.size.y);

			vertexBuffer.update((const sf::Vertex*) & vertices);

			vertexWorld.push_back(
				std::move(vertexBuffer)
			);
		}

		return true;
	}

	void Environment::processLogic()
	{
		for (DynamicTile* const tile : getTileType<DynamicTile>())
		{
			tile->processLogic();
		}
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
}
