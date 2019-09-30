#include "TileExit.h"

namespace Shared
{
	const TileDescription TileExit = TileDescription(
		TileCreation::Create<
			Game::TileExit,
			Editor::ExitTemplate,
			Resource::TileExit>(),

		L"Exit",
		L"Touch to finish the level",
		sf::Color::Color(50, 50, 200),
		sf::Color::Color(50, 50, 200)
	);
}

namespace Game
{
	GameTileBase* TileExit::Create(
		const Resource::Tile* const tile, 
		const TileIdentity identity)
	{
		const Resource::TileExit* const resource = (const Resource::TileExit*) tile;

		if (tile->Header.contentSize != sizeof(Resource::TileExit::Content))
		{
			return NULL;
		}

		return new TileExit(
			identity,
			sf::Vector2f(
				tile->Header.x,
				tile->Header.y
			),
			sf::Vector2f(
				tile->Header.width,
				tile->Header.height
		));
	}

	void TileExit::registerType(Environment* env)
	{
		CollidableTile::registerCollisionType(
			env,
			CollisionType::NormalCollision
		);
		ExitableTile::registerType(env);
		StaticTile::registerType(env);
	}
}

namespace Editor
{
	Resource::TileBase* TileExit::createContent(const Resource::VectorTileSize size, const Resource::VectorTilePosition position) const
	{
		return new Resource::TileExit();
	}
}
