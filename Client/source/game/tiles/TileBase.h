#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/tiles/TileId.h>

#include <Client/source/resource/ResourceBase.h>

#include <SFML/Graphics.hpp>

#ifndef GAME
#define GAME ::Game
#endif

#ifndef CONTAINS_ENUM
#define CONTAINS_ENUM(val, element) ((int) val & (int) element)
#endif

namespace Game
{
	namespace Tile
	{
		enum class Type
		{
			Invalid		= 0b000,

			Collidable	= 0b100,
			Timed		= 0b010,
			Static		= 0b001
		};

		struct BaseSettings
		{
			sf::Vector2f position, size;
			Type type;
			sf::Color color;

			Id id;
		};
		
		class Base
		{
		public:
			Base(
				const BaseSettings settings)
				:
				type(settings.type),
				id(settings.id)
			{
				shape.setPosition(settings.position);
				shape.setSize(settings.size);
				shape.setFillColor(settings.color);
			}

			virtual ~Base()
			{
			}

			void onDraw() const
			{
				DEVICE::Interface::GetScreen()->onDraw(&shape);
			}

			sf::Vector2f getSize() const;
			sf::Vector2f getPosition() const;

			sf::Color getColor() const;
			Type getType() const;

			Id getId() const
			{
				return id;
			}
		protected:
			const Id id;
			sf::RectangleShape shape;

		private:
			Type type;
		};

		struct BaseResource
			:
			public RESOURCE::Base
		{
			virtual sf::Uint32 getSize() const = 0;

			_Success_(return == true)
			virtual bool loadFromTile(
				Tile::Base* tile) = 0;

			virtual Tile::Base* create() = 0;
		};

		inline sf::Vector2f Base::getSize() const
		{
			return shape.getSize();
		}

		inline sf::Vector2f Base::getPosition() const
		{
			return shape.getPosition();
		}

		inline sf::Color Base::getColor() const
		{
			return shape.getFillColor();
		}

		inline Type Base::getType() const
		{
			return type;
		}

		static const float defaultTileSize = 1.0f;
	}
}