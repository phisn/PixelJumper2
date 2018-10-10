#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

namespace Game
{
	namespace Tile
	{
		enum class Type
		{
			Invalid,

			Collidable = 0b1,
			Touchable = 0b01,
		};

		class Base
		{
		public:
			Base(
				const Type type,
				const sf::Color color)
				:
				type(type)
			{
				shape.setFillColor(color);
			}

			void draw() const
			{
				DEVICE::Interface::getScreen()->draw(&shape);
			}

			sf::Vector2f getSize() const
			{
				return shape.getSize();
			}

			sf::Vector2f getPosition() const
			{
				return shape.getPosition();
			}
		protected:
			sf::RectangleShape shape;

		private:
			Type type;
		};

		const float defaultTileSize = 1.0f;
	}
}

Game::Tile::Type type = Game::Tile::Type::Invalid;
