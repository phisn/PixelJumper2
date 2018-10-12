#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

#ifndef GAME
#define GAME ::Game
#endif

namespace Game
{
	namespace Tile
	{
		enum class Type
		{
			Invalid,

			Collidable = 0b1,
			Touchable = 0b01,
			Timed = 0b001
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

			sf::Vector2f getSize() const;
			sf::Vector2f getPosition() const;

			Type getType() const;
		protected:
			sf::RectangleShape shape;

		private:
			Type type;
		};

		inline sf::Vector2f Base::getSize() const
		{
			return shape.getSize();
		}

		inline sf::Vector2f Base::getPosition() const
		{
			return shape.getPosition();
		}

		inline Type Base::getType() const
		{
			return type;
		}

		const float defaultTileSize = 1.0f;
	}
}

Game::Tile::Type type = Game::Tile::Type::Invalid;
