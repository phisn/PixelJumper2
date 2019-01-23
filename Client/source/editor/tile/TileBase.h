#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileId.h>

#include <Client/source/editor/grid/GridProperties.h>
#include <Client/source/resource/TileResource.h>

#include <Client/source/Common.h>

namespace Editor
{
	class TileBase
	{
		const VectorTileSize size =
		{
			1, 1
		};

	public:
		TileBase(
			const sf::Color color,
			const VectorTilePosition position)
		{
			shape.setFillColor(color);
			setPosition(position);
			shape.setSize(sf::Vector2f(
				size.x * GridProperties.tileSize, 
				size.y * GridProperties.tileSize
			));
		}

		virtual bool equals(
			TileBase* const tile) const
		{
			return true; // id == tile->id;
		}

		void setPosition(const VectorTilePosition position)
		{
			shape.setPosition(sf::Vector2f(
				position.x * GridProperties.tileSize,
				position.y * GridProperties.tileSize
			));
		}

		VectorTilePosition getPosition() const
		{
			return VectorTilePosition(
				shape.getPosition().x / GridProperties.tileSize,
				shape.getPosition().y / GridProperties.tileSize
			);
		}

		const sf::RectangleShape* getShape() const
		{
			return &shape;
		}

		virtual Resource::TileBase* create(
			const Resource::VectorTileSize size,
			const Resource::VectorTilePosition position) const = 0;
	private:
		sf::RectangleShape shape;
	};
}