#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/game/tile/TileId.h>

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
			const VectorTilePosition position,
			Game::TileId id)
			:
			id(id)
		{
			shape.setFillColor(color);
			setPosition(position);
			shape.setSize(sf::Vector2f(
				(float) size.x * GridProperties.tileSize, 
				(float) size.y * GridProperties.tileSize
			));
		}

		virtual bool equals(
			TileBase* const tile) const
		{
			return id == tile->id;
		}

		void setPosition(const VectorTilePosition position)
		{
			shape.setPosition(sf::Vector2f(
				(float) position.x * GridProperties.tileSize,
				(float) position.y * GridProperties.tileSize
			));
		}

		VectorTilePosition getPosition() const
		{
			return VectorTilePosition(
				(TilePosition) (shape.getPosition().x / GridProperties.tileSize),
				(TilePosition) (shape.getPosition().y / GridProperties.tileSize)
			);
		}

		const sf::RectangleShape* getShape() const
		{
			return &shape;
		}

		virtual Resource::TileBase* create(
			const Resource::VectorTileSize size,
			const Resource::VectorTilePosition position) const = 0;
		virtual bool adopt(
			const Resource::TileBase* const tile) = 0;

		const Game::TileId id;
	private:
		sf::RectangleShape shape;
	};
}