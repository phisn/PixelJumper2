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
		const int identity)
	{
		const Resource::WallTile* const resource = (const Resource::WallTile*) tile->Content;

		if (tile->Header.contentSize != sizeof(Resource::WallTile::Content))
		{
			return NULL;
		}

		return new WallTile(
			identity,

			resource->Content.density,
			resource->Content.friction,

			sf::Vector2f(
				tile->Header.x,
				tile->Header.y
			),
			sf::Vector2f(
				tile->Header.width,
				tile->Header.height
			)
		);
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

			if (fabsf(movement.y) < friction)
			{
				movement.y = 0;
			}
			else
			{
				movement.y += movement.y > 0 ? -friction : friction;
			}
		}
		else
		{
			movement.x = collision.player->getProperties().movement->x;
			remainOffset.x = (collision.target.x - collision.info.position.x);

			if (fabsf(movement.x) < friction)
			{
				movement.x = 0;
			}
			else
			{
				movement.x += movement.x > 0 ? -friction : friction;
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
	
	Resource::TileBase* TileWall::createContent(
		const Resource::VectorTileSize size, 
		const Resource::VectorTilePosition position) const
	{
		Resource::WallTile* const tile = new Resource::WallTile();

		tile->Content.density = density; // TODO: Make Variable
		tile->Content.friction = friction;

		return tile;
	}
	
	bool TileWall::adopt(
		const Resource::TileBase* const tile)
	{
		const Resource::WallTile* const wt = (const Resource::WallTile*) tile;
		
		density = wt->Content.density;
		friction = wt->Content.friction;

		return true; // ignore
	}
}
