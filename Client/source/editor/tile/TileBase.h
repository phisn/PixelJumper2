#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileId.h>

#include <Client/source/resource/TileResource.h>

namespace Editor
{
	struct TileState
	{
		sf::Vector2f position;

		virtual bool isSameGroup(
			const TileState* const state) const
		{
			return true;
		}
	};

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

		const sf::RectangleShape* getShape() const
		{
			return &shape;
		}

		virtual const TileState* getState() const = 0;
	private:
		sf::RectangleShape shape;
	};
}