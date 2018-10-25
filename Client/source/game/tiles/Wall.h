#pragma once

#include <Client/source/game/tiles/CollidableTile.h>
#include <Client/source/game/tiles/TileBase.h>
#include <Client/source/game/tiles/TileManager.h>

namespace Game
{
	namespace Tile
	{
		struct WallSettings
		{
			sf::Vector2f position, size;
		};

		class Wall
			:
			public Collidable
		{
		public:
			Wall(
				const WallSettings settings)
				:
				Collidable(
					{
						settings.position,
						settings.size,

						Type::Collidable,
						sf::Color::White,

						Id::Wall
					})
			{
			}

			~Wall()
			{
			}

			bool onCollision(
				const Collision collision,
				LocalPlayer* player,
				sf::Vector2f* remainingDestination)
			{
				if (collision.type == Collision::Vertical &&
					player->getPosition().y < collision.position.y)
				{
					player->changeMovement()->setOnGround(0.4f);
				}

				player->setPosition(
					collision.position);

				if (collision.type == Collision::Vertical)
				{
					player->changeMovement()->muliMovement(
						{ 1.f, 0.f });

					remainingDestination->x = collision.position.x + collision.remaining;
					remainingDestination->y = collision.position.y;
				}
				else
				{
					player->changeMovement()->muliMovement(
						{ 0.f, 1.f });

					remainingDestination->x = collision.position.x;
					remainingDestination->y = collision.position.y + collision.remaining;
				}

				return true;
			}
		};

		class WallResource
			:
			public BaseResource
		{
		public:
			static void registerResource()
			{
				Manager::registerTileResource( Id::Wall, new WallResource() );
			}

			_Success_(return > 0)
			int appendToBuffer(
				RESOURCE::ByteWriter* buffer) override
			{
				buffer->appendValue(&settings);

				return sizeof(settings);
			}

			_Success_(return > 0)
			int loadFromBuffer(
				char* buffer,
				const int length) override
			{
				const int size = sizeof(settings);

				if (length < size)
				{
					return 0;
				}

				memcpy(
					&settings,
					buffer,
					size);

				return size;
			}

			_Success_(return == true)
			bool loadFromTile(
				Tile::Base* tile) override
			{
				if (tile->getId() != Id::Wall)
				{
					return false;
				}

				settings.position = tile->getPosition();
				settings.size = tile->getSize();

				return true;
			}

			Tile::Base* create() override
			{
				return (Tile::Base*) new Wall(settings);
			}

		private:
			WallSettings settings;
		};
	}
}
