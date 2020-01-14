#include "TileWall.h"

namespace Shared
{
	const TileDescription TileWall = TileDescription(
		TileCreation::Create<
			Game::WallTile, 
			Editor::WallTemplate, 
			Resource::WallTile>(),

		L"Wall",
		L"Solid Wall",
		sf::Color::Color(200, 200, 200),
		sf::Color::Color(200, 200, 200)
	);
}

namespace Game
{
	GameTileBase* WallTile::Create(
		const Resource::Tile* const tile,
		const Resource::TileInstanceWrapper* const instanceWrapper,
		const TileIdentity identity)
	{
		return new WallTile(
			identity,
			sf::Vector2f(
				tile->content.x,
				tile->content.y
			),
			sf::Vector2f(
				tile->content.width,
				tile->content.height
			),
			instanceWrapper->getInstance<Resource::WallTile>()->content);
	}

	void WallTile::registerType(Environment* const env)
	{
		env->registerTile<Game::WallTile>(this);

		CollidableTile::registerCollisionType(
			env,
			CollisionType::NormalCollision
		);
		StaticTile::registerType(env);
	}
	
	sf::Vector2f WallTile::onCollision(const CollisionType type, const Collision collision)
	{
		collision.player->getProperties().position = collision.info.position;

		sf::Vector2f movement = {};
		sf::Vector2f remainOffset = {};

		if (collision.info.isHorizontal())
		{
			movement.y = collision.player->getProperties().movement->y;
			remainOffset.y = (collision.target.y - collision.info.position.y);

			if (fabsf(movement.y) < content.friction)
			{
				movement.y = 0;
			}
			else
			{
				movement.y += movement.y > 0 ? -content.friction : content.friction;
			}
		}
		else
		{
			movement.x = collision.player->getProperties().movement->x;
			remainOffset.x = (collision.target.x - collision.info.position.x);

			if (fabsf(movement.x) < content.friction)
			{
				movement.x = 0;
			}
			else
			{
				movement.x += movement.x > 0 ? -content.friction : content.friction;
			}
		}

		collision.player->getProperties().movement = movement;
		notifyCollisionEvent(&type, &collision);

		return remainOffset;
	}
}

namespace Editor
{
	Menu::ElementBase* WallTemplate::createRepresentation()
	{
		return &button;
	}
	
	void TileWall::assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const
	{
		instanceWrapper->getInstance<Resource::WallTile>()->content = content;
	}

	bool TileWall::adopt(const Resource::TileInstanceWrapper* const instanceWrapper)
	{
		content = instanceWrapper->getInstance<Resource::WallTile>()->content;
		return true;
	}
}
