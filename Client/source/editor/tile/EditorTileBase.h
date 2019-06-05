#pragma once

#include <Client/source/game/tile/TileId.h>
#include <Client/source/resource/TileResource.h>

#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

namespace Editor
{
	class TileBase
	{
	public:
		TileBase(const Game::TileId id)
			:
			id(id)
		{
		}

		virtual void setPosition(const sf::Vector2f position)
		{
			this->position = position;
		}

		virtual void setSize(const sf::Vector2f size)
		{
			this->size = size;
		}

		sf::Vector2f getPosition() const
		{
			return position;
		}

		sf::Vector2f getSize() const
		{
			return size;
		}

		virtual bool equals(
			TileBase* const tile) const
		{
			return id == tile->id;
		}

		virtual bool adopt(const Resource::TileBase* const tile) = 0;
		virtual Resource::TileBase* createContent(
			const Resource::VectorTileSize size,
			const Resource::VectorTilePosition position) const = 0;

		virtual void draw() const = 0;

	private:
		sf::Vector2f size{ 1, 1 }, position{ 0, 0 };

		const Game::TileId id;
	};
}