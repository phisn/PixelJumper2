#include "Environment.h"

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/tiletrait/StaticTile.h>
#include <Client/source/game/tiletrait/DrawableTile.h>
#include <Client/source/game/tiletrait/DynamicTile.h>

#include <Client/source/logger/Logger.h>

namespace Game
{
	bool Environment::initialize(
		Resource::World* const resource)
	{
		Log::Section section(L"");

		for (StaticTile* const tile : getTileType<StaticTile>())
		{
			sf::VertexBuffer vertexBuffer(sf::VertexBuffer::Static);
			
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
			vertices[2].position = tile->getPosition() + sf::Vector2f(0, tile->getSize().y);
			vertices[3].position = tile->getPosition() + tile->getSize();

			if (!vertexBuffer.update(vertexBuffer))
			{
				Log::Error(L"Failed to update vertex buffer");

				return false;
			}

			vertexWorld.push_back(
				std::move(vertexBuffer)
			);
		}

		return true;
	}

	void Environment::draw() const
	{
		for (DrawableTile* const tile : getTileType<DrawableTile>())
		{
			tile->onDraw();
		}

		for (const sf::VertexBuffer& vertexTile : vertexWorld)
		{
			Device::Screen::Draw(vertexTile);
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
