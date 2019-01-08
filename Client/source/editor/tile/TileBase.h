#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileId.h>

#include <Client/source/resource/TileResource.h>

namespace Editor
{
	class TileBase
	{
		const sf::Vector2f size =
		{
			1.f, 1.f
		};

	public:
		TileBase(
			const sf::Color color,
			const sf::Vector2f position)
		{
			shape.setFillColor(color);
			shape.setPosition(position);
			shape.setSize(size);
		}

		virtual bool equals(
			TileBase* const tile) const
		{
			return true; // id == tile->id;
		}

		const sf::RectangleShape* getShape() const
		{
			return &shape;
		}

		virtual Resource::TileBase* create(
			const sf::Vector2f size,
			const sf::Vector2f position) const = 0;
	private:
		sf::RectangleShape shape;
	};
}